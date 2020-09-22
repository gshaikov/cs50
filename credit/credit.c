#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <cs50.h>

const string VISA = "VISA";
const string AMEX = "AMEX";
const string MASTERCARD = "MASTERCARD";
const string INVALID = "INVALID";
const string UNKNOWN = "UNKNOWN";

struct CardMeta
{
    int first_two_digits;
    int number_of_digits;
};

void run_tests(void);
string compute_issuer_or_invalid(long card_number);

int is_long_enough(void)
{
    return LONG_MAX > 9999999999999999;
}

int main(void)
{
    if (!is_long_enough())
    {
        printf("Error: 'long' type doesn't have enough capacity "
               "for the card number\n");
        exit(EXIT_FAILURE);
    }

    run_tests();

    long card_number = get_long("card? ");
    string card_name = compute_issuer_or_invalid(card_number);
    printf("%s\n", card_name);
}

// invariant: *digits* has at most 2 digits
int compute_checksum_digit(int digits)
{
    return (digits / 10) + (digits % 10);
}

// invariant: *multiplier* is either 1 or 2
int luhn_sum_every_other_digit(long card_number, int multiplier)
{
    int sum_luhn_digits = 0;
    long trimmed = card_number;
    while (trimmed > 0)
    {
        sum_luhn_digits += compute_checksum_digit((trimmed % 10) * multiplier);
        trimmed /= 100;
    }
    return sum_luhn_digits;
}

int verify_luhn_checksum(long card_number)
{
    int first_sum = luhn_sum_every_other_digit(card_number / 10, 2);
    int second_sum = luhn_sum_every_other_digit(card_number, 1);
    return (first_sum + second_sum) % 10 == 0;
}

// VISA has either 13 or 16 digits and starts with 4
int is_visa(struct CardMeta cm)
{
    return (
        (cm.first_two_digits >= 40 && cm.first_two_digits < 50) &&
        (cm.number_of_digits == 13 || cm.number_of_digits == 16));
}

// MASTERCARD has 16 digits and starts with [51, 55] range
int is_mastercard(struct CardMeta cm)
{
    return (
        (cm.first_two_digits >= 51 && cm.first_two_digits <= 55) &&
        (cm.number_of_digits == 16));
}

// AMEX has 15 digits and starts with one of {34, 37}
int is_amex(struct CardMeta cm)
{
    return (
        (cm.first_two_digits == 34 || cm.first_two_digits == 37) &&
        (cm.number_of_digits == 15));
}

string compute_issuer_or_unknown(struct CardMeta cm)
{
    if (is_visa(cm))
    {
        return VISA;
    }
    else if (is_mastercard(cm))
    {
        return MASTERCARD;
    }
    else if (is_amex(cm))
    {
        return AMEX;
    }
    else
    {
        return UNKNOWN;
    }
}

// invariant:
// card_number >= 10
struct CardMeta compute_card_meta(long card_number)
{
    struct CardMeta cm;
    cm.number_of_digits = 2;
    while (card_number >= 100)
    {
        card_number /= 10;
        cm.number_of_digits += 1;
    }
    cm.first_two_digits = card_number;
    return cm;
}

string compute_issuer_or_invalid(long card_number)
{
    struct CardMeta cm = compute_card_meta(card_number);
    string issuer_or_unknown = compute_issuer_or_unknown(cm);
    if (issuer_or_unknown == UNKNOWN || !verify_luhn_checksum(card_number))
    {
        return INVALID;
    }
    return issuer_or_unknown;
}

// tests

void test_luhn_sum_every_other_digit(void)
{
    assert(luhn_sum_every_other_digit(0, 1) == 0);
    assert(luhn_sum_every_other_digit(1, 1) == 1);
    assert(luhn_sum_every_other_digit(12, 1) == 2);
    assert(luhn_sum_every_other_digit(123, 1) == 1 + 3);
    assert(luhn_sum_every_other_digit(199, 1) == 1 + 9);

    assert(luhn_sum_every_other_digit(0, 2) == 0);
    assert(luhn_sum_every_other_digit(1, 2) == 2);
    assert(luhn_sum_every_other_digit(12, 2) == 4);
    assert(luhn_sum_every_other_digit(123, 2) == 8);
    assert(luhn_sum_every_other_digit(199, 2) == 2 + 1 + 8);
}

void test_verify_luhn_checksum(void)
{
    // AMEX
    assert(verify_luhn_checksum(378282246310005) == 1);
    assert(verify_luhn_checksum(371449635398431) == 1);
    // MASTERCARD
    assert(verify_luhn_checksum(5555555555554444) == 1);
    assert(verify_luhn_checksum(5105105105105100) == 1);
    // VISA
    assert(verify_luhn_checksum(4111111111111111) == 1);
    assert(verify_luhn_checksum(4012888888881881) == 1);

    assert(verify_luhn_checksum(371449635398432) == 0);
    assert(verify_luhn_checksum(5555555555554443) == 0);
    assert(verify_luhn_checksum(4012888888881889) == 0);
}

void test_card_meta(void)
{
    struct CardMeta result;
    result = compute_card_meta(12);
    assert(result.first_two_digits == 12);
    assert(result.number_of_digits == 2);
    result = compute_card_meta(123);
    assert(result.first_two_digits == 12);
    assert(result.number_of_digits == 3);
    result = compute_card_meta(1234);
    assert(result.first_two_digits == 12);
    assert(result.number_of_digits == 4);
    result = compute_card_meta(60110009901);
    assert(result.first_two_digits == 60);
    assert(result.number_of_digits == 11);
    result = compute_card_meta(5105105105105100);
    assert(result.first_two_digits == 51);
    assert(result.number_of_digits == 16);
}

void compute_find_issuer_or_unknown(void)
{
    assert(compute_issuer_or_invalid(378282246310005) == AMEX);
    assert(compute_issuer_or_invalid(371449635398431) == AMEX);

    assert(compute_issuer_or_invalid(5555555555554444) == MASTERCARD);
    assert(compute_issuer_or_invalid(5105105105105100) == MASTERCARD);

    assert(compute_issuer_or_invalid(4111111111111111) == VISA);
    assert(compute_issuer_or_invalid(4012888888881881) == VISA);

    assert(compute_issuer_or_invalid(0) == INVALID);
    assert(compute_issuer_or_invalid(-1) == INVALID);
    assert(compute_issuer_or_invalid(9999999) == INVALID);
    assert(compute_issuer_or_invalid(6011000990139424) == INVALID);
    assert(compute_issuer_or_invalid(2221000000000009) == INVALID);
}

void run_tests(void)
{
    test_luhn_sum_every_other_digit();
    test_verify_luhn_checksum();
    test_card_meta();
    compute_find_issuer_or_unknown();
}
