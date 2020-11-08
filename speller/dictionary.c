// Implements a dictionary's functionality

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <ctype.h>

#include "dictionary.h"

// Number of buckets in hash table
const unsigned int N = 201101;

// Hash table
node *TABLE[N];

// pointer to node array
node *NODE_ARRAY = NULL;
int NODES_IN_ARRAY = 0;

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
    return check_list(hash_table[hash_with_djb2(word, size)], word);
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    return check_hash_table(N, TABLE, word);
}

// hash

// http://www.cse.yorku.ca/~oz/hash.html
// max words in list = 8 for N = 150151
unsigned int hash_with_djb2(const char *word, unsigned int size)
{
    unsigned long hash = 5381;
    for (int i = 0; word[i] != '\0'; i++)
    {
        hash = ((hash << 5) + hash) + (word[i] >= 'a' ? word[i] - 'a' : word[i] - 'A'); /* hash * 33 + c */
    }
    return hash % size;
}

// polynomial rolling hash function
// https://cp-algorithms.com/string/string-hashing.html
// compute hash code for an array with `size`
// guarantees that the hash code is smaller than `size`
// max words in list = 9 for N = 150151
unsigned int hash_with_array_size(const char *word, unsigned int size)
{
    unsigned int code = 0;
    unsigned int pow = 1;
    for (int i = 0; word[i] != '\0'; i++)
    {
        code = (code + ((word[i] >= 'a' ? word[i] - 'a' : word[i] - 'A') + 1) * pow) % size;
        pow = (pow * 31) % size;
    }
    return code;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    return hash_with_djb2(word, N);
}

// load

bool store_in_table(node *hash_table[], unsigned int hash_code, node *n)
{
    n->next = hash_table[hash_code];
    hash_table[hash_code] = n;
    return true;
}

bool load_file_into_table(int size, node *hash_table[size], FILE *dict)
{
    NODE_ARRAY = malloc(size * sizeof(node));
    if (NODE_ARRAY == NULL)
    {
        return false;
    }
    int nidx;
    node *n = NULL;
    for (nidx = 0; nidx < size; nidx++)
    {
        n = &NODE_ARRAY[nidx];
        init_node(n);
        // consume one string until a whitespace character reached
        // Example: a string "foo\n" isconsumed as "foo"
        if (fscanf(dict, "%s", n->word) == EOF)
        {
            break;
        }
        store_in_table(hash_table, hash_with_djb2(n->word, size), n);
    }
    NODES_IN_ARRAY = nidx;

    if (ferror(dict))
    {
        printf("error while reading file\n");
        return false;
    }
    if (feof(dict))
    {
        return true;
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
    bool result = load_file_into_table(N, TABLE, dict);
    fclose(dict);
    return result;
}

// size

unsigned int count_words_in_list(node *n)
{
    unsigned int words = 0;
    while (n != NULL)
    {
        words++;
        n = n->next;
    }
    return words;
}

unsigned int size_hash_table(int size, node *hash_table[size])
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
    return NODES_IN_ARRAY;
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
    return map_table(N, TABLE, destroy_list);
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    free(NODE_ARRAY);
    return true;
}
