#ifndef UTI_FINDDISTANCE_H
#define UTI_FINDDISTANCE_H


#include <cmath>
#include <stdint.h>

namespace ColliderUtils
{
    static const double MAGIC_NUMBER = 1 - 1 / (std::sqrt(2) * 3.14);

    ///-------------------------------------------------------------------------------------------------------
    ///NORMAL FIND DISTANCE         --------------------------------------------------------------------------
    static inline int
        FindDistance(const int dist_x,
                     const int dist_y)
    {
        int64_t xS = dist_x * static_cast<int64_t>(dist_x);
        int64_t yS = dist_y * static_cast<int64_t>(dist_y);
        return std::sqrt(xS + yS);
    };
    ///-------------------------------------------------------------------------------------------------------
    ///USED FOR SMALLER DISTANCES AND ADDED HIGHER PRECISIONS ( THIS WILL OVERFLOW )        ------------------
    static inline int
        FindDistance(const double dist_x,
                     const double dist_y)
    {
        double xS = dist_x * dist_x;
        double yS = dist_y * dist_y;
        return std::sqrt(xS + yS);
    };

    static inline int
        FindQuickDistance(const int dist_x,
                          const int dist_y)
    {
        const int dist_s = abs(dist_x) + abs(dist_y);

        return dist_s * MAGIC_NUMBER;
    };

};

#endif // UTI_FINDDISTANCE_H
