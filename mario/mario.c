#include <stdio.h>
#include <cs50.h>

int get_correct_height(string message);
int is_height_correct(int height);
void print_mario_row(int n_empty, int n_blocks);
void print_n_times(int n, char c);

int main(void)
{
    int height = get_correct_height("height? ");
    for (int i = 1; i <= height; i++)
    {
        print_mario_row(height - i, i);
    }
}

int get_correct_height(string message)
{
    int height = 0;
    while (1)
    {
        height = get_int("%s", message);
        if (is_height_correct(height))
        {
            return height;
        }
    }
}

int is_height_correct(int height)
{
    return height >= 1 && height <= 8;
}

// n_empty is number of empty places
// n_blocks is number of blocks
void print_mario_row(int n_empty, int n_blocks)
{
    print_n_times(n_empty, ' ');
    print_n_times(n_blocks, '#');
    print_n_times(2, ' ');
    print_n_times(n_blocks, '#');
    printf("\n");
}

void print_n_times(int n, char c)
{
    for (int i = 0; i < n; i++)
    {
        printf("%c", c);
    }
}
