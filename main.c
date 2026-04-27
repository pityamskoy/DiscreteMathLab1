#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BASE (1ULL << sizeof(unsigned int) * 8)

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
 *
 * Actually, I don't use it in my program due to lack of necessity to do that.
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
 * @param a Bigint number, which becomes a return value.
 * @param b Bigint number
 * @return a+b
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

        a->firstDigit = newA->firstDigit * (-1);
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

        unsigned int digit1_initial = digit1;
        digit1 = digit1 + digit2 + perenos;
        a->numbers[i] = digit1 % BASE;

        if (digit1_initial >= digit1) {
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
 * @param a Bigint number, which becomes a return value.
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
        // -a - (-b) = b - a
        a->firstDigit = a->firstDigit * (-1);
        b->firstDigit = b->firstDigit * (-1);

        Bigint* newA = substraction(deepCopy(b), a);
        if (newA == nullptr) {
            return nullptr;
        }

        a->firstDigit = newA->firstDigit;
        free(a->numbers);
        a->numbers = newA->numbers;
        newA->numbers = nullptr;
        delete(newA);
        return a;
    } else if ((a->firstDigit >= 0 && b->firstDigit < 0) || (a->firstDigit <= 0 && b->firstDigit > 0)) {
        // a - (-b) = a + b
        // -a - b = (a + b) * (-1). Sign changing is executed in summation function
        b->firstDigit = -b->firstDigit;
        return summation(a, b);
    }

    unsigned int max;
    int try = prepare(a, b, &max);
    if (try == 1) {
        return nullptr;
    }

    // Compare magnitudes from most significant digit to find which is larger
    int cmp = 0;
    if (a->numbers[a->numbers[0]] > b->numbers[b->numbers[0]]) {
        cmp = 1;
    } else if (a->numbers[a->numbers[0]] < b->numbers[b->numbers[0]]) {
        cmp = -1;
    } else {
        a->firstDigit = 0;
        a->numbers[0] = 1;
        return a;
    }

    // a < b
    if (cmp == -1) {
        unsigned int* tmp = a->numbers;
        a->numbers = b->numbers;
        b->numbers = tmp;
    }

    long long borrow = 0;
    for (unsigned int i = 1; i <= max; i++) {
        long long digit1 = a->numbers[i];
        long long digit2 = b->numbers[i];

        long long diff = digit1 - digit2 - borrow;

        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }

        a->numbers[i] = (unsigned int) diff;
    }

    while (max > 1 && a->numbers[max] == 0) {
        max--;
    }

    a->numbers[0] = max;
    a->firstDigit = (int) a->numbers[max];
    a->numbers[max] = 0;

    if (cmp == -1) {
        a->firstDigit = -a->firstDigit;
    }

    return a;
}

/**
 * @return The least significant half of an argument.
 * Списал с семинара :D
 */
unsigned int lowWord(unsigned int value) {
    return value & ((1 << (sizeof(unsigned int) << 2)) - 1);
}

/**
 * @return The most significant half of an argument.
 * Это тоже списал :D
 */
unsigned int highWord(unsigned int value) {
    return (value >> (sizeof(unsigned int) << 2)) & ((1 << (sizeof(unsigned int) << 2)) - 1);
}

/**
 * It multiplies two unsigned int numbers and write the result in low and high part.
 * Splits each operand into two 16-bit halves, then combines four 16x16-bit products.
 * a*b = p3*2^32 + (p1+p2)*2^16 + p0
 */
