// Declares a dictionary's functionality

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>

// Maximum length for a word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
#define LENGTH 45

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// Prototypes
bool check(const char *word);
unsigned int hash(const char *word);
bool load(const char *dictionary);
unsigned int size(void);
bool unload(void);

unsigned int hash_with_array_size(const char *word, unsigned int size);
bool store_in_table(node *hash_table[], unsigned int hash_code, node *n);
bool load_file_into_table(int size, node *hash_table[size], FILE *dict);

#endif // DICTIONARY_H
