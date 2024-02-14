/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 15:58:56 by rbourgea          #+#    #+#             */
/*   Updated: 2024/02/14 16:07:26 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H

# include <sys/resource.h>
# include <sys/mman.h>
# include <sys/time.h>
# include <pthread.h>
# include <stdbool.h>
# include <signal.h>
# include <stddef.h>
# include <stdint.h>
# include <string.h>
# include <unistd.h>
# include <stdio.h>
# include <errno.h>

# define HEXA "0123456789ABCDEF"
# define DEC "0123456789"
# define PRINT(s) write(STDOUT_FILENO, s, ft_strlen(s))
# define ULONG_INT(ptr) (uint64_t)ptr

# define NZONES 3
# define NBINS 2

# define TINY 0
# define SMALL 1
# define LARGE 2

# define TINY_ZONE_SIZE (uint64_t)(getpagesize() * 25)
# define SMALL_ZONE_SIZE (uint64_t)(getpagesize() * 400)
# define ZONE_SIZE(z) (z == TINY ? TINY_ZONE_SIZE : SMALL_ZONE_SIZE)

# define MAX_TINY (uint32_t)(TINY_ZONE_SIZE / 100)
# define MAX_SMALL (uint32_t)(SMALL_ZONE_SIZE / 100)
# define MAX_ZONE(z) (z == TINY ? MAX_TINY : MAX_SMALL)

# define IS_TINY(size) (size <= MAX_TINY)
# define IS_SMALL(size) (size > MAX_TINY && size <= MAX_SMALL)
# define IS_lARGE(size) (size > MAX_SMALL)

# define ZONE_LIMIT(top, z) ((uint64_t)(top) + ZONE_SIZE(z) - HEAD_SIZE)
# define ALIGN_BITS(n) ((n) % 16 == 0 ? (n) : ((n) / 16 + 1) * 16)

# define S_TINY 0b1000
# define S_SMALL 0b0100
# define S_LARGE 0b0010
# define S_FREE 0b0001
# define GET_STATUS(n) (n & (S_LARGE | S_SMALL | S_TINY))
# define STATUS_ERROR(n) (GET_STATUS(n) != S_LARGE && GET_STATUS(n) != S_SMALL && GET_STATUS(n) != S_TINY)

# define GET_SIZE(n) (n & ~0b1111)
# define HEAD_SIZE 32
# define NEXT_CHUNK(chunk) ((t_chunk *)((void *)(chunk) + ((chunk)->size & ~0b1111)))

typedef struct	s_chunk
{
	size_t			prev_size;
	size_t			size;
	struct s_chunk	*previous;
	struct s_chunk	*next;
}				t_chunk;

typedef struct	s_heap
{
	t_chunk *zones[NZONES];
	t_chunk *bins[NBINS];
}				t_heap;

typedef struct	s_mutexes
{
	pthread_mutex_t malloc;
	pthread_mutex_t realloc;
}				t_mutexes;

void		*create_heap(size_t size);
void		*recycle_chunk(t_chunk **bins, size_t size);
void		*new_chunk(size_t size, t_chunk *next);
void		delete_chunk(t_chunk *chunk, t_chunk **bin);
void		free_chunk(t_chunk *chunk, t_chunk *next, uint64_t limit);
void		*extend_chunk(t_chunk *chunk, size_t size, t_chunk **bin, uint64_t limit);
void		defragment_zone(t_chunk *chunk, t_chunk **bin, uint64_t limit);
t_chunk		*split_chunk(t_chunk *chunk, size_t size);
void		print_block(t_chunk *block);
void		hexa_dump(t_chunk *chunk);
uint64_t	iter_heap_zone(void *top, void (*funct)(t_chunk *), uint8_t zone);
void		ft_putnbr_base(int64_t n, char *base);
size_t		ft_strlen(const char *str);
void		*ft_memcpy(void *dst, const void *src, size_t n);

void		free(void *ptr);
void		*malloc(size_t size);
void		*realloc(void *ptr, size_t size);
void		show_alloc_mem(void);

#endif
