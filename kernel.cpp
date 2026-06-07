#include <stddef.h>
#include <cstdint>
#include <stdint.h>

#define WIDTH 320
#define HEIGHT 200

#define TERMINAL_COLUMNS (WIDTH / 8 + 1)
#define TERMINAL_ROWS    (HEIGHT / 8)

int terminal_column = 1;
int terminal_row = 1;

// 15 - white
// 86 - pink :3 adsfijv ajjuygvhashyhrbasggyftsv
int color = 15;
int color2 = 15;

int total_apps = 3;

static unsigned char* framebuffer = (unsigned char*)0xa0000;

// coooooooooooooooooooooooooooooooooooooooooooooooooooooooooool functions

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

// not mine adgjkfdjgadga (both)
int strcmp(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return *a == *b;
}

int stoi(const char* str) {
    int result = 0;
    int sign = 1;

    // handle whitespace (optional but useful)
    while (*str == ' ' || *str == '\t')
        str++;

    // handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // convert digits
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}

// drawing: base
void put_pixel(int x, int y, unsigned char color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    framebuffer[y * WIDTH + x] = color;
}

void clear_screen(unsigned char color) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        framebuffer[i] = color;
    }
}

// drawing: shapes
void draw_hline(int x, int y, int len, unsigned char color) {
    for (int i = 0; i < len; i++)
        put_pixel(x + i, y, color);
}

void draw_vline(int x, int y, int len, unsigned char color) {
    for (int i = 0; i < len; i++)
        put_pixel(x, y + i, color);
}

void draw_rect(int x, int y, int w, int h, unsigned char color) {
    draw_hline(x, y, w, color);
    draw_hline(x, y + h - 1, w, color);
    draw_vline(x, y, h, color);
    draw_vline(x + w - 1, y, h, color);
}

void draw_rect_fill(int x, int y, int w, int h, unsigned char color) {
    for (int j = 0; j < h; j++)
        draw_hline(x, y + j, w, color);
}

void draw_circle(int xc, int yc, int r, unsigned char color) {
    int x = 0, y = r;
    int d = 3 - 2 * r;

    while (y >= x) {
        put_pixel(xc + x, yc + y, color);
        put_pixel(xc - x, yc + y, color);
        put_pixel(xc + x, yc - y, color);
        put_pixel(xc - x, yc - y, color);
        put_pixel(xc + y, yc + x, color);
        put_pixel(xc - y, yc + x, color);
        put_pixel(xc + y, yc - x, color);
        put_pixel(xc - y, yc - x, color);

        x++;

        if (d > 0) {
            y--;
            d += 4 * (x - y) + 10;
        } else {
            d += 4 * x + 6;
        }
    }
}

void draw_circle_fill(int xc, int yc, int r, unsigned char color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r)
                put_pixel(xc + x, yc + y, color);
        }
    }
}

// drawing bitmaps
struct Bitmap {
    int width;
    int height;
    const unsigned char* data;
};

void draw_bitmap(int x, int y, Bitmap bmp, int pix_size) {
    for (int j = 0; j < bmp.height; j++) {

        for (int i = 0; i < bmp.width; i++) {

            unsigned char color =
                bmp.data[j * bmp.width + i];

            // draw scaled pixel block
            for (int py = 0; py < pix_size; py++) {

                for (int px = 0; px < pix_size; px++) {

                    put_pixel(
                        x + (i * pix_size) + px,
                        y + (j * pix_size) + py,
                        color
                    );
                }
            }
        }
    }
}

