
#include "easy_draw.h"


static int8_t sg_ed_graphics_enabled;
static uint16_t sg_screen_width=0;
static uint16_t sg_screen_height = 0;
static uint8_t sg_ed_brush_size = 1;
static easy_draw_driver_t* sg_ed_driver = NULL;

//计算向量的长度
double easy_draw_vector_length(ed_vector_t v) 
{
	return sqrt(v.dx * v.dx + v.dy * v.dy);
}

// 将向量归一化为单位向量
ed_vector_t easy_draw_normalize_vector(ed_vector_t v)
{
	double length = easy_draw_vector_length(v);
	ed_vector_t result = 
	{
		.dx = v.dx / length,
		.dy = v.dy / length
	};
	return result;
}

void easy_helper_brush_vertical_line(int16_t x, int16_t y, uint16_t h, uint32_t color)
{
	if (ED_IS_DRIVER_NULL(sg_ed_driver, ed_pixel_draw))
		return;
	int16_t end = y + h;
	for (int a = y; a < end; a++)
	{
		if (x >= 0 && x < sg_screen_width && y >= 0 && y < sg_screen_height)
		{
			sg_ed_driver->ed_pixel_draw(x, y, color);
		}
	}
}

void easy_helper_brush_fill(int16_t x0, int16_t y0, uint16_t r, uint32_t color)
{
	easy_helper_brush_vertical_line(x0, y0 - r, 2 * r + 1, color);

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	int16_t delta = 0;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x;

		easy_helper_brush_vertical_line(x0 + x, y0 - y, 2 * y + 1 + delta, color);
		easy_helper_brush_vertical_line(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		easy_helper_brush_vertical_line(x0 - x, y0 - y, 2 * y + 1 + delta, color);
		easy_helper_brush_vertical_line(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		
	}

}



static void easy_draw_brush(int16_t x, int16_t y, uint32_t color)
{

#if ED_DRAW_BRUSH_STYLE==0

	int8_t brush_half_size_s = 0;
	int8_t brush_half_size_e = 0;
	if (sg_ed_brush_size % 2 == 0)
	{
		 brush_half_size_s = sg_ed_brush_size / 2-1;
		 brush_half_size_e = sg_ed_brush_size / 2;
	}
	else
	{
		brush_half_size_s = sg_ed_brush_size / 2;
		brush_half_size_e = sg_ed_brush_size / 2;
	}


	for (int i = -brush_half_size_s; i <= brush_half_size_e; i++)
	{
		for (int j = -brush_half_size_s; j <= brush_half_size_e; j++)
		{
			int16_t xi = x + i;
			int16_t yj = y + j;
			if (xi >= 0 && xi < sg_screen_width && yj >= 0 && yj < sg_screen_height)
			{
				sg_ed_driver->ed_pixel_draw(xi, yj, color);
			}
		}
	}
#elif ED_DRAW_BRUSH_STYLE==1

	easy_helper_brush_fill(x, y, sg_ed_brush_size, color);

#endif


}


// 将向量旋转给定角度
static ed_vector_t easy_helper_rotate_vector(ed_vector_t v, uint16_t angle_degrees)
{
	// 将角度转换为弧度
	double angle_radians = angle_degrees * ED_PI / 180.0;

	// 应用旋转矩阵
	ed_vector_t result = 
	{
		.dx = v.dx * cos(angle_radians) - v.dy * sin(angle_radians),
		.dy = v.dx * sin(angle_radians) + v.dy * cos(angle_radians)
	};

	return result;
}


// 将点围绕中心点旋转一定角度，并返回新的坐标
ed_point_t easy_helper_rotate_point(ed_point_t point, ed_point_t center, uint16_t angle_degrees)
{
	// 计算方向向量
	ed_vector_t direction = 
	{
		.dx = point.x - center.x,
		.dy = point.y - center.y
	};

	//double length = easy_draw_vector_length(direction);

	//direction = easy_draw_normalize_vector(direction);

	// 将方向向量旋转给定角度
	ed_vector_t rotated_direction = easy_helper_rotate_vector(direction, angle_degrees);

	// 将旋转后的方向向量与中心点相加，得到新的坐标点
	//ed_point_t result =
	//{
	//	.x = center.x + rotated_direction.dx* length,
	//	.y = center.y + rotated_direction.dy* length
	//};

	ed_point_t result =
	{
		.x = center.x + rotated_direction.dx ,
		.y = center.y + rotated_direction.dy 
	};

	return result;
}

//笔刷建议使用奇数
void easy_draw_set_brush_size(uint8_t size)
{
	sg_ed_brush_size = size;
}


void easy_draw_pixel(int16_t x, int16_t y, uint32_t color)
{
	if ( ED_IS_DRIVER_NULL(sg_ed_driver, ed_pixel_draw) )
		return;

	if (sg_ed_brush_size > 1)
	{
		easy_draw_brush(x, y, color);
	}
	else
	{
		if (x >= 0 && x < sg_screen_width && y >= 0 && y < sg_screen_height)
		{
			sg_ed_driver->ed_pixel_draw(x, y, color);
		}
	}
}


uint32_t easy_get_pixel(int16_t x, int16_t y)
{
	//if (ED_IS_DRIVER_NULL(sg_ed_driver, ed_pixel_get))
	//	return;
	//return sg_ed_driver->ed_pixel_get(x, y);
}


void easy_draw_horizon_line(int16_t x, int16_t y, uint16_t w, uint32_t color)
{
	int16_t end = x + w;
	for (int a = x; a < end; a++)
	{
		easy_draw_pixel(a, y, color);
	}
}



void easy_draw_vertical_line(int16_t x,int16_t y, uint16_t h,uint32_t color)
{
	int16_t end = y + h;
	for (int a = y; a < end; a++)
	{
		easy_draw_pixel(x, a, color);
	}
}


void easy_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color) 
{
	//if (ED_IS_DRIVER_NULL(sg_ed_driver, ed_line_draw) == false)
	//{
	//	sg_ed_driver->ed_line_draw(x0, y0, x1, y1, color);
	//	return;
	//}

	int16_t dx = abs(x1 - x0);
	int16_t dy = abs(y1 - y0);
	int16_t sx = x0 < x1 ? 1 : -1;
	int16_t sy = y0 < y1 ? 1 : -1;
	int16_t err = dx - dy;

	while (x0 != x1 || y0 != y1) 
	{
		float intensity;
		//if (dx == 0 && dy == 0) 
		//{
		//	intensity = 1.0f;
		//}
		//else 
		//{
		int16_t tmp = sqrtf(dx * dx + dy * dy);
		if (tmp == 0)
		{
			intensity = 1.0f;
		}
		else
		{
			intensity = sqrtf((dx * dx + dy * dy) - (err * err)) / tmp;
		}
			
		/*}*/
		easy_draw_pixel(x0, y0, color);
		int16_t e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
	easy_draw_pixel(x0, y0, color);
}


void easy_draw_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint32_t color)
{
	//if (ED_IS_DRIVER_NULL(sg_ed_driver, ed_rect_draw) == false)
	//{
	//	sg_ed_driver->ed_rect_draw(x, y, width, height, color);
	//	return;
	//}

	easy_draw_line(x, y, x + width - 1, y, color);

	// 绘制左边线
	easy_draw_line(x, y, x, y + height - 1, color);

	// 绘制下边线
	easy_draw_line(x, y + height - 1, x + width - 1, y + height - 1, color);

	// 绘制右边线
	easy_draw_line(x + width - 1, y, x + width - 1, y + height - 1, color);
}

