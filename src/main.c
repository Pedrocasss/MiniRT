/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pedro <pedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 18:41:38 by pvital-m          #+#    #+#             */
/*   Updated: 2023/12/24 02:22:22 by pedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <center.h>

//parsing notes
bool	parse_data(char *file);

int	main(int ac, char **av)
{
	if (ac < 2 || ac > 2)
		write(2, "Invalid number arguments\n", 26);
	else if (!ft_strnstr(av[1], ".rt", ft_strlen(av[1])))
		err("Error: Invalid file format");
	else if (parse_data(av[1]))
	{
		err("Error");
		return (1);
	}
}