// bitmaps data
// bg
unsigned char bg1_data[] = {
    12,5,12,12,5,12,12,5,12,12,5,12,7,12,5,12,7,12,12,7,12,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,7,12,7,7,12,
    12,7,5,12,7,5,7,12,7,5,7,12,5,12,7,12,5,12,7,5,12,7,12,5,12,7,12,12,7,12,12,7,12,12,7,12,12,14,12,7,12,14,12,14,12,7,12,14,13,14,12,7,12,15,12,7,7,13,12,7,13,7,13,7,
    5,12,12,7,12,12,5,12,12,12,12,5,7,12,5,12,7,12,12,7,12,5,7,12,7,12,7,12,12,7,12,7,12,7,12,15,12,13,14,12,13,12,15,12,13,14,13,12,12,7,12,15,12,7,12,7,12,7,7,12,7,12,15,12,
    12,7,5,12,5,12,7,5,7,5,7,12,12,7,12,7,12,5,7,12,7,12,12,7,12,12,13,12,7,12,13,12,14,13,12,12,14,12,7,12,14,12,12,7,12,12,14,7,12,15,12,12,7,12,7,13,14,13,12,15,12,15,12,15,
    12,5,12,12,7,5,12,12,12,12,5,12,5,12,5,12,7,12,12,5,12,7,12,12,7,12,7,12,7,12,14,12,7,12,14,12,7,12,14,13,12,15,12,14,13,14,12,13,14,12,7,7,12,15,12,7,12,7,7,13,7,13,7,12,
    12,7,5,12,5,12,7,5,7,12,7,12,7,12,7,12,5,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,12,15,12,14,13,12,14,12,7,12,7,12,7,12,14,13,12,7,12,15,12,7,13,7,12,15,12,14,13,14,13,
    5,12,12,7,12,5,12,12,5,12,5,12,7,12,5,12,7,12,12,7,12,12,13,12,7,12,12,12,7,12,13,14,12,15,12,12,12,14,13,12,14,12,15,12,14,13,14,13,12,7,12,7,12,7,12,7,12,15,12,13,7,15,13,14,
    12,7,5,12,5,7,12,7,12,7,12,7,5,12,7,12,12,7,12,12,7,12,7,12,12,7,13,14,12,14,12,12,7,12,14,13,14,12,14,13,12,7,12,12,7,12,12,7,7,12,15,12,7,13,7,13,7,13,7,15,12,13,14,13,
    12,5,12,7,12,12,5,12,5,12,8,12,12,7,12,7,12,5,7,12,7,12,12,7,12,12,12,7,12,13,14,13,12,14,12,12,7,12,12,14,12,15,12,15,12,15,12,7,12,7,12,7,13,14,12,7,14,13,14,13,14,13,15,12,
    12,7,12,5,12,8,12,7,12,7,12,7,12,5,12,12,7,12,12,7,12,7,12,7,12,7,12,14,12,12,12,14,12,13,14,12,15,12,15,12,7,12,12,7,12,7,12,7,13,14,13,7,12,7,13,15,12,15,12,15,13,14,13,15,
    5,12,5,12,7,12,5,12,7,5,12,5,12,7,12,7,12,7,12,12,13,12,7,12,7,12,15,12,7,14,13,12,14,12,7,12,12,14,12,7,12,15,12,7,12,15,12,7,12,7,7,12,15,13,14,13,12,15,13,14,13,15,12,15,
    12,7,12,5,12,7,12,5,12,12,7,12,7,12,12,7,12,12,7,12,7,12,12,7,12,12,12,7,12,12,14,13,12,14,13,14,13,12,7,12,15,12,7,12,15,12,7,13,7,13,14,13,7,12,15,12,15,12,15,13,14,13,14,13,
    12,5,12,7,5,12,7,12,7,12,5,12,12,7,12,5,7,12,7,12,12,7,12,12,15,12,14,13,14,13,12,14,12,7,12,12,14,12,15,12,12,7,12,15,12,7,13,14,12,15,12,15,13,14,13,15,13,15,12,15,13,14,13,15,
    12,7,5,12,12,5,12,5,12,7,12,7,12,5,7,12,12,7,12,7,12,7,12,7,12,12,7,12,12,14,12,7,12,14,13,14,13,12,7,12,15,12,7,12,7,13,7,13,7,13,7,12,15,13,14,13,14,13,14,13,12,15,13,12,
    5,12,12,7,5,7,12,7,12,5,12,7,12,7,12,12,7,12,12,13,12,12,7,12,14,13,12,14,12,13,12,14,13,12,14,12,7,12,15,12,7,7,13,7,13,14,12,15,12,15,12,15,12,15,12,15,13,12,15,13,15,12,15,13,
    12,7,5,12,12,12,5,12,7,12,7,12,12,12,7,12,12,7,12,7,12,7,12,13,12,14,12,13,14,12,7,12,14,12,13,12,7,12,7,12,7,12,7,12,15,13,7,13,14,13,15,13,14,13,15,12,15,13,14,13,15,13,14,13,
    5,12,12,7,5,7,12,7,12,5,12,7,5,12,7,12,7,12,12,7,12,12,14,12,7,12,7,12,12,7,12,15,12,7,14,13,14,13,7,7,13,15,12,15,12,15,12,15,12,15,12,15,13,14,13,13,14,13,15,12,13,14,13,15,
    12,7,5,12,12,12,5,12,8,12,7,12,12,7,12,12,13,12,7,12,7,12,13,12,14,12,12,14,13,14,12,12,7,12,12,7,12,7,12,7,12,7,13,14,13,14,13,15,13,14,13,14,13,13,14,13,15,12,13,15,13,15,12,13,
    12,5,12,8,5,7,12,7,12,12,5,12,7,12,7,12,7,12,12,7,12,7,12,7,12,7,13,12,12,7,12,15,12,7,13,14,13,7,12,15,13,14,13,15,12,13,14,13,14,13,15,13,14,13,15,12,15,13,15,12,15,12,15,13,
    12,7,12,12,12,5,12,5,12,7,12,7,12,5,12,7,12,7,12,12,12,7,12,12,7,12,14,12,7,12,14,12,7,12,7,12,7,12,15,12,13,14,13,12,15,13,15,12,15,13,12,15,13,14,13,15,13,14,13,15,13,15,12,15,
    5,12,5,7,5,12,7,12,7,5,12,12,7,12,7,12,5,12,7,12,7,12,7,12,14,13,12,7,12,15,12,13,12,7,12,7,13,7,12,15,12,15,12,15,13,14,13,15,12,15,13,14,13,13,15,12,13,15,12,13,14,13,13,15,
    12,7,12,5,12,7,12,5,12,12,7,5,12,12,5,12,7,12,12,7,12,12,13,12,12,7,12,14,12,12,7,12,7,12,15,12,7,12,15,12,13,15,13,14,13,14,13,13,15,12,15,13,15,12,15,13,14,13,15,13,15,12,15,12,
    12,5,12,12,7,5,12,7,12,5,12,7,12,7,12,7,12,7,12,13,12,7,12,7,12,12,7,12,13,7,12,7,12,15,12,7,13,7,13,14,13,14,13,14,13,13,14,13,14,13,13,14,13,13,14,13,13,15,12,15,12,15,13,15,
    12,5,7,5,12,12,5,12,5,7,12,5,12,5,12,12,5,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,12,7,13,14,12,15,13,14,13,7,13,14,13,15,13,13,14,13,13,14,13,15,13,14,13,13,14,13,13,14,13,
    12,7,12,12,5,12,7,12,12,12,7,12,7,12,7,12,7,12,12,7,12,12,12,7,12,7,12,13,14,12,7,13,7,13,12,7,13,7,12,13,14,13,14,13,15,12,15,12,15,13,15,12,15,13,12,15,13,14,13,13,15,12,15,13,
    5,12,5,12,7,5,12,5,7,5,12,5,12,5,12,7,5,7,12,5,7,12,7,12,13,12,7,12,7,12,7,12,12,7,7,12,15,12,15,12,15,13,13,14,13,13,15,13,12,15,12,15,13,15,13,14,13,13,14,13,14,13,13,14,
    12,5,12,7,12,12,8,12,12,5,12,7,12,7,5,12,12,12,7,12,12,7,12,7,12,7,12,7,12,13,12,7,7,12,13,14,13,12,15,13,12,15,12,15,12,15,12,15,13,15,13,13,14,13,12,15,13,14,13,15,13,15,12,15,
    12,7,12,5,12,5,12,5,7,12,7,5,12,12,7,12,7,5,12,7,12,5,12,7,12,12,7,12,7,12,7,12,13,7,12,13,14,13,12,15,12,15,13,13,15,12,13,14,13,12,15,12,13,15,13,14,13,15,13,14,13,14,13,13,
    12,5,12,5,12,7,12,12,5,12,12,12,5,7,5,12,5,12,7,5,12,7,12,7,12,7,12,13,12,7,13,7,12,15,12,7,13,14,13,14,13,12,15,12,15,13,15,13,15,12,15,13,15,12,15,13,12,15,12,13,15,13,14,13,
    12,5,12,7,12,5,12,7,12,5,7,5,12,12,12,7,12,7,12,12,7,12,13,12,7,13,12,7,12,7,12,7,12,7,13,14,13,12,15,13,14,13,15,12,13,14,13,12,15,13,13,14,13,15,12,15,13,15,13,14,13,14,13,14,
    12,7,12,5,12,5,12,5,12,7,12,12,7,5,12,5,12,5,12,7,5,12,7,5,12,7,12,7,12,13,12,7,13,12,7,13,14,13,12,15,13,14,13,15,12,13,15,12,13,14,13,15,12,13,15,12,15,12,15,13,14,13,15,13,
    5,12,5,12,7,12,7,12,5,12,5,12,5,12,7,12,7,12,5,12,7,12,12,7,12,5,7,12,7,12,7,12,7,13,12,12,13,14,13,12,15,12,13,14,13,15,12,15,13,15,12,13,15,12,15,13,12,15,12,13,15,12,12,15,
    12,7,12,5,12,5,12,5,12,7,12,5,7,12,5,12,5,7,12,7,12,5,7,12,7,12,13,12,13,7,13,12,13,14,13,14,13,12,15,12,13,15,12,13,15,12,15,13,14,13,15,12,15,13,12,15,13,15,12,15,12,15,13,12,
    12,5,12,12,7,12,5,7,12,5,12,12,12,7,12,7,12,12,5,12,13,12,7,13,12,7,12,7,12,12,7,12,7,12,13,14,13,15,12,15,12,15,13,14,13,13,14,13,13,14,13,15,12,15,13,14,13,14,13,14,13,12,15,7,
    12,5,7,5,12,5,12,12,5,12,7,5,12,5,12,5,12,7,12,7,12,7,12,12,13,12,13,12,7,13,12,13,7,13,14,13,12,7,13,14,13,12,15,12,15,12,15,12,15,12,13,14,13,14,13,14,13,12,15,13,14,13,12,15,
    12,12,12,5,12,7,12,5,7,12,12,7,12,7,12,7,5,12,13,12,5,12,13,12,7,12,7,13,12,7,7,12,15,12,13,14,13,14,13,12,15,13,14,13,14,13,13,14,13,15,12,13,15,12,13,15,12,15,12,13,12,15,12,7,
    5,7,5,12,12,5,12,12,12,5,12,5,12,5,12,12,13,12,7,12,7,12,7,12,13,12,7,12,7,12,13,12,13,14,13,12,13,12,15,13,14,13,12,15,13,14,13,15,12,13,15,12,15,13,14,13,14,13,14,13,7,12,7,13,
    12,12,12,7,5,12,7,5,7,12,7,12,5,7,12,5,12,7,5,12,5,12,13,12,7,12,13,12,13,12,15,12,13,12,15,12,15,12,15,12,13,14,13,14,13,12,15,12,15,12,15,12,15,12,13,14,13,12,13,14,13,7,12,7,
    5,7,5,12,12,8,12,12,5,12,5,12,7,12,5,7,12,12,12,7,12,7,12,7,5,12,7,12,7,12,13,14,12,13,12,15,12,13,12,15,12,15,12,13,14,13,14,13,12,15,12,13,12,15,12,13,7,7,12,7,12,7,7,12,
    12,12,5,12,7,5,12,5,7,12,7,5,12,5,12,12,5,7,5,12,13,12,5,12,7,13,12,7,13,12,7,13,7,12,15,12,13,14,13,14,13,12,15,13,14,13,13,14,13,12,15,12,15,12,7,12,15,12,7,13,7,12,13,14
};