void easy_draw_fillRect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint32_t color)
{

	//if (ED_IS_DRIVER_NULL(sg_ed_driver, ed_rect_fill_draw) == false)
	//{
	//	sg_ed_driver->ed_rect_fill_draw(x, y, width, height, color);
	//}

	uint16_t i;
	for (i = 0; i < height; i++) {
		easy_draw_line(x, y + i, x + width - 1, y + i, color);
	}
}

void easy_draw_arc(int xc, int yc, int r, int start_angle, int end_angle, uint32_t color)
{
	//if (ED_IS_DRIVER_NULL(sg_ed_driver, ed_arc_draw) == false)
	//{
	//	sg_ed_driver->ed_arc_draw(xc, yc, r, start_angle, end_angle, color);
	//}

	int x, y, d;
	float radians, start_radians, end_radians;

	// 将角度转换为弧度
	start_radians = start_angle * 3.1415926f / 180.0f;
	end_radians = end_angle * 3.1415926f / 180.0f;

	// 初始化起始点的坐标
	x = r * cos(start_radians) + xc;
	y = r * sin(start_radians) + yc;

	// 绘制圆弧
	for (radians = start_radians; radians <= end_radians; radians += 0.01f) {
		// 计算下一个像素点的坐标
		x = r * cos(radians) + xc;
		y = r * sin(radians) + yc;

		// 绘制当前像素点
		easy_draw_pixel(x, y, color);
	}
}


void easy_draw_sector(int xc, int yc, int r, int start_angle, int end_angle, uint32_t color)
{

	int x, y, d;
	float radians, start_radians, end_radians;
	ed_point_t last_point = { 0 };

	// 将角度转换为弧度
	start_radians = start_angle * 3.1415926f / 180.0f;
	end_radians = end_angle * 3.1415926f / 180.0f;

	// 初始化起始点的坐标
	x = r * cos(start_radians) + xc;
	y = r * sin(start_radians) + yc;
	last_point.x = x;
	last_point.y = y;

	// 绘制圆弧
	for (radians = start_radians; radians <= end_radians; radians += 0.1f) {
		// 计算下一个像素点的坐标
		x = r * cos(radians) + xc;
		y = r * sin(radians) + yc;
		//
		easy_draw_line(last_point.x, last_point.y, x, y, color);
		last_point.x = x;
		last_point.y = y;
		// 绘制当前像素点
		//easy_draw_pixel(x, y, color);
	}
	easy_draw_line(last_point.x, last_point.y, r * cos(end_radians) + xc, r * sin(end_radians) + yc, color);

	// 绘制线段
	easy_draw_line(xc, yc, r * cos(start_radians) + xc, r * sin(start_radians) + yc, color);
	easy_draw_line(xc, yc, r * cos(end_radians) + xc, r * sin(end_radians) + yc, color);
}




//使用三角形填充
void easy_draw_fillSector(int16_t xc, int16_t yc, int16_t r, int16_t start_angle, int16_t end_angle, uint32_t color)
{

	int16_t  mid_x, mid_y,d, sx,sy,ex,ey;
	float radians, start_radians, end_radians;
	ed_point_t triangle_points[3] = { 0 }, last_point = {0};


	// 将角度转换为弧度
	start_radians = start_angle * ED_PI / 180.0f;
	end_radians = end_angle * ED_PI / 180.0f;

	// 初始化起始点的坐标
	//x = r * cos(start_radians) + xc;
	//y = r * sin(start_radians) + yc;

	sx= r * cos(start_radians) + xc;
	sy = r * sin(start_radians) + yc;

	ex = r * cos(end_radians) + xc;
	ey = r * sin(end_radians) + yc;

	triangle_points[0].x = xc;
	triangle_points[0].y = yc;
	triangle_points[1].x = sx;
	triangle_points[1].y = sy;
	triangle_points[2].x = ex;
	triangle_points[2].y = ey;

	easy_draw_fillTriangle(
	triangle_points[0].x, triangle_points[0].y,
	triangle_points[1].x, triangle_points[1].y,
	triangle_points[2].x, triangle_points[2].y,
	color
);
	//easy_draw_triangle2(triangle_points, color);



	//triangle_points[0].x = (triangle_points[1].x + triangle_points[2].x) / 2;
	//triangle_points[0].y = (triangle_points[1].y + triangle_points[2].y) / 2;

	// 绘制圆弧
	for (radians = start_radians; radians <= end_radians; radians += 0.1f) {
		// 计算下一个像素点的坐标

		triangle_points[2].x = r * cos(radians) + xc;
		triangle_points[2].y = r * sin(radians) + yc;

		easy_draw_line(triangle_points[1].x, triangle_points[1].y, triangle_points[2].x, triangle_points[2].y, color);
		
		// 绘制当前像素点
		//easy_draw_pixel(x, y, color);

		//easy_draw_triangle2(triangle_points, color);
		easy_draw_fillTriangle(
			triangle_points[0].x, triangle_points[0].y,
			triangle_points[1].x, triangle_points[1].y,
			triangle_points[2].x, triangle_points[2].y,
			color
		);
		triangle_points[1] = triangle_points[2];

	}

	triangle_points[2].x = ex;
	triangle_points[2].y = ey;
	easy_draw_line(triangle_points[1].x, triangle_points[1].y, triangle_points[2].x, triangle_points[2].y, color);
	easy_draw_fillTriangle(
		triangle_points[0].x, triangle_points[0].y,
		triangle_points[1].x, triangle_points[1].y,
		triangle_points[2].x, triangle_points[2].y,
		color
	);


	// 绘制线段
	easy_draw_line(xc, yc, sx, sy, color);
	easy_draw_line(xc, yc, ex,ey, color);
}


