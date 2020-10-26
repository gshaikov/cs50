#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"

// Convert image to grayscale

void mut_pixel_grey(RGBTRIPLE *c);

void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            mut_pixel_grey(&image[h][w]);
        }
    }
}

void mut_pixel_grey(RGBTRIPLE *c)
{
    int g = round((c->rgbtRed + c->rgbtGreen + c->rgbtBlue) / 3.0);
    c->rgbtRed = g;
    c->rgbtGreen = g;
    c->rgbtBlue = g;
}

// Reflect image horizontally

void mut_swap_pixels(RGBTRIPLE *pix_left, RGBTRIPLE *pix_right);

void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int h = 0; h < height; h++)
    {
        for (int w = 0, half_width = width / 2; w < half_width; w++)
        {
            mut_swap_pixels(&image[h][w], &image[h][width - w - 1]);
        }
    }
}

void mut_swap_pixels(RGBTRIPLE *pix_left, RGBTRIPLE *pix_right)
{
    RGBTRIPLE tmp = *pix_left;
    *pix_left = *pix_right;
    *pix_right = tmp;
}

// Blur image

void copy_image(int height, int width, RGBTRIPLE from[height][width], RGBTRIPLE to[height][width])
{
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            to[h][w] = from[h][w];
        }
    }
}

// PixCoord
// interp. coordinates of a pixel
typedef struct
{
    int row;
    int col;
} PixCoord;

void mut_pix_coord(PixCoord *c, int row, int col)
{
    c->row = row;
    c->col = col;
}

int valid_pix_coord(PixCoord pix, int max_h, int max_w)
{
    return (pix.row >= 0 && pix.row < max_h && pix.col >= 0 && pix.col < max_w);
}

RGBTRIPLE compute_blurred_pixel(int height, int width, RGBTRIPLE image[height][width], PixCoord pix);

void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE(*new_image)
    [width] = calloc(height, width * sizeof(RGBTRIPLE));
    PixCoord pix = {.row = 0, .col = 0};
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            mut_pix_coord(&pix, h, w);
            new_image[h][w] = compute_blurred_pixel(height, width, image, pix);
        }
    }
    copy_image(height, width, new_image, image);
    free(new_image);
}

int kidx(int p, int d)
{
    return p - 1 + d;
}

int trim_uint8(int c)
{
    return fmax(fmin(c, UINT8_MAX), 0);
}

int round_divide(int a, int b)
{
    return round(a / (double)b);
}

RGBTRIPLE compute_blurred_pixel(int height, int width, RGBTRIPLE image[height][width], PixCoord pix)
{
    RGBTRIPLE c = {.rgbtRed = 0, .rgbtGreen = 0, .rgbtBlue = 0};
    PixCoord kpix = {.row = 0, .col = 0};
    int red = 0;
    int green = 0;
    int blue = 0;
    int count = 0;
    for (int h = 0; h < 3; h++)
    {
        for (int w = 0; w < 3; w++)
        {
            mut_pix_coord(&kpix, kidx(pix.row, h), kidx(pix.col, w));
            if (!valid_pix_coord(kpix, height, width))
            {
                continue;
            }
            red += image[kpix.row][kpix.col].rgbtRed;
            green += image[kpix.row][kpix.col].rgbtGreen;
            blue += image[kpix.row][kpix.col].rgbtBlue;
            count++;
        }
    }
    c.rgbtRed = trim_uint8(round_divide(red, count));
    c.rgbtGreen = trim_uint8(round_divide(green, count));
    c.rgbtBlue = trim_uint8(round_divide(blue, count));
    return c;
}

// Detect edges

typedef const int Kernel[3][3];

Kernel KERNEL_HORIZONTAL = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}};

Kernel KERNEL_VERTICAL = {
    {-1, -2, -1},
    {0, 0, 0},
    {1, 2, 1}};

RGBTRIPLE compute_edge_pixel(int height, int width, RGBTRIPLE image[height][width], PixCoord pix);

void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE(*new_image)
    [width] = calloc(height, width * sizeof(RGBTRIPLE));
    PixCoord pix = {.row = 0, .col = 0};
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            mut_pix_coord(&pix, h, w);
            new_image[h][w] = compute_edge_pixel(height, width, image, pix);
        }
    }
    copy_image(height, width, new_image, image);
    free(new_image);
}

typedef struct
{
    int red;
    int green;
    int blue;
} RGBint;

void mut_add_color_scale(RGBint *c, RGBTRIPLE other, int scale)
{
    c->red += other.rgbtRed * scale;
    c->green += other.rgbtGreen * scale;
    c->blue += other.rgbtBlue * scale;
}

int hypot_byte(int x, int y)
{
    return trim_uint8(round(hypot(x, y)));
}

RGBTRIPLE compute_edge_pixel(int height, int width, RGBTRIPLE image[height][width], PixCoord pix)
{
    RGBTRIPLE c = {.rgbtRed = 0, .rgbtGreen = 0, .rgbtBlue = 0};
    PixCoord kpix = {.row = 0, .col = 0};
    RGBint ch = {.red = 0, .green = 0, .blue = 0};
    RGBint cv = {.red = 0, .green = 0, .blue = 0};
    for (int h = 0; h < 3; h++)
    {
        for (int w = 0; w < 3; w++)
        {
            mut_pix_coord(&kpix, kidx(pix.row, h), kidx(pix.col, w));
            if (!valid_pix_coord(kpix, height, width))
            {
                continue;
            }
            mut_add_color_scale(&ch, image[kpix.row][kpix.col], KERNEL_HORIZONTAL[h][w]);
            mut_add_color_scale(&cv, image[kpix.row][kpix.col], KERNEL_VERTICAL[h][w]);
        }
    }
    c.rgbtRed = trim_uint8(hypot_byte(ch.red, cv.red));
    c.rgbtGreen = trim_uint8(hypot_byte(ch.green, cv.green));
    c.rgbtBlue = trim_uint8(hypot_byte(ch.blue, cv.blue));
    return c;
}

// tests

#include <assert.h>

void test_compute_edge_pixel(void)
{
    {
        RGBTRIPLE image[3][3] = {
            {{0, 10, 25}, {0, 10, 30}, {40, 60, 80}},
            {{20, 30, 90}, {30, 40, 100}, {80, 70, 90}},
            {{20, 20, 40}, {30, 10, 30}, {50, 40, 10}}};
        PixCoord pix = {.row = 1, .col = 1};
        RGBTRIPLE result_h = compute_edge_pixel(3, 3, image, pix);
        assert(result_h.rgbtBlue == 210);
        assert(result_h.rgbtGreen == 150);
        assert(result_h.rgbtRed == 60);
    }
}

void test_helpers(void)
{
    test_compute_edge_pixel();
}