void mulWord(unsigned int a, unsigned int b, unsigned int* low, unsigned int* high) {
    unsigned int a0 = lowWord(a);
    unsigned int a1 = highWord(a);
    unsigned int b0 = lowWord(b);
    unsigned int b1 = highWord(b);

    unsigned int p0 = a0 * b0;
    unsigned int p1 = a0 * b1;
    unsigned int p2 = a1 * b0;
    unsigned int p3 = a1 * b1;

    unsigned int middle = p1 + p2;
    unsigned int carryMid = (middle < p1);

    unsigned int lowPart = p0 + (middle << (sizeof(unsigned int) << 2));
    unsigned int carryLow = (lowPart < p0);

    // carryMid represents an overflow of 2^32 in middle; its contribution to high is 2^32/2^16 = 2^16
    unsigned int highPart = p3 + (middle >> (sizeof(unsigned int) << 2))
    + (carryMid << (sizeof(unsigned int) << 2)) + carryLow;

    *low = lowPart;
    *high = highPart;
}

void normalizeBigint(Bigint* a) {
    if (a == nullptr || a->numbers == nullptr) {
        return;
    }

    while (a->firstDigit == 0 && a->numbers[0] > 1) {
        a->numbers[0]--;
        a->firstDigit = (int) a->numbers[a->numbers[0]];
        a->numbers[a->numbers[0]] = 0;
    }
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

    unsigned int *res = calloc(2 * max + 4, sizeof(unsigned int));
    if (res == nullptr) {
        return nullptr;
    }

    for (unsigned int j = 1; j <= max; j++) {
        unsigned int bj = b->numbers[j];
        unsigned int perenos = 0;

        for (unsigned int i = 1; i <= max; i++) {
            unsigned int ai = a->numbers[i];
            unsigned int k = i + j - 1;

            unsigned int mulLow, mulHigh;
            mulWord(ai, bj, &mulLow, &mulHigh);

            unsigned int tmp = mulLow + perenos;
            mulHigh += (tmp < mulLow);

            unsigned int old = res[k];
            res[k] += tmp;
            mulHigh += (res[k] < old);

            perenos = mulHigh;
        }

        unsigned int pos = max + j;
        while (perenos != 0) {
            unsigned int old = res[pos];
            res[pos] += perenos;
            perenos = (res[pos] < old) ? 1 : 0;
            pos++;
        }
    }

    unsigned int countRealQuantityOfDigits = 2 * max;
    while (countRealQuantityOfDigits > 1 && res[countRealQuantityOfDigits] == 0) {
        countRealQuantityOfDigits--;
    }

    free(a->numbers);
    a->numbers = res;
    a->numbers[0] = countRealQuantityOfDigits;
    a->firstDigit = (int) a->numbers[countRealQuantityOfDigits];

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

    unsigned int* tmp1 = realloc(highNumber->numbers, sizeof(unsigned int) * (n / 2 + 2));
    unsigned int* tmp2 = realloc(smallNumber->numbers, sizeof(unsigned int) * (n / 2 + 2));

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

    unsigned int oldQuantity = a->numbers[0];
    unsigned int* tmp = realloc(a->numbers,sizeof(unsigned int) * (oldQuantity + m + 1));

    if (tmp == nullptr) {
        return nullptr;
    }

    a->numbers = tmp;

    for (unsigned int i = oldQuantity; i >= 1; i--)
        a->numbers[i + m] = a->numbers[i];

    for (unsigned int i = 1; i <= m; i++)
        a->numbers[i] = 0;

    a->numbers[0] = oldQuantity + m;

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
    unsigned int m = a->numbers[0] < b->numbers[0] ? a->numbers[0] : b->numbers[0];

    if (n < 2 || m < 2) {
        return multiply(a, b);
    }

    Bigint* aHigh = init();
    Bigint* aLow = init();
    Bigint* bHigh = init();
    Bigint* bLow = init();

    splitOnHalf(a, aHigh, aLow);
    splitOnHalf(b, bHigh, bLow);

    Bigint* aLowCopy = deepCopy(aLow);
    Bigint* bLowCopy1 = deepCopy(bLow);

    Bigint* s1 = summation(deepCopy(aHigh), aLowCopy);
    Bigint* s2 = summation(deepCopy(bHigh), bLowCopy1);

    Bigint* bLowCopy2 = deepCopy(bLow);
    Bigint* s2Copy = deepCopy(s2);
    Bigint* bHighCopy = deepCopy(bHigh);

    Bigint* res1 = multiply(deepCopy(aLow), bLowCopy2);
    Bigint* tmpRes1 = multiply(deepCopy(s1), s2Copy);
    Bigint* res3 = multiply(deepCopy(aHigh), bHighCopy);

    Bigint* res3Copy = deepCopy(res3);
    Bigint* res1Copy1 = deepCopy(res1);

    Bigint* tmpRes2 = substraction(deepCopy(tmpRes1), res3Copy);
    Bigint* res2 = substraction(deepCopy(tmpRes2), res1Copy1);

    res3 = shiftRight(res3, 2 * (n / 2));
    res2 = shiftRight(res2, n / 2);

    Bigint* res2Copy = deepCopy(res2);
    Bigint* res1Copy2 = deepCopy(res1);

    Bigint* result = summation(deepCopy(res3), res2Copy);
    result = summation(result, res1Copy2);

    free(a->numbers);
    a->firstDigit = result->firstDigit;
    a->numbers = result->numbers;
    result->numbers = nullptr;

    delete(aLowCopy);
    delete(bLowCopy1);
    delete(bLowCopy2);
    delete(s2Copy);
    delete(bHighCopy);
    delete(res3Copy);
    delete(res1Copy1);
    delete(res1Copy2);
    delete(res2Copy);
    delete(aHigh);
    delete(bHigh);
    delete(aLow);
    delete(bLow);
    delete(s1);
    delete(s2);
    delete(res1);
    delete(tmpRes1);
    delete(res3);
    delete(tmpRes2);
    delete(res2);
    delete(result);

    normalizeBigint(a);
    return a;
}