Bitmap bg1 = {
    64,
    40,
    bg1_data
};

// desktop app
unsigned char desktop_app_pic_data[] = {
    16, 16, 16, 29, 16,
    16, 16, 16, 16, 29,
    16, 16, 16, 16, 16,
    16, 16, 16, 16, 16,
    27, 27, 27, 27, 27
};

Bitmap desktop_app_pic = {
    5,
    5,
    desktop_app_pic_data
};

// terminal app
unsigned char terminal_app_pic_data[] = {
    0, 0, 0, 0, 0,
    0, 15, 15, 0, 0,
    0, 0, 0, 15, 0,
    0, 15, 15, 0, 0,
    0, 0, 0, 0, 0
};

Bitmap terminal_app_pic = {
    5,
    5,
    terminal_app_pic_data
};

// about app
unsigned char about_app_pic_data[] = {
    9, 9, 15, 9, 9,
    9, 9, 9, 9, 9,
    9, 9, 15, 9, 9,
    9, 9, 15, 9, 9,
    9, 9, 15, 9, 9,
};

Bitmap about_app_pic = {
    5,
    5,
    about_app_pic_data
};

// not mine !!!1111  y09!!* 0j8yt0q0yqiuvgfsiflvsik
void draw_rect_gradient(int x, int y, int w, int h, unsigned char c1, unsigned char c2)
{
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            unsigned char t = (unsigned char)((i + j) * 255 / (w + h - 2));
            unsigned char color = c1 + ((c2 - c1) * t) / 255;

            put_pixel(x + i, y + j, color);
        }
    }
}

void gradient() {
    for (int y = 0; y < 200; y++) {
        for (int x = 0; x < 320; x++) {
            framebuffer[y * 320 + x] =
                (x + y) % 256;
        }
    }
}

// drawing: text
unsigned char font[96][8] = {
    // SPACE (32)
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

    // !
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00},

    // "
    {0x36,0x36,0x24,0x00,0x00,0x00,0x00,0x00},

    // #
    {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00},

    // $
    {0x18,0x3E,0x58,0x3C,0x1A,0x7C,0x18,0x00},

    // %
    {0x62,0x66,0x0C,0x18,0x30,0x66,0x46,0x00},

    // &
    {0x1C,0x36,0x1C,0x38,0x67,0x66,0x3F,0x00},

    // '
    {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00},

    // (
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00},

    // )
    {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00},

    // *
    {0x00,0x66,0x3C,0x7E,0x3C,0x66,0x00,0x00},

    // +
    {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00},

    // ,
    {0x00,0x00,0x00,0x00,0x18,0x18,0x30,0x00},

    // -
    {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00},

    // .
    {0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00},

    // /
    {0x06,0x0C,0x18,0x30,0x60,0x40,0x00,0x00},

    // 0
    {0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00},

    // 1
    {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00},

    // 2
    {0x3C,0x66,0x06,0x1C,0x30,0x66,0x7E,0x00},

    // 3
    {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00},

    // 4
    {0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00},

    // 5
    {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00},

    // 6
    {0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00},

    // 7
    {0x7E,0x66,0x0C,0x18,0x18,0x18,0x18,0x00},

    // 8
    {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00},

    // 9
    {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00},

    // :
    {0x00,0x18,0x18,0x00,0x18,0x18,0x00,0x00},

    // ;
    {0x00,0x18,0x18,0x00,0x18,0x18,0x30,0x00},

    // <
    {0x0E,0x18,0x30,0x60,0x30,0x18,0x0E,0x00},

    // =
    {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00},

    // >
    {0x70,0x18,0x0C,0x06,0x0C,0x18,0x70,0x00},

    // ?
    {0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00},

    // @
    {0x3C,0x42,0x5A,0x5A,0x5E,0x40,0x3C,0x00},

    // A
    {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00},

    // B
    {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00},

    // C
    {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00},

    // D
    {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00},

    // E
    {0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00},

    // F
    {0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00},

    // G
    {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00},

    // H
    {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00},

    // I
    {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},

    // J
    {0x1E,0x0C,0x0C,0x0C,0x0C,0x6C,0x38,0x00},

    // K
    {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00},

    // L
    {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00},

    // M
    {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00},

    // N
    {0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00},

    // O
    {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},

    // P
    {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00},

    // Q
    {0x3C,0x66,0x66,0x66,0x6E,0x3C,0x0E,0x00},

    // R
    {0x7C,0x66,0x66,0x7C,0x78,0x6C,0x66,0x00},

    // S
    {0x3C,0x66,0x70,0x3C,0x0E,0x66,0x3C,0x00},

    // T
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00},

    // U
    {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},

    // V
    {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00},

    // W
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00},

    // X
    {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00},

    // Y
    {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00},

    // Z
    {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00},

    // [
    {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00},

    // backslash
    {0x60,0x30,0x18,0x0C,0x06,0x02,0x00,0x00},

    // ]
    {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00},

    // ^
    {0x18,0x3C,0x66,0x42,0x00,0x00,0x00,0x00},

    // _
    {0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00},

    // `
    {0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00},

    // a
    {0x00,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00},

    // b
    {0x60,0x60,0x7C,0x66,0x66,0x66,0x7C,0x00},

    // c
    {0x00,0x00,0x3C,0x66,0x60,0x66,0x3C,0x00},

    // d
    {0x06,0x06,0x3E,0x66,0x66,0x66,0x3E,0x00},

    // e
    {0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00},

    // f
    {0x1C,0x36,0x30,0x7C,0x30,0x30,0x30,0x00},

    // g
    {0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x7C},

    // h
    {0x60,0x60,0x7C,0x66,0x66,0x66,0x66,0x00},

    // i
    {0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00},

    // j
    {0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0x6C,0x38},

    // k
    {0x60,0x60,0x66,0x6C,0x78,0x6C,0x66,0x00},

    // l
    {0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},

    // m
    {0x00,0x00,0x6C,0x7E,0x7E,0x6B,0x63,0x00},

    // n
    {0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00},

    // o
    {0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00},

    // p
    {0x00,0x00,0x7C,0x66,0x66,0x7C,0x60,0x60},

    // q
    {0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x06},

    // r
    {0x00,0x00,0x6E,0x70,0x60,0x60,0x60,0x00},

    // s
    {0x00,0x00,0x3E,0x60,0x3C,0x06,0x7C,0x00},

    // t
    {0x30,0x30,0x7C,0x30,0x30,0x36,0x1C,0x00},

    // u
    {0x00,0x00,0x66,0x66,0x66,0x66,0x3E,0x00},

    // v
    {0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00},

    // w
    {0x00,0x00,0x63,0x63,0x6B,0x7F,0x36,0x00},

    // x
    {0x00,0x00,0x66,0x3C,0x18,0x3C,0x66,0x00},

    // y
    {0x00,0x00,0x66,0x66,0x66,0x3E,0x06,0x7C},

    // z
    {0x00,0x00,0x7E,0x0C,0x18,0x30,0x7E,0x00},

    // {
    {0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00},

    // |
    {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00},

    // }
    {0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00},

    // ~
    {0x31,0x6B,0x46,0x00,0x00,0x00,0x00,0x00},

    // DEL
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