void easy_draw_circle(int16_t x0, int16_t y0, uint16_t r, uint32_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	easy_draw_pixel(x0, y0 + r, color);
	easy_draw_pixel(x0, y0 - r, color);
	easy_draw_pixel(x0 + r, y0, color);
	easy_draw_pixel(x0 - r, y0, color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x;

		easy_draw_pixel(x0 + x, y0 + y, color);
		easy_draw_pixel(x0 - x, y0 + y, color);
		easy_draw_pixel(x0 + x, y0 - y, color);
		easy_draw_pixel(x0 - x, y0 - y, color);
		easy_draw_pixel(x0 + y, y0 + x, color);
		easy_draw_pixel(x0 - y, y0 + x, color);
		easy_draw_pixel(x0 + y, y0 - x, color);
		easy_draw_pixel(x0 - y, y0 - x, color);
	}
}

void easy_helper_fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t sides, int16_t delta, uint8_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x;

		if (sides & 0x1) // right side
		{
			easy_draw_vertical_line(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			easy_draw_vertical_line(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}

		if (sides & 0x2) // left side
		{
			easy_draw_vertical_line(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			easy_draw_vertical_line(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}

void easy_helper_circle(int16_t x0, int16_t y0, uint32_t r, uint8_t corners, uint32_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x;

		if (corners & 0x4) // lower right
		{
			easy_draw_pixel(x0 + x, y0 + y, color);
			easy_draw_pixel(x0 + y, y0 + x, color);
		}
		if (corners & 0x2) // upper right
		{
			easy_draw_pixel(x0 + x, y0 - y, color);
			easy_draw_pixel(x0 + y, y0 - x, color);
		}
		if (corners & 0x8) // lower left
		{
			easy_draw_pixel(x0 - y, y0 + x, color);
			easy_draw_pixel(x0 - x, y0 + y, color);
		}
		if (corners & 0x1) // upper left
		{
			easy_draw_pixel(x0 - y, y0 - x, color);
			easy_draw_pixel(x0 - x, y0 - y, color);
		}
	}
}



void easy_draw_fillCircle(int16_t x0, int16_t y0, uint16_t r, uint32_t color)
{
	easy_draw_vertical_line(x0, y0 - r, 2 * r + 1, color);
	easy_helper_fillCircle(x0, y0, r, 3, 0, color);
}


void easy_draw_roundedrect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t r, uint32_t color)
{
	// smarter version
	easy_draw_horizon_line(x + r, y, w - 2 * r, color); // Top
	easy_draw_horizon_line(x + r, y + h - 1, w - 2 * r, color); // Bottom
	easy_draw_vertical_line(x, y + r, h - 2 * r, color); // Left
	easy_draw_vertical_line(x + w - 1, y + r, h - 2 * r, color); // Right
	// draw four corners
	easy_helper_circle(x + r, y + r, r, 1, color);
	easy_helper_circle(x + w - r - 1, y + r, r, 2, color);
	easy_helper_circle(x + w - r - 1, y + h - r - 1, r, 4, color);
	easy_helper_circle(x + r, y + h - r - 1, r, 8, color);
}

void easy_draw_fillRoundedRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t r, uint32_t color)
{

	easy_draw_fillRect(x + r, y, w - 2 * r, h, color);
	easy_helper_fillCircle(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	easy_helper_fillCircle(x + r, y + r, r, 2, h - 2 * r - 1, color);
}


void easy_draw_ellipse2(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint32_t color)
{
	int32_t a = abs(x1 - x0);
	int32_t b = abs(y1 - y0);	//get diameters
	int32_t b1 = b & 1;
	int64_t dx = 4 * (1 - a) * b * b;
	int64_t dy = 4 * (b1 + 1) * a * a;
	int64_t err = dx + dy + b1 * a * a;
	int64_t e2;

	if (x0 > x1) { x0 = x1; x1 += a; }
	if (y0 > y1) { y0 = y1; }
	y0 += (b + 1) / 2;
	y1 = y0 - b1;
	a *= 8 * a;
	b1 = 8 * b * b;

	do {
		easy_draw_pixel(x1, y0, color);
		easy_draw_pixel(x0, y0, color);
		easy_draw_pixel(x0, y1, color);
		easy_draw_pixel(x1, y1, color);
		e2 = 2 * err;
		if (e2 >= dx) {
			x0++;
			x1--;
			err += dx += b1;
		}
		if (e2 <= dy) {
			y0++;
			y1--;
			err += dy += a;
		}
	} while (x0 <= x1);

	while (y0 - y1 < b) 
	{
		easy_draw_pixel(x0 - 1, y0,color);
		easy_draw_pixel(x1 + 1, y0++, color);
		easy_draw_pixel(x0 - 1, y1, color);
		easy_draw_pixel(x1 + 1, y1--, color);
	}
}

void easy_draw_ellipse(int16_t xCenter, int16_t yCenter, int16_t Rx, int Ry,uint32_t color )
{
	int32_t Rx2 = Rx * Rx;
	int32_t Ry2 = Ry * Ry;
	int32_t twoRx2 = 2 * Rx2;
	int32_t twoRy2 = 2 * Ry2;
	int32_t p;
	int32_t x = 0;
	int32_t y = Ry;
	int32_t px = 0;
	int32_t py = twoRx2 * y;
	easy_draw_pixel(xCenter + x, yCenter + y, color);
	easy_draw_pixel(xCenter - x, yCenter + y, color);
	easy_draw_pixel(xCenter + x, yCenter - y, color);
	easy_draw_pixel(xCenter - x, yCenter - y, color);
	//Region?1
	p = (int32_t)(Ry2 - Rx2 * Ry + 0.25 * Rx2);
	while (px < py)
	{
		x++;
		px += twoRy2;
		if (p < 0)
			p += Ry2 + px;
		else
		{
			y--;
			py -= twoRx2;
			p += Ry2 + px - py;
		}
		easy_draw_pixel(xCenter + x, yCenter + y, color);
		easy_draw_pixel(xCenter - x, yCenter + y, color);
		easy_draw_pixel(xCenter + x, yCenter - y, color);
		easy_draw_pixel(xCenter - x, yCenter - y, color);
	}
	//Region?2
	p = (int32_t)(Ry2 * (x + 0.5) * (x + 0.5) + Rx2 * (y - 1) * (y - 1) - Rx2 * Ry2);
	while (y > 0)
	{
		y--;
		py -= twoRx2;
		if (p > 0)
			p += Rx2 - py;
		else
		{
			x++;
			px += twoRy2;
			p += Rx2 - py + px;
		}
		easy_draw_pixel(xCenter + x, yCenter + y, color);
		easy_draw_pixel(xCenter - x, yCenter + y, color);
		easy_draw_pixel(xCenter + x, yCenter - y, color);
		easy_draw_pixel(xCenter - x, yCenter - y, color);
	}
}

void easy_draw_fillEllipse(int16_t xCenter, int16_t yCenter, int16_t rx, int16_t ry,uint32_t color)
{
	int32_t x, y;
	int32_t xchg, ychg;
	int32_t err;
	int32_t rxrx2;
	int32_t ryry2;
	int32_t stopx, stopy;

	rxrx2 = rx;
	rxrx2 *= rx;
	rxrx2 *= 2;

	ryry2 = ry;
	ryry2 *= ry;
	ryry2 *= 2;

	x = rx;
	y = 0;

	xchg = 1;
	xchg -= rx;
	xchg -= rx;
	xchg *= ry;
	xchg *= ry;

	ychg = rx;
	ychg *= rx;

	err = 0;

	stopx = ryry2;
	stopx *= rx;
	stopy = 0;

	while (stopx >= stopy)
	{

		easy_draw_vertical_line(xCenter + x, yCenter - y, y + 1,color);
		easy_draw_vertical_line(xCenter - x, yCenter - y, y + 1, color);
		easy_draw_vertical_line(xCenter + x, yCenter, y + 1, color);
		easy_draw_vertical_line(xCenter - x, yCenter, y + 1, color);
		//draw_filled_ellipse_section(u8g, x, y, x0, y0, option);
		y++;
		stopy += rxrx2;
		err += ychg;
		ychg += rxrx2;
		if (2 * err + xchg > 0)
		{
			x--;
			stopx -= ryry2;
			err += xchg;
			xchg += ryry2;
		}
	}

	x = 0;
	y = ry;

	xchg = ry;
	xchg *= ry;

	ychg = 1;
	ychg -= ry;
	ychg -= ry;
	ychg *= rx;
	ychg *= rx;

	err = 0;

	stopx = 0;

	stopy = rxrx2;
	stopy *= ry;


	while (stopx <= stopy)
	{
		easy_draw_vertical_line(xCenter + x, yCenter - y, y + 1,color);
		easy_draw_vertical_line(xCenter - x, yCenter - y, y + 1, color);
		easy_draw_vertical_line(xCenter + x, yCenter, y + 1, color);
		easy_draw_vertical_line(xCenter - x, yCenter, y + 1, color);
		// u8g_draw_filled_ellipse_section(u8g, x, y, x0, y0, option);
		x++;
		stopx += ryry2;
		err += xchg;
		xchg += ryry2;
		if (2 * err + ychg > 0)
		{
			y--;
			stopy -= rxrx2;
			err += ychg;
			ychg += rxrx2;
		}
	}

}


// arduboy
void easy_draw_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color)
{

	int16_t a, b, y, last;
	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1)
	{
		ED_SWAP_INT(y0, y1); ED_SWAP_INT(x0, x1);
	}
	if (y1 > y2)
	{
		ED_SWAP_INT(y2, y1); ED_SWAP_INT(x2, x1);
	}
	if (y0 > y1)
	{
		ED_SWAP_INT(y0, y1); ED_SWAP_INT(x0, x1);
	}

	if (y0 == y2)
	{ // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)
		{
			a = x1;
		}
		else if (x1 > b)
		{
			b = x1;
		}
		if (x2 < a)
		{
			a = x2;
		}
		else if (x2 > b)
		{
			b = x2;
		}
		easy_draw_horizon_line(a, y0, b - a + 1, color);
		return;
	}

	int16_t dx01 = x1 - x0,
		dy01 = y1 - y0,
		dx02 = x2 - x0,
		dy02 = y2 - y0,
		dx12 = x2 - x1,
		dy12 = y2 - y1,
		sa = 0,
		sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2)
	{
		last = y1;   // Include y1 scanline
	}
	else
	{
		last = y1 - 1; // Skip it
	}


	for (y = y0; y <= last; y++)
	{
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;

		if (a > b)
		{
			ED_SWAP_INT(a, b);
		}

		easy_draw_horizon_line(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);

	for (; y <= y2; y++)
	{
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;

		if (a > b)
		{
			ED_SWAP_INT(a, b);
		}

		easy_draw_horizon_line(a, y, b - a + 1, color);
	}
}


