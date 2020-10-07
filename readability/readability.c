#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void test(void);

int compute_grade(string);
void print_grade(int);

int main(int argc, string argv[])
{
    if (strcmp(argv[argc - 1], "--test") == 0)
    {
        test();
        printf("tests passed\n");
        exit(0);
    }
    else
    {
        string text = get_string("Input: ");
        print_grade(compute_grade(text));
        exit(0);
    }
}

typedef struct
{
    int letters;
    int words;
    int sentences;
} TextStats;

TextStats NewTextStats(void)
{
    TextStats stats;
    stats.letters = 0;
    stats.words = 0;
    stats.sentences = 0;
    return stats;
}

void print_stats(TextStats ts)
{
    printf(
        "{letters: %d, words: %d, sentences: %d}\n",
        ts.letters,
        ts.words,
        ts.sentences);
}

// State machine.

typedef string State;

// int is 32 bit signed integer
const State WORD_ALPHA = "WORD_ALPHA";
const State WORD_NOT_ALPHA = "WORD_NOT_ALPHA";
const State SPACE = "SPACE";
const State END_SENTENCE = "END_SENTENCE";

State compute_next_state(State state, char c)
{
    if (isalpha(c))
    {
        return WORD_ALPHA;
    }
    else if (c == ' ')
    {
        if (state == END_SENTENCE)
        {
            return END_SENTENCE;
        }
        return SPACE;
    }
    else if (c == '.' || c == '!' || c == '?')
    {
        return END_SENTENCE;
    }
    else
    {
        if (state == END_SENTENCE)
        {
            return END_SENTENCE;
        }
        return WORD_NOT_ALPHA;
    }
}

void update_stats(TextStats *stats, State state, State next_state)
{
    if ((state == WORD_ALPHA && next_state == WORD_ALPHA) || (state == WORD_NOT_ALPHA && next_state == WORD_ALPHA))
    {
        stats->letters++;
    }
    else if (state == SPACE && next_state == WORD_ALPHA)
    {
        stats->letters++;
        stats->words++;
    }
    else if (state == SPACE && next_state == WORD_NOT_ALPHA)
    {
        stats->words++;
    }
    else if (state == END_SENTENCE && next_state == WORD_ALPHA)
    {
        stats->letters++;
        stats->words++;
        stats->sentences++;
    }
    else if (state == END_SENTENCE && next_state == WORD_NOT_ALPHA)
    {
        stats->words++;
        stats->sentences++;
    }
    else if (state == END_SENTENCE && next_state == SPACE)
    {
        printf("illegal transition: %s -> %s", state, next_state);
        exit(1);
    }
}

float compute_rate_a_per_100_b(int a, int b)
{
    if (b == 0)
    {
        printf("fatal: division by zero (a: %i)", a);
        exit(1);
    }
    return (float)(a * 100) / (float)b;
}

float compute_coleman_liau_index(float letters_per_100_words, float sentences_per_100_words)
{
    return 0.0588 * letters_per_100_words - 0.296 * sentences_per_100_words - 15.8;
}

TextStats compute_stats(string text)
{
    TextStats stats = NewTextStats();

    State state = END_SENTENCE;
    State next_state = "";
    for (int cursor = 0, sentence_length = strlen(text); cursor < sentence_length; cursor++)
    {
        next_state = compute_next_state(state, text[cursor]);
        update_stats(&stats, state, next_state);
        state = next_state;
    }
    // print_stats(stats);
    return stats;
}

int compute_grade(string text)
{
    TextStats stats = compute_stats(text);
    return (int)round(compute_coleman_liau_index(
        compute_rate_a_per_100_b(stats.letters, stats.words),
        compute_rate_a_per_100_b(stats.sentences, stats.words)));
}

