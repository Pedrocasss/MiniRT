/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycolor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pedro <pedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 19:52:57 by psoares-          #+#    #+#             */
/*   Updated: 2024/02/15 08:26:16 by pedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <center.h>

t_ray	getraydir(t_vector o, double x, double y)
{
	t_cam	*cam;
	t_ray	ray;

	cam = g_scene->camera;
	ray.o = o;
	ray.d.x = ((x) / g_scene->width) * cam->width;
	ray.d.y = ((y) / g_scene->height) * cam->height;
	ray.d.z = 1;
	ray.o = cam->o;
	ray.val = (t_values){.t0 = INFINITY, .t1 = INFINITY};
	ray.objc = NULL;
	return (ray);
}

int	compcolor(int obj_color, t_vec4 objectColor)
{
	int	r;
	int	g;
	int	b;

	r = ((obj_color >> 16 & 255)) * objectColor.r;
	g = ((obj_color >> 8 & 255)) * objectColor.g;
	b = ((obj_color & 255)) * objectColor.b;
	return (newrgb(r, g, b));
}

static	int	calculreflectcolor(t_ray refray, int depth, double reflect, int lc)
{
	int	reflectedcolor;

	reflectedcolor = raycolor(refray, depth - 1);
	lc = newrgb((int)(mulcomp(lc, 16, 1 - reflect) + \
	mulcomp(reflectedcolor, 16, reflect)), (int)(mulcomp(lc, 8, 1 - reflect) + \
	mulcomp(reflectedcolor, 8, reflect)), (int)(mulcomp(lc, 0, 1 - reflect) + \
	mulcomp(reflectedcolor, 0, reflect)));
	return (lc);
}

static	int	calcullocalcolo(t_ray rayTrace, t_obj *obj)
{
	t_vec4	objectcolor;

	if (dot(rayTrace.d, rayTrace.normal) > 0)
		rayTrace.normal = mul(rayTrace.normal, -1);
	objectcolor = calcligh(rayTrace._hit, rayTrace.normal, \
	rayTrace.d, obj->specular);
	return (compcolor(obj->color, objectcolor));
}

int	raycolor(t_ray rayTrace, int depth)
{
	int			lc;
	t_obj		*obj;
	double		reflection;
	t_vector	reflected;
	t_ray		r_ray;

	obj = intersections(&rayTrace, INFINITY, 0, true);
	if (!obj)
		return (0);
	lc = calcullocalcolo(rayTrace, obj);
	if (obj->reflection <= 0 || obj->specular <= 0 || depth <= 0)
		return (lc);
	reflection = obj->reflection;
	reflected = reflect(rayTrace.d, rayTrace.normal);
	r_ray = (t_ray){add(rayTrace._hit, mul(reflected, 0.001)), reflected};
	return (calculreflectcolor(r_ray, depth, reflection, lc));
}
