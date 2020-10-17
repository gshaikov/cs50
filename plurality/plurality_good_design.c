#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Max number of candidates
#define MAX 9

// Candidates have name and vote count
typedef struct
{
    string name;
    int votes;
} Candidate;

void mut_init_candidate(Candidate *c, string name, int votes)
{
    c->name = name;
    c->votes = votes;
}

bool mut_vote(int candidate_count, Candidate candidates[], string name);
int mut_find_winners(int candidate_count, Candidate candidates[], Candidate winners[]);
void test(void);

int get_candidate_count_from_args(int argc)
{
    if (argc < 2)
    {
        printf("Usage: plurality [Candidate ...]\n");
        exit(1);
    }
    int candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        exit(2);
    }
    return candidate_count;
}

int main(int argc, string argv[])
{
    if (strcmp(argv[argc - 1], "--test") == 0)
    {
        test();
        printf("tests passed\n");
        return 0;
    }

    int candidate_count = get_candidate_count_from_args(argc);
    Candidate candidates[candidate_count];
    for (int i = 0; i < candidate_count; i++)
    {
        mut_init_candidate(&candidates[i], argv[i + 1], 0);
    }

    int voter_count = get_int("Number of voters: ");
    for (int i = 0; i < voter_count; i++)
    {
        string name = get_string("Vote: ");
        if (!mut_vote(candidate_count, candidates, name))
        {
            printf("Invalid vote.\n");
        }
    }

    Candidate winners[candidate_count];
    int len_winners = mut_find_winners(candidate_count, candidates, winners);
    for (int i = 0; i < len_winners; i++)
    {
        printf("%s\n", winners[i].name);
    }
}

bool mut_vote(int candidate_count, Candidate candidates[], string name)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i].name, name) == 0)
        {
            candidates[i].votes++;
            return true;
        }
    }
    return false;
}

// returns: number of winners in winners array, starting from 0th place
int mut_find_winners(int candidate_count, Candidate candidates[], Candidate winners[])
{
    int winners_cursor = 0;
    winners[winners_cursor] = candidates[winners_cursor];
    for (int i = 1; i < candidate_count; i++)
    {
        if (candidates[i].votes > winners[winners_cursor].votes)
        {
            winners[winners_cursor] = candidates[i];
            continue;
        }
        else if (candidates[i].votes == winners[winners_cursor].votes)
        {
            winners_cursor++;
            winners[winners_cursor] = candidates[i];
            continue;
        }
    }
    return winners_cursor + 1;
}

// tests

#include <assert.h>

void test_mut_vote(void)
{
    {
        Candidate candidates[] = {
            {.name = "foo", .votes = 0},
            {.name = "bar", .votes = 0}};
        char expected[4] = {'f', 'o', 'o', '\0'};
        bool result = mut_vote(2, candidates, expected);
        assert(result == true);
        assert(candidates[0].votes == 1);
        assert(candidates[1].votes == 0);
    }
    {
        Candidate candidates[] = {
            {.name = "foo", .votes = 0},
            {.name = "bar", .votes = 0}};
        char expected[4] = {'b', 'a', 'r', '\0'};
        bool result = mut_vote(2, candidates, expected);
        assert(result == true);
        assert(candidates[0].votes == 0);
        assert(candidates[1].votes == 1);
    }
    {
        Candidate candidates[] = {
            {.name = "foo", .votes = 0},
            {.name = "bar", .votes = 0}};
        char expected[4] = {'r', 'a', 'b', '\0'};
        bool result = mut_vote(2, candidates, expected);
        assert(result == false);
        assert(candidates[0].votes == 0);
        assert(candidates[1].votes == 0);
    }
}

void test_mut_find_winners(void)
{
    {
        Candidate candidates[] = {
            {.name = "foo", .votes = 0},
            {.name = "bar", .votes = 0}};
        Candidate winners[2];
        int result = mut_find_winners(2, candidates, winners);
        assert(result == 2);
        assert(strcmp(winners[0].name, "foo") == 0);
        assert(strcmp(winners[1].name, "bar") == 0);
    }
    {
        Candidate candidates[] = {
            {.name = "foo", .votes = 1},
            {.name = "bar", .votes = 0}};
        Candidate winners[2];
        int result = mut_find_winners(2, candidates, winners);
        assert(result == 1);
        assert(strcmp(winners[0].name, "foo") == 0);
    }
    {
        Candidate candidates[] = {
            {.name = "foo", .votes = 0},
            {.name = "bar", .votes = 1}};
        Candidate winners[2];
        int result = mut_find_winners(2, candidates, winners);
        assert(result == 1);
        assert(strcmp(winners[0].name, "bar") == 0);
    }
}

void test(void)
{
    test_mut_vote();
    test_mut_find_winners();
}
