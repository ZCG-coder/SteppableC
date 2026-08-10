#include "stp_number.h"

#include <math.h>

/*
 * ln Gamma(x) = (x - .5)ln(x) - x + 0.91894
 * log Gamma(x) = (x - .5)log10(x)
 *              - 0.4342945 x
 *              + 0.3990899
 *              + 0.0361912 / x
 */
double _log10_gamma(double x)
{
    double l_gamma = (x - 0.5) * log(x);
    l_gamma -= 0.4342945 * x;
    l_gamma += 0.3990899;
    l_gamma += 0.0361912 / x;
    l_gamma = ceil(l_gamma);

    return l_gamma;
}

uint64_t _sin_loops(int64_t wp)
{
    int64_t target_log = -(wp + 4);
    double log10_pi = 0.4971498726941339;

    for (uint64_t n = 1; n < wp + 10; ++n)
    {
        double two_n = (double)n * 2.0;
        double log_fact = _log10_gamma(two_n + 2);
        double log_term = (two_n + 1) * log10_pi - log_fact;

        if (log_term < target_log)
            return n;
    }

    return wp + 10;
}