/**
 * It is the implementation of increment function for Bigint numbers.
 */
void increment(Bigint* n) {
    if (n == nullptr) {
        return;
    }

    Bigint* i = init();
    if (i == nullptr) {
        return;
    }

    i->numbers = calloc(2, sizeof(unsigned int));
    i->firstDigit= 1;
    i->numbers[0] = 1;

    summation(n, i);
    delete(i);
}

/**
 * It is the implementation of decrement function for Bigint numbers.
 */
void decrement(Bigint* n) {
    if (n == nullptr) {
        return;
    }

    Bigint* i = init();
    if (i == nullptr) {
        return;
    }

    i->numbers = calloc(2, sizeof(unsigned int));
    i->firstDigit= 1;
    i->numbers[0] = 1;

    substraction(n, i);
    delete(i);
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
    new->numbers = calloc(n->numbers[0] + 1, sizeof(unsigned int));

    for (unsigned int i = 0; i < n->numbers[0]; i++) {
        new->numbers[i] = n->numbers[i];
    }

    return new;
}

/**
 * Compares two Bigint numbers by module.
 * @return -1 if |a| < |b|, 0 if |a| == |b|, 1 if |a| > |b|
 */
int compareBigint(Bigint* a, Bigint* b) {
    if (a->numbers[0] != b->numbers[0]) {
        return a->numbers[0] > b->numbers[0] ? 1 : -1;
    }

    unsigned int am = (unsigned int)(a->firstDigit >= 0 ? a->firstDigit : -a->firstDigit);
    unsigned int bm = (unsigned int)(b->firstDigit >= 0 ? b->firstDigit : -b->firstDigit);

    if (am != bm) {
        return am > bm ? 1 : -1;
    }

    for (int i = (int) a->numbers[0] - 1; i >= 1; i--) {
        if (a->numbers[i] != b->numbers[i]) {
            return a->numbers[i] > b->numbers[i] ? 1 : -1;
        }
    }

    return 0;
}

/**
 * Factorial function for Bigint numbers.
 */
