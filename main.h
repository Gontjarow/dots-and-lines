#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIN_WIDTH 1024
#define WIN_HEIGHT 512
#define MID_WIDTH (WIN_WIDTH / 2)
#define MID_HEIGHT (WIN_HEIGHT / 2)

#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000

#define LINE_SIDES_CROSS -1
#define LINE_SIDES_EQUAL  1

#define LINE_SIDE_RIGHT   1
#define LINE_SIDE_IN      1

#define LINE_SIDE_LEFT   -1
#define LINE_SIDE_OUT    -1

#define LINE_SIDE_NONE    0

typedef struct s_xy
{
	union
	{
		struct {double	x, y;};
		struct {double	left, right;};
		struct {double	top, bottom;};
		struct {double	start, stop;};
	};
} t_xy;

typedef struct s_line
{
	union
	{
		struct {t_xy head, tail;};
		struct {t_xy begin, end;};
		struct {t_xy start, stop;};
	};
	int color;
} t_line;

enum { primary, secondary, tertiary };
enum { false, true };

SDL_Window	*new_window(const char *title, int width, int height);
void	clear_surface(SDL_Surface *surface);

void	handle_events(int *running);
void	render_frame();

void	put_pixel(int x, int y, int color, unsigned *surface);
void	draw_line(t_xy start, t_xy end, int color, unsigned *surface);
void	draw_line2(t_line line, unsigned *surface);
void	draw_box(t_xy center, int radius, int color, unsigned *surface);

void	vec2p(const char *name, t_xy v);
t_xy	vec2(double x, double y);
t_xy	vec2_add(t_xy a, t_xy b);
t_xy	vec2_sub(t_xy a, t_xy b);
t_xy	vec2_mul(t_xy v, double factor);
t_xy	vec2_div(t_xy v, double divisor);
double	vec2_dot(t_xy a, t_xy b);
double	vec2_mag(t_xy v);
t_xy	vec2_norm(t_xy v);
t_xy	vec2_rot(t_xy v, double angle);

double	vec2_projected_length(t_xy vec, t_xy normal);
double	vec2_point_line_distance(t_xy point, t_xy line_pos, t_xy line_dir);
t_xy	vec2_point_to_line(t_xy point, t_xy line_pos, t_xy line_dir);
signed	vec2_point_side(t_xy point, t_xy start, t_xy end);
double	vec2_project_to_hypotenuse(t_xy v, t_xy hypotenuse);

void	vec2_clip(t_xy p1, t_xy p2, t_xy *c1, t_xy *c2, t_xy plane_start, t_xy plane_end);
void	vec2_clip_line(t_line in, t_line *out, t_line plane);

t_line vec2_line(double ax, double ay, double bx, double by, int color);
t_line vec2_line_xy(t_xy start, t_xy stop, int color);


#endif