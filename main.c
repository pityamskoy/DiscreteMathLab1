#include <stdio.h>
#include <stdlib.h>

#define BASE (256 << sizeof(int))

typedef struct Bigint {
    int firstDigit;
    unsigned int* numbers;
} Bigint;

/**
 * Initialize a Bigint instance with 2 assigned memory cells in numbers
 */
Bigint* init() {
    Bigint* bigint = (Bigint*) malloc(sizeof(Bigint));
    if (bigint == NULL) {
        return nullptr;
    }

    bigint->numbers = (unsigned int*) malloc(sizeof(unsigned int) * 2);

    return bigint;
}

/**
 * Safely delete a Bigint number.
 * It will work correctly even with passed nullptr as argument.
 */
void delete(Bigint* number) {
    if (number == nullptr) {
        return;
    }

    if (number->numbers != NULL) {
        free(number->numbers);
    }

    free(number);
}

/**
 * Rough input in BASE number system with little endian (LE) format.
 */
int inputBigint(Bigint* number) {
    if (number == nullptr) {
        return 1;
    }

    unsigned int k;
    scanf("%u", &k);

    if (k == 0) {
        return 1;
    }

    unsigned int* tmp = realloc(number->numbers, sizeof(unsigned int) * (k + 1));
    if (tmp == NULL) {
        return 1;
    }

    number->numbers = tmp;
    number->numbers[0] = k;

    int firstNumber;
    scanf("%d", &firstNumber);
    if (firstNumber > BASE) {
        return 1;
    }
    number->firstDigit = firstNumber;
    k--;

    if (k == 0) {
        return 0;
    }

    unsigned int digit;

    for (int i = 1; k>0; k-- && i++) {
        scanf("%u", &digit); //assure user will enter a bigint in little endian format
        if (digit > BASE)
        number->numbers[i] = digit;
    }

    return 0;
}

/**
 * It prepares two arguments for any operation to be executed.
 *
 * Generally, it makes two arguments be the same maximum size,
 * and it transfers firstDigits to arrays of numbers (for convenience).
 *
 * @return Returns 1 if executing of this function fails, 0 if it executes correctly.
 */
int prepare(Bigint* a, Bigint* b, unsigned int* maximum) {
    unsigned int max;
    if (a->numbers[0] >= b->numbers[0]) {
        max = a->numbers[0];
    } else {
        max = b->numbers[0];
    }

    unsigned int *tmp1 = realloc(a->numbers, sizeof(unsigned int) * (2 * max + 4));

    if (tmp1 == NULL) {
        return 1;
    }

    a->numbers = tmp1;
    a->numbers[a->numbers[0]] = a->firstDigit;

    unsigned int *tmp2 = realloc(b->numbers, sizeof(unsigned int) * (2 * max + 4));

    if (tmp2 == NULL) {
        return 1;
    }

    b->numbers = tmp2;
    b->numbers[b->numbers[0]] = b->firstDigit;

    for (unsigned int i = a->numbers[0] + 1; i <= 2 * max + 2; i++) {
        a->numbers[i] = 0;
    }

    for (unsigned int i = b->numbers[0] + 1; i <= 2 * max + 2; i++) {
        b->numbers[i] = 0;
    }

    a->numbers[0] = max;
    b->numbers[0] = max;

    *maximum = max;
    return 0;
}

Bigint* substraction(Bigint* a, Bigint* b);
Bigint* deepCopy(Bigint* n);

/**
 * Summation of two Bigint numbers.
 * @param a - a Bigint number, which becomes a return value.
 */
Bigint* summation(Bigint* a, Bigint* b) {
    if (a == nullptr || b == nullptr) {
        return nullptr;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return nullptr;
    }

    if (a->firstDigit == 0 && b->firstDigit == 0) {
        return a;
    }

    if (a->firstDigit <= 0 && b->firstDigit <= 0) {
        a->firstDigit = a->firstDigit * (-1);
        b->firstDigit = b->firstDigit * (-1);

        Bigint* newA = summation(deepCopy(a), b);
        if (newA == nullptr) {
            return nullptr;
        }

        a->firstDigit = a->firstDigit * (-1);
        free(a->numbers);
        a->numbers = newA->numbers;
        return a;
    }

    if (a->firstDigit >= 0 && b->firstDigit <= 0) {
        b->firstDigit = b->firstDigit * (-1);
        return substraction(a, b);
    } else if (a->firstDigit <= 0 && b->firstDigit >= 0) {
        a->firstDigit = a->firstDigit * (-1);
        return substraction(b, a);
    }

    unsigned int max;
    int try = prepare(a, b, &max);
    if (try == 1) {
        return nullptr;
    }

    unsigned int perenos = 0;
    for (unsigned int i = 1; i <= max; i++) {
        unsigned int digit1 = a->numbers[i];
        unsigned int digit2 = b->numbers[i];

        digit1 = digit1 + digit2 + perenos;
        a->numbers[i] = digit1 % BASE;

        if (digit1 >= BASE) {
            perenos = digit1 / BASE;
        } else {
            perenos = 0;
        }
    }

    if (perenos > 0) {
        a->numbers[max + 1] = perenos;
        a->numbers[0]++;
    } else {
        a->numbers[0] = max;
    }

    a->firstDigit = (int) a->numbers[a->numbers[0]];
    a->numbers[a->numbers[0]] = 0;

    return a;
}

/**
 * Substraction of two Bigint numbers.
 * @param a - a Bigint number, which becomes a return value.
 */
Bigint* substraction(Bigint* a, Bigint* b) {
    if (a == nullptr || b == nullptr) {
        return nullptr;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return nullptr;
    }

    if (a->firstDigit == 0 && b->firstDigit == 0) {
        return a;
    }

    if (a->firstDigit < 0 && b->firstDigit <= 0) {
        a->firstDigit = a->firstDigit * (-1);
        b->firstDigit = b->firstDigit * (-1);
        return substraction(b, a);
    } else if ((a->firstDigit >= 0 && b->firstDigit < 0) || (a->firstDigit <= 0 && b->firstDigit > 0)) {
        b->firstDigit = b->firstDigit * (-1);
        return summation(a, b);
    }

    unsigned int max;
    int try = prepare(a, b, &max);
    if (try == 1) {
        return nullptr;
    }

    long long borrow = 0;

    for (unsigned int i = 1; i <= max; i++) {
        long long digit1 = (long long) a->numbers[i];
        long long digit2 = (long long) b->numbers[i];

        long long diff = digit1 - digit2 - borrow;

        if (diff < 0 && i != max) {
            diff += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }

        a->numbers[i] = (unsigned int) diff;
    }

    while (a->numbers[max] == 0) {
        max--;
    }

    a->firstDigit = (int) a->numbers[max];
    a->numbers[max] = 0;

    return a;
}

/**
 * @return The least significant half of an argument.
 * Списал с семинара :D
 */
int lowWord(int value) {
    return value &((1 << sizeof(int) << 2) - 1);
}

/**
 * @return The most significant half of an argument.
 * Это тоже списал :D
 */
int highWord(int value) {
    return (value >> (sizeof(value) << 2) & (1 << ((sizeof(value) << 2) - 1)));
}

/**
 * It multiplies two unsigned int numbers and write the result in low and high part.
 *
 * Due to number system equal 2^12 this function is deprecated since overflowing doesnt happen at all.
 * Example: 2^12 * 2^12 = 2^24 < 2^32 - 1 (maximum number written in int)
 */
void mulWord(unsigned int a, unsigned int b, unsigned int* low,unsigned int* high) {
    unsigned int a0 = lowWord((int) a);
    unsigned int a1 = highWord((int) a);
    unsigned int b0 = lowWord((int) b);
    unsigned int b1 = highWord((int) b);

    unsigned int p0 = a0 * b0;
    unsigned int p1 = a0 * b1;
    unsigned int p2 = a1 * b0;
    unsigned int p3 = a1 * b1;

    /**
     * 1214 * 1618
     * 14*18 - low words
     * 18*12 - mid words
     * 12*16 - mid words
     * 12*16 - high words
     *
     * a*b = p3·2^(2*sizeof(int)<<2) + (p1+p2)·2^sizeof(int)<<2 + p0
     */

    unsigned int middle = p1 + p2;
    unsigned int carryMid = (middle < p1);

    unsigned int lowPart = p0 + (middle << (sizeof(int) << 2));
    unsigned int carryLow = (lowPart < p0);

    unsigned int highPart = p3 + (middle >> (sizeof(int) << 2)) + carryMid + carryLow;

    *low = lowPart;
    *high = highPart;
}

/**
 * Multiplication of two Bigint numbers.
 * @param a - a Bigint number, which becomes a return value.
 */
Bigint* multiply(Bigint* a, Bigint* b) {
    if (a == nullptr || b == nullptr) {
        return nullptr;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return nullptr;
    }

    /**
     * We assume that a and b are normalized numbers written in BASE number system.
     */

    if (a->firstDigit == 0) {
        return a;
    } else if (b->firstDigit == 0) {
        return b;
    }

    int sign = 1;
    if ((a->firstDigit < 0 && b->firstDigit > 0) || (a->firstDigit > 0 && b->firstDigit < 0)) {
        sign = -1;
    }

    unsigned int max;
    int try = prepare(a, b, &max);
    if (try == 1) {
        return nullptr;
    }

    unsigned int *res = calloc(2 * max + 2, sizeof(unsigned int));
    if (res == nullptr) {
        return nullptr;
    }

    res[0] = max;
    unsigned int countRealQuantityOfDigits = 0;

    for (unsigned int j = 1; j <= max; j++) {
        unsigned int bj = b->numbers[j];
        unsigned int subCount = 0;
        unsigned int perenos = 0;

        for (unsigned int i = 1; i <= max; i++) {
            unsigned int ai = a->numbers[i];

            /**
             * We assume that overflowing always doesn't happen due to number system (BASE = 2^12).
             */

            unsigned int mult = ai * bj + perenos;
            unsigned int k = i + j - 1; // shift to the left in BE multiplication
            perenos = 0;

            res[k] += mult % BASE;
            if (res[k] >= BASE) {
                res[k] = res[k] % BASE;
                perenos += res[k] / BASE;
            }

            perenos += mult / BASE;
            subCount++;
        }

        unsigned int count = 1;
        while (perenos != 0) {
            res[subCount + count] += perenos;

            if (res[subCount + count] >= BASE) {
                perenos = res[subCount + count] / BASE;
                res[subCount + count] = res[subCount + count] % BASE;
            } else {
                perenos = 0;

                if (countRealQuantityOfDigits < subCount + count) {
                    countRealQuantityOfDigits = subCount + count;
                } else {
                    countRealQuantityOfDigits = countRealQuantityOfDigits;
                }
            }

            count++;
        }

        if (countRealQuantityOfDigits < subCount) {
            countRealQuantityOfDigits = subCount;
        } else {
            countRealQuantityOfDigits = countRealQuantityOfDigits;
        }
    }

    free(a->numbers);
    a->numbers = res;

    if (a->numbers[0] > 1) {
        while (a->numbers[0] > 1 && a->firstDigit == 0) {
            a->firstDigit = (int) a->numbers[a->numbers[0]];
            a->numbers[0]--;
        }
    }

    a->numbers[0] = countRealQuantityOfDigits;
    a->firstDigit = (int) a->numbers[a->numbers[0]];

    if (sign < 0) {
        a->firstDigit = a->firstDigit * (-1);
    }

    return a;
}

/**
 * It splits a Bigint number of two halves.
 * @param x A Bigint number.
 * @param highNumber Pointer to the high half part of {@code x}.
 * @param smallNumber Pointer to the small half part of {@code x}.
 */
void splitOnHalf(Bigint* x, Bigint* highNumber, Bigint* smallNumber) {
    if (x == nullptr || highNumber == nullptr || smallNumber == nullptr) {
        return;
    }

    unsigned int n = x->numbers[0];

    if (n < 2) {
        return;
    }

    unsigned int* tmp1 = realloc(highNumber->numbers, sizeof(unsigned int) * (n / 2) + 2);
    unsigned int* tmp2 = realloc(smallNumber->numbers, sizeof(unsigned int) * (n / 2) + 2);

    if (tmp1 == nullptr || tmp2 == nullptr) {
        return;
    }

    highNumber->numbers = tmp1;
    highNumber->numbers[0] = 0;
    smallNumber->numbers = tmp2;
    smallNumber->numbers[0] = 0;

    /**
     * Due to Little Endian (LE) the first n/2 numbers are smallest digits in a bigint,
     * and the last n/2 numbers are biggest.
     */

    unsigned int l = 1, h = 1;
    for (unsigned int i = 1; i <= n; i++) {
        if (i < n/2) {
            smallNumber->numbers[0]++;
            smallNumber->numbers[l] = x->numbers[i];
            l++;
        } else if (i == n/2) {
            smallNumber->numbers[0]++;
            smallNumber->firstDigit = (int) x->numbers[i];
        } else if (i > n/2 && i < n) {
            highNumber->numbers[0]++;
            highNumber->numbers[h] = x->numbers[i];
            h++;
        } else if (i == n) {
            highNumber->firstDigit = x->firstDigit;
            highNumber->numbers[0]++;
        }
    }
}

/**
 * It shifts all numbers in array to the right on m.
 */
Bigint* shiftRight(Bigint* a, unsigned int m) {
    if (a == nullptr || m == 0)
        return a;

    unsigned int oldCount = a->numbers[0];
    realloc(a->numbers,sizeof(unsigned int) * (oldCount + m + 1));

    for (unsigned int i = oldCount; i >= 1; i--)
        a->numbers[i + m] = a->numbers[i];

    for (unsigned int i = 1; i <= m; i++)
        a->numbers[i] = 0;

    a->numbers[0] = oldCount + m;

    return a;
}

/**
 * It is the implementation of karatsuba algorithm for two Bigint numbers.
 * @param a - a Bigint number, which becomes a return value.
 */
Bigint* karatsuba(Bigint* a, Bigint* b) {
    if (a == nullptr || b == nullptr) {
        return nullptr;
    }

    unsigned int n = a->numbers[0] > b->numbers[0] ? a->numbers[0] : b->numbers[0];

    if (n < 2) {
        return multiply(a, b);
    }

    Bigint* aHigh = init();
    Bigint* aLow = init();
    Bigint* bHigh = init();
    Bigint* bLow = init();

    splitOnHalf(a, aHigh, aLow);
    splitOnHalf(b, bHigh, bLow);

    Bigint* s1 = summation(aHigh, aLow);
    Bigint* s2 = summation(bHigh, bLow);

    Bigint* res1 = multiply(aLow, bLow);
    Bigint* tmpRes1 = multiply(s1, s2);
    Bigint* res3 = multiply(aHigh, bHigh);

    Bigint* tmpRes2 = substraction(tmpRes1, res3);
    Bigint* res2 = substraction(tmpRes2, res1);

    res3 = shiftRight(res3, 2 * (n / 2));
    res2 = shiftRight(res2, n / 2);

    Bigint* result = summation(res3, res2);
    result = summation(result, res1);

    free(a->numbers);
    a->firstDigit = result->firstDigit;
    a->numbers = result->numbers;
    result->numbers = nullptr;

    delete(result);
    delete(bHigh);
    delete(bLow);
    delete(s1);

    return a;
}

/**
 * It is the implementation of increment function for Bigint numbers.
 */
void increment(Bigint* n) {
    if (n == nullptr) {
        return;
    }

    unsigned int count = 1;
    for (unsigned int i = 1; i < n->numbers[0]; i++) {
        if (n->numbers[i] == BASE - 1) {
            count++;
        } else {
            break;
        }
    }

    if (count == n->numbers[0]) {
        n->firstDigit++;

        if (n->firstDigit == BASE) {
            realloc(n->numbers, sizeof(unsigned int) * (n->numbers[0] + 1));
            n->numbers[0]++;
            n->firstDigit = 1;

            for (unsigned i = 1; i < n->numbers[0]; i++) {
                n->numbers[i] = 0;
            }
        }
    } else {
        n->numbers[count]++;
    }
}

/**
 * Implementation of making a deep copy for Bigint numbers.
 */
Bigint* deepCopy(Bigint* n) {
    if (n == nullptr) {
        return nullptr;
    }

    Bigint* new = init();
    if (new == nullptr) {
        return nullptr;
    }

    new->firstDigit = n->firstDigit;
    new->numbers = calloc(n->numbers[0], sizeof(unsigned int));

    for (unsigned int i = 0; i < n->numbers[0]; i++) {
        new->numbers[i] = n->numbers[i];
    }

    return new;
}

/**
 * Factorial function for Bigint numbers.
 */
Bigint* factorial(Bigint* n) {
    Bigint* i = init();
    if (i == nullptr) {
        return nullptr;
    }

    i->firstDigit = 1;
    i->numbers = calloc(1, sizeof(unsigned int));
    i->numbers[0] = 1;

    Bigint* result = deepCopy(i);
    if (result == nullptr) {
        return nullptr;
    }

    for (; result->numbers[0] < n->numbers[0] && result->firstDigit < n->firstDigit; increment(i)) {
        result = multiply(result, i);
    }

    return result;
}

/**
 * ТЗ номер 3 п.a
 */
Bigint* af(Bigint* n) {
    if (n == nullptr) {
        return nullptr;
    }

    Bigint* i = init();
    if (i == nullptr) {
        return nullptr;
    }

    i->firstDigit = 1;
    i->numbers = calloc(1, sizeof(unsigned int));
    if (i->numbers == NULL) {
        return nullptr;
    }
    i->numbers[0] = 1;

    Bigint* result = init();
    result->numbers = calloc(n->numbers[0], sizeof(unsigned int));
    result->firstDigit = 0;
    result->numbers[0] = 1;

    for (;i->numbers[0] < n->numbers[0] && i->firstDigit < n->firstDigit; increment(i)) {
        Bigint* diff = deepCopy(n);
        if (diff == nullptr) {
            return nullptr;
        }

        diff = substraction(diff, i);
        int sign;
        if (diff->numbers[0] == 1) {
            if (diff->firstDigit % 2 == 0) {
                sign = 1;
            } else {
                sign = -1;
            }
        } else {
            if (diff->numbers[diff->numbers[0] - 1] % 2 == 0) {
                sign = 1;
            } else {
                sign = -1;
            }
        }

        Bigint* nFactorial = factorial(n);
        if (nFactorial == nullptr) {
            return nullptr;
        }
        nFactorial->firstDigit = nFactorial->firstDigit*sign;

        result = summation(result, nFactorial);
    }

    return result;
}

/**
 * function of counting digits in numbers written in 10-numeric system.
 */
unsigned int countNumberOfDigits(unsigned int result, unsigned int n) {
    if (n / 10 > 0) {
        result++;
        countNumberOfDigits(result, n/10);
    }

    return result;
}

/**
 * It is the function of casting an unsigned int number to a Bigint number.
 */
Bigint* toBase(unsigned int n) {
    Bigint* new = init();
    if (new == nullptr) {
        return nullptr;
    }

    new->numbers = calloc(countNumberOfDigits(1, n) + 2, sizeof(unsigned int) * 1);
    new->numbers[0] = 0;

    unsigned int i = 1;
    while (n / BASE > 0) {
        new->numbers[0]++;
        new->numbers[i] = n / BASE;
        i++;
        n = n / BASE;
    }

    new->numbers[i] = n % BASE;
    new->firstDigit = (int) new->numbers[new->numbers[0]];
    new->numbers[new->numbers[0]] = 0;

    return new;
}

/**
 * It is the implementation of exponentiation in positive degree.
 */
Bigint* positiveDegree(Bigint* number, Bigint* degree) {
    if (number == nullptr || degree == nullptr) {
        return nullptr;
    }

    Bigint* one = init();
    if (one == nullptr) {
        return nullptr;
    }

    one->firstDigit = 1;
    realloc(one->numbers, sizeof(unsigned int));
    one->numbers[0] = 1;

    while (degree->numbers[0] > 1 && degree->firstDigit > 0) {
        multiply(number, number);
        if (number == nullptr) {
            return nullptr;
        }

        substraction(degree, one);
        if (degree == nullptr) {
            return nullptr;
        }
    }

    return number;
}

