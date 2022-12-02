#ifndef UTI_FINDINSIDE_H
#define UTI_FINDINSIDE_H

#include <cmath>

#include <str_PairedValues.hpp>
#include <_Utilities/uti_FindDistance.hpp>

namespace ColliderUtils
{
    ///-------------------------------------------------------------------------------------------------------
    ///FINDS IF SELF IS INSIDE OF TARGET RECTANGLE                  ------------------------------------------
    template <typename T>
    static inline bool
        FindInside_Square(const T& self,   ///<Expects structure with w|h|x|y members
                          const T& target) ///<Expects structure with w|h|x|y members
    {
        //half of self size
        const auto self_half   = Prs::xyPr<unsigned int>(self.w * 0.5f,
                                                         self.h * 0.5f);
        //half of target size
        const auto target_half = Prs::xyPr<unsigned int>(target.w * 0.5f,
                                                         target.h * 0.5f);
        //both halves added
        const auto total_half  = Prs::xyPr<unsigned int>(self_half.x + target_half.x,
                                                         self_half.y + target_half.y);
        //distance of both X and Y
        const auto distance    = Prs::xyPr<int>(abs(target.x - self.x) - total_half.x,
                                                abs(target.y - self.y) - total_half.y);

        //not inside X can return no collision
        if ( !signbit(distance.x) )
            return false;
        //not inside Y can return no collision
        if ( !signbit(distance.y) )
            return false;

        //collision
        return true;
    };

    ///-------------------------------------------------------------------------------------------------------
    ///FINDS IF SELF IS INSIDE OF TARGET RADIUS                     ------------------------------------------
    template <typename T>
    static inline bool
        FindInside_Radius_Q(const T& self,   ///<Expects structure with w|h|x|y members
                            const T& target) ///<Expects structure with w|h|x|y members
    {
        //distance of both X and Y
        const auto distXY = Prs::xyPr<int>(target.x - self.x,
                                           target.y - self.y);

        //perform quicker ( but less accurate ) distance check first
        const int qkDist = FindQuickDistance(distXY.x, distXY.y);
        const int both_size = self.w + target.w;
        //outside of quick distance can return no collision
        if ( qkDist > both_size )
            return false;

        //perform accurate distance check
        const int distance = FindDistance(distXY.x, distXY.y);
        const int both_halves = self.w * 0.5f + target.w * 0.5f;
        //outside of accurate distance can return no collision
        if ( distance > both_halves )
            return false;

        //collision
        return true;
    };

    ///-------------------------------------------------------------------------------------------------------
    ///FINDS IF SELF IS INSIDE OF TARGET RADIUS                     ------------------------------------------
    template <typename T>
    static inline bool
        FindInside_Radius(const T& self,   ///<Expects structure with w|h|x|y members
                          const T& target) ///<Expects structure with w|h|x|y members
    {
        //distance of both X and Y
        const auto distXY = Prs::xyPr<int>(target.x - self.x,
                                           target.y - self.y);

        //perform accurate distance check
        const int distance = FindDistance(distXY.x, distXY.y);
        const int both_halves = self.w * 0.5f + target.w * 0.5f;
        //outside of accurate distance can return no collision
        if ( distance > both_halves )
            return false;

        //collision
        return true;
    };

};

#endif // UTI_FINDINSIDE_H
