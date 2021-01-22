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
	signed	x, y;
	SDL_GetMouseState(&x, &y);

	// Line clipping:
	t_xy	mouse = vec2(x, y);
	t_xy	plane_center = vec2(MID_WIDTH, MID_HEIGHT);
	t_xy	opposite_pos = vec2_add(plane_center, vec2_sub(plane_center, mouse));
	t_line	line = vec2_line_xy(opposite_pos, mouse, 0x6666ff);

	// Set some points.
	t_xy A = vec2(          200,            200);
	t_xy B = vec2(WIN_WIDTH-200,            200);
	t_xy C = vec2(WIN_WIDTH-200, WIN_HEIGHT-200);
	t_xy D = vec2(          200, WIN_HEIGHT-200);

	// Create a bounding box from them.
	t_line *bounds = set_clip_bounds(A, B, C, D);

	// Clip the line to those bounds.
	t_line clipped = vec2_line(0,0, 0,0, line.color);
	clip_to_bounds(line, &clipped, bounds);
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
