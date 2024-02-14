/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:11:03 by rbourgea          #+#    #+#             */
/*   Updated: 2024/02/14 16:11:03 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void	*create_heap(size_t size)
{
	void			*ptr;
	struct rlimit	rlim;
	int				ret;

	ret = getrlimit(RLIMIT_DATA, &rlim);
	if (ret == 0 && rlim.rlim_max > size) {
		ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,\
					MAP_PRIVATE | MAP_ANON, -1, 0);
		if (ptr != MAP_FAILED) {
			((t_chunk *)ptr)->size = size | S_FREE;
		}
		else {
			ptr = NULL;
		}
	}
	else {
		ptr = NULL;
	}
	return (ptr);
}

static void	*use_free_chunk(t_chunk *free_chunk, size_t size, size_t status)
{
	t_chunk	*newptr;

	if (GET_SIZE(free_chunk->size) == size ||
			GET_SIZE(free_chunk->size) - size < HEAD_SIZE) {
		if (free_chunk->previous)
			free_chunk->previous->next = free_chunk->next;
		if (free_chunk->next)
			free_chunk->next->previous = free_chunk->previous;
		newptr = free_chunk->next;
		free_chunk->size = free_chunk->size | status;
		free_chunk->size &= ~S_FREE;
	}
	else {
		newptr = (void *)free_chunk + size;
		newptr->size = (free_chunk->size - size) | status;
		newptr->size |= S_FREE;
		newptr->previous = free_chunk->previous;
		newptr->prev_size = size | status;
		newptr->next = free_chunk->next;
		if (newptr->next)
			newptr->next->previous = newptr;
		if (newptr->previous)
			newptr->previous->next = newptr;
		free_chunk->size = size | status;
	}
	free_chunk->next = 0;
	free_chunk->previous = 0;
	return (newptr);
}

void	*extend_chunk(t_chunk *chunk, size_t size, t_chunk **bin, uint64_t limit)
{
	t_chunk	*free_part;
	t_chunk	tmp;
	int64_t	add_size;

	add_size = size - GET_SIZE(chunk->size);
	if (add_size <= 0)
		return (chunk);
	if (GET_STATUS(chunk->size) & S_LARGE ||
			(GET_STATUS(chunk->size) & S_SMALL && !IS_SMALL(size)) ||
			(GET_STATUS(chunk->size) & S_TINY && !IS_TINY(size)))
		return (NULL);
	free_part = NEXT_CHUNK(chunk);
	if (ULONG_INT(free_part) > limit || !(free_part->size & S_FREE)
		|| free_part->size < (uint64_t)add_size)
		return (NULL);
	tmp = *free_part;
	tmp.size -= add_size;
	tmp.prev_size = size | GET_STATUS(chunk->size);
	ft_memcpy((void *)chunk + size, &tmp, sizeof(tmp));
	if (tmp.previous)
		tmp.previous->next = (void *)chunk + size;
	else
		*bin = (void *)chunk + size;
	chunk->size = size | GET_STATUS(chunk->size);
	return (chunk);
}

void	*recycle_chunk(t_chunk **bin, size_t size)
{
	uint8_t	zone = IS_TINY(size) ? TINY : SMALL;
	t_chunk	*free_chunk, *ptr;

	if (!bin || !*bin)
		return (NULL);
	ptr = *bin;
	while (ptr->next && GET_SIZE(ptr->size) < size
			&& GET_SIZE(ptr->size) - size <= HEAD_SIZE) {
		ptr = ptr->next;
	}
	if (!ptr || GET_SIZE(ptr->size) < size) {
		return (NULL);
	}
	free_chunk = use_free_chunk(ptr, size, zone == TINY ? S_TINY : S_SMALL);
	if (!free_chunk || !free_chunk->previous)
		*bin = free_chunk;
	return (ptr);
}

void	*new_chunk(size_t size, t_chunk *next)
{
	t_chunk	*chunk;

	chunk = create_heap(size);
	if (chunk) {
		chunk->size &= ~S_FREE;
		chunk->size |= S_LARGE;
		chunk->next = next;
		if (next) {
			next->previous = chunk;
		}
	}
	return (chunk);
}

t_chunk	*split_chunk(t_chunk *chunk, size_t n)
{
	t_chunk	*next = (void *)(chunk + 1) + n - HEAD_SIZE;

	next->size = chunk->size - n;
	next->previous = chunk;
	next->next = chunk->next;
	chunk->next = next;
	next->prev_size = n | GET_STATUS(chunk->size);
	chunk->size = n | GET_STATUS(chunk->size);
	return (next);
}

void	free_chunk(t_chunk *chunk, t_chunk *next, uint64_t limit)
{
	chunk->size |= S_FREE;
	chunk->next = next;
	chunk->previous = 0x0;
	if (ULONG_INT(NEXT_CHUNK(chunk)) < limit)
		NEXT_CHUNK(chunk)->prev_size = chunk->size;
	if (chunk->next) {
		chunk->next->previous = chunk;
	}
}

void	delete_chunk(t_chunk *chunk, t_chunk **bin)
{
	if (chunk->next) {
		chunk->next->previous = chunk->previous;
		chunk->next->prev_size = chunk->prev_size;
	}
	if (chunk->previous)
		chunk->previous->next = chunk->next;
	else
		*bin = chunk->next;
	munmap(chunk, GET_SIZE(chunk->size));
}

void	defragment_zone(t_chunk *chunk, t_chunk **bin, uint64_t limit)
{
	t_chunk	*prev, *next, *front_merge;

	if (!chunk || !(chunk->size & S_FREE))
		return ;
	next = NEXT_CHUNK(chunk);
	front_merge = NULL;
	if (ULONG_INT(next) < limit && next->size & S_FREE) {
		chunk->size += GET_SIZE(next->size);
		if (next->next)
			next->next->previous = next->previous;
		if (next->previous)
			next->previous->next = next->next;
		if (ULONG_INT(NEXT_CHUNK(chunk)) < limit)
			NEXT_CHUNK(chunk)->prev_size = chunk->size;
	}
	if (chunk->prev_size & S_FREE) {
		prev = (void *)chunk - GET_SIZE(chunk->prev_size);
		front_merge = prev;
		front_merge->size += GET_SIZE(chunk->size);
		if (chunk->next)
			chunk->next->previous = chunk->previous;
		if (chunk->previous)
			chunk->previous->next = chunk->next;
		if (ULONG_INT(NEXT_CHUNK(front_merge)) < limit)
			NEXT_CHUNK(front_merge)->prev_size = front_merge->size;
	}
	if (!front_merge) {
		*bin = chunk;
	}
}