void draw_fillTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint32_t color)
{
	int16_t i, y, ymax, xmax1, xmax2, temp;
	float m1, m2;
	if (y1 > y2) {
		temp = y1; y1 = y2; y2 = temp;
		temp = x1; x1 = x2; x2 = temp;
	}
	if (y1 > y3) {
		temp = y1; y1 = y3; y3 = temp;
		temp = x1; x1 = x3; x3 = temp;
	}
	if (y2 > y3) {
		temp = y2; y2 = y3; y3 = temp;
		temp = x2; x2 = x3; x3 = temp;
	}
	m1 = (float)(x2 - x1) / (y2 - y1);
	m2 = (float)(x3 - x1) / (y3 - y1);
	for (y = y1; y <= y3; y++) {
		if (y < y2) {
			xmax1 = x1 + (m1 * (y - y1));
			xmax2 = x1 + (m2 * (y - y1));
		}
		else {
			xmax1 = x2 + (m1 * (y - y2));
			xmax2 = x3 + (m2 * (y - y3));
		}
		if (xmax1 > xmax2) {
			temp = xmax1; xmax1 = xmax2; xmax2 = temp;
		}
		for (i = xmax1; i <= xmax2; i++) {
			easy_draw_pixel(i, y, color);
		}
	}

}


void easy_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color)
{
	easy_draw_line(x0, y0, x1, y1, color);
	easy_draw_line(x1, y1, x2, y2, color);
	easy_draw_line(x2, y2, x0, y0, color);
}

