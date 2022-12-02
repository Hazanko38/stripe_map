#ifndef STR_PAIREDVALUES_H
#define STR_PAIREDVALUES_H

#include <algorithm>

///Both structures are the same other than variable name
namespace Prs
{
    ///STRUCTURE CONTAINING X & Y VARIABLES OF SAME TYPE
    template <typename T>
    struct xyPr
    {
        ///MEMBER VARIABLES
        T x;
        T y;

        ///DEFAULT CONSTRUCTOR = LEFT UNINITIALIZED
        ///--- ( INSTANTIATION OF XYPR WITH DEFAULT CONST
        ///---   REQUIRES OBJECTS WITH DEFAULT CONSTRUCTORS )
        xyPr()
        {
            x = T();
            y = T();
        };
        ///CONSTRUCT FROM MOVE WITH DEFINED VALUES
        xyPr(const T& nX,
             const T& nY):
            x(nX),
            y(nY)
        {};
        ///CONSTRUCT COPY
        xyPr(const xyPr& cp):
            x(cp.x),
            y(cp.y)
        {};
        ///CONSTRUCT FROM MOVE
        xyPr(xyPr&& mv):
            x(std::move(mv.x)),
            y(std::move(mv.y))
        {};
        ///CONSTRUCT FROM MOVE WITH DEFINED VALUES
        xyPr(T&& nX,
             T&& nY):
            x(std::move(nX)),
            y(std::move(nY))
        {};
        ///ASSIGNMENT OPERATOR TO ANOTHER XYPR
        inline xyPr&
            operator=(const xyPr& op)
        {
            x = op.x;
            y = op.y;

            return *this;
        };
        ///ASSIGNMENT OPERATOR TO ANOTHER TYPE WITH X & Y
        template <typename U>
        inline xyPr&
            operator=(const U& op)
        {
            x = op.x;
            y = op.y;

            return *this;
        };
        ///ASSIGNMENT OPERATOR TO ANOTHER TYPE WITH X & Y
        template <typename U>
        inline xyPr&
            operator=(const U* op)
        {
            x = op->x;
            y = op->y;

            return *this;
        };
    };

    ///STRUCTURE CONTAINING _1 & _2 VARIABLES OF SAME TYPE
    template <typename T>
    struct tpPr
    {
        ///MEMBER VARIABLES
        T _1;
        T _2;

        ///DEFAULT CONSTRUCTOR = LEFT UNINITIALIZED
        ///--- ( INSTANTIATION OF TPPR WITH DEFAULT CONST
        ///---   REQUIRES OBJECTS WITH DEFAULT CONSTRUCTORS )
        tpPr()
        {
            _1 = T();
            _2 = T();
        };
        ///CONSTRUCT FROM MOVE WITH DEFINED VALUES
        tpPr(const T& _1,
             const T& _2):
            _1(_1),
            _2(_2)
        {};
        ///CONSTRUCT COPY
        tpPr(const tpPr& cp):
            _1(cp._1),
            _2(cp._2)
        {};
        ///CONSTRUCT FROM MOVE
        tpPr(tpPr&& mv):
            _1(std::move(mv._1)),
            _2(std::move(mv._2))
        {};
        ///CONSTRUCT FROM MOVE WITH DEFINED VALUES
        tpPr(T&& _1,
             T&& _2):
            _1(std::move(_1)),
            _2(std::move(_2))
        {};
        ///ASSIGNMENT OPERATOR TO ANOTHER TPPR
        inline tpPr&
            operator=(const tpPr& op)
        {
            _1 = op._1;
            _2 = op._2;

            return *this;
        };
        ///ASSIGNMENT OPERATOR TO ANOTHER TYPE WITH _1 & _2
        template <typename U>
        inline tpPr&
            operator=(const U& op)
        {
            _1 = op._1;
            _2 = op._2;

            return *this;
        };
        ///ASSIGNMENT OPERATOR TO ANOTHER TYPE WITH _1 & _2
        template <typename U>
        inline tpPr&
            operator=(const U* op)
        {
            _1 = op->_1;
            _2 = op->_2;

            return *this;
        };
    };

    ///STRUCTURES CONTAINING _1 & _2 VARIABLES OF DIFFERENT TYPES
    template <typename T,typename U>
    struct tpsPr
    {
        ///MEMBER VARIABLES
        T _1;
        U _2;

        ///DEFAULT CONSTRUCTOR = LEFT UNINITIALIZED
        ///--- ( INSTANTIATION OF TPSPR WITH DEFAULT CONST
        ///---   REQUIRES OBJECTS WITH DEFAULT CONSTRUCTORS )
        tpsPr()
        {
            _1 = T();
            _2 = U();
        };
        ///CONSTRUCT FROM MOVE WITH DEFINED VALUES
        tpsPr(const T& _1,
              const U& _2):
            _1(_1),
            _2(_2)
        {};
        ///CONSTRUCT COPY
        tpsPr(const tpsPr& cp):
            _1(cp._1),
            _2(cp._2)
        {};
        ///CONSTRUCT FROM MOVE
        tpsPr(tpsPr&& mv):
            _1(std::move(mv._1)),
            _2(std::move(mv._2))
        {};
        ///CONSTRUCT FROM MOVE WITH DEFINED VALUES
        tpsPr(T&& _1,
              U&& _2):
            _1(std::move(_1)),
            _2(std::move(_2))
        {};
        ///ASSIGNMENT OPERATOR TO ANOTHER TPSPR
        inline tpsPr&
            operator=(const tpsPr& op)
        {
            _1 = op._1;
            _2 = op._2;

            return *this;
        };
    };

};


#endif // STR_PAIREDVALUES_H
