#include <iostream>
#include <chrono>
#include <assert.h>

#include <SDL2/SDL.h>

#include <stripe_map.hpp>
#include <_Utilities/uti_RandomGenerator.h>
#include <_Utilities/uti_FindInside.hpp>

static constexpr size_t TOTAL_AMOUNT = 5000;
static constexpr size_t CHECK_TIMES  = 10;

const int mapWidth  = 500000;
const int mapHeight = 500000;
const int mapCols = 12;
const int mapRows = 12;
const int mapWidth_s  = mapWidth / 1000;
const int mapHeight_s = mapHeight / 1000;

const int rectSize = 350;

uti_RandomGenerator uti_RandomGenerator::instance;

std::vector<SDL_Rect> stripeLoadList;
qmap::stripe_map<SDL_Rect*> stripeMap(mapWidth, 1000);

///-------------------------------------------------------------------------------------------------------
///STRIPE_MAP TEST
namespace CheckStripeMap
{
    ///
    void
        BuildOriginalList_StripeMap()
    {
        const auto beforeTime = std::chrono::high_resolution_clock::now();

        const uint64_t totalAmount = TOTAL_AMOUNT;

        for ( uint64_t i = 0; i < totalAmount; i++ )
        {
            SDL_Rect tLoc;
            tLoc.x = uti_RandomGenerator::Instance().GenerateRandom() * mapWidth_s;
            tLoc.y = uti_RandomGenerator::Instance().GenerateRandom() * mapHeight_s;
            tLoc.w = rectSize;
            tLoc.h = rectSize;

            assert(tLoc.x < mapWidth && tLoc.y < mapHeight);

            stripeLoadList.push_back(tLoc);
        }

        const auto afterTime  = std::chrono::high_resolution_clock::now();
        const auto listBuildTime = std::chrono::duration_cast<std::chrono::microseconds>
                                        (afterTime - beforeTime);

        //REPORT TIME FOR BUILDING LIST
        std::cout << "Time to build stripe_map load list: " << listBuildTime.count() << "ms" << std::endl;
    };
    ///
    void
        RunDistanceCheck_StripeMap()
    {
        const auto beforeTime = std::chrono::high_resolution_clock::now();

        const size_t totalCheck = CHECK_TIMES;

        size_t itemsChecked = 0;
        size_t itemsChecked_total = 0;
        size_t collisions = 0;

        //Checks totalChecks(N) amount of times
        for ( size_t i = 0; i < totalCheck; i++ )
        {
            //builds Hash map to check off from
            const auto beforeCreateTime = std::chrono::high_resolution_clock::now();
            stripeMap.reset();

            for ( auto & item : stripeLoadList )
                stripeMap.add(Prs::tpsPr<size_t, SDL_Rect*>(item.x, &item));

            stripeMap.shrink();

            const auto afterCreateTime = std::chrono::high_resolution_clock::now();
            const auto gridCreateTime = std::chrono::duration_cast<std::chrono::microseconds>
                                            (afterCreateTime - beforeCreateTime);
            std::cout << "Time to load stripe_map: " << gridCreateTime.count() << std::endl;

            //Checks all items in list against all other items
            for ( auto & testItem : stripeLoadList )
            {
                int beforestripe = 0;
                if ( testItem.x > 1000 )
                    beforestripe = testItem.x - 1000;
                int afterstripe = mapWidth;
                if ( testItem.x < mapWidth - 1000 )
                    testItem.x += 1000;

                const auto beginstripe = stripeMap.begin(beforestripe);
                const auto endstripe   = stripeMap.end(afterstripe);

                for ( auto i = beginstripe; i != endstripe; i++ )
                {
                    const auto & checkItem = i->_2;
                    if ( &testItem == checkItem ) continue;

                    const int distCheck = abs(testItem.x - checkItem->x) - 1000;
                    if ( !std::signbit(distCheck) )
                        continue;

                    bool inside = ColliderUtils::FindInside_Radius_Q(testItem,
                                                                     *checkItem);
                    if ( inside )
                        collisions++;

                    itemsChecked_total++;
                }
                itemsChecked++;
            }

        }

        const auto afterTime = std::chrono::high_resolution_clock::now();
        const auto distCheckTime = std::chrono::duration_cast<std::chrono::microseconds>
                                        (afterTime - beforeTime);

        //REPORT TIME FOR RUNNING DISTANCE CHECK
        std::cout << "Time to check distance: " << distCheckTime.count() << "ms" << std::endl;
        std::cout << "Items Checked: " << itemsChecked << std::endl;
        std::cout << "Total Items Checked: " << itemsChecked_total << std::endl;
        std::cout << "Total Collisions: " << collisions << std::endl;
    };
};

int main()
{
        CheckStripeMap::BuildOriginalList_StripeMap();
        CheckStripeMap::RunDistanceCheck_StripeMap();

    return 0;
}