void easy_draw_triangle2(ed_point_t* points, uint32_t color)
{
	easy_draw_line(points[0].x, points[0].y, points[1].x, points[1].y, color);
	easy_draw_line(points[1].x, points[1].y, points[2].x, points[2].y, color);
	easy_draw_line(points[2].x, points[2].y, points[0].x, points[0].y, color);
}



/* 判断两条线段是否相交 */
int easy_helper_is_intersect(ed_point_t a1, ed_point_t a2, ed_point_t b1, ed_point_t b2) 
{
	int c1 = (a2.x - a1.x) * (b1.y - a1.y) - (b1.x - a1.x) * (a2.y - a1.y);
	int c2 = (a2.x - a1.x) * (b2.y - a1.y) - (b2.x - a1.x) * (a2.y - a1.y);
	int c3 = (b2.x - b1.x) * (a1.y - b1.y) - (a1.x - b1.x) * (b2.y - b1.y);
	int c4 = (b2.x - b1.x) * (a2.y - b1.y) - (a2.x - b1.x) * (b2.y - b1.y);
	return (c1 * c2 < 0 && c3 * c4 < 0);
}

int easy_helper_is_convex_polygon(ed_point_t* points, int num_points) 
{
	int i, j, k;
	int flag = 0;
	for (i = 0; i < num_points; i++) 
	{
		j = (i + 1) % num_points;
		k = (j + 1) % num_points;
		int cross = (points[j].x - points[i].x) * (points[k].y - points[j].y) - (points[j].y - points[i].y) * (points[k].x - points[j].x);
		if (i == 0) 
		{
			flag = cross > 0;
		}
		else if (flag != (cross > 0))
		{
			return 0;  /* 存在拐向内部的情况，说明不是凸多边形 */
		}
	}
	return 1;  /* 不存在拐向内部的情况，说明是凸多边形 */
}


/* 判断坐标点数组是否能够组成一个多边形 */
int easy_helper_is_polygon(ed_point_t* points, int num_points)
{
	int i, j;
	if (num_points < 3)
		return 0;  /* 至少需要3个点才能组成多边形 */
	for (i = 0; i < num_points; i++) {
		for (j = i + 1; j < num_points; j++) 
		{
			/* 判断每条边是否和其他边相交 */
			if (easy_helper_is_intersect(points[i], points[(i + 1) % num_points], points[j], points[(j + 1) % num_points])) 
			{
				return 0;  /* 存在相交的情况，说明无法组成多边形 */
			}
		}
	}
	return 1;  /* 不存在相交的情况，说明可以组成多边形 */
}

void easy_draw_polygon(ed_point_t * points, uint16_t num_points,uint32_t color)
{
	int i;
	if (!easy_helper_is_polygon(points, num_points)) 
		return;
	for (i = 0; i < num_points; i++) 
	{
		easy_draw_line(points[i].x, points[i].y, points[(i + 1) % num_points].x, points[(i + 1) % num_points].y, color);
	}
}


ed_polygonRect_helper_t easy_helper_getPolygonRect(uint16_t num_points, ed_point_t tar_polygon[])
{


	ed_polygonRect_helper_t boxRect;

	boxRect.minX = 50000;
	boxRect.minY = 50000;
	boxRect.maxX = -50000;
	boxRect.maxY = -50000;

	for (int i = 0; i < num_points; i++) {


		if (tar_polygon[i].x < boxRect.minX) boxRect.minX = tar_polygon[i].x;
		if (tar_polygon[i].y < boxRect.minY) boxRect.minY = tar_polygon[i].y;
		if (tar_polygon[i].x > boxRect.maxX) boxRect.maxX = tar_polygon[i].x;
		if (tar_polygon[i].y > boxRect.maxY) boxRect.maxY = tar_polygon[i].y;
	}
	return boxRect;
}



bool easy_helper_inPolygon(uint16_t num_points, ed_point_t tar_polygon[], ed_point_t testPoint)
{

	if (num_points < 3)
		return false;

	bool  inSide = false;
	float lineSlope, interSectX;
	int   i = 0, j = num_points - 1;

	for (i = 0; i < num_points; i++) {


		if ((tar_polygon[i].y < testPoint.y && tar_polygon[j].y >= testPoint.y ||
			tar_polygon[j].y < testPoint.y && tar_polygon[i].y >= testPoint.y) &&
			(tar_polygon[i].x <= testPoint.x || tar_polygon[j].x <= testPoint.x)) {


			if (tar_polygon[j].x != tar_polygon[i].x) {


				lineSlope = (float)(tar_polygon[j].y - tar_polygon[i].y) / (tar_polygon[j].x - tar_polygon[i].x);
				interSectX = (int)(tar_polygon[i].x + (testPoint.y - tar_polygon[i].y) / lineSlope);
			}
			else
				interSectX = tar_polygon[i].x;
			if (interSectX < testPoint.x)
				inSide = !inSide;
		}
		j = i;
	}

	return inSide;
}


//逐点法填充 //https://www.codetd.com/article/11761841
void easy_draw_fillPolygon(ed_point_t* points, uint16_t num_points, uint32_t color)
{

	ed_polygonRect_helper_t polyRect;

	polyRect = easy_helper_getPolygonRect(num_points, points);

	ed_point_t testPoint;
	//从最小点到最大点一次判断，如果在该多边形内部，则进行填充
	for (testPoint.x = polyRect.minX; testPoint.x < polyRect.maxX; testPoint.x++)
		for (testPoint.y = polyRect.minY; testPoint.y < polyRect.maxY; testPoint.y++) {


			if (easy_helper_inPolygon(num_points, points, testPoint))
				easy_draw_pixel(testPoint.x, testPoint.y, color);

		}
}


int easy_draw_init(easy_draw_driver_t* easy_draw_driver,uint16_t width, uint16_t height)
{

	if (ED_IS_NULL(easy_draw_driver))
		return ED_ERROR;

	if (ED_IS_MEMBER_NULL(easy_draw_driver, ed_init))
		return ED_ERROR;

	sg_screen_width = width;
	sg_screen_height = height;

	sg_ed_driver = easy_draw_driver;

	sg_ed_driver->ed_init();
		
	return ED_SUCCESS;
}



