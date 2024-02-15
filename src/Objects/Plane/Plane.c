/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Plane.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pedro <pedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 20:31:23 by pedro             #+#    #+#             */
/*   Updated: 2024/02/15 08:23:31 by pedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <center.h>

t_pl	*newplane(int type, char **props)
{
	t_vector	color;
	t_pl		*p;

	p = (t_pl *)newobject(sizeof(t_pl), \
		(t_values (*)(t_obj *, t_ray))planecolision);
	p->type = type;
	if (g_scene->error != 2)
		p->o = getvec4(props[1], true, INT16_MAX, -INT16_MAX);
	if (g_scene->error != 2)
		p->d = getvec4(props[2], true, 1, -1);
	if (g_scene->error != 2)
		color = getvec4(props[3], true, 255, 0);
	p->color = newrgb((int)color.x, (int)color.y, (int)color.z);
	if (props[4])
	{
		p->specular = getfloat(props[4], true, (float []){1000, 0}, 0);
		p->reflection = getfloat(props[5], true, (float []){1, 0}, 0);
		p->checkerboard = getfloat(props[6], true, (float []){1, 0}, 0);
	}
	p->next = NULL;
	return ((t_pl *)object_error_handler((t_obj *)p, \
		(void **)props, "-> Invalid plane\n"));
}
