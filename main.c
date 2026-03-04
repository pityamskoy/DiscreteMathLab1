#include <stdio.h>
#include <stdlib.h>

#define BASE (256 << sizeof(int))

typedef struct Bigint {
    int firstDigit;
    unsigned int* numbers;
} Bigint;

Bigint* init() {
    Bigint* bigint = (Bigint*) malloc(sizeof(Bigint));
    if (bigint == NULL) {
        return nullptr;
    }

    bigint->numbers = (unsigned int*) malloc(sizeof(unsigned int) + 2);

    return bigint;
}

void delete(Bigint* number) {
    if (number == nullptr) {
        return;
    }

    if (number->numbers != NULL) {
        free(number->numbers);
    }

    free(number);
}

int inputBigint(Bigint* number) {
    if (number == nullptr) {
        return 1;
    }

    unsigned int k;
    scanf("%u", &k);

    if (k == 0) {
        return 1;
    }

    realloc(number->numbers, sizeof(unsigned int) * (k + 1));
    if (number->numbers == NULL) {
        return 1;
    }
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

Bigint* summation(Bigint* a, Bigint* b) {
    if (a == nullptr || b == nullptr) {
        return nullptr;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return nullptr;
    }

    unsigned long long max = (a->numbers[0] > b->numbers[0]) ? a->numbers[0]: b->numbers[0];
    if (a->numbers[0] <= max) {
        unsigned int *tmp = realloc(a->numbers, sizeof(unsigned int) * (max + 2));

        if (tmp == NULL) {
            return nullptr;
        }
        a->numbers = tmp;
    }

    unsigned long long perenos = 0;
    for (unsigned int i = 1; i <= max; i++) {
        unsigned long long digit1 = (i <= a->numbers[0] - 1) ? a->numbers[i] : 0;
        unsigned long long digit2 = (i <= b->numbers[0] - 1) ? b->numbers[i] : 0;

        digit1 = digit1 + digit2 + perenos;
        a->numbers[i] = (unsigned int) digit1 % BASE;

        if (digit1 >= BASE) {
            perenos = digit1 / BASE;
        } else {
            perenos = 0;
        }
    }

    if (perenos > 0) {
        a->numbers[max + 1] = (unsigned int) perenos;
        a->numbers[0] = max + 1;
    } else {
        a->numbers[0] = max;
    }

    int check = 0;
    long long result = a->firstDigit + b->firstDigit;
    if (result >= BASE || result <= (-1*BASE)) {
        check = (int) result / BASE;
        a->numbers[0] = max + 1;
        a->numbers[a->numbers[0]] = (unsigned int) result % BASE;
        a->firstDigit = check;
    } else {
        a->firstDigit = (int) result;
    }

    return a;
}

Bigint* substraction(Bigint* a, Bigint* b) {
    if (a == nullptr || b == nullptr) {
        return nullptr;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return nullptr;
    }

    unsigned long long max = (a->numbers[0] > b->numbers[0]) ? a->numbers[0]: b->numbers[0];
    if (a->numbers[0] <= max) {
        unsigned int *tmp = realloc(a->numbers, sizeof(unsigned int) * (max + 2));

        if (tmp == NULL) {
            return nullptr;
        }
        a->numbers = tmp;
    }

    long long borrow = 0;

    for (unsigned int i = 1; i <= max; i++) {
        long long digit1 = (i <= a->numbers[0] - 1) ? a->numbers[i] : 0;
        long long digit2 = (i <= b->numbers[0] - 1) ? b->numbers[i] : 0;

        long long diff = digit1 - digit2 - borrow;

        if (diff<0) {
            diff += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }

        a->numbers[i] = (unsigned int)diff;
    }

    while (max + 1 > 1 && a->firstDigit == 0) {
        a->firstDigit = a->numbers[max + 1];
        max--;
    }

    a->numbers[0] = max;

    int digit1 = a->firstDigit;
    int digit2 = b->firstDigit;

    long long diff = digit1 - digit2 - borrow;

    if (diff <= BASE*(-1)) {
        a->firstDigit = (int) diff % BASE;
    }

    a->numbers[a->numbers[0]] = (int) (diff / BASE);

    return a;
}

int lowWord(int value) {
    return value &((1 << sizeof(int) << 2) - 1);
}

int highWord(int value) {
    return (value >> (sizeof(value) << 2) & (1 << ((sizeof(value) << 2) - 1)));
}

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
    unsigned int carry_mid = (middle < p1);

    unsigned int low_part = p0 + (middle << (sizeof(int) << 2));
    unsigned int carry_low = (low_part < p0);

    unsigned int high_part = p3 + (middle >> (sizeof(int) << 2)) +carry_mid + carry_low;

    *low = low_part;
    *high = high_part;
}

