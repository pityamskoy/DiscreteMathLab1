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
        return NULL;
    }

    bigint->numbers = (unsigned int*) malloc(sizeof(unsigned int));

    return bigint;
}

void delete(Bigint* number) {
    if (number == NULL) {
        return;
    }

    if (number->numbers != NULL) {
        free(number->numbers);
    }

    free(number);
}

int inputBigint(Bigint* number) {
    if (number == NULL) {
        return 1;
    }

    unsigned int k;
    scanf("%u", &k);

    if (k == 0) {
        return 1;
    }

    number->numbers = (unsigned int*) realloc(number->numbers, sizeof(unsigned int) * (k + 1));
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

Bigint* summ(Bigint* a, Bigint* b) {
    if (a == NULL || b == NULL) {
        return NULL;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return NULL;
    }

    unsigned long long max = (a->numbers[0] > b->numbers[0]) ? a->numbers[0]: b->numbers[0];
    if (a->numbers[0] <= max) {
        unsigned int *tmp = realloc(a->numbers, sizeof(unsigned int) * (max + 2));

        if (tmp == NULL) {
            return NULL;
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
    if (a == NULL || b == NULL) {
        return NULL;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return NULL;
    }

    unsigned long long max = (a->numbers[0] > b->numbers[0]) ? a->numbers[0]: b->numbers[0];
    if (a->numbers[0] <= max) {
        unsigned int *tmp = realloc(a->numbers, sizeof(unsigned int) * (max + 2));

        if (tmp == NULL) {
            return NULL;
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

Bigint* multiply(Bigint* a, Bigint* b) {
    if (a == NULL || b == NULL) {
        return NULL;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return NULL;
    }

    unsigned int new_size = a->numbers[0] + b->numbers[0] + 1;

    unsigned int* res = calloc(new_size, sizeof(unsigned int));
    if (res == NULL) {
        return NULL;
    }

    res[0] = new_size;
    unsigned long long carry = 0;

    for (unsigned int i = 1; i <= a->numbers[0]; i++) {
        carry = 0;
        unsigned long long ai = (i == a->numbers[0]) ? (unsigned long long)a->firstDigit: (unsigned long long)a->numbers[i];

        for (unsigned int j = 1; j <= b->numbers[0]; j++) {
            unsigned long long bj = (j == b->numbers[0]) ? (unsigned long long)b->firstDigit: (unsigned long long)b->numbers[j];

            unsigned long long sum = ai * bj + res[i + j - 1] + carry;

            res[i + j - 1] = (unsigned int)(sum % BASE);
            carry = sum / BASE;
        }

        res[i + b->numbers[0]] += (unsigned int)carry;
    }

    while (res[0] > 1 && res[res[0]] == 0) {
        res[0]--;
    }

    free(a->numbers);
    a->numbers = res;

    a->firstDigit = res[res[0]];

    return a;
}

int lowWord(int value) {
    return value &((1 << sizeof(int) << 2) - 1);
}

int highWord(int value) {
    return (value >> (sizeof(value) << 2) & (1 << ((sizeof(value) << 2) - 1)));
}



int main(void) {
     return 0;
}
