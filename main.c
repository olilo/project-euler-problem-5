#include <stdio.h>
#include <time.h>
#include "bigd.h"

//#define DEBUG
//#define CHECK_RESULT_WITH_SEARCH
//#define SHOW_USED_TIME

// IMPORTANT: the returned BIGD has to be freed outside!
BIGD calculateLCM(int max) {
    // create all BIGD number containers for later
    BIGD lcm = bdNew();
    BIGD n = bdNew();
    BIGD primeFactor = bdNew();
    BIGD tmp = bdNew();
    BIGD primeRemainder = bdNew();
    BIGD primeQuotient = bdNew();

    // init lcm with max, because that's a given
    bdSetShort(lcm, max);

    // iterate n from (max - 1) to 1, and multiply the lcm with n (if necessary)
    for (bdSetShort(n, max - 1); !bdIsZero(n); bdDecrement(n)) {
        #ifdef DEBUG
        printf("DEBUG: Executing step ");
        bdPrint(n, BD_PRINT_NL);
        #endif
        
        // check if lcm already "contains" n
        bdModulo(tmp, lcm, n);
        if (!bdIsZero(tmp)) {
            // copy n into tmp, so that we can change tmp without affecting n
            bdSetEqual(tmp, n);

            // go through all numbers until all prime factors of n were found:
            // TODO this can be optimized to only take the prime numbers up to n
            bdSetShort(primeFactor, 2);
            while (bdCompare(primeFactor, tmp) == -1) {
                bdDivide(primeQuotient, primeRemainder, tmp, primeFactor);
                if (bdIsZero(primeRemainder)) {
                    bdSetEqual(tmp, primeQuotient);

                    // if lcm is divisible by the prime factor, then divide it too
                    bdDivide(primeQuotient, primeRemainder, lcm, primeFactor);
                    if (bdIsZero(primeRemainder)) {
                        // debugging (if enabled)
                        #ifdef DEBUG
                        printf("DEBUG: Deviding ");
                        bdPrint(lcm, 0);
                        printf("by prime factor ");
                        bdPrint(primeFactor, BD_PRINT_NL);
                        #endif

                        // do it
                        bdSetEqual(lcm, primeQuotient);
                    }
                } else {
                    // this prime factor is finished: go to the next one
                    bdIncrement(primeFactor);
                }
            }

            // finally: multiply by n
            bdMultiply(tmp, n, lcm);
            bdSetEqual(lcm, tmp);
        } else {
            #ifdef DEBUG
            printf("DEBUG: Already divisible by ");
            bdPrint(n, BD_PRINT_NL);
            #endif
        }
    }

    // free all temporary BIGD container (but NOT lcm)
    bdFree(&n);
    bdFree(&primeFactor);
    bdFree(&tmp);
    bdFree(&primeRemainder);
    bdFree(&primeQuotient);

    return lcm;
}

unsigned long int searchForLCM(int max) {
    if (max == 1) {
        return 1;
    }
    long unsigned int check;
    for (check = max * (max - 1); check <= 0xFFFFFFFFL; check += (max * (max - 1))) {
        int found = 1;
        int i;
        for (i = 2; i <= max - 2; i++) {
            found = found && (check % i == 0);
            if (!found) break;
        }
        if (found) {
            return check;
        }
    }
    
    return -1;
}

void showUsedTime(int max) {
    // clocks for calculating used time
    clock_t start_time = clock();
    clock_t end_time;

    // iterations, depending on max (because granularity of CLOCKS_PER_SEC is quite variable)
    int iterationsForCalculate;
    if (max < 10) {
        iterationsForCalculate = 100000;
    } else if (max < 100) {
        iterationsForCalculate = 10000;
    } else if (max < 500) {
        iterationsForCalculate = 1000;
    } else if (max < 5000) {
        iterationsForCalculate = 10;
    } else {
        iterationsForCalculate = 1;
    }
        
    int iter_count;
    for (iter_count = 0; iter_count < iterationsForCalculate; iter_count++) {
        BIGD ignored = calculateLCM(max);
        bdFree(&ignored);
    }
    
    // used time statistic
    end_time = clock();
    long int clockMultiplier = 1000000000L / CLOCKS_PER_SEC;
    printf("Used %ld nanoseconds\n",
        ((end_time - start_time) * clockMultiplier) / iterationsForCalculate);

    // clocks for calculating used time
    clock_t start_time_search = clock();
    clock_t end_time_search;

    int iterationsForSearch;
    if (max < 7) {
        iterationsForSearch = 10000000;
    } else if (max < 11) {
        iterationsForSearch = 1000000;
    } else if (max < 15) {
        iterationsForSearch = 10000;
    } else if (max < 19) {
        iterationsForSearch = 100;
    } else {
        iterationsForSearch = 5;
    }

    for (iter_count = 1; iter_count < iterationsForSearch; iter_count++) {
        searchForLCM(max);
    }
    
    // used time statistic
    end_time_search = clock();
    printf("Searching would need %ld nanoseconds, or %ld milliseconds\n",
        ((end_time_search - start_time_search) * clockMultiplier) / iterationsForSearch,
        ((end_time_search - start_time_search) * clockMultiplier) / 1000000 / iterationsForSearch);
}

int main(void) {
    printf("Please insert a number: ");

    // read number
    char chars[11];
    fgets(chars, 11, stdin);
    int max = atoi(chars);

    if (max <= 0) {
        printf("Sorry, only positive numbers allowed\n");
        return 2;
    }

    // find least common multiple
    BIGD lcm = calculateLCM(max);

    // print result
    size_t decimalDigits = bdConvToDecimal(lcm, NULL, 0);
    printf("Least common multiple of all natural numbers in [1..%d] ", max);
    printf("has %d octals:\n", bdSizeof(lcm));
    bdPrint(lcm, BD_PRINT_NL);
    // print lcm as decimal, too
    char lcmDecimal[decimalDigits + 1];
    bdConvToDecimal(lcm, lcmDecimal, decimalDigits + 1);
    printf("In decimal (%d digits):\n%s\n", decimalDigits, lcmDecimal);

    // check whether the result is correct
    /*
    unsigned int n;
    for (n = 1; n <= max; n++) {
        if (lcm % n > 0) {
            printf("ERROR: The found multiple is NOT divisible by %d\n", n);
            return 1;
        }
    }
    */

    #ifdef CHECK_RESULT_WITH_SEARCH
    // find correct result by simple search and compare it
    unsigned long int check = searchForLCM(max);

    if (lcm != check) {
        printf("ERROR: Found incorrect result; found %ld but should be %ld\n", lcm, check);
        return 1;
    } else {
        printf("Searching for correct value has provided the same result :)\n");
    }
    #endif

    #ifdef SHOW_USED_TIME
    showUsedTime(max);
    #endif

    bdFree(&lcm);

    // all is well, return 0
    return 0;
}