Bigint* multiply(Bigint* a, Bigint* b) {
    if (a == nullptr || b == nullptr) {
        return nullptr;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return nullptr;
    }

    unsigned int count1 = a->numbers[0];
    unsigned int count2 = b->numbers[0];

    unsigned int res_size = count1 + count2;

    unsigned int *res = calloc(res_size + 1, sizeof(unsigned int));
    if (res == nullptr) {
        return nullptr;
    }

    res[0] = res_size;

    int sign = 1; // result sign
    if (a->firstDigit < 0) sign *= -1;
    if (b->firstDigit < 0) sign *= -1;
    unsigned int countRealQuantityOfDigits = 0;

    for (unsigned int i = 1; i <= count1; i++) {
        unsigned int ai = a->numbers[i];
        unsigned int carry = 0;

        for (unsigned int j = 1; j <= count2; j++) {
            unsigned int bj = b->numbers[j];

            unsigned int low, high;
            mulWord(ai, bj, &low, &high);

            unsigned int k = i + j - 1;
            countRealQuantityOfDigits++;
            unsigned int tmp = res[k] + low;
            unsigned int carry1 = (tmp < res[k]);

            res[k] = tmp;
            carry = high + carry1;
            tmp = res[k + 1] + carry;
            carry = (tmp < res[k + 1]);
            res[k + 1] = tmp;
        }
    }

    res[0] = countRealQuantityOfDigits + 1;
    unsigned int low, high;
    mulWord(a->firstDigit, b->firstDigit, &low, &high);
    a->firstDigit = 0;
    unsigned int temporary = res[res[0]] + low;
    unsigned int carry2 = (temporary < res[res[0]]);

    res[res[0]] = temporary;
    unsigned int new_carry = high + carry2;
    temporary = res[res[0] + 1] + new_carry;
    new_carry = (temporary < res[res[0] + 1]);
    res[res[0] + 1] = temporary;
    res[0]++;

    free(a->numbers);
    a->numbers = res;

    if (a->numbers[0] > 1) {
        while (a->numbers[0] > 1 && a->firstDigit == 0) {
            a->firstDigit = a->numbers[a->numbers[0] - 1];
            a->numbers[0]--;
        }
    }

    if (sign < 0)
        a->firstDigit = -(int) res[res[0]];
    else
        a->firstDigit = (int) res[res[0]];

    return a;
}

void splitOnHalf(Bigint* x, Bigint* highNumber, Bigint* smallNumber) {
    if (x == nullptr || highNumber == nullptr || smallNumber == nullptr) {
        return;
    }

    unsigned int n = x->numbers[0];

    if (n < 2) {
        return;
    }

    realloc(highNumber->numbers, sizeof(unsigned int)*(n/2) + 2);
    realloc(smallNumber->numbers, sizeof(unsigned int)*(n/2) + 2);

    /**
     * Due to Little Endian (LE) the first n/2 numbers are smallest digits in a bigint,
     * and the last n/2 numbers are biggest.
     */

    unsigned int l = 1, h = 1;
    for (unsigned int i = 1; i < n; i++) {
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
        }
    }
}

Bigint* shiftLeft(Bigint* a, unsigned int m) {
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

    res3 = shiftLeft(res3, 2*(n/2));
    res2 = shiftLeft(res2, n/2);

    Bigint* result = summation(res3, res2);
    result = summation(result, res1);

    free(a->numbers);
    a->firstDigit = result->firstDigit;
    a->numbers = result->numbers;
    result->numbers = nullptr;

    delete(result);
    delete(aHigh);
    delete(aLow);
    delete(bHigh);
    delete(bLow);
    delete(s1);
    delete(s2);
    delete(res1);
    delete(res2);
    delete(res3);
    delete(tmpRes1);
    delete(tmpRes2);

    return a;
}

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

Bigint* saveCopy(Bigint* n) {
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


Bigint* factorial(Bigint* n) {
    Bigint* i = init();
    if (i == nullptr) {
        return nullptr;
    }

    i->firstDigit = 1;
    i->numbers = calloc(1, sizeof(unsigned int));
    i->numbers[0] = 1;

    Bigint* result = saveCopy(i);
    if (result == nullptr) {
        return nullptr;
    }

    for (; result->numbers[0] < n->numbers[0] && result->firstDigit < n->firstDigit; increment(i)) {
        result = multiply(result, i);
    }

    return result;
}

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
        Bigint* diff = saveCopy(n);
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

unsigned int countNumberOfDigits(unsigned int result, unsigned int n) {
    if (n/10 > 0) {
        result++;
        countNumberOfDigits(result, n/10);
    }

    return result;
}

Bigint* toBase(unsigned int n) {
    Bigint* new = init();
    if (new == nullptr) {
        return nullptr;
    }

    new->numbers = calloc(countNumberOfDigits(1, n) + 2, sizeof(unsigned int) * 1);
    new->numbers[0] = 0;

    unsigned int i = 1;
    while (n > 0) {
        new->numbers[0]++;
        new->numbers[i] = n / BASE;
        i++;
        n = n / BASE;
    }

    new->firstDigit = new->numbers[new->numbers[0]];
    new->numbers[new->numbers[0]] = 0;

    return new;
}

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

Bigint* count(Bigint* n) {
    if (n == nullptr) {
        return nullptr;
    }

    Bigint* number = toBase(115249);
    Bigint* degree = toBase(4183);

    if (number == nullptr || degree == nullptr) {
        return nullptr;
    }


}

int main(void) {
     return 0;
}
