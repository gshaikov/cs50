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

// Remaining digits after the first digits that identify the issuer.
//  VISA starts with 4
//  MASTERCARD starts with 51 to 55 inclusive
//  AMEX starts with either 34 or 37
// (Can't use math.h)
const long REMAINDER_VISA_1 = 1000000000000;       // 10^12, 13-digit visa
const long REMAINDER_VISA_2 = 1000000000000000;    // 10^15, 16-digit visa
const long REMAINDER_MASTERCARD = 100000000000000; // 10^14, 16-digit mastercard
const long REMAINDER_AMEX = 10000000000000;        // 10^13, 15-digit amex

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

int is_visa(int first_digits)
{
    return first_digits == 4;
}

int is_mastercard(int first_digits)
{
    return first_digits >= 51 && first_digits <= 55;
}

int is_amex(int first_digits)
{
    return first_digits == 34 || first_digits == 37;
}

string compute_issuer_or_unknown(long card_number)
{
    if (is_visa(card_number / REMAINDER_VISA_1) || is_visa(card_number / REMAINDER_VISA_2))
    {
        return VISA;
    }
    else if (is_mastercard(card_number / REMAINDER_MASTERCARD))
    {
        return MASTERCARD;
    }
    else if (is_amex(card_number / REMAINDER_AMEX))
    {
        return AMEX;
    }
    else
    {
        return UNKNOWN;
    }
}

string compute_issuer_or_invalid(long card_number)
{
    string issuer_or_unknown = compute_issuer_or_unknown(card_number);
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

void compute_find_issuer_or_unknown(void)
{
    assert(compute_issuer_or_unknown(378282246310005) == AMEX);
    assert(compute_issuer_or_unknown(371449635398431) == AMEX);

    assert(compute_issuer_or_unknown(5555555555554444) == MASTERCARD);
    assert(compute_issuer_or_unknown(5105105105105100) == MASTERCARD);

    assert(compute_issuer_or_unknown(4111111111111111) == VISA);
    assert(compute_issuer_or_unknown(4012888888881881) == VISA);

    assert(compute_issuer_or_unknown(0) == UNKNOWN);
    assert(compute_issuer_or_unknown(-1) == UNKNOWN);
    assert(compute_issuer_or_unknown(9999999) == UNKNOWN);
    assert(compute_issuer_or_unknown(6011000990139424) == UNKNOWN);
    assert(compute_issuer_or_unknown(2221000000000009) == UNKNOWN);
}

void run_tests(void)
{
    test_luhn_sum_every_other_digit();
    test_verify_luhn_checksum();
    compute_find_issuer_or_unknown();
}