//水平扫描  2值化图片
void easy_draw_2bit_pic(int16_t x, int16_t y, const uint8_t* picdata, uint8_t w, uint8_t h,uint8_t isOverlay)
{
	uint8_t bit_color = 0;
	int16_t xi, yi, byteWidth = (w + 7) / 8;
	for (yi = 0; yi < h; yi++)
	{
		for (xi = 0; xi < w; xi++)
		{
			bit_color = *(picdata + yi * byteWidth + xi / 8) & (128 >> (xi & 7));
			if (isOverlay)
			{
				easy_draw_pixel(x + xi, y + yi, bit_color ? 1:0);
			}
			else if(bit_color)
			{
				easy_draw_pixel(x + xi, y + yi, 1);
			}
		}
	}
}

//水平扫描  4阶灰度图片
void easy_draw_4bit_pic(int16_t x, int16_t y, const uint8_t* picdata, uint8_t w, uint8_t h, uint8_t color)
{
	uint8_t bit_color = 0;
	int16_t xi, yi, byteWidth = (w + 3) / 4;
	for (yi = 0; yi < h; yi++)
	{
		for (xi = 0; xi < w; xi++)
		{
			bit_color = *(picdata + yi * byteWidth + xi / 4) & (192 >> ((xi & 3)*2) );
			bit_color = (bit_color << ((xi & 3) * 2) >>4  );
			easy_draw_pixel(x + xi, y + yi, bit_color  );

		}
	}
}

#if ED_TEXT_DRAW_FUNC_ENABLE
ed_text_format_config_t ed_text_config = { 1,0,1 };

