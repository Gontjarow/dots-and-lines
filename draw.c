#include "main.h"

void	put_pixel(int x, int y, int color, unsigned *surface)
{
	int index;

	index = (y * WIN_WIDTH + x);
	if (0 <= index && index < WIN_WIDTH * WIN_HEIGHT)
		surface[index] = color;
}

void	draw_box(t_xy center, int radius, int color, unsigned *surface)
{
	center.x = center.x - radius;
	center.y = center.y - radius;

	int size = radius * 2;
	for (int x = 0; x < size; ++x)
	for (int y = 0; y < size; ++y)
		put_pixel(center.x + x, center.y + y, color, surface);
}

void	draw_line(t_xy start, t_xy end, int color, unsigned *surface)
{
	t_xy	length;
	t_xy	ratio;
	t_xy	pos;
	signed	pixels;

	length = vec2(fabs(end.x - start.x), fabs(end.y - start.y));
	ratio.x = (start.y != end.y) ? (length.x / length.y) : 1;
	ratio.y = (start.x != end.x) ? (length.y / length.x) : 1;
	ratio.x = (ratio.x > ratio.y) ? 1 : ratio.x;
	ratio.y = (ratio.y > ratio.x) ? 1 : ratio.y;
	pos = start;
	pixels = (length.x > length.y) ? length.x : length.y;
	while (pixels-- > 0)
	{
		put_pixel(pos.x, pos.y, color, surface);
		pos.x += ratio.x * ((start.x < end.x) ? 1 : -1);
		pos.y += ratio.y * ((start.y < end.y) ? 1 : -1);
	}
}
