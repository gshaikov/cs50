// Declares a dictionary's functionality

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdio.h>
#include <stdbool.h>

// Maximum length for a word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
#define LENGTH 45

// Represents a node in a hash table
typedef struct node
{
    char *word;
    struct node *next;
} node;

// Prototypes
bool check(const char *word);
unsigned int hash(const char *word);
bool load(const char *dictionary);
unsigned int size(void);
bool unload(void);

bool check_list(node *n, const char *word);
bool check_hash_table(int size, node *hash_table[], const char *word);

unsigned int hash_with_djb2(const char *word, unsigned int size);
unsigned int hash_with_array_size(const char *word, unsigned int size);
bool store_in_table(node *hash_table[], unsigned int hash_code, node *n);
bool load_file_into_table(int size, node *hash_table[], node node_array[], char *dict);

#endif // DICTIONARY_H