Bigint* factorial(Bigint* multiplication_algorithm(Bigint* a, Bigint* b), Bigint* n) {
    if (n == nullptr) {
        return nullptr;
    }

    if (n->firstDigit < 0) {
        return nullptr;
    }

    Bigint* i = init();
    if (i == nullptr) {
        return nullptr;
    }

    i->firstDigit = 1;
    i->numbers = calloc(2, sizeof(unsigned int));
    i->numbers[0] = 1;

    Bigint* result = deepCopy(i);
    if (result == nullptr) {
        delete(i);
        return nullptr;
    }

    for (; compareBigint(i, n) <= 0; increment(i)) {
        Bigint* iCopy = deepCopy(i);
        result = multiplication_algorithm(result, iCopy);
        delete(iCopy);
    }

    delete(i);
    return result;
}

/**
 * ТЗ номер 3 п.a
 */
Bigint* af(Bigint* multiplication_algorithm(Bigint* a, Bigint*b), Bigint* n) {
    if (n == nullptr) {
        return nullptr;
    }

    if (n->firstDigit <= 0) { // assume Bigint n is always natural.
        return nullptr;
    }

    long long check;
    if (n->numbers[0] == 1) {
        check = (long long) n->firstDigit;
    } else {
        check = (long long) n->numbers[1];
    }

    if (check % 2 == 0) {
        Bigint *result = init();
        if (result == nullptr) {
            return nullptr;
        }

        result->firstDigit = 0;
        result->numbers[0] = 1;
        return result;
    } else {
        return factorial(multiplication_algorithm, n);
    }
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
    new->numbers[0] = 1;

    unsigned int i = 1;
    while (n / BASE > 0) {
        new->numbers[0]++;
        new->numbers[i] = n % BASE;
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
 * Returns base^degree as a new Bigint. Destroys degree (decrements to 0).
 */
Bigint* positiveDegree(Bigint* multiplication_algorithm(Bigint* a, Bigint* b), Bigint* base, Bigint* degree) {
    if (base == nullptr || degree == nullptr) {
        return nullptr;
    }

    Bigint* result = toBase(1);
    if (result == nullptr) {
        return nullptr;
    }

    while (degree->firstDigit > 0) {
        Bigint* baseCopy = deepCopy(base);
        result = multiplication_algorithm(result, baseCopy);
        delete(baseCopy);
        if (result == nullptr) {
            return nullptr;
        }
        decrement(degree);
    }

    return result;
}

/**
 * Computes a mod b using repeated subtraction. O(a/b) iterations.
 */
Bigint* mod(Bigint* a, Bigint*b) {
    if (a == nullptr || b == nullptr) {
        return nullptr;
    }

    Bigint* result = deepCopy(a);
    if (result == nullptr) {
        return nullptr;
    }

    while (compareBigint(result, b) >= 0) {
        Bigint* bCopy = deepCopy(b);
        result = substraction(result, bCopy);
        delete(bCopy);
        if (result == nullptr) {
            return nullptr;
        }
    }

    return result;
}

/**
 * Т3 номер 3, п.b — computes 1152494183 mod 2^n
 */
Bigint* count(Bigint* multiplication_algorithm(Bigint* a, Bigint* b), Bigint* n) {
    if (n == nullptr) {
        return nullptr;
    }

    Bigint* number = toBase(1152494183u);
    Bigint* two = toBase(2);
    Bigint* nCopy = deepCopy(n);

    if (number == nullptr || two == nullptr || nCopy == nullptr) {
        delete(number);
        delete(two);
        delete(nCopy);
        return nullptr;
    }

    Bigint* modulus = positiveDegree(multiplication_algorithm, two, nCopy);
    delete(two);
    delete(nCopy);

    if (modulus == nullptr) {
        delete(number);
        return nullptr;
    }

    Bigint* result = mod(number, modulus);
    delete(number);
    delete(modulus);
    return result;
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

    realloc(a->numbers, sizeof(unsigned int) * 3);
    realloc(b->numbers, sizeof(unsigned int) * 3);

    a->numbers[0] = 3;
    a->numbers[1] = 100;
    a->numbers[2] = 3000;
    a->firstDigit = 13;

    b->numbers[0] = 3;
    b->numbers[1] = 1293;
    b->numbers[2] = 4000;
    b->firstDigit = 120;

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
    delete(bCopy1);

    Bigint* bCopy2 = deepCopy(b);
    Bigint* myKaratsuba = karatsuba(deepCopy(a), bCopy2);
    printBigint(myKaratsuba); // true
    delete(bCopy2);
    delete(myKaratsuba);

    delete(a);
    delete(b);

    Bigint* cAf = init();
    if (cAf == nullptr) {
        return 1;
    }
    cAf->numbers[0] = 1;
    cAf->firstDigit = 151;

    Bigint* cCopy1 = deepCopy(cAf);
    clock_t startDefault = clock();
    Bigint* afByDefault = af(multiply, cCopy1);
    clock_t endDefault = clock();
    printBigint(afByDefault);
    printf("af with multiply for small values:  %.3f s\n", (double) (endDefault - startDefault) / CLOCKS_PER_SEC);
    delete(afByDefault);
    delete(cCopy1);

    Bigint* cCopy2 = deepCopy(cAf);
    clock_t startKaratsuba = clock();
    Bigint* afByKaratsuba = af(karatsuba, cCopy2);
    clock_t endKaratsuba = clock();
    printBigint(afByKaratsuba);
    printf("af with karatsuba for small values: %.3f s\n", (double) (endKaratsuba - startKaratsuba) / CLOCKS_PER_SEC);
    delete(afByKaratsuba);
    delete(cCopy2);
    delete(cAf);

    Bigint* bigC = init();
    if (bigC == nullptr) {
        return 1;
    }
    bigC->numbers[0] = 1;
    bigC->firstDigit = 501;

    Bigint* cCopy3 = deepCopy(bigC);
    clock_t startBigDefault = clock();
    Bigint* bigAfByDefault = af(multiply, cCopy3);
    clock_t bigEndDefault = clock();
    printf("af with multiply for big values:  %.9f s\n", (double) (bigEndDefault - startBigDefault) / CLOCKS_PER_SEC);
    delete(bigAfByDefault);
    delete(cCopy3);

    Bigint* cCopy4 = deepCopy(bigC);
    clock_t startBigKaratsuba = clock();
    Bigint* bigAfByKaratsuba = af(karatsuba, cCopy4);
    clock_t bigEndKaratsuba = clock();
    printf("af with karatsuba for big values: %.9f s\n", (double) (bigEndKaratsuba - startBigKaratsuba) / CLOCKS_PER_SEC);
    delete(bigAfByKaratsuba);
    delete(cCopy4);
    delete(bigC);

    Bigint* cCount = init();
    if (cCount == nullptr) {
        return 1;
    }
    cCount->numbers[0] = 1;
    cCount->firstDigit = 30;

    int reps = 20000;

    clock_t startCountByDefault = clock();
    Bigint* countDefault = nullptr;
    for (int k = 0; k < reps; k++) {
        Bigint* cCopy5 = deepCopy(cCount);
        delete(countDefault);
        countDefault = count(multiply, cCopy5);
        delete(cCopy5);
    }
    clock_t endCountByDefault = clock();
    printBigint(countDefault);
    printf("count with multiply  (%d reps): %.3f s\n", reps, (double)(endCountByDefault - startCountByDefault) / CLOCKS_PER_SEC);
    delete(countDefault);

    clock_t startCountByKaratsuba = clock();
    Bigint* countKaratsuba = nullptr;
    for (int k = 0; k < reps; k++) {
        Bigint* cCopy6 = deepCopy(cCount);
        delete(countKaratsuba);
        countKaratsuba = count(karatsuba, cCopy6);
        delete(cCopy6);
    }
    clock_t endCountByKaratsuba = clock();
    printBigint(countKaratsuba);
    printf("count with karatsuba (%d reps): %.3f s\n", reps, (double)(endCountByKaratsuba - startCountByKaratsuba) / CLOCKS_PER_SEC);
    delete(countKaratsuba);
    delete(cCount);

    return 0;
}
