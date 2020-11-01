// Implements a dictionary's functionality

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "dictionary.h"

// Number of buckets in hash table
const unsigned int N = 1;

// Hash table
node *table[N];

void init_node(node *n)
{
    n->word[0] = '\0';
    n->next = NULL;
}

// check

bool check_list(node *n, const char *word)
{
    while (n != NULL)
    {
        if (strcasecmp(n->word, word) == 0)
        {
            return true;
        }
        n = n->next;
    }
    return false;
}

bool check_hash_table(int size, node *hash_table[size], const char *word)
{
    return check_list(hash_table[hash_with_array_size(word, size)], word);
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    return check_hash_table(N, table, word);
}

// hash

// compute hash code for an array with `size`
// guarantees that the hash code is smaller than `size`
unsigned int hash_with_array_size(const char *word, unsigned int size)
{
    return strlen(word) % size;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    return hash_with_array_size(word, N);
}

// load

bool store_in_table(node *hash_table[], unsigned int hash_code, node *n)
{
    node *tn = hash_table[hash_code];
    if (tn != NULL)
    {
        n->next = tn;
    }
    hash_table[hash_code] = n;
    return true;
}

bool load_file_into_table(int size, node *hash_table[size], FILE *dict)
{
    node *n = malloc(sizeof(node));
    if (n == NULL)
    {
        return false;
    }
    init_node(n);
    int idx = 0;
    for (char c = fgetc(dict); c != EOF; c = fgetc(dict))
    {
        if (isalpha(c) || (c == '\'' && idx > 0))
        {
            n->word[idx] = c;
            idx++;
        }
        else if (c == '\n')
        {
            n->word[idx] = '\0';
            if (!store_in_table(hash_table, hash_with_array_size(n->word, size), n))
            {
                printf("failed to store node in hash table\n");
                free(n);
                return false;
            }
            n = malloc(sizeof(node));
            if (n == NULL)
            {
                return false;
            }
            init_node(n);
            idx = 0;
        }
        else
        {
            printf("illegal character in file: %c\n", c);
            free(n);
            return false;
        }
    }
    free(n);

    if (ferror(dict))
    {
        printf("error while reading file\n");
        return false;
    }
    if (feof(dict) && idx == 0)
    {
        return true;
    }
    if (feof(dict) && idx > 0)
    {
        printf("file ends in the middle of a word\n");
        return false;
    }
    printf("unknown logic error\n");
    return false;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        return false;
    }
    bool result = load_file_into_table(N, table, dict);
    fclose(dict);
    return result;
}

// size

unsigned int count_words_in_list(node *n)
{
    unsigned int words;
    for (words = 0; n != NULL; n = n->next)
    {
        words++;
    }
    return words;
}

unsigned int size_hash_table(int size, node *hash_table[])
{
    unsigned int words = 0;
    for (int i = 0; i < size; i++)
    {
        words += count_words_in_list(hash_table[i]);
    }
    return words;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return size_hash_table(N, table);
}

// unload

bool destroy_list(node *n)
{
    for (node *tmp = n; n != NULL; n = tmp)
    {
        tmp = n->next;
        free(n);
    }
    return true;
}

bool map_table(unsigned int size, node *hash_table[size], bool (*fp)(node *))
{
    for (int idx = 0; idx < size; idx++)
    {
        if (!(*fp)(hash_table[idx]))
        {
            printf("map function failed on list\n");
            return false;
        }
    }
    return true;
}

bool unload_table(unsigned int size, node *hash_table[size])
{
    return map_table(N, table, destroy_list);
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    return unload_table(N, table);
}
