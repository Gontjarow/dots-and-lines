#include "main.h"
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
	t_xy	point_to_line = vec2_sub(line_pos, point);                          //             (q - p)
	double	projected_length = vec2_projected_length(point_to_line, line_dir);  //             (q - p) ⋅ n
	t_xy	projected_offset = vec2_mul(line_dir, projected_length);            //           (((q - p) ⋅ n) * n)
	t_xy	perpendicular = vec2_sub(point_to_line, projected_offset);          // (q - p) - (((q - p) ⋅ n) * n)
	return (vec2_mag(perpendicular));
}

// Closest position on the line.
t_xy	vec2_point_to_line(t_xy point, t_xy line_pos, t_xy line_dir)
{
	t_xy	point_to_line = vec2_sub(line_pos, point);                          //            (q - p)
	double	projected_length = -vec2_projected_length(point_to_line, line_dir); //            (q - p) ⋅ n
	t_xy	projected_offset = vec2_mul(line_dir, projected_length);            //           ((q - p) ⋅ n) * n
	return (vec2_add(line_pos, projected_offset));                              //       q + ((q - p) ⋅ n) * n
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
	double d =	(end.x - start.x) * (point.y - start.y) -
				(point.x - start.x) * (end.y - start.y);
	if (d > 0)
		return (1);
	else if (d < 0)
		return (-1);
	else
		return (0);
}

// Length that v (adjescent) projects onto the hypotenuse
double	vec2_project_to_hypotenuse(t_xy v, t_xy hypotenuse)
{
	t_xy	v_normal = vec2_norm(v);
	double	projected = vec2_projected_length(hypotenuse, v_normal);
	double	v_length = vec2_mag(v);
	return (vec2_mag(hypotenuse) * (v_length / projected));
}

// aaaaaaaaaaaaaaaaaa
void	vec2_clip(t_xy p1, t_xy p2, t_xy *c1, t_xy *c2, t_xy plane_start, t_xy plane_end)
{
	*c1 = p1;
	*c2 = p2;
	t_xy clipping_plane = vec2_norm(vec2_sub(plane_end, plane_start));
	signed c1side = vec2_point_side(*c1, plane_start, plane_end);
	signed c2side = vec2_point_side(*c2, plane_start, plane_end);
	// printf("c1: %2i | c2: %2i |      %i\n", c1side, c2side, c1side * c2side);

	// -1 * -1 =  1
	//  1 *  1 =  1
	// -1 *  1 = -1
	if (c1side * c2side == LINE_SIDES_CROSS)
	{
		double d;
		if (c1side < 0)
		{
			*c1 = clip(*c1, *c2, plane_end, clipping_plane);
		}
		else
		{
			*c2 = clip(*c2, *c1, plane_end, clipping_plane);
		}
	}
	else if ((c1side == LINE_SIDE_OUT && c2side == LINE_SIDE_OUT)
	|| (c1side == LINE_SIDE_NONE && c2side == LINE_SIDE_OUT)
	|| (c2side == LINE_SIDE_NONE && c1side == LINE_SIDE_OUT))
	{
		*c1 = vec2(0,0);
		*c2 = vec2(0,0);
		// Line is outside or too short, ignore.
	}
	else
	{
		// Accept as-is
	}
}

t_xy clip(t_xy c1, t_xy c2, t_xy plane_end, t_xy clipping_plane)
{
	t_xy	red = vec2_sub(c2, c1);
	t_xy	red_normal = vec2_norm(red);
	t_xy	on_clip_plane = vec2_point_to_line(c1, plane_end, clipping_plane);
	t_xy	end_to_white = vec2_sub(on_clip_plane, c1);
	double	length = vec2_project_to_hypotenuse(end_to_white, red);
	t_xy	cyan_offset = vec2_mul(red_normal, length);
	return (vec2_add(c1, cyan_offset));
}

// t_xy	red = vec2_sub(c2, c1); // order not important
// t_xy	red_normal = vec2_norm(red);
// t_xy	on_clip_plane = vec2_point_to_line(c2, plane_end, clipping_plane);
// t_xy	end_to_white = vec2_sub(on_clip_plane, c2);
// double	length = vec2_project_to_hypotenuse(end_to_white, red);
// t_xy	cyan_offset = vec2_mul(red_normal, length);
// c2 = vec2_add(c2, cyan_offset);