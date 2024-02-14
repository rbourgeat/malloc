/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   iter_heap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 15:21:17 by rbourgea          #+#    #+#             */
/*   Updated: 2024/02/14 16:00:20 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

static void	print_zone(t_chunk *addr, uint8_t zone)
{
	static char	*names[] = {"TINY", "SMALL", "LARGE"};

	PRINT(names[zone]);
	PRINT(" : 0x");
	ft_putnbr_base(ULONG_INT(addr), HEXA);
	PRINT("\n");
}

uint64_t	iter_heap_zone(void *top, void (*funct)(t_chunk *), uint8_t zone)
{
	uint64_t	zones_flag[] = {S_TINY, S_SMALL, S_LARGE};
	uint64_t	total = 0;
	t_chunk		*it = top;

	print_zone(top, zone);
	if (zone == TINY || zone == SMALL) {
		while (it && ULONG_INT(it) < ZONE_LIMIT(top, zone)
				&& it->size & zones_flag[zone]) {
			if (!(it->size & S_FREE)) {
				(*funct)(it);
				total += GET_SIZE(it->size) - HEAD_SIZE;
			}
			it = NEXT_CHUNK(it);
		}
	}
	else {
		while (it) {
			(*funct)(it);
			total += GET_SIZE(it->size) - HEAD_SIZE;
			it = it->next;
		}
	}
	return (total);
}
