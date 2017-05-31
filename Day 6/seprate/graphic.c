void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

// 定义各种颜色的序号
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

void init_palette(void){	//初始化调色板需要配置的颜色
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黑 */
		0xff, 0x00, 0x00,	/*  1:亮红 */
		0x00, 0xff, 0x00,	/*  2:亮绿 */
		0xff, 0xff, 0x00,	/*  3:亮黄 */
		0x00, 0x00, 0xff,	/*  4:亮蓝 */
		0xff, 0x00, 0xff,	/*  5:亮紫 */
		0x00, 0xff, 0xff,	/*  6:浅亮蓝 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		0x84, 0x00, 0x00,	/*  9:暗红 */
		0x00, 0x84, 0x00,	/* 10:暗绿 */
		0x84, 0x84, 0x00,	/* 11:暗黄 */
		0x00, 0x00, 0x84,	/* 12:暗青 */
		0x84, 0x00, 0x84,	/* 13:暗紫 */
		0x00, 0x84, 0x84,	/* 14:浅暗蓝 */
		0x84, 0x84, 0x84	/* 15:暗灰 */
	};
	set_palette(0, 15, table_rgb);
	return;
}

void rectangle(unsigned char* vram, int xsize, unsigned char color, int x0, int y0, int x1, int y1){	//画矩形
	int x,y;
	for (x=x0; x <=x1; x++){
		for (y=y0; y<=y1; y++){
			vram[ y * xsize + x] = color;
		}
	}
}

void set_palette(int start, int end, unsigned char *rgb){	//调色板配置
	int i, eflags;
	eflags = io_load_eflags();
	io_cli();
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);
	return;
}

void init_screen(char *vram, int x, int y){	//初始化背景
	rectangle(vram, x, COL8_008484,  0,     0,      x -  1, y - 29);
	rectangle(vram, x, COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
	rectangle(vram, x, COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
	rectangle(vram, x, COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

	rectangle(vram, x, COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
	rectangle(vram, x, COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
	rectangle(vram, x, COL8_848484,  3,     y -  4, 59,     y -  4);
	rectangle(vram, x, COL8_848484, 59,     y - 23, 59,     y -  5);
	rectangle(vram, x, COL8_000000,  2,     y -  3, 59,     y -  3);
	rectangle(vram, x, COL8_000000, 60,     y - 24, 60,     y -  3);

	rectangle(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
	rectangle(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
	rectangle(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	rectangle(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);
	return;
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font){	//显示8x16的字体
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

void putfont8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s){
	extern char hankaku[4096];
	for(; *s != 0x00; s++){
		putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
}

void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize){
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}
