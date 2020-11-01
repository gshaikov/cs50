#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../dictionary.h"
#include "test_dictionary.h"

void init_table(int size, node *table[size])
{
    for (int i = 0; i < size; i++)
    {
        table[i] = NULL;
    }
}

void test_hash_with_array_size(void)
{
    assert(hash_with_array_size("", 1) == 0);
    assert(hash_with_array_size("abc", 1) == 0);
    assert(hash_with_array_size("sfsdgsd'sdfdsfsd'sfd", 1) == 0);
    assert(hash_with_array_size("abc", 2) == 1);
    assert(hash_with_array_size("abcde", 3) == 2);
}

void test_store_in_table(void)
{
    {
        node *table[10];
        int hash_code = 0;
        node new_node = {.word = "abc", .next = NULL};
        assert(store_in_table(table, hash_code, &new_node));
        assert(table[hash_code] == &new_node);
    }
    {
        node *table[10];
        node node_in_table = {.word = "foobar", .next = NULL};
        node new_node = {.word = "abc", .next = NULL};
        int hash_code = 5;
        table[hash_code] = &node_in_table;
        assert(store_in_table(table, hash_code, &new_node));
        assert(table[hash_code] == &new_node);
        assert(table[hash_code]->next == &node_in_table);
    }
}

void test_load_file_into_table(void)
{
    {
        FILE *dict = tmpfile();
        fputs("foo\nbar\n", dict);
        rewind(dict);

        int size = 10;
        node *table[size];
        init_table(size, table);
        assert(load_file_into_table(size, table, dict));
        // TODO: `check` table

        unload_table(size, table);
        fclose(dict);
    }
}

void test_count_words_in_list(void)
{
    {
        node *n0 = NULL;
        assert(count_words_in_list(n0) == 0);
    }
    {
        node n1 = {.word = "foo", .next = NULL};
        node n2 = {.word = "bar", .next = &n1};
        assert(count_words_in_list(&n1) == 1);
        assert(count_words_in_list(&n2) == 2);
    }
}

void test_size_hash_table(void)
{
    {
        int size = 10;
        node *table[size];
        init_table(size, table);
        assert(size_hash_table(size, table) == 0);
    }
    {
        int size = 10;
        node *table[size];
        init_table(size, table);
        node node1 = {.word = "foobar", .next = NULL};
        node node2 = {.word = "abc", .next = NULL};
        table[0] = &node1;
        table[5] = &node2;
        assert(size_hash_table(size, table) == 2);
    }
}

void test_dictionary(void)
{
    test_hash_with_array_size();
    test_store_in_table();
    test_size_hash_table();
    test_count_words_in_list();
    test_load_file_into_table();
}