/**
 * Difficulty of algorithm is O(n^3).
 * Илья Сергеевич, не бейте сильно :D
 */
Bigint* mod(Bigint* a, Bigint*b) {
    if (a == nullptr || b == nullptr) {
        return nullptr;
    }

    Bigint* iter1 = deepCopy(b);
    Bigint* iter2 = deepCopy(b);
    Bigint* iter3 = deepCopy(b);
    Bigint* one = init();
    if (one == nullptr || iter1 == nullptr) {
        delete(one);
        delete(iter1);
        delete(iter2);
        delete(iter3);
        return nullptr;
    }

    one->firstDigit = 1;
    realloc(one->numbers, sizeof(unsigned int));
    one->numbers[0] = 1;

    while (a->numbers[0] >= multiply(iter1, iter1)->numbers[0] && a->firstDigit > multiply(iter2, iter2)->firstDigit) {
        if (iter1 == nullptr || iter2 == nullptr) {
            return nullptr;
        }

        multiply(iter3, iter3);
    }

    delete(one);
    delete(iter1);
    delete(iter2);
    delete(iter3);
    return substraction(a, iter3);
}

/**
 * Т3 номер 3, п.b
 */
Bigint* count(Bigint* n) {
    if (n == nullptr) {
        return nullptr;
    }

    Bigint* number = toBase(115249);
    Bigint* degree = toBase(4183);
    Bigint* two = toBase(2);

    if (number == nullptr || degree == nullptr || two == nullptr) {
        return nullptr;
    }

    positiveDegree(number, degree);
    if (number == nullptr) {
        return nullptr;
    }

    positiveDegree(two, n);
    if (two == nullptr) {
        return nullptr;
    }

    return mod(number, two);
}

/**
 * It is the function of printing a Bigint number.
 */
void printBigint(Bigint* n) {
    if (n == nullptr) {
        return;
    }

    printf("Printing Bigint number in Little Endian (LE) format...\n");
    for (unsigned int i = 1; i < n->numbers[0]; i++) {
        printf("%u\n", n->numbers[i]);
    }

    printf("%d\n", n->firstDigit);
}

int main(void) {
    /**
     * Generating test cases for laboratory work.
     */

    Bigint* a = init();
    Bigint* b = init();
    if (a == nullptr || b == nullptr) {
        return 1;
    }

    realloc(a->numbers, sizeof(unsigned int) * 4);
    realloc(b->numbers, sizeof(unsigned int) * 3);

    a->numbers[0] = 4;
    a->numbers[1] = 1293;
    a->numbers[2] = 3405;
    a->numbers[3] = 10;
    a->firstDigit = 120;

    b->numbers[0] = 3;
    b->numbers[1] = 100;
    b->numbers[2] = 4000;
    b->firstDigit = 13;

    Bigint* aCopy = deepCopy(a);
    Bigint* mySubstraction = substraction(deepCopy(b), aCopy);
    printBigint(mySubstraction); // true
    delete(aCopy);
    delete(mySubstraction);

    Bigint* bCopy = deepCopy(b);
    Bigint* mySummation = summation(deepCopy(a), bCopy);
    printBigint(mySummation); // true
    delete(bCopy);
    delete(mySummation);

    /**
     * For checking multiplication and Karatsuba algorithm more trivial values have been set.
     */

    a->firstDigit = 4000;
    a->numbers[0] = 2;
    a->numbers[1] = 10;

    b->firstDigit = 3000;
    b->numbers[0] = 2;
    b->numbers[1] = 20;

    Bigint* bCopy1 = deepCopy(b);
    Bigint* myMultiplication = multiply(deepCopy(a), bCopy1);
    printBigint(myMultiplication); // true
    delete(myMultiplication);

    Bigint* myKaratsuba = karatsuba(deepCopy(a), bCopy1);
    printBigint(myKaratsuba);
    delete(bCopy1);
    delete(myKaratsuba);

    delete(a);
    delete(b);

    return 0;
}
