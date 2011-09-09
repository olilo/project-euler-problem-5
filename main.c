#include <stdio.h>
#include <time.h>

//#define DEBUG
//#define CHECK_RESULT_WITH_SEARCH
//#define SHOW_USED_TIME

unsigned long int calculateLCM(int max) {
    long int lcm = max;
    unsigned int i;
    // TODO find better names for: i and tmp
    for (i = max - 1; i > 1; i--) {
        if (lcm % i > 0) {
            // copy i into new variable: tmp
            unsigned int tmp = i;

            // go through all numbers until all prime factors of i were found:
            int primeFactor = 2;
            while (primeFactor < tmp) {
                if (tmp % primeFactor == 0) {
                    tmp = tmp / primeFactor;
                    // if lcm is divisible by the prime factor, then divide it too
                    if (lcm % primeFactor == 0) {
                        #ifdef DEBUG
                        printf("DEBUG: step %d - Deviding %ld by prime factor %d\n", i, lcm, primeFactor);
                        #endif
                        lcm = lcm / primeFactor;
                    }
                } else {
                    primeFactor++;
                }
            }

            // finally: multiply by i
            lcm *= i;
        } else {
            #ifdef DEBUG
            printf("DEBUG: Already divisible by %d\n", i);
            #endif
        }
    }

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

    int iter_count;
    int iterationsForCalculate = 1000000;
    for (iter_count = 1; iter_count < iterationsForCalculate; iter_count++) {
        calculateLCM(max);
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
    if (max >= 23) {
        printf("Sorry, this number is too high, maximum is 22; the resulting output exceeds uint ranges\n");
        printf("Future versions will perhaps support higher numbers\n");
        return 2;
    }

    // find least common multiple
    unsigned long int lcm = calculateLCM(max);

    // print result
    printf("Least common multiple of all natural numbers in [1..%d] is %ld\n", max, lcm);

    // check whether the result is correct
    unsigned int n;
    for (n = 1; n <= max; n++) {
        if (lcm % n > 0) {
            printf("ERROR: The found multiple is NOT divisible by %d\n", n);
            return 1;
        }
    }

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

    // all is well, return 0
    return 0;
}
