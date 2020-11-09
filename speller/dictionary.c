// Implements a dictionary's functionality

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
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

// pointer to mmapped dictionary
char *DICT = NULL;
int DICT_SIZE = 0;

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

bool load_file_into_table(int size, node *hash_table[size], node node_array[size], char *dict)
{
    // scan the string, replace delimiters, return pointers
    // https://en.cppreference.com/w/c/string/byte/strtok
    char *token = strtok(dict, "\n");
    node *n = NULL;
    int nidx;
    for (nidx = 0; nidx < size; nidx++)
    {
        if (token == NULL)
        {
            NODES_IN_ARRAY = nidx;
            return true;
        }
        n = &node_array[nidx];
        n->word = token;
        n->next = NULL;
        store_in_table(hash_table, hash_with_djb2(token, size), n);
        token = strtok(NULL, "\n");
    }
    printf("array of nodes is too small to hold the dictionary: %i\n", nidx);
    return false;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    NODE_ARRAY = malloc(N * sizeof(node));
    if (NODE_ARRAY == NULL)
    {
        return false;
    }
    // open(path, mode, permissions) file and return file descriptor
    // https://man7.org/linux/man-pages/man2/open.2.html
    int fd_dict = open(dictionary, O_RDONLY, S_IRUSR);
    if (fd_dict == -1)
    {
        return false;
    }
    // get info about the file
    // https://man7.org/linux/man-pages/man2/fstat.2.html
    struct stat sb;
    if (fstat(fd_dict, &sb) == -1)
    {
        return false;
    }
    DICT_SIZE = sb.st_size;
    // map file into virtual memory of the process
    // https://man7.org/linux/man-pages/man2/mmap.2.html
    DICT = mmap(NULL, DICT_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd_dict, 0);
    close(fd_dict);
    return load_file_into_table(N, TABLE, NODE_ARRAY, DICT);
}

// size

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return NODES_IN_ARRAY;
}

// unload

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    free(NODE_ARRAY);
    munmap(DICT, DICT_SIZE);
    return true;
}