const  unsigned char  font5x7[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
	0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
	0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
	0x18, 0x3C, 0x7E, 0x3C, 0x18,
	0x1C, 0x57, 0x7D, 0x57, 0x1C,
	0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
	0x00, 0x18, 0x3C, 0x18, 0x00,
	0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
	0x00, 0x18, 0x24, 0x18, 0x00,
	0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
	0x30, 0x48, 0x3A, 0x06, 0x0E,
	0x26, 0x29, 0x79, 0x29, 0x26,
	0x40, 0x7F, 0x05, 0x05, 0x07,
	0x40, 0x7F, 0x05, 0x25, 0x3F,
	0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
	0x7F, 0x3E, 0x1C, 0x1C, 0x08,
	0x08, 0x1C, 0x1C, 0x3E, 0x7F,
	0x14, 0x22, 0x7F, 0x22, 0x14,
	0x5F, 0x5F, 0x00, 0x5F, 0x5F,
	0x06, 0x09, 0x7F, 0x01, 0x7F,
	0x00, 0x66, 0x89, 0x95, 0x6A,
	0x60, 0x60, 0x60, 0x60, 0x60,
	0x94, 0xA2, 0xFF, 0xA2, 0x94,
	0x08, 0x04, 0x7E, 0x04, 0x08,
	0x10, 0x20, 0x7E, 0x20, 0x10,
	0x08, 0x08, 0x2A, 0x1C, 0x08,
	0x08, 0x1C, 0x2A, 0x08, 0x08,
	0x1E, 0x10, 0x10, 0x10, 0x10,
	0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
	0x30, 0x38, 0x3E, 0x38, 0x30,
	0x06, 0x0E, 0x3E, 0x0E, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x5F, 0x00, 0x00,
	0x00, 0x07, 0x00, 0x07, 0x00,
	0x14, 0x7F, 0x14, 0x7F, 0x14,
	0x24, 0x2A, 0x7F, 0x2A, 0x12,
	0x23, 0x13, 0x08, 0x64, 0x62,
	0x36, 0x49, 0x56, 0x20, 0x50,
	0x00, 0x08, 0x07, 0x03, 0x00,
	0x00, 0x1C, 0x22, 0x41, 0x00,
	0x00, 0x41, 0x22, 0x1C, 0x00,
	0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
	0x08, 0x08, 0x3E, 0x08, 0x08,
	0x00, 0x80, 0x70, 0x30, 0x00,
	0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x60, 0x60, 0x00,
	0x20, 0x10, 0x08, 0x04, 0x02,
	0x3E, 0x51, 0x49, 0x45, 0x3E,
	0x00, 0x42, 0x7F, 0x40, 0x00,
	0x72, 0x49, 0x49, 0x49, 0x46,
	0x21, 0x41, 0x49, 0x4D, 0x33,
	0x18, 0x14, 0x12, 0x7F, 0x10,
	0x27, 0x45, 0x45, 0x45, 0x39,
	0x3C, 0x4A, 0x49, 0x49, 0x31,
	0x41, 0x21, 0x11, 0x09, 0x07,
	0x36, 0x49, 0x49, 0x49, 0x36,
	0x46, 0x49, 0x49, 0x29, 0x1E,
	0x00, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x40, 0x34, 0x00, 0x00,
	0x00, 0x08, 0x14, 0x22, 0x41,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x00, 0x41, 0x22, 0x14, 0x08,
	0x02, 0x01, 0x59, 0x09, 0x06,
	0x3E, 0x41, 0x5D, 0x59, 0x4E,
	0x7C, 0x12, 0x11, 0x12, 0x7C,
	0x7F, 0x49, 0x49, 0x49, 0x36,
	0x3E, 0x41, 0x41, 0x41, 0x22,
	0x7F, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x49, 0x49, 0x49, 0x41,
	0x7F, 0x09, 0x09, 0x09, 0x01,
	0x3E, 0x41, 0x41, 0x51, 0x73,
	0x7F, 0x08, 0x08, 0x08, 0x7F,
	0x00, 0x41, 0x7F, 0x41, 0x00,
	0x20, 0x40, 0x41, 0x3F, 0x01,
	0x7F, 0x08, 0x14, 0x22, 0x41,
	0x7F, 0x40, 0x40, 0x40, 0x40,
	0x7F, 0x02, 0x1C, 0x02, 0x7F,
	0x7F, 0x04, 0x08, 0x10, 0x7F,
	0x3E, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x09, 0x09, 0x09, 0x06,
	0x3E, 0x41, 0x51, 0x21, 0x5E,
	0x7F, 0x09, 0x19, 0x29, 0x46,
	0x26, 0x49, 0x49, 0x49, 0x32,
	0x03, 0x01, 0x7F, 0x01, 0x03,
	0x3F, 0x40, 0x40, 0x40, 0x3F,
	0x1F, 0x20, 0x40, 0x20, 0x1F,
	0x3F, 0x40, 0x38, 0x40, 0x3F,
	0x63, 0x14, 0x08, 0x14, 0x63,
	0x03, 0x04, 0x78, 0x04, 0x03,
	0x61, 0x59, 0x49, 0x4D, 0x43,
	0x00, 0x7F, 0x41, 0x41, 0x41,
	0x02, 0x04, 0x08, 0x10, 0x20,
	0x00, 0x41, 0x41, 0x41, 0x7F,
	0x04, 0x02, 0x01, 0x02, 0x04,
	0x40, 0x40, 0x40, 0x40, 0x40,
	0x00, 0x03, 0x07, 0x08, 0x00,
	0x20, 0x54, 0x54, 0x78, 0x40,
	0x7F, 0x28, 0x44, 0x44, 0x38,
	0x38, 0x44, 0x44, 0x44, 0x28,
	0x38, 0x44, 0x44, 0x28, 0x7F,
	0x38, 0x54, 0x54, 0x54, 0x18,
	0x00, 0x08, 0x7E, 0x09, 0x02,
	0x18, 0xA4, 0xA4, 0x9C, 0x78,
	0x7F, 0x08, 0x04, 0x04, 0x78,
	0x00, 0x44, 0x7D, 0x40, 0x00,
	0x20, 0x40, 0x40, 0x3D, 0x00,
	0x7F, 0x10, 0x28, 0x44, 0x00,
	0x00, 0x41, 0x7F, 0x40, 0x00,
	0x7C, 0x04, 0x78, 0x04, 0x78,
	0x7C, 0x08, 0x04, 0x04, 0x78,
	0x38, 0x44, 0x44, 0x44, 0x38,
	0xFC, 0x18, 0x24, 0x24, 0x18,
	0x18, 0x24, 0x24, 0x18, 0xFC,
	0x7C, 0x08, 0x04, 0x04, 0x08,
	0x48, 0x54, 0x54, 0x54, 0x24,
	0x04, 0x04, 0x3F, 0x44, 0x24,
	0x3C, 0x40, 0x40, 0x20, 0x7C,
	0x1C, 0x20, 0x40, 0x20, 0x1C,
	0x3C, 0x40, 0x30, 0x40, 0x3C,
	0x44, 0x28, 0x10, 0x28, 0x44,
	0x4C, 0x90, 0x90, 0x90, 0x7C,
	0x44, 0x64, 0x54, 0x4C, 0x44,
	0x00, 0x08, 0x36, 0x41, 0x00,
	0x00, 0x00, 0x77, 0x00, 0x00,
	0x00, 0x41, 0x36, 0x08, 0x00,
	0x02, 0x01, 0x02, 0x04, 0x02,
	0x3C, 0x26, 0x23, 0x26, 0x3C,
	0x1E, 0xA1, 0xA1, 0x61, 0x12,
	0x3A, 0x40, 0x40, 0x20, 0x7A,
	0x38, 0x54, 0x54, 0x55, 0x59,
	0x21, 0x55, 0x55, 0x79, 0x41,
	0x21, 0x54, 0x54, 0x78, 0x41,
	0x21, 0x55, 0x54, 0x78, 0x40,
	0x20, 0x54, 0x55, 0x79, 0x40,
	0x0C, 0x1E, 0x52, 0x72, 0x12,
	0x39, 0x55, 0x55, 0x55, 0x59,
	0x39, 0x54, 0x54, 0x54, 0x59,
	0x39, 0x55, 0x54, 0x54, 0x58,
	0x00, 0x00, 0x45, 0x7C, 0x41,
	0x00, 0x02, 0x45, 0x7D, 0x42,
	0x00, 0x01, 0x45, 0x7C, 0x40,
	0xF0, 0x29, 0x24, 0x29, 0xF0,
	0xF0, 0x28, 0x25, 0x28, 0xF0,
	0x7C, 0x54, 0x55, 0x45, 0x00,
	0x20, 0x54, 0x54, 0x7C, 0x54,
	0x7C, 0x0A, 0x09, 0x7F, 0x49,
	0x32, 0x49, 0x49, 0x49, 0x32,
	0x32, 0x48, 0x48, 0x48, 0x32,
	0x32, 0x4A, 0x48, 0x48, 0x30,
	0x3A, 0x41, 0x41, 0x21, 0x7A,
	0x3A, 0x42, 0x40, 0x20, 0x78,
	0x00, 0x9D, 0xA0, 0xA0, 0x7D,
	0x39, 0x44, 0x44, 0x44, 0x39,
	0x3D, 0x40, 0x40, 0x40, 0x3D,
	0x3C, 0x24, 0xFF, 0x24, 0x24,
	0x48, 0x7E, 0x49, 0x43, 0x66,
	0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
	0xFF, 0x09, 0x29, 0xF6, 0x20,
	0xC0, 0x88, 0x7E, 0x09, 0x03,
	0x20, 0x54, 0x54, 0x79, 0x41,
	0x00, 0x00, 0x44, 0x7D, 0x41,
	0x30, 0x48, 0x48, 0x4A, 0x32,
	0x38, 0x40, 0x40, 0x22, 0x7A,
	0x00, 0x7A, 0x0A, 0x0A, 0x72,
	0x7D, 0x0D, 0x19, 0x31, 0x7D,
	0x26, 0x29, 0x29, 0x2F, 0x28,
	0x26, 0x29, 0x29, 0x29, 0x26,
	0x30, 0x48, 0x4D, 0x40, 0x20,
	0x38, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x38,
	0x2F, 0x10, 0xC8, 0xAC, 0xBA,
	0x2F, 0x10, 0x28, 0x34, 0xFA,
	0x00, 0x00, 0x7B, 0x00, 0x00,
	0x08, 0x14, 0x2A, 0x14, 0x22,
	0x22, 0x14, 0x2A, 0x14, 0x08,
	0x95, 0x00, 0x22, 0x00, 0x95,
	0xAA, 0x00, 0x55, 0x00, 0xAA,
	0xAA, 0x55, 0xAA, 0x55, 0xAA,
	0x00, 0x00, 0x00, 0xFF, 0x00,
	0x10, 0x10, 0x10, 0xFF, 0x00,
	0x14, 0x14, 0x14, 0xFF, 0x00,
	0x10, 0x10, 0xFF, 0x00, 0xFF,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x14, 0x14, 0x14, 0xFC, 0x00,
	0x14, 0x14, 0xF7, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x14, 0x14, 0xF4, 0x04, 0xFC,
	0x14, 0x14, 0x17, 0x10, 0x1F,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0x1F, 0x00,
	0x10, 0x10, 0x10, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0xF0, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0xFF, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x14,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x00, 0x1F, 0x10, 0x17,
	0x00, 0x00, 0xFC, 0x04, 0xF4,
	0x14, 0x14, 0x17, 0x10, 0x17,
	0x14, 0x14, 0xF4, 0x04, 0xF4,
	0x00, 0x00, 0xFF, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x14, 0x14, 0xF7, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x17, 0x14,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0xF4, 0x14,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x00, 0x00, 0x1F, 0x10, 0x1F,
	0x00, 0x00, 0x00, 0x1F, 0x14,
	0x00, 0x00, 0x00, 0xFC, 0x14,
	0x00, 0x00, 0xF0, 0x10, 0xF0,
	0x10, 0x10, 0xFF, 0x10, 0xFF,
	0x14, 0x14, 0x14, 0xFF, 0x14,
	0x10, 0x10, 0x10, 0x1F, 0x00,
	0x00, 0x00, 0x00, 0xF0, 0x10,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x38, 0x44, 0x44, 0x38, 0x44,
	0x7C, 0x2A, 0x2A, 0x3E, 0x14,
	0x7E, 0x02, 0x02, 0x06, 0x06,
	0x02, 0x7E, 0x02, 0x7E, 0x02,
	0x63, 0x55, 0x49, 0x41, 0x63,
	0x38, 0x44, 0x44, 0x3C, 0x04,
	0x40, 0x7E, 0x20, 0x1E, 0x20,
	0x06, 0x02, 0x7E, 0x02, 0x02,
	0x99, 0xA5, 0xE7, 0xA5, 0x99,
	0x1C, 0x2A, 0x49, 0x2A, 0x1C,
	0x4C, 0x72, 0x01, 0x72, 0x4C,
	0x30, 0x4A, 0x4D, 0x4D, 0x30,
	0x30, 0x48, 0x78, 0x48, 0x30,
	0xBC, 0x62, 0x5A, 0x46, 0x3D,
	0x3E, 0x49, 0x49, 0x49, 0x00,
	0x7E, 0x01, 0x01, 0x01, 0x7E,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x44, 0x44, 0x5F, 0x44, 0x44,
	0x40, 0x51, 0x4A, 0x44, 0x40,
	0x40, 0x44, 0x4A, 0x51, 0x40,
	0x00, 0x00, 0xFF, 0x01, 0x03,
	0xE0, 0x80, 0xFF, 0x00, 0x00,
	0x08, 0x08, 0x6B, 0x6B, 0x08,
	0x36, 0x12, 0x36, 0x24, 0x36,
	0x06, 0x0F, 0x09, 0x0F, 0x06,
	0x00, 0x00, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x00,
	0x30, 0x40, 0xFF, 0x01, 0x01,
	0x00, 0x1F, 0x01, 0x01, 0x1E,
	0x00, 0x19, 0x1D, 0x17, 0x12,
	0x00, 0x3C, 0x3C, 0x3C, 0x3C,
	0x00, 0x00, 0x00, 0x00, 0x00,
};


