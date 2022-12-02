#ifndef UTI_FINDGRIDLOCATION_H
#define UTI_FINDGRIDLOCATION_H

#include <math.h>

namespace GridMapUtils
{
    ///-------------------------------------------------------------------------------------------------------
    ///FIND SINGLE GRID LOCATION FROM ROW|COL LOC, ROW|COL SIZE, AND TOTAL SIZE OF GRIDMAP              ------
    static inline size_t
        FindGridLoc(const int loc,          //X|Y location in totalSize
                    const int totalSize,    //Total size of GridMap
                    const int gridSize)     //Size of actual grid col|row
    {
        return trunc(( totalSize - ( totalSize - loc ) ) / gridSize);
    };
};

#endif // UTI_FINDGRIDLOCATION_H
