#ifndef UTI_RANDOMGENERATOR_H
#define UTI_RANDOMGENERATOR_H

#include <random>
#include <chrono>

class uti_RandomGenerator
{
    public:
        static uti_RandomGenerator &Instance()
        {
            return instance;
        }

        inline int
            GenerateRandom()
        {
            return rand_mt() %1000;
        }

    private:
        static uti_RandomGenerator instance;

        uti_RandomGenerator():
            rand_mt(std::time(nullptr))
        {}
        ~uti_RandomGenerator(){}

        std::mt19937 rand_mt;
};

#endif // UTI_RANDOMGENERATOR_H