void easy_draw_set_text_format(uint8_t text_size,uint32_t bk_color,uint8_t bk_color_isValid)
{
	ed_text_config.size = text_size;
	ed_text_config.bk_color = bk_color;
	ed_text_config.bk_color_isValid = bk_color_isValid;
}

void easy_draw_text_char(int16_t x, int16_t y, uint8_t c,uint32_t color )
{
	int16_t i, j;
	uint8_t a, b;
	uint8_t size = ed_text_config.size;
	uint32_t text_color = color;
	uint32_t bk_color = ed_text_config.bk_color;
	uint32_t bk_color_isValid = ed_text_config.bk_color_isValid;

	for (i = 0; i < 6; i++)
	{
		int16_t line;
		if (i == 5)
		{
			line = 0x0;
		}
		else
		{
			line = *(font5x7 + (c * 5) + i);
		}

		for (j = 0; j < 8; j++)
		{
			uint8_t text_bit_val = (line & 0x1);

			for (a = 0; a < size; a++) 
			{
				for (b = 0; b < size; b++) 
				{
					if (text_bit_val)
					{
						easy_draw_pixel(x + (i * size) + a, y + (j * size) + b, text_color);
					}
					else 
					{
						if (bk_color_isValid)
							easy_draw_pixel(x + (i * size) + a, y + (j * size) + b, bk_color);
					}

				}
			}
			line >>= 1;
		}
	}
}

void easy_draw_text_string(int16_t x, int16_t y, char* str, uint32_t color)
{
	int16_t j = 0, tempx = x, tempy = y;
	uint8_t size = ed_text_config.size;
	while (str[j] != '\0')
	{
		if (str[j] == '\n')
		{
			tempy += 8 * size;
			tempx = x;
			j++;
			continue;
		}
		easy_draw_text_char(tempx, tempy, str[j], color);
		tempx += size * 6;
		j++;
	}
}

uint32_t easy_helper_pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--)
		result *= m;
	return result;
}

void easy_draw_text_num(int16_t x, int16_t y, uint32_t num, uint8_t len, uint32_t color)
{
	int16_t t, temp;
	int16_t enshow = 0;
	uint8_t size = ed_text_config.size;
	for (t = 0;t < len;t++)
	{
		temp = (num / easy_helper_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				easy_draw_text_char(x + (size * 6) * t, y, '0', color);
				continue;
			}
			else enshow = 1;
		}
		easy_draw_text_char(x + (size * 6) * t, y, temp + '0', color);
	}
}

void easy_draw_printf(int16_t x, int16_t y, uint32_t color,const char* fmt, ...)
{
	char str[ED_TEXT_PRINTF_BUFFER];
	va_list args;

	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);

	easy_draw_text_string(x,y, str,color);
}

#endif




