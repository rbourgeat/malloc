/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_base.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 15:21:09 by rbourgea          #+#    #+#             */
/*   Updated: 2024/02/14 16:00:17 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void	ft_putnbr_base(int64_t n, char *base)
{
	if (n == 0)
		write(STDOUT_FILENO, "0", 1);
	else
	{
		if (n < 0)
		{
			write(STDOUT_FILENO, "-", 1);
			n *= -1;
		}
		if (n > 0)
		{
			if (n / ft_strlen(base) != 0)
				ft_putnbr_base(n / ft_strlen(base), base);
			write(STDOUT_FILENO, &base[n % ft_strlen(base)], 1);
		}
	}
}
