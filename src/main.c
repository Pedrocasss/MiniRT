/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pedro <pedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 18:41:38 by pvital-m          #+#    #+#             */
/*   Updated: 2024/01/28 20:36:40 by pedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <center.h>

gscene *scene = NULL;

void objectAdd(Object *nObj, Object **lst)
{
	if (!*lst)
	{
		*lst = nObj;
		return;
	}
	Object *tmp = *lst;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = nObj;
}

Vec4 getBackgroundColor(Ray raytrace)
{
	Vec4 color = (Vec4){0, 0, 0, 0};
	Vec3 unit_direction = unitVector(raytrace.d);
	double t = 0.5 * (unit_direction.y + 1.0);
	unsigned int white = 0xFFFFFF; // RGB white
	unsigned int blue = 0x7FB2FF;  // RGB blue
	color.r = (unsigned int)((1.0 - t) * ((white >> 16) & 0xFF) + t * ((blue >> 16) & 0xFF));
	color.g = (unsigned int)((1.0 - t) * ((white >> 8) & 0xFF) + t * ((blue >> 8) & 0xFF));
	color.b = (unsigned int)((1.0 - t) * (white & 0xFF) + t * (blue & 0xFF));
	return color;
}

Vec3 normalCalc(Object *obj, Vec3 p)
{
	Vec3 normal;
	normal = (Vec3){0, 0, 0};
	if (!obj)
		return normal;
	if (obj->type == SPHERE)
		normal = unitVector(Sub(p, ((Sphere *)obj)->o));
	else if (obj->type == PLANE)
		normal = ((Plane *)obj)->d;
	return normal;
}

Object *getClosestObject(Ray *rayTrace, double maxDistance, double minDistance)
{
	double ct = INFINITY;
	Object *closest = NULL;
	for (Object *lst = scene->objects; lst; lst = lst->next)
	{
		rayTrace->val = lst->colision(lst, *rayTrace);
		if ((rayTrace->val.t0 > minDistance && rayTrace->val.t0 < maxDistance) && rayTrace->val.t0 < ct)
		{
			closest = lst;
			ct = rayTrace->val.t0;
		}
		if ((rayTrace->val.t1 > minDistance && rayTrace->val.t1 < maxDistance) && rayTrace->val.t1 < ct)
		{
			closest = lst;
			ct = rayTrace->val.t1;
		}
	}
	rayTrace->normal = normalCalc(closest, Add(rayTrace->o, Mul(rayTrace->d, ct)));
	return closest;
}

Vec4 calculateLighting(Object *obj, Vec3 point, Vec3 normal) {
    Vec4 color = {0, 0, 0, 0};
    for (Light *light = scene->lights; light; light = (Light *)light->next) {
        Vec3 lightDirection = Sub(light->o, point);
        double distanceToLight = Length(lightDirection);
        Normalize(lightDirection);
        
        // Verificar se o ponto está na sombra
        Ray shadowRay = {point, lightDirection};
        Object *shadowObj = getClosestObject(&shadowRay, distanceToLight, 0.001);
        if (shadowObj) {
            continue;
        }
        
        // Calcular a cor do ponto
        double lightIntensity = Dot(normal, lightDirection);
        if (lightIntensity > 0) {
            color = Add4(color, Mul4(light->color, lightIntensity * light->intensity));
        }
    }
    return color;
}

Vec4 RayColor(Ray rayTrace)
{
    Vec4 CurrentColor = getBackgroundColor(rayTrace);
    Object *obj = getClosestObject(&rayTrace, INFINITY, 0);
    if (!obj)
        return CurrentColor;
    Vec4 objectColor = obj->color;
    Vec4 lightingColor = calculateLighting(obj, Add(rayTrace.o, Mul(rayTrace.d, rayTrace.val.t0)), rayTrace.normal);
    return Mul4(Add4(objectColor, lightingColor), 0.5);
}

void renderFrame()
{
	// Calculate teh vector across the horizontal and down the vertical viewport edges.
	// Calculate the horizontal and vertical delta vector form the pixel to pixel.
	// calculate the location of the upper left pixel.

	for (double y = scene->height / 2; y > -scene->height / 2; y--)
	{
		for (double x = -scene->width / 2; x < scene->width / 2; x++)
		{
			Ray ray = GetRayDir((scene->camera)->o, x, y);
			Vec4 color = RayColor(ray);
			my_mlx_pixel_put(toCanvas(x, false), toCanvas(y, true), color);
		}
	}
	mlx_put_image_to_window(scene->mlx->mlx, scene->mlx->win, scene->mlx->img, 0, 0);
	printf("\rDone.\n");
}

#ifdef __APPLE__
#define UP 126
#define DOWN 125
#define LEFT 123
#define RIGHT 124
#define ESC 53
#define W 13
#define A 0
#define S 1
#define D 2
#else
#define UP 65362  //126
#define DOWN 65364  //125
#define LEFT 65361  //123
#define RIGHT 65363 //124
#define ESC 65307   //53
#define W 119    //13
#define A 97     //0
#define S 115    //1 D // 2
#define D 100	
#endif

int keyhook(int keycode)
{
	printf("keycode: %d\n", keycode);
	if (keycode == UP)
	{
		scene->camera->o.z += 0.1;
		renderFrame();
	}
	if (keycode == DOWN)
	{
		scene->camera->o.z -= 0.1;
		renderFrame();
	}
	if (keycode == LEFT)
	{
		scene->camera->o.x -= 0.1;
		renderFrame();
	}
	if (keycode == RIGHT)
	{
		scene->camera->o.x += 0.1;
		renderFrame();
	}
	if (keycode == W)
	{
		scene->camera->o.y += 0.1;
		renderFrame();
	}
	if (keycode == S)
	{
		scene->camera->o.y -= 0.1;
		renderFrame();
	}
	if (keycode == ESC)
	{
		mlx_clear_window(scene->mlx->mlx, scene->mlx->win);
		mlx_destroy_window(scene->mlx->mlx, scene->mlx->win);
		mlx_destroy_image(scene->mlx->mlx, scene->mlx->img);
		mlx_destroy_display(scene->mlx->mlx);
		free(scene->mlx->mlx);
		free(scene->mlx);
		exit(0);
	}
	return 0;
}

int main(void)
{
	scene = malloc(sizeof(gscene));
	if (!scene)
		return 1;
	scene->width = 700;
	scene->height = 700;
	scene->camera = NULL;
	scene->objects = NULL;
	scene->lights = NULL;

	scene->mlx = malloc(sizeof(t_mlxdata));
	if (!scene->mlx)
		return 1;

	objectAdd(
        (Object *)newCamera(
            (Vec3){0, 0, -5},
            (Vec3){0, 0, 1},
            90,
            (Vec3){0, 0, 0}),
        (Object **)&scene->camera);
    objectAdd(
        (Object *)newLight(
        (Vec3){0, 10, 0}, // Posição da luz acima das esferas
        (Vec3){0, -1, 0}, // Direção da luz para baixo
        (Vec4){255, 255, 255, 255},
           (Vec3){0, 0, 0},
        0.6,
        POINT),
    (Object **)&scene->lights);
    objectAdd(
        (Object *)newSphere(
            (Vec3){-2, 0, 0},
            (Vec3){0, 0, 1},
            (Vec4){255, 0, 0, 255},
            (Vec3){0, 0, 0},
            1,
            sphereColision),
        (Object **)&scene->objects);
    objectAdd(
        (Object *)newSphere(
            (Vec3){0, 0, 0},
            (Vec3){0, 0, 1},
            (Vec4){0, 255, 0, 255},
            (Vec3){0, 0, 0},
            1,
            sphereColision),
        (Object **)&scene->objects);
    objectAdd(
        (Object *)newSphere(
            (Vec3){2, 0, 0},
            (Vec3){0, 0, 1},
            (Vec4){0, 0, 255, 255},
            (Vec3){0, 0, 0},
            1,
            sphereColision),
        (Object **)&scene->objects);
    objectAdd(
        (Object *)newPlane(
            (Vec3){0, -2, 0}, // Posição do plano abaixo das esferas
            (Vec3){0, 1, 0}, // Direção do plano para cima
            (Vec4){255, 255, 255, 255},
            (Vec3){0, 0, 0},
            1,
            planeColision),
        (Object **)&scene->objects);

	if (!scene->camera)
		return printf("No camera found\n"), 1;
	if (!initialize_mlx())
		return printf("Error initializing mlx\n"), 1;

	renderFrame();
	mlx_key_hook(scene->mlx->win, keyhook, scene->mlx);
	// mlx_loop_hook(scene->mlx->mlx, keyhook, NULL);
	mlx_loop(scene->mlx->mlx);
	return 0;
}