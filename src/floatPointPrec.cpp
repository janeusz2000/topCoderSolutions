#include "gtest/gtest.h"
#include <cmath>
#include <iostream>

int getNumberOfAllDivisions(int number)
{
    int output = 1;
    while (number % 2 == 0)
    {
        output++;
        number /= 2;
    }
    return output;
}

void print() {
    std::cout << "HERE" << std::endl;
}

int representations(int number, int mantissa, int exponent)
{
    int howManyDivisions = getNumberOfAllDivisions(number);
    int maxMantissima = std::pow(2, mantissa) - 1;
    int maxExponent = std::pow(2, exponent) - 1;

    int output = 0;
    for (int current = 1; current <= howManyDivisions; current++)
    {
        if (number <= maxMantissima && current <= maxExponent)
        {
            output++;
        }
        number /= 2;
    }

    return output;
}

TEST(BASIC_TEST, GIVEN_TEST)
{
    ASSERT_EQ(representations(24, 4, 2), 3);
    ASSERT_EQ(representations(1, 3, 3), 1);
    ASSERT_EQ(representations(8, 3, 3), 3);
    ASSERT_EQ(representations(16, 5, 2), 4);
}
