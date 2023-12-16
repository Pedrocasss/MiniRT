#include <center.h>

t_light		light = {{3, 3, 2}, 1.0, 0x00FFFFFF};
t_sphere	sphere = {{0, 0, 0}, 5, {202, 113, 45, 0}};
t_vector	rayOrigin = {0, 0, 2};
t_vector	rayDirection = {0, 0, 1};

/*!SECTION

	orgin = camera position
	direction = camera direction
	(bx^2 + by^2 + bz^2)t^2 + 2(bxox + byoy + bzoz)t + (ox^2 + oy^2 + oz^2- r^2) = 0
	a = ray origin
	b = ray direction
	t = distance along ray
	r = radius of sphere
	lets get started


	(bx^2 + by^2 + bz^2)t^2 + 2(bxox + byoy + bzoz)t + (ox^2 + oy^2 + oz^2- r^2) = 0
	is a quadratic equation
	we can solve it using the quadratic formula
	(-b +- sqrt(b^2 - 4ac)) / 2a
	we can use the discriminant to determine if the ray intersects the sphere
	if the discriminant is greater than 0 then the ray intersects the sphere

	example:

	Ray Origin = (0, 0, 60)
	Ray Direction = (0, 1, 1)

	(1^2 + 1^2 + 1^2)t^2 + 2(0*0 + 1*0 + 1*60)t + (0^2 + 0^2 + 60^2 - 1^2) = 0
	
	Quadratic A = (1^2 + 1^2 + 1^2) = 3
	Quadratic B = 2(0*0 + 1*0 + 1*60) = 120
	Quadratic C = (0^2 + 0^2 + 60^2 - 1^2) = 3599

	Quadratic Discriminant = b^2 - 4ac
	(-120 +- sqrt(120^2 - 4(3)(3599))) / 2(3)
	(-120 +- sqrt(14400 - 43188)) / 6
	(-120 +- sqrt(-28788)) / 6
	(-120 +- 169.6i) / 6

	
	all this process is just to determine if the ray intersects the sphere
	if the discriminant is greater than 0 then the ray intersects the sphere
	if the discriminant is less than 0 then the ray does not intersect the sphere
*/

int	PerPixel(t_vector fragCoord)
{
	float	a;
	float	b;
	float	c;
	float	discriminant;

	__uint32_t r = (__uint32_t)(fragCoord.x * 255); // Scale to [0, 255]
	__uint32_t g = (__uint32_t)(fragCoord.y * 255); // Scale to [0, 255]
	

	rayDirection.x = fragCoord.x - sphere.origin.x;
	rayDirection.y = fragCoord.y - sphere.origin.y;
	rayDirection.z = fragCoord.z - sphere.origin.z;
	a = dot(rayDirection, rayDirection);
	b = 2 * dot(rayOrigin, rayDirection);
	c = dot(rayOrigin, rayOrigin) - 0.5 * 0.5;

	discriminant = b * b - 4 * a * c;
	if (discriminant >= 0)
	{
		float t[] = {(-b + sqrt(discriminant)) / (2 * a), (-b - sqrt(discriminant)) / (2 * a)};

		for (int i = 0; i < 2; i++)
		{
			t_vector hitpos = {rayOrigin.x + rayDirection.x * t[i], rayOrigin.y + rayDirection.y * t[i], rayOrigin.z + rayDirection.z * t[i]};
			
			// Creating the normal shading color normal calculation is done by subtracting the hit position from the sphere center 
			t_vector normal = {hitpos.x, hitpos.y, hitpos.z};

			normalized(&normal);
			
			// Scale the normalized values to the range [0, 255]
			int red = (int)(normal.x * 0.5 * 255);
			int green = (int)(normal.y * 0.5 * 255);
			int blue = (int)(normal.z * 0.5 * 255);
			
			// Ensure the color values are in the valid range [0, 255]
			red = fmin(255, fmax(0, red));
			green = fmin(255, fmax(0, green));
			blue = fmin(255, fmax(0, blue));


			float lh = Max(0, dot(normal, light.origin));

			//the sphere color is Mangenta
			return (create_trgb(0, sphere.color.r * lh, sphere.color.g * lh, sphere.color.b * lh));
		}
	}
	return (0x00333333);
}

void	Render(void)
{
	for (__uint32_t y = 0; y < HEIGHT; y++)
	{
		for (__uint32_t x = 0; x < WIDTH; x++)
		{
			t_vector	coords = {(float)x / (float)WIDTH, (float)y / (float)HEIGHT, -1};
			coords.x = coords.x * 2 - 1;
			coords.y = coords.y * 2 - 1;
			my_mlx_pixel_put(x, y, PerPixel(coords));
		}
	}
	ShowUi();
}

int	key_hook(int keycode, int *param)
{
	printf("%d\n", keycode);
	for(__uint32_t y = 0; y < HEIGHT; y++)
		for(__uint32_t x = 0; x < WIDTH; x++)
			my_mlx_pixel_put(x, y, 0x00000000);
	mlx_put_image_to_window(var()->mlx, var()->mlx_win, var()->img, 0, 0);
	if (keycode == 65307)
	{
		mlx_destroy_window(var()->mlx, var()->mlx_win);
		exit(0);
	}
	if (keycode == 65362)
		sphere.origin.y += 0.01;
	if (keycode == 65364)
		sphere.origin.y -= 0.01;
	if (keycode == 65363)
		sphere.origin.x -= 0.01;
	if (keycode == 65361)
		sphere.origin.x += 0.01;
	if (keycode == 65360)
		sphere.origin.z += 0.01;
	if (keycode == 65367)
		sphere.origin.z -= 0.01;
	Render();
}
int	main(void)
{
	t_sphere sp;

	if (!initialize_mlx())
		return (1);
	Render();
	mlx_put_image_to_window(var()->mlx, var()->mlx_win, var()->img, 0, 0);
	mlx_hook(var()->mlx_win, 2, 1L << 0, key_hook, &sp);
	mlx_loop(var()->mlx);
	return (0);
}