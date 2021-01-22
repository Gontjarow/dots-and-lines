#include "main.h"

// ONLY FOR DEBUGGING.
// NOT USED FOR IMPLEMENTATION.
extern SDL_Window *g_window[3];
extern SDL_Surface *g_surface[3];

void	vec2p(const char *name, t_xy v)
{
	printf("%s{%8.3f, %8.3f}\n", name, v.x, v.y);
}

t_xy	vec2(double x, double y)
{
	return ((t_xy){x, y});
}

t_xy	vec2_add(t_xy a, t_xy b)
{
	return (vec2(
		a.x + b.x,
		a.y + b.y
	));
}

t_xy	vec2_sub(t_xy a, t_xy b)
{
	return (vec2(
		a.x - b.x,
		a.y - b.y
	));
}

t_xy	vec2_mul(t_xy v, double factor)
{
	return (vec2(
		v.x * factor,
		v.y * factor
	));
}

t_xy	vec2_div(t_xy v, double divisor)
{
	return (vec2_mul(v, 1 / divisor));
}

double	vec2_dot(t_xy a, t_xy b)
{
	return ((a.x * b.x) + (a.y * b.y));
}

double	vec2_mag(t_xy v)
{
	return (sqrt(v.x * v.x + v.y * v.y));
}

t_xy	vec2_norm(t_xy v)
{
	return (vec2_div(v, vec2_mag(v)));
}

// https://en.wikipedia.org/wiki/Rotation_matrix
// [ cos  -sin ]
// [ sin   cos ]
// (Counter-clockwise)
t_xy	vec2_rot(t_xy v, double angle)
{
	double c = cos(angle);
	double s = sin(angle);

	return ((t_xy){
		v.x * c - v.y * s,
		v.x * s + v.y * c
	});
}

// 0 when perpendicular,
// 1 when parallel and both vectors are normal,
// otherwise the distance along the normal.
double	vec2_projected_length(t_xy v, t_xy normal)
{
	return (vec2_dot(v, normal));
}

// Absolute distance from line.
// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Vector_formulation
double	vec2_point_line_distance(t_xy point, t_xy line_pos, t_xy line_dir)
{
	t_xy	point_to_line;
	double	projected_length;
	t_xy	projected_offset;
	t_xy	perpendicular;

	point_to_line = vec2_sub(line_pos, point);                          //             (q - p)
	projected_length = vec2_projected_length(point_to_line, line_dir);  //             (q - p) ⋅ n
	projected_offset = vec2_mul(line_dir, projected_length);            //           (((q - p) ⋅ n) * n)
	perpendicular = vec2_sub(point_to_line, projected_offset);          // (q - p) - (((q - p) ⋅ n) * n)
	return (vec2_mag(perpendicular));
}

// Closest position on the line.
t_xy	vec2_point_to_line(t_xy point, t_xy line_pos, t_xy line_dir)
{
	t_xy	point_to_line;
	double	projected_length;
	t_xy	projected_offset;

	point_to_line = vec2_sub(line_pos, point);                          //            (q - p)
	projected_length = -vec2_projected_length(point_to_line, line_dir); //            (q - p) ⋅ n
	projected_offset = vec2_mul(line_dir, projected_length);            //           ((q - p) ⋅ n) * n
	return (vec2_add(line_pos, projected_offset));                      //       q + ((q - p) ⋅ n) * n
}

// 2D Cross Product; Determinant of a 2D matrix.
// Does not result in useful values, only the sign matters.
// [a   b]   [x2-x1   x3-x1]
// [  ╳  ] = [      ╳      ] =             a*d - b*c
// [c   d]   [y2-y1   y3-y1]   (x2-x1)*(y3-y1) - (x3-x1)*(y2-y1)
//  1 == right
//  0 == on the line
// -1 == left
signed	vec2_point_side(t_xy point, t_xy start, t_xy end)
{
	double ad_bc;

	ad_bc = (end.x - start.x) * (point.y - start.y) -
			(point.x - start.x) * (end.y - start.y);
	if (ad_bc > 0)
		return (LINE_SIDE_RIGHT);
	else if (ad_bc < 0)
		return (LINE_SIDE_LEFT);
	else
		return (LINE_SIDE_NONE);
}

// Length that v (adjescent) projects onto the hypotenuse
double	vec2_project_to_hypotenuse(t_xy v, t_xy hypotenuse)
{
	t_xy	v_normal;
	double	v_length;
	double	projected;

	v_normal = vec2_norm(v);
	v_length = vec2_mag(v);
	projected = vec2_projected_length(hypotenuse, v_normal);
	return (vec2_mag(hypotenuse) * (v_length / projected));
}

static t_xy	clip(t_xy p1, t_xy p2, t_xy line_pos, t_xy line_dir)
{
	t_xy	line;
	t_xy	on_clip_plane;
	t_xy	p1_to_plane;
	double	length;
	t_xy	offset;

	line = vec2_sub(p2, p1);
	on_clip_plane = vec2_point_to_line(p1, line_pos, line_dir);
	p1_to_plane = vec2_sub(on_clip_plane, p1);
	length = vec2_project_to_hypotenuse(p1_to_plane, line);
	offset = vec2_mul(vec2_norm(line), length);
	return (vec2_add(p1, offset));
}

// The direction of the clipping line defines which side is "inside"
// Let's decide that "right" (1) is "inside"
// The left side will be moved to the clipping plane.
void	vec2_clip_line(t_line in, t_line *out, t_line plane)
{
	vec2_clip(
		in.start, in.stop,
		&(out->start), &(out->stop),
		plane.start, plane.stop);
}

void	vec2_clip(t_xy p1, t_xy p2, t_xy *c1, t_xy *c2, t_xy start, t_xy end)
{
	t_xy	clipping_line_dir;
	signed	p1_side;
	signed	p2_side;

	*c1 = p1;
	*c2 = p2;
	clipping_line_dir = vec2_norm(vec2_sub(end, start));
	p1_side = vec2_point_side(p1, start, end);
	p2_side = vec2_point_side(p2, start, end);
	if (p1_side * p2_side == LINE_SIDES_CROSS)
	{
		if (p1_side < 0)
			*c1 = clip(p1, p2, start, clipping_line_dir);
		else
			*c2 = clip(p2, p1, start, clipping_line_dir);
	}
	else if ((p1_side == LINE_SIDE_OUT && p2_side == LINE_SIDE_OUT)
	|| (p1_side == LINE_SIDE_NONE && p2_side == LINE_SIDE_OUT)
	|| (p2_side == LINE_SIDE_NONE && p1_side == LINE_SIDE_OUT))
	{
		*c1 = vec2(0, 0);
		*c2 = vec2(0, 0);
	}
}

t_line	vec2_line(double ax, double ay, double bx, double by, int color)
{
	return ((t_line){(t_xy){ax, ay}, (t_xy){bx, by}, color});
}

t_line	vec2_line_xy(t_xy start, t_xy stop, int color)
{
	return ((t_line){start, stop, color});
}

t_line	*set_clip_bounds(t_xy top_l, t_xy top_r, t_xy bot_r, t_xy bot_l)
{
	static t_line bound[4] = {0};

	bound[0] = vec2_line_xy(top_l, top_r, 0xffffff);
	bound[1] = vec2_line_xy(top_r, bot_r, 0xffffff);
	bound[2] = vec2_line_xy(bot_r, bot_l, 0xffffff);
	bound[3] = vec2_line_xy(bot_l, top_l, 0xffffff);

	return (bound);
}

void	clip_to_bounds(t_line in, t_line *out, t_line bounds[4])
{
	vec2_clip_line(in,   out, bounds[0]);
	vec2_clip_line(*out, out, bounds[1]);
	vec2_clip_line(*out, out, bounds[2]);
	vec2_clip_line(*out, out, bounds[3]);
}
