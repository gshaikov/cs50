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

void test_check_list(void)
{
    {
        node *n0 = NULL;
        assert(!check_list(n0, "abc"));
    }
    {
        node n1 = {.word = "foo", .next = NULL};
        assert(check_list(&n1, "foo"));
        assert(check_list(&n1, "FoO"));
        assert(!check_list(&n1, "bar"));
        node n2 = {.word = "bar", .next = &n1};
        assert(check_list(&n2, "foo"));
        assert(check_list(&n2, "fOo"));
        assert(check_list(&n2, "bar"));
        assert(check_list(&n2, "bAR"));
    }
}

void test_check_hash_table(void)
{
    {
        int size = 10;
        node *table[size];
        init_table(size, table);
        assert(!check_hash_table(size, table, "foo"));
    }
    {
        int size = 10;
        node *table[size];
        init_table(size, table);
        node node1 = {.word = "foobar", .next = NULL};
        node node2 = {.word = "abc", .next = NULL};
        table[hash_with_djb2(node1.word, size)] = &node1;
        table[hash_with_djb2(node2.word, size)] = &node2;
        assert(check_hash_table(size, table, "abc"));
    }
}

void test_hash_with_array_size(void)
{
    assert(hash_with_array_size("", 1) == 0);
    assert(hash_with_array_size("abc", 1) == 0);
    assert(hash_with_array_size("sfsdgsd'sdfdsfsd'sfd", 1) == 0);
    unsigned int pre_code;
    pre_code = 1 + 2 * 31 + 3 * 31 * 31;
    assert(hash_with_array_size("abc", 11) == pre_code % 11);
    pre_code = 1 + 2 * 31 + 3 * 31 * 31 + 4 * 31 * 31 * 31 + 5 * 31 * 31 * 31 * 31;
    assert(hash_with_array_size("abcde", 11) == pre_code % 11);
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

// integration test
void test_load_file_into_table(void)
{
    {
        int size = 10;
        node *table[size];
        init_table(size, table);
        node node_array[size];
        char dict[] = "foo\nbar\n";

        assert(load_file_into_table(size, table, node_array, dict));

        assert(check_hash_table(size, table, "foo"));
        assert(check_hash_table(size, table, "Foo"));
        assert(!check_hash_table(size, table, "abc"));
    }
}

void test_dictionary(void)
{
    test_check_list();
    test_check_hash_table();
    test_hash_with_array_size();
    test_store_in_table();
    test_load_file_into_table();
}
