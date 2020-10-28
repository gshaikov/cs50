#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FAT_BLOCK_SIZE_BYTES 512
#define OUTFILE_DIR "out"

typedef unsigned char Byte;

int recover_images(FILE *infile, char *infile_name, char *outfile_dir);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: recover infile\n");
        return 1;
    }
    char *infile_name = argv[1];
    FILE *infile = fopen(infile_name, "r");
    if (infile == NULL)
    {
        printf("failed to open file: %s\n", infile_name);
        return 1;
    }
    int result = recover_images(infile, infile_name, OUTFILE_DIR);
    fclose(infile);
    return result;
}

int jpeg_start(Byte *buf)
{
    return buf[0] == 0xff && buf[1] == 0xd8 && buf[2] == 0xff && (buf[3] >> 4) == 0xe;
}

int fclose_if_open(FILE *f)
{
    if (f != NULL)
    {
        return fclose(f);
    }
    return 0;
}

int openned(FILE *f)
{
    return f != NULL;
}

int recover_images(FILE *infile, char *infile_name, char *outfile_dir)
{
    FILE *jpeg = NULL;
    Byte buf[FAT_BLOCK_SIZE_BYTES];
    char jpeg_path[100];
    int count = 0;

    while (fread(buf, sizeof(Byte), FAT_BLOCK_SIZE_BYTES, infile) == sizeof(Byte) * FAT_BLOCK_SIZE_BYTES)
    {
        if (jpeg_start(buf))
        {
            fclose_if_open(jpeg);
            sprintf(jpeg_path, "%s/%3.3i.jpg", outfile_dir, count);
            count++;
            jpeg = fopen(jpeg_path, "w");
            if (jpeg == NULL)
            {
                printf("failed to open file: %s\n", jpeg_path);
                return 1;
            }
        }
        if (openned(jpeg) && fwrite(buf, sizeof(Byte), FAT_BLOCK_SIZE_BYTES, jpeg) != sizeof(Byte) * FAT_BLOCK_SIZE_BYTES)
        {
            printf("failed to write a block of %i bytes into file %s\n", FAT_BLOCK_SIZE_BYTES, jpeg_path);
            fclose_if_open(jpeg);
            return 1;
        }
    }

    fclose_if_open(jpeg);

    if (!feof(infile))
    {
        printf("failed to read a block of %i bytes from file %s\n", FAT_BLOCK_SIZE_BYTES, infile_name);
        return 1;
    }
    return 0;
}
