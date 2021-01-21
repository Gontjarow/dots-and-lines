#include "main.h"

// Plans for multiple windows.
// Header defines enum: primary = 0, secondary = 1, tertiary = 2
SDL_Window *g_window[3];
SDL_Surface *g_surface[3];

signed	main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("SDL: %s\n", SDL_GetError());
		return 1;
	}

	g_window[primary] = new_window("Dots And Lines", WIN_WIDTH, WIN_HEIGHT);
	g_surface[primary] = SDL_GetWindowSurface(g_window[primary]);

	int running = true;
	while (running)
	{
		handle_events(&running);

		// Clear the buffer...
		clear_surface(g_surface[primary]);

		render_frame();

		SDL_UpdateWindowSurface(g_window[primary]);
		SDL_Delay(1000 / 60); // 60 FPS, but not dynamic.
	}

	printf("return (0)\n");
	return (0);
}

// ===================
// Program closes with the window or ESC
// ===================

void	render_frame()
{
	// Draw some static lines.
	t_xy p1 = {256, 200};
	t_xy p2 = {768, 400};
	draw_line(p1, p2, 0xFF0000, g_surface[primary]->pixels);

	t_xy p3 = {MID_WIDTH, 0};
	t_xy p4 = {MID_WIDTH, WIN_HEIGHT-1};
	draw_line(p3, p4, 0x00FF00, g_surface[primary]->pixels);

	// Prepare to draw more lines based on mouse position.
	int x;
	int y;
	SDL_GetMouseState(&x, &y);
	t_xy mouse = vec2(x, y);
	int color = 0xFFA000;

	// Normalized direction (length: 1) for line p1->p2
	t_xy v1 = vec2_norm(vec2_sub(p2, p1));
	// Closest point on the line defined by p1 and the direction
	t_xy m1 = vec2_point_to_line(mouse, p1, v1);
	draw_line(m1, mouse, color, g_surface[primary]->pixels);
	draw_box( m1, 3,     color, g_surface[primary]->pixels);

	// Same thing again for the second line. (Green)
	t_xy v2 = vec2_norm(vec2_sub(p3, p4));
	t_xy m2 = vec2_point_to_line(mouse, p3, v2);
	draw_line(m2, mouse, color, g_surface[primary]->pixels);
	draw_box( m2, 3,     color, g_surface[primary]->pixels);

	// Show some stats about the mouse position.
	double d1 = vec2_point_line_distance(mouse, p1, v1);
	double d2 = vec2_point_line_distance(mouse, p3, v2);
	int side1 = vec2_point_side(mouse, p1, p2);
	int side2 = vec2_point_side(mouse, p3, p4);
	double dot1 = vec2_projected_length(vec2_norm(vec2_sub(mouse, p1)), v1);
	double dot2 = vec2_projected_length(vec2_sub(mouse, p3), v2);

	// Clip green line:
	// The direction of the clipping plane defines which side is "inside"
	// Let's decide that "right" (1) is "inside"
	// The left side will be moved to the clipping plane.
	t_xy plane_start = vec2(MID_WIDTH, MID_HEIGHT);
	t_xy opposite_line = vec2_add(plane_start, vec2_mul(vec2_sub(mouse, plane_start), -1));
	draw_line(plane_start,   mouse, 0xccccff, g_surface[primary]->pixels);
	draw_line(opposite_line, mouse, 0xccccff, g_surface[primary]->pixels);

	t_xy c1, c2;
	vec2_clip(p1, p2, &c1, &c2, plane_start, mouse);
	draw_line(c1, c2, 0xFFFFFF, g_surface[primary]->pixels);

	// printf("mouse: %4.0f %-4.0f | R: (%2i) %-4.0f %-4.3f | G: (%2i) %-4.0f %-4.3f\n",
	// 	mouse.x, mouse.y, side1, d1, dot1, side2, d2, dot2);
}

void	handle_events(int *running)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN)
		{
			SDL_Scancode key = event.key.keysym.scancode;
			if (key == SDL_SCANCODE_ESCAPE)
			{
				printf("ESCAPE\n");
				*running = false;
				return;
			}
		}
		else if ((event.type == SDL_QUIT) || (event.type == SDL_WINDOWEVENT_CLOSE))
		{
			printf("SDL_QUIT\n");
			*running = false;
			return;
		}
	}
}

SDL_Window	*new_window(const char *title, int width, int height)
{
	return (SDL_CreateWindow(title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0));
}

void	clear_surface(SDL_Surface *surface)
{
	// https://wiki.libsdl.org/SDL_Surface
	SDL_LockSurface(surface);
	SDL_memset(surface->pixels, 0, surface->h * surface->pitch);
	SDL_UnlockSurface(surface);
}