void print_grade(int grade)
{
    if (grade < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (grade > 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", grade);
    }
}

// Tests

void test_compute_next_state(void)
{
    assert(compute_next_state(WORD_ALPHA, 'a') == WORD_ALPHA);
    assert(compute_next_state(WORD_ALPHA, 'A') == WORD_ALPHA);
    assert(compute_next_state(WORD_ALPHA, '-') == WORD_NOT_ALPHA);
    assert(compute_next_state(WORD_ALPHA, '\'') == WORD_NOT_ALPHA);
    assert(compute_next_state(WORD_ALPHA, ',') == WORD_NOT_ALPHA);
    assert(compute_next_state(WORD_ALPHA, '_') == WORD_NOT_ALPHA);
    assert(compute_next_state(WORD_ALPHA, ';') == WORD_NOT_ALPHA);
    assert(compute_next_state(WORD_ALPHA, ' ') == SPACE);
    assert(compute_next_state(WORD_ALPHA, '.') == END_SENTENCE);

    assert(compute_next_state(SPACE, 'a') == WORD_ALPHA);
    assert(compute_next_state(WORD_NOT_ALPHA, 'a') == WORD_ALPHA);

    assert(compute_next_state(END_SENTENCE, '.') == END_SENTENCE);
    assert(compute_next_state(END_SENTENCE, ' ') == END_SENTENCE);
    assert(compute_next_state(END_SENTENCE, '"') == END_SENTENCE);
    assert(compute_next_state(END_SENTENCE, 'a') == WORD_ALPHA);
}

void test_update_stats(void)
{
    {
        TextStats stats = NewTextStats();
        update_stats(&stats, WORD_ALPHA, WORD_ALPHA);
        assert(stats.letters == 1);
        assert(stats.words == 0);
        assert(stats.sentences == 0);
    }
    {
        TextStats stats = NewTextStats();
        update_stats(&stats, SPACE, WORD_ALPHA);
        assert(stats.letters == 1);
        assert(stats.words == 1);
        assert(stats.sentences == 0);
    }
    {
        TextStats stats = NewTextStats();
        update_stats(&stats, END_SENTENCE, WORD_ALPHA);
        assert(stats.letters == 1);
        assert(stats.words == 1);
        assert(stats.sentences == 1);
    }
}

const string BEFORE_GRADE_1 = "One fish. Two fish. Red fish. Blue fish.";
const string GRADE_5 = "Harry Potter was a highly unusual boy in many ways. For one thing, he hated the summer holidays more than any other time of year. For another, he really wanted to do his homework, but was forced to do it in secret, in the dead of the night. And he also happened to be a wizard.";
const string GRADE_7 = "In my younger and more vulnerable years my father gave me some advice that I've been turning over in my mind ever since.";
const string GRADE_8 = "When he was nearly thirteen, my brother Jem got his arm badly broken at the elbow. When it healed, and Jem's fears of never being able to play football were assuaged, he was seldom self-conscious about his injury. His left arm was somewhat shorter than his right; when he stood or walked, the back of his hand was at right angles to his body, his thumb parallel to his thigh.";
const string GRADE_8_ONE_SENTENCE = "Alice was beginning to get very tired of sitting by her sister on the bank, and of having nothing to do: once or twice she had peeped into the book her sister was reading, but it had no pictures or conversations in it, \"and what is the use of a book,\" thought Alice \"without pictures or conversation?\"";
const string GRADE_16_PLUS = "A large class of computational problems involve the determination of properties of graphs, digraphs, integers, arrays of integers, finite families of finite sets, boolean formulas and elements of other countable domains.";

void test_compute_stats(void)
{
    {
        TextStats stats = compute_stats(BEFORE_GRADE_1);
        assert(stats.letters == 29);
        assert(stats.words == 8);
        assert(stats.sentences == 4);
    }
    {
        TextStats stats = compute_stats(GRADE_8_ONE_SENTENCE);
        assert(stats.sentences == 1);
    }
}

void test_compute_grade(void)
{
    assert(compute_grade(BEFORE_GRADE_1) < 1);
    assert(compute_grade(GRADE_5) == 5);
    assert(compute_grade(GRADE_7) == 7);
    assert(compute_grade(GRADE_8) == 8);
    assert(compute_grade(GRADE_8_ONE_SENTENCE) == 8);
    assert(compute_grade(GRADE_16_PLUS) > 16);
}

void test(void)
{
    test_compute_next_state();
    test_update_stats();
    test_compute_stats();
    test_compute_grade();
}
