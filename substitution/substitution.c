#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void test(void);

typedef string Key;

const Key ALPHABET = "abcdefghijklmnopqrstuvwxyz";

Key get_key_from_args(int, string[]);
void mut_compute_shift(Key, Key, int[]);
void mut_encode_text(int[], string, char[]);

int main(int argc, string argv[])
{
    if (strcmp(argv[argc - 1], "--test") == 0)
    {
        test();
        printf("tests passed\n");
        return 0;
    }

    Key key = get_key_from_args(argc, argv);

    int shift[strlen(key)];
    mut_compute_shift(ALPHABET, key, shift);

    string plaintext = get_string("plaintext: ");

    char ciphertext[strlen(plaintext) + 1];
    mut_encode_text(shift, plaintext, ciphertext);

    printf("ciphertext: %s\n", ciphertext);
}

bool valid_key(Key key)
{
    int len = 'z' + 1;
    int map[len];
    for (int i = 0; i < len; i++)
    {
        map[i] = 0;
    }
    char alpha;
    int i;
    for (i = 0; key[i] != '\0'; i++)
    {
        if (!isalpha(key[i]))
        {
            return 0;
        }
        alpha = tolower(key[i]);
        if (map[alpha] != 0)
        {
            return 0;
        }
        map[alpha] = 1;
    }
    return (i == 26);
}

Key get_key_from_args(int argc, string argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        exit(1);
    }
    Key key = argv[1];
    if (!valid_key(key))
    {
        printf("Key must contain 26 characters.\n");
        exit(1);
    }
    return key;
}

char compute_shift(char base, char key)
{
    return tolower(key) - tolower(base);
}

void mut_compute_shift(Key base, Key key, int shift[])
{
    int i;
    for (i = 0; key[i] != '\0' && base[i] != '\0'; i++)
    {
        shift[i] = compute_shift(base[i], key[i]);
    }
    if (key[i] != '\0' || base[i] != '\0')
    {
        printf("base and key are not the same length\n");
        exit(1);
    }
}

char shift_char(int shift, char c)
{
    return c + shift;
}

int alpha_index(char c)
{
    return islower(c) ? c - 'a' : c - 'A';
}

void mut_encode_text(int shift[], string plaintext, char ciphertext[])
{
    int i;
    char c;
    for (i = 0; plaintext[i] != '\0'; i++)
    {
        c = plaintext[i];
        if (isalpha(c))
        {
            ciphertext[i] = shift_char(shift[alpha_index(c)], c);
        }
        else
        {
            ciphertext[i] = c;
        }
    }
    ciphertext[i] = '\0';
}

// tests

#include <assert.h>

void test_mut_compute_shift(void)
{
    {
        int result[3];
        mut_compute_shift("AbC", "aCE", result);
        int expected[] = {0, 1, 2};
        for (int i = 0; i < 3; i++)
        {
            assert(result[i] == expected[i]);
        }
    }
    {
        int result[3];
        mut_compute_shift("XYz", "VxZ", result);
        int expected[] = {-2, -1, 0};
        for (int i = 0; i < 3; i++)
        {
            assert(result[i] == expected[i]);
        }
    }
}

void test_shift_char(void)
{
    assert(shift_char(0, 'm') == 'm');
    assert(shift_char(0, 'M') == 'M');
    assert(shift_char(1, 'a') == 'b');
    assert(shift_char(1, 'A') == 'B');
    assert(shift_char(-1, 'b') == 'a');
    assert(shift_char(-1, 'B') == 'A');
    assert(shift_char(25, 'a') == 'z');
    assert(shift_char(-25, 'z') == 'a');
}

void test_mut_encode_text(void)
{
    {
        int shift_one_right[] = {1, 1, 1};
        char result[4];
        mut_encode_text(shift_one_right, "cBa  ", result);
        assert(strcmp(result, "dCb  ") == 0);
    }
    {
        int shift_shuffle[] = {1, 0, -1};
        char result[4];
        mut_encode_text(shift_shuffle, " Ba,C!", result);
        assert(strcmp(result, " Bb,B!") == 0);
    }
}

void test(void)
{
    test_mut_compute_shift();
    test_shift_char();
    test_mut_encode_text();
}
