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
	t_line	red = vec2_line(256, 200, 768, 400, 0xff0000);
	// draw_line2(red,   g_surface[primary]->pixels);

	t_line	green = vec2_line(MID_WIDTH, 0, MID_WIDTH, WIN_HEIGHT-1, 0x00ff00);
	draw_line2(green, g_surface[primary]->pixels);

	// Prepare to draw more lines based on mouse position.
	signed	x, y;
	SDL_GetMouseState(&x, &y);
	t_xy	mouse = vec2(x, y);

	t_xy	red_normal = vec2_norm(vec2_sub(red.stop, red.start));
	t_xy	m1 = vec2_point_to_line(mouse, red.start, red_normal);
	t_line	red_to_mouse = vec2_line_xy(m1, mouse, 0xff6666);
	draw_line2(red_to_mouse,            g_surface[primary]->pixels);
	draw_box(m1, 3, red_to_mouse.color, g_surface[primary]->pixels);

	t_xy	grn_normal = vec2_norm(vec2_sub(green.start, green.stop));
	t_xy	m2 = vec2_point_to_line(mouse, green.start, grn_normal);
	t_line	grn_to_mouse = vec2_line_xy(m2, mouse, 0x66ff66);
	draw_line2(grn_to_mouse,            g_surface[primary]->pixels);
	draw_box(m2, 3, grn_to_mouse.color, g_surface[primary]->pixels);

	// Show some stats about the mouse position.
	double	red_dist = vec2_point_line_distance(mouse, red.start, red_normal);
	double	grn_dist = vec2_point_line_distance(mouse, green.start, grn_normal);
	signed	red_side = vec2_point_side(mouse, red.start, red.stop);
	signed	grn_side = vec2_point_side(mouse, green.start, green.stop);
	double	dot1 = vec2_projected_length((vec2_sub(mouse, red.start)), red_normal);
	double	dot2 = vec2_projected_length((vec2_sub(mouse, green.start)), grn_normal);
	printf("mouse: %4.0f %-4.0f | R: (%2i) %-4.0f %-4.3f | G: (%2i) %-4.0f %-4.3f\n",
		mouse.x, mouse.y, red_side, red_dist, dot1, grn_side, grn_dist, dot2);

	// Line clipping:
	t_xy plane_center = vec2(MID_WIDTH, MID_HEIGHT);
	t_xy opposite_pos = vec2_add(plane_center, vec2_sub(plane_center, mouse));
	draw_line(mouse, opposite_pos, 0x6666ff, g_surface[primary]->pixels);

	t_line plane = vec2_line_xy(opposite_pos, mouse, 0);
	t_line clipped = vec2_line(0,0, 0,0, red.color);
	vec2_clip_line(red, &clipped, plane);
	draw_line2(clipped, g_surface[primary]->pixels);
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