void draw_char(int x, int y, char c, unsigned char color) {
    if (c < 32 || c > 127)
        return;

    unsigned char* glyph = font[c - 32];

    for (int row = 0; row < 8; row++) {
        unsigned char line = glyph[row];

        for (int col = 0; col < 8; col++) {
            if (line & (0x80 >> col)) {
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

void draw_string(int x, int y, const char* str, unsigned char color) {
    int cx = x;

    for (int i = 0; str[i]; i++) {
        if (str[i] == '\n') {
            y += 10;
            cx = x;
            continue;
        }

        draw_char(cx, y, str[i], color);
        cx += 8;
    }
}

void draw_string_centered(int cx, int cy, const char* str, unsigned char color) {
    int len = strlen(str);

    int text_width = len * 8;
    int text_height = 8;

    int x = cx - text_width / 2;
    int y = cy - text_height / 2;

    draw_string(x, y, str, color);
}

// drawing: text in terminal
void scroll_screen(unsigned char bg_color) {
    int row_size_pixels = 8;
    int pitch = WIDTH;

    // move each pixel row up by 8 pixels
    for (int y = 0; y < HEIGHT - row_size_pixels; y++) {
        for (int x = 0; x < WIDTH; x++) {
            framebuffer[y * pitch + x] =
                framebuffer[(y + row_size_pixels) * pitch + x];
        }
    }

    // clear last row
    for (int y = HEIGHT - row_size_pixels; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            framebuffer[y * pitch + x] = bg_color;
        }
    }
}

void terminal_print(const char* text, unsigned char col) {
    while (*text) {

        if (*text == '\n') {
            terminal_column = 1;
            terminal_row++;
        } else {
            draw_char(terminal_column*8-8, terminal_row*8-8, *text, col);

            terminal_column++;

            if (terminal_column >= TERMINAL_COLUMNS) {
                terminal_column = 1;
                terminal_row++;
            }
        }

        if (terminal_row >= TERMINAL_ROWS) {
            scroll_screen(color);
            terminal_row = TERMINAL_ROWS - 1;
        }

        text++;
    }
}

// input
int shift_pressed = 0;

// okay ngl i dont understand ts at all :(
static inline unsigned char inb(unsigned short port) {
    unsigned char result;

    asm volatile ("inb %1, %0"
                  : "=a"(result)
                  : "Nd"(port));

    return result;
}

// yup this function got written by chatgpt aint no way im writing this
char scancode_to_ascii(unsigned char scancode) {
    static char normal_map[128] = {
        0,
        27,
        '1','2','3','4','5','6','7','8','9','0',
        '-','=',
        '\b',
        '\t',
        'q','w','e','r','t','y','u','i','o','p',
        '[',']',
        '\n',
        0,
        'a','s','d','f','g','h','j','k','l',
        ';','\'','`',
        0,
        '\\',
        'z','x','c','v','b','n','m',
        ',', '.', '/',
        0,0,0,' '
    };

    static char shift_map[128] = {
        0,
        27,
        '!','@','#','$','%','^','&','*','(',')',
        '_','+',
        '\b',
        '\t',
        'Q','W','E','R','T','Y','U','I','O','P',
        '{','}',
        '\n',
        0,
        'A','S','D','F','G','H','J','K','L',
        ':','"','~',
        0,
        '|',
        'Z','X','C','V','B','N','M',
        '<','>','?',
        0,0,0,' '
    };

    if (scancode > 127)
        return 0;

    if (shift_pressed)
        return shift_map[scancode];

    return normal_map[scancode];
}

char keyboard_get_scancode() {// wait until keyboard has data
    while (!(inb(0x64) & 1)) {}

    return inb(0x60);
}

// time delay
void delay(uint32_t milliseconds)
{
    for (uint32_t ms = 0; ms < milliseconds; ms++)
    {
        for (volatile uint32_t count = 0; count < 100000; count++)
        {
            __asm__ volatile("nop");
        }
    }
}

// desktop
// 0 - desktop 1 - terminal 2 - about
int state = 0;
int desktop_app_hover = 1;
// 1 - startup gradient 2 - another gradient but more messy
int wallpaper = 1;

void desktop_frame() {
    if (wallpaper == 1) {gradient();}
    if (wallpaper == 2) {draw_bitmap(0, 0, bg1, 5);}
    if (desktop_app_hover == 1) {draw_bitmap(30, 60, desktop_app_pic, 5);} else {draw_bitmap(30, 60,  desktop_app_pic, 4);}
    if (desktop_app_hover == 2) {draw_bitmap(60, 60, terminal_app_pic, 5);} else {draw_bitmap(60, 60, terminal_app_pic, 4);}
    if (desktop_app_hover == 3) {draw_bitmap(90, 60, about_app_pic, 5);} else {draw_bitmap(90, 60, about_app_pic, 4);}
    
    draw_rect_fill(0, 0, WIDTH, 30, 54);
    draw_string(5, 5, "Navigate with WSAD or number (1-0) keys.", 0);
    draw_string(5, 15, "Press enter to open an app.", 0);
    draw_rect_fill(0, HEIGHT-30, WIDTH, 30, 54);
    if (desktop_app_hover == 1) {draw_string_centered(WIDTH/2, HEIGHT-15, "Desktop (you're currently in it)", 0);}
    if (desktop_app_hover == 2) {draw_string_centered(WIDTH/2, HEIGHT-15, "Terminal", 0);}
    if (desktop_app_hover == 3) {draw_string_centered(WIDTH/2, HEIGHT-15, "About page", 0);}
}

// terminal command handling (is it the correct way to write handling? idk im to lazy to check it yet im not lazy enough to write this long ass comment)

// shell functions (not mine)
char* skip_spaces(char* s) {
    while (*s == ' ') s++;
    return s;
}

void get_command(char* input, char* cmd, char* args) {
    int i = 0;

    // copy command until space
    while (input[i] && input[i] != ' ') {
        cmd[i] = input[i];
        i++;
    }
    cmd[i] = 0;

    // skip spaces
    char* rest = skip_spaces(input + i);

    // copy args
    int j = 0;
    while (rest[j]) {
        args[j] = rest[j];
        j++;
    }
    args[j] = 0;
}

#define VIDEO_WIDTH 10
#define VIDEO_HEIGHT 6
#define FRAME_COUNT 37

const uint8_t videoData[FRAME_COUNT][60] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1},
    {1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1},
    {1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1},
    {1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1},
    {1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1},
    {1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1},
    {1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,0,1,0,0,1,1,1,1,0,0,0,1,1,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,1,1,1,0,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,0,1,1,1},
    {1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1},
    {1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,1,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,0,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1},
};

void bad_apple() {
    const int blockW = WIDTH / VIDEO_WIDTH;   // 32
    const int blockH = HEIGHT / VIDEO_HEIGHT; // 33

    for (int frame = 0; frame < FRAME_COUNT; frame++) {

        clear_screen(0);

        for (int y = 0; y < VIDEO_HEIGHT; y++) {
            for (int x = 0; x < VIDEO_WIDTH; x++) {

                int idx = y * VIDEO_WIDTH + x;

                if (videoData[frame][idx]) {

                    draw_rect_fill(
                        x * blockW,
                        y * blockH,
                        blockW,
                        blockH,
                        15
                    );
                }
            }
        }

        delay(2000);
    }
}

// shell check (mine)
void shell_command_check(char* input) {
    char cmd[64];
    char args[192];

    get_command(input, cmd, args);

    if (strcmp(cmd, "help")) {
        terminal_print("SoupOS shell commands list:\n", color2);
        terminal_print("- help: print all the commands\n", color2);
        terminal_print("- echo: prints whatever is after it\n", color2);
        terminal_print("ex: echo Hello, World!\n", color2);
        terminal_print("- bg: changes background color (0-255)\n", color2);
        terminal_print("ex: bg 0\n", color2);
        terminal_print("- fg: changes foreground color (0-255)\n", color2);
        terminal_print("ex: fg 15\n", color2);
        terminal_print("- colors: prints some color theme ideas\n", color2);
        terminal_print("- exit: leaves the terminal\n", color2);
        terminal_print("- wallpaper: changes a wallpaper (1-2)\n", color2);
        terminal_print("ex: wallpaper 1\n", color2);
        terminal_print("- clear: clears the screen\n", color2);
    }
    else if (strcmp(cmd, "echo")) {
        terminal_print(args, color2);
        terminal_print("\n", color2);
    } else if (strcmp(cmd, "bg")) {
        terminal_column = 1;
        terminal_row = 1;
        color = stoi(args);
        clear_screen(color);
    } else if (strcmp(cmd, "fg")) {
        terminal_column = 1;
        terminal_row = 1;
        color2 = stoi(args);
        clear_screen(color);
    } else if (strcmp(cmd, "colors")) {
        terminal_print("SoupOS shell color theme ideas:\n", color2);
        terminal_print("- base: bg 0, fg 15\n", color2);
        terminal_print("- temple: bg 15, fg 1 (good idea is changing fg first if your fg is 15)\n", color2);
        terminal_print("- awful light: bg 14, fg 15\n", color2);
    } else if (strcmp(cmd, "exit")) {
        state = 0;
        terminal_column = 1;
        terminal_row = 1;
        desktop_app_hover = 1;
        desktop_frame();
    } else if (strcmp(cmd, "wallpaper")) {
        if (stoi(args) == 1) {
            wallpaper = 1;
        } else if (stoi(args) == 2) {
            wallpaper = 2;
        } else {
            terminal_print("Such wallpaper doesn't exist D:\n", 40);
        }
    } else if (strcmp(cmd, "badapple")) {
        bad_apple();
        terminal_column = 1;
        terminal_row = 1;
        clear_screen(color);
    } else if (strcmp(cmd, "clear")) {
        terminal_column = 1;
        terminal_row = 1;
        clear_screen(color);
    } else {
        terminal_print("Invalid command D:\n", 40);
    }
}

void print_logo() {
	terminal_print("########################################\n\n", color2);
    terminal_print(" _____                   _____ _____ \n", color2);
    terminal_print("/  ___|                 |  _  /  ___|\n", color2);
    terminal_print("\\ `--.  ___  _   _ _ __ | | | \\ `--. \n", color2);
    terminal_print(" `--. \\/ _ \\| | | | '_ \\| | | |`--. \\\n", color2);
    terminal_print("/\\__/ / (_) | |_| | |_) \\ \\_/ /\\__/ /\n", color2);
    terminal_print("\\____/ \\___/ \\__,_| .__/ \\___/\\____/ \n", color2);
    terminal_print("                  | |                \n", color2);
    terminal_print("                  |_|                \n", color2);
	terminal_print("\n########################################\n\n", color2);
}

void startup_logo() {
    int delay_time = 300;
    // im so sorry
    delay(delay_time);
    gradient();
    draw_string_centered(WIDTH/2, HEIGHT/2, "S", color);
    delay(delay_time);
    gradient();
    draw_string_centered(WIDTH/2, HEIGHT/2, "So", color);
    delay(delay_time);
    gradient();
    draw_string_centered(WIDTH/2, HEIGHT/2, "Sou", color);
    delay(delay_time);
    gradient();
    draw_string_centered(WIDTH/2, HEIGHT/2, "Soup", color);
    delay(delay_time);
    gradient();
    draw_string_centered(WIDTH/2, HEIGHT/2, "SoupO", color);
    delay(delay_time);
    gradient();
    draw_string_centered(WIDTH/2, HEIGHT/2, "SoupOS", color);
    delay(delay_time);
    gradient();
    draw_string_centered(WIDTH/2, HEIGHT/2-2, "SoupOS", color);
    delay(delay_time);
    gradient();
    draw_string_centered(WIDTH/2, HEIGHT/2-4, "SoupOS", color);
    delay(delay_time);
    gradient();
    int rw = WIDTH / 1.2;
    int rh = HEIGHT / 4;

    int rx = (WIDTH - rw) / 2;
    int ry = (HEIGHT - rh) / 2;

    //draw_rect_gradient(rx, ry, rw, rh, 1, 15);
    draw_rect_fill(rx, ry, rw, rh, 8);
    draw_string_centered(WIDTH/2, HEIGHT/2-6, "SoupOS", color);
    draw_string_centered(WIDTH/2, HEIGHT/2+6, "Made by zurek. Github: zurek4320", color);
}

void terminal_popup() {
    color = 0;
    clear_screen(color);
    color2 = 15;
    print_logo();
    // color - bg
    // color2 - fg
    terminal_print("$ ", 10);
}

// about
void about_popup() {
    gradient();
    int rw = WIDTH / 1.05;
    int rh = HEIGHT / 3;

    int rx = (WIDTH - rw) / 2;
    int ry = (HEIGHT - rh) / 2;

    //draw_rect_gradient(rx, ry, rw, rh, 1, 15);
    draw_rect_fill(rx, ry, rw, rh, 8);
    draw_string_centered(WIDTH/2, HEIGHT/2-18, "SoupOS", 15);
    draw_string_centered(WIDTH/2, HEIGHT/2-6, "Made by zurek. Github: zurek4320", 15);
    draw_string_centered(WIDTH/2, HEIGHT/2+6, "I started working on it on 5VI2026.", 15);
    draw_string_centered(WIDTH/2, HEIGHT/2+18, "Press enter to go back to the desktop.", 15);
}

// main
extern "C" void main() {
    // delay to make sure everything loades
    delay(1000);
    // logo
    startup_logo();

    delay(3000);
    desktop_frame();

    // input (as you can probably see)
    char current_input[256];
    int input_pos = 0;

    current_input[0] = 0;

    while (1) {
        unsigned char scancode = keyboard_get_scancode();

        if (state == 1) {
            // backspace (not mine i was tired okay it was 1:02 and i woke up on like 7:30 that day) note from next day: I went to sleep at ~3:36 that day btw
            if (scancode == 0x0E) {

                if (input_pos > 0) {

                    input_pos--;
                    current_input[input_pos] = 0;

                    terminal_column--;

                    if (terminal_column < 1) {
                        terminal_column = 1;
                    }

                    draw_rect_fill(terminal_column * 8 - 8, terminal_row * 8 - 8, 8, 8, color);
                }

                continue;
            }

            // check for shift (pressed and then released as you can probably figure out)
            if (scancode == 0x2A || scancode == 0x36) {
                shift_pressed = 1;
                continue;
            }

            if (scancode == 0xAA || scancode == 0xB6) {
                shift_pressed = 0;
                continue;
            }
        }

        // check for release and if you release it ignores it
        if (scancode & 0x80)
            continue;

        // check for enter (0x1C)
        if (scancode == 0x1C) {
            if (state == 1) {
                terminal_print("\n", color2);
                shell_command_check(current_input);

                input_pos = 0;
                current_input[0] = 0;
                if (state == 1) {
                    terminal_print("$ ", 10);
                }
            } else if (state == 0) {
                if (desktop_app_hover == 2) {state = 1; terminal_popup();}
                if (desktop_app_hover == 3) {state = 2; about_popup();}
            } else if (state == 2) {
                state = 0;
                desktop_frame();
            }
            continue;
        }

        char c = scancode_to_ascii(scancode);

        if (c) {
            if (state == 1) {
                current_input[input_pos] = c;
                input_pos++;

                // null terminate
                current_input[input_pos] = 0;

                char str[2];
                str[0] = c;
                str[1] = 0;

                terminal_print(str, color2);
            }
            if (state == 0) {
                if (c == 'a') {desktop_app_hover--; if (desktop_app_hover <= 0) {desktop_app_hover = 1;}}
                if (c == 'd') {desktop_app_hover++; if (desktop_app_hover > total_apps) {desktop_app_hover = total_apps;}}
                if (c == '1') {desktop_app_hover = 1;}
                if (c == '2') {desktop_app_hover = 2;}
                if (c == '3') {desktop_app_hover = 3;}
                desktop_frame();
            }
        }
    }
}
