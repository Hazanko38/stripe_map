#ifndef STRIPE_MAP_HPP
#define STRIPE_MAP_HPP

#include <cmath>

#include <uti_FindGridLocation.hpp>
#include <str_PairedValues.hpp>

namespace qmap
{
    namespace implem
    {
        ///Default setup values if nothing is provided
        static constexpr size_t SMAP_INIT_SLOT_COUNT    = 64;
        static constexpr size_t SMAP_INIT_STRIPE_AMOUNT = 8;
        static constexpr size_t SMAP_INIT_WIDTH         = 8;
        static constexpr size_t SMAP_INIT_MAX_DEPTH     = UINT32_MAX;
        static constexpr float STRIPE_EXTEND_AMOUNT     = 2.0f;

        ///-------------------------------------------------------------------------------------------------------
        ///BEGIN OF STRIPE STRUCTURE
        struct stripe
        {
            ///-------------------------------------------------------------------------------------------------------
            ///OPERATIONAL FUNCTIONS

            ///Returns true if add is available and adjusting of stripe(s) succeeds
            ///--- ( FAILURE OF THIS FUNCTION INDICATES STRIPE_MAP MUST BE REBUILT )
            inline const Prs::tpsPr<bool, size_t>
                add()
            {
                //Current stripe not full
                if ( avail_now() && add_slot_now() )
                    return Prs::tpsPr<bool, size_t>(true, _stripe_position - 1);

                return Prs::tpsPr<bool, size_t>(false, 0);
            };
            inline const Prs::tpsPr<bool, size_t>
                erase(const size_t eraseIndex)
            {
                //Nothing to erase here
                if ( is_empty() )
                    return Prs::tpsPr<bool, size_t>(false, eraseIndex);

                const size_t posminus1 = _stripe_position - 1;

                size_t swapposition = eraseIndex;
                if ( eraseIndex != posminus1 )
                    swapposition = posminus1;

                _stripe_position--;

                return Prs::tpsPr<bool, size_t>(true, swapposition);
            };
            inline auto
                clear_stripe()
            {
                if ( is_empty() )
                    return Prs::tpsPr<bool, size_t>(false, 0);

                const size_t clearamount = used();

                _stripe_position = _stripe_start;

                return Prs::tpsPr<bool, size_t>(true, clearamount);
            };
            ///Assigns critical values during stripe rebuild
            inline stripe&
                operator=(const stripe& ostripe)
            {
                _stripe_depth      = ostripe._stripe_depth;
                _stripe_count      = ostripe._stripe_count;
                _stripe_start      = ostripe._stripe_start;
                _stripe_position   = ostripe._stripe_position;
                _stripe_end        = ostripe._stripe_end;

                return *this;
            };
            ///Returns starting index of stripe + supplied stripeIndex
            inline size_t
                operator[](size_t stripeIndex)
            {
                if ( stripeIndex > _stripe_count )
                    stripeIndex = _stripe_count;

                return _stripe_start + stripeIndex;
            };

            ///-------------------------------------------------------------------------------------------------------
            ///INFO FUNCTIONS

            ///Returns the width ( amount of total slots ) of this stripe
            inline size_t
                width(){
                return _stripe_end - _stripe_start;
            };
            ///Returns the amount of available slots within this stripe
            inline size_t
                available(){
                return _stripe_end - _stripe_position;
            };
            ///Returns amount of slots used in this stripe
            inline size_t
                used(){
                return _stripe_position - _stripe_start;
            };
            ///Returns true if there is an available slot in this stripe
            inline bool
                avail_now(){
                return _stripe_position != _stripe_end;
            };
            ///Returns true if there are no available slots
            inline bool
                is_empty(){
                return _stripe_position == _stripe_start ;
            };
            ///Returns the starting index position of this stripe
            inline size_t
                get_start(){
                return _stripe_start;
            };
            ///Returns the current index position of this stripe
            inline size_t
                get_position(){
                return _stripe_position;
            };
            ///Returns the ending index position of this stripe
            inline size_t
                get_end(){
                return _stripe_end;
            };
            ///Returns the current starting depth of this stripe
            inline size_t
                get_depth(){
                return _stripe_depth;
            };
            ///Returns true if there exists a stripe previous to this one
            inline bool
                avail_stripe_prev(){
                return _stripe_previous != nullptr;
            };
            ///Returns true if there exists a stripe after this one
            inline bool
                avail_stripe_next(){
                return _stripe_next != nullptr;
            };
            ///Returns pointer to the previous stripe ( nullptr if this is the first stripe )
            inline auto
                get_prev(){
                return _stripe_previous;
            };
            ///Returns pointer to the next stripe ( nullptr if this is the last stripe )
            inline auto
                get_next(){
                return _stripe_next;
            };

            ///-------------------------------------------------------------------------------------------------------
            ///SEARCH FUNCTIONS

            ///Returns true if the given depth matches this stripe's depth, while below next stripe depth
            inline bool
                depth_match(const size_t depth)
            {
                if ( !avail_stripe_next() )
                    return true;

                return depth >= _stripe_depth && depth < get_next()->_stripe_depth;
            };
            ///Returns true if the given index is between this stripe's start and current position
            inline bool
                index_match(const size_t slotIndex)
            {
                return slotIndex >= _stripe_start && slotIndex < _stripe_position;
            };

            ///-------------------------------------------------------------------------------------------------------
            ///INIT FUNCTIONS

            ///Sets pointer to previous stripe ( or nullptr if this is the first stripe )
            inline void
                set_stripe_prev(stripe* stripePrev)
            {
                _stripe_previous = stripePrev;
            };
            ///Sets pointer to next stripe ( or nullptr if this is the last stripe )
            inline void
                set_stripe_next(stripe* stripeNext)
            {
                _stripe_next = stripeNext;
            };
            ///Sets the stripe depth information
            inline void
                set_stripe_depth(const size_t depth)
            {
                _stripe_depth = depth;
            };
            ///Sets the start index of this stripe
            inline void
                set_stripe_start(const size_t sStart,
                                 const size_t sPos = 0)
            {
                _stripe_start    = sStart;
                _stripe_position = sStart + sPos;
            };
            ///Sets the end index of this stripe
            inline void
                set_stripe_end(const size_t sEnd)
            {
                _stripe_end = sEnd;
            };
            ///Trims the stripe's end position to the current insert position
            inline size_t
                trim_stripe_end(const size_t trimoffset = 0)
            {
                const size_t trimamount = _stripe_end - _stripe_position;

                _stripe_start    -= trimoffset;
                _stripe_position -= trimoffset;
                _stripe_end       = _stripe_position;

                return trimamount;
            };

            private:
                ///Adjusts this stripe info to accommodate add
                inline bool
                    add_slot_now()
                {
                    _stripe_position++;
                    _stripe_count++;

                    return true;
                };

            ///Internal stripe variables
            stripe* _stripe_previous = nullptr;  ///< previous stripe if there is one
            stripe* _stripe_next     = nullptr;  ///< next stripe if there is one

            size_t _stripe_start    = 0;         ///< start of stripe item pointers
            size_t _stripe_position = 0;         ///< current location of entry
            size_t _stripe_end      = 0;         ///< width of stripe ( end of stripe slots index )

            size_t _stripe_depth = 0;            ///< total depth into stripe_map
            size_t _stripe_count = 0;            ///< slots filled within stripe
        };

        ///-------------------------------------------------------------------------------------------------------
        ///HELPER FUNCTIONS

        ///FINDER FUNCTIONS
        namespace
        {
            ///Returns stripe corresponding to given depth ( SCANS )
            static inline stripe*
                find_stripe_match_depth(stripe* stripePtr,
                                        const size_t depthKey)
            {
                while ( stripePtr != nullptr )
                {
                    if ( stripePtr->depth_match(depthKey) )
                        return stripePtr;

                    stripePtr = stripePtr->get_next();
                }

                return nullptr;
            };
            ///Returns pointer to stripe given depthKey and current depthMax of stripe_map
            ///--- ( EXPECTS POINTER TO FIRST STRIPE - BOUND CHECKING FOR DEPTHKEY TO MAX IS NOT PERFORMED )
            static inline auto
                find_stripe_jump_depth(stripe* stripePtr,
                                       size_t depthKey,
                                       size_t depthMax)
            {
                const size_t stripeDepth = stripePtr->get_next()->get_depth();
                const size_t stripeindex = trunc(( depthMax - ( depthMax - depthKey ) ) / stripeDepth );

                return &stripePtr[stripeindex];
            };
            ///Returns first stripe that aligns with given slotIndex ( SCANS )
            ///--- ( EXPECTS POINTER TO FIRST STRIPE OR INDEX ALIGNED WITH SUPPLIED POINTER )
            static inline const auto
                find_stripe_from_adjd_index(stripe* stripePtr,
                                       const size_t slotIndex)
            {
                size_t currindex = slotIndex;
                while ( stripePtr != nullptr )
                {
                    if ( stripePtr->is_empty() )
                        goto next;
                {
                    //Width (total capac) of current stripe
                    const auto stripeused = stripePtr->used();

                    //Return curr stripe and item index in stripe_map array if currindex aligns with stripe
                    if ( stripeused > currindex )
                        return Prs::tpsPr<stripe*, size_t>(stripePtr,
                                                              stripePtr->get_start() + currindex);

                    //Reduce currindex by stripeused per pass to check alignment
                    currindex -= stripeused;
                }   //start of next
                next:

                    stripePtr = stripePtr->get_next();
                };

                return Prs::tpsPr<stripe*, size_t>(nullptr, 0);
            };
            ///
            static inline const auto
                find_stripe_start_index_adj(stripe* stripePtr,
                                            const size_t slotIndex)
            {
                size_t adjindex = 0;
                while ( stripePtr != nullptr )
                {
                    if ( stripePtr->is_empty() )
                        goto next;
                {
                    if ( stripePtr->index_match(slotIndex) )
                        return Prs::tpsPr<stripe*, size_t>(stripePtr,
                                                           adjindex);

                    adjindex += stripePtr->used();
                } //start of next
                next:

                    stripePtr = stripePtr->get_next();
                };

                return Prs::tpsPr<stripe*, size_t>(nullptr, 0);
            };
            ///
            static inline size_t
                find_start_index_adj(stripe* stripePtr,
                                     const size_t slotIndex)
            {
                size_t adjindex = 0;
                while ( stripePtr != nullptr )
                {
                    if ( stripePtr->is_empty() )
                        goto next;
                {
                    if ( stripePtr->index_match(slotIndex) )
                        return adjindex;

                    adjindex += stripePtr->used();
                }
                next:

                    stripePtr = stripePtr->get_next();
                }

                return 0;
            };
            ///
            static inline size_t
                find_start_index_adj_match_depth(stripe* stripePtr,
                                                 const size_t depthKey)
            {
                size_t adjindex = 0;
                while ( stripePtr != nullptr )
                {
                    if ( stripePtr->is_empty() )
                        goto next;
                {
                    if ( stripePtr->depth_match(depthKey) )
                        return adjindex;

                    adjindex += stripePtr->used();
                }
                next:

                    stripePtr = stripePtr->get_next();
                }

                return 0;
            };
            ///
            static inline size_t
                find_end_index_adj_match_depth(stripe* stripePtr,
                                               const size_t depthKey)
            {
                size_t adjindx = 0;
                while ( stripePtr != nullptr )
                {
                    if ( stripePtr->is_empty() )
                        goto next;
                {
                    adjindx += stripePtr->used();

                    if ( stripePtr->depth_match(depthKey) )
                        return adjindx;
                }
                next:

                    stripePtr = stripePtr->get_next();
                }

                return 0;
            };
            ///Returns index number back while ignoring stripes entirely
            ///--- ( THIS IS TO BE USED WHEN STRIPES HAVE NO GAPS & PERF NEEDED )
            static inline const auto
                find_stripe_nogap_index(stripe* stripePtr,
                                        const size_t slotIndex)
            {
                while ( stripePtr != nullptr )
                {
                    if ( stripePtr->is_empty() )
                        goto next;
                {
                    if ( slotIndex >= stripePtr->get_start() )
                        return Prs::tpsPr<stripe*, size_t>(stripePtr, slotIndex);
                }   //start of next
                next:

                    stripePtr = stripePtr->get_next();
                }

                return Prs::tpsPr<stripe*, size_t>(nullptr, 0);
            };
            ///Returns first stripe that aligns with given slotIndex ( SCANS )
            ///--- ( EXPECTS POINTER TO FIRST STRIPE OR INDEX ALIGNED WITH SUPPLIED POINTER )
            static inline const size_t
                find_adjd_index(stripe* stripePtr,
                                const size_t slotIndex)
            {
                size_t currindex = slotIndex;
                while ( stripePtr != nullptr )
                {
                    if ( stripePtr->is_empty() )
                        goto next;
                {
                    //Width (total capac) of current stripe
                    const auto stripeused = stripePtr->used();

                    //Return curr stripe and item index in stripe_map array if currindex aligns with stripe
                    if ( stripeused > currindex )
                        return stripePtr->get_start() + currindex;

                    //Reduce currindex by stripeused per pass to check alignment
                    currindex -= stripeused;
                }   //start of next
                next:

                    stripePtr = stripePtr->get_next();
                };

                return 0;
            };

        };  //end of finder functions namespace

        ///INIT SETUP FUNCTIONS
        namespace
        {
            ///Calculates depth of each stripe ( as portion of grid percent )
            static inline size_t
                calc_init_depth_increm(const size_t stripeAmnt,
                                       const size_t depthMax)
            {
                return depthMax / stripeAmnt;
            };

        };  //end of init setup functions namespace

        ///STRIPE SETUP FUNCTIONS
        namespace
        {
            ///Creates initialization stripe info structures and performs setup
            static inline stripe*
                make_init_stripes(const size_t stripeAmnt,
                                  const size_t stripeWidth,
                                  const size_t stripeDepth)
            {
                auto newstripes = new stripe[stripeAmnt];

                for ( size_t i = 0; i < stripeAmnt; i++ )
                {
                    auto & currstripe = newstripes[i];

                    //Set stripe neighbor pointers where applicable
                    if ( i != 0 )
                        currstripe.set_stripe_prev(&newstripes[i-1]);
                    if ( i != stripeAmnt - 1 )
                        currstripe.set_stripe_next(&newstripes[i+1]);

                    //Set start and end index positions of current stripe
                    currstripe.set_stripe_start(stripeWidth * i);
                    currstripe.set_stripe_end(stripeWidth * (i + 1));
                    //Set the starting depth of the current stripe
                    currstripe.set_stripe_depth(stripeDepth * i);
                };

                return newstripes;
            };
            ///Copies relevant information to new stripes and adjusts start+end indices
            ///--- ( Returns new required item slot # )
            static inline size_t
                fix_new_stripes(stripe* newStripes,
                                stripe* oldStripes,
                                const size_t stripeAmnt)
            {
                size_t currslotindex = 0;
                for ( size_t i = 0; i < stripeAmnt; i++ )
                {
                    //Assign relevant info from old stripe to new stripe
                    newStripes[i] = oldStripes[i];  //THIS IS NOT POINTER ASSIGNMENT

                    //Set pointers to next and previous stripes when applicable
                    if ( i != 0 )
                        newStripes[i].set_stripe_prev(&newStripes[i-1]);
                    if ( i != stripeAmnt - 1 )
                        newStripes[i].set_stripe_next(&newStripes[i+1]);

                    size_t newwidth = newStripes[i].width();
                    const size_t halfwidth = ceil(newwidth * 0.5f);

                    const auto stripeused = newStripes[i].used();

                    //Extend stripe width if it exceeds half capacity
                    if ( stripeused >= halfwidth )
                        newwidth = stripeused * STRIPE_EXTEND_AMOUNT;
                    else if ( stripeused == 0 )
                        newwidth = SMAP_INIT_WIDTH;

                    newStripes[i].set_stripe_start(currslotindex, stripeused);
                    //Increment index based on new stripe width
                    currslotindex += newwidth;
                    newStripes[i].set_stripe_end(currslotindex);

                    #if DEBUG_SMAP > 4
                        std::cout << "s|e : " << newStripes[i].get_start() << " | " << newStripes[i].get_end() << std::endl;
                        std::cout << "Stripe used slots-> " << stripeused << std::endl;
                        std::cout << "NEW STRIPE WIDTH->  " << newwidth << std::endl;
                    #endif // DEBUG_SMAP
                }

                return currslotindex;
            };
            ///Sets up new stripe array from old stripe array during expansion(re-striping)
            ///--- ( Returns pointer to new array of stripes adjusted to new slot requirement )
            static inline stripe*
                setup_new_stripes(stripe* oldStripes,
                                  const size_t stripeAmnt,
                                  size_t& slotCount)
            {
                //New stripes to be filled
                auto newstripes = new stripe[stripeAmnt];

                //Updates stripe_map slot count from new stripes
                slotCount = fix_new_stripes(newstripes,
                                            oldStripes,
                                            stripeAmnt);

                return newstripes;
            };

        }; //end of stripe setup function namespace

        ///ITEM ARRAY SETUP FUNCTIONS
        namespace
        {
            ///Moves stripe items to their proper locations in new stripe
            template <typename V>
            static inline void
                move_stripe_items(Prs::tpsPr<size_t, V>* newItems,
                                  Prs::tpsPr<size_t, V>* oldItems,
                                  stripe* newStripe,
                                  stripe* oldStripe)
            {
                while ( newStripe != nullptr )
                {
                    const size_t oldstripeused = oldStripe->used();

                    //Move to next old stripe and continue if stripe has no used slots
                    if ( oldstripeused == 0 )
                        goto next;
                {
                    //Copy stripe from old items to new items at proper index
                    for ( size_t i = 0; i < oldstripeused; i++ )
                        newItems[newStripe->get_start() + i] = std::move(oldItems[oldStripe->get_start() + i]);

                    #if DEBUG_SMAP > 5
                        std::cout << "copied " << sizeof(Prs::tpsPr<size_t, V>)
                                  << " * " << oldstripeused << " bytes to...\n";
                        std::cout << newStripe->get_start() << " from " << oldStripe->get_start() << std::endl;
                    #endif
                }   //start of next
                next:

                    //Move to next stripes
                    newStripe = newStripe->get_next();
                    oldStripe = oldStripe->get_next();
                }
            };
            ///Setup new item array from old item array during expansion(re-striping)
            ///--- ( Returns pointer to new array of items striped according to new & old stripes )
            template <typename V>
            static inline Prs::tpsPr<size_t, V>*
                setup_new_items(Prs::tpsPr<size_t, V>* oldItems,
                                stripe* newStripes,
                                stripe* oldStripes,
                                const size_t slotCount)
            {
                //New item array to be accommodate resizing
                auto newitems = new Prs::tpsPr<size_t, V>[slotCount];

                move_stripe_items(newitems,
                                  oldItems,
                                  newStripes,
                                  oldStripes);

                return newitems;
            };
            ///Shrinks items to single contiguous array
            ///--- ( Returns pointer to new array of contiguous items & adjusts stripes to match new indices )
            template <typename V>
            static inline Prs::tpsPr<size_t, V>*
                shrink_items(Prs::tpsPr<size_t, V>* oldItems,
                             stripe* stripePtr,
                             const size_t itemCount)
            {
                auto newitems = new Prs::tpsPr<size_t, V>[itemCount];

                size_t moveindex  = 0;
                size_t trimoffset = 0;
                while ( stripePtr != nullptr )
                {
                    const size_t stripeused = stripePtr->used();

                    //Copy stripe from old items to new items at proper index
                    for ( size_t i = 0; i < stripeused; i++ )
                        newitems[moveindex + i] = std::move(oldItems[stripePtr->get_start() + i]);

                    const size_t stripeoffset = stripePtr->trim_stripe_end(trimoffset);

                    #if DEBUG_SMAP > 5
                        std::cout << "stripe used: " << stripeused << " offset: " << stripeoffset << std::endl;
                        std::cout << "move index: " << moveindex << " trimoffset: " << trimoffset << std::endl;
                    #endif

                    trimoffset += stripeoffset;
                    moveindex  += stripeused;

                    stripePtr = stripePtr->get_next();
                }

                return newitems;
            };

        };  //end of item array setup functions namespace
        ///ITEM ARRAY CONTROL FUNCTIONS
        namespace
        {
            ///Erases item from given stripe pointer and item array
            ///--- ( Expects given stripe to already align given eraseIndex )
            template <typename V>
            static inline bool
                erase_item_from_stripe(Prs::tpsPr<size_t, V>* itemsPtr,
                                       stripe* stripePtr,
                                       const size_t eraseIndex)
            {
                const auto erasesuccess = stripePtr->erase(eraseIndex);

                //Erase failed return early
                if ( !erasesuccess._1 )
                    return false;

                //Swaps good item into erased item's location if erase wasn't end of stripe
                if ( eraseIndex != erasesuccess._2 )
                    itemsPtr[eraseIndex]  = std::move(itemsPtr[erasesuccess._2]);

                //Erase success
                return true;
            };
            ///Removes all items from from stripes that match given value
            template <typename V, typename F>
            static inline auto
                remove_values_from_stripes(Prs::tpsPr<size_t, V>* itemsPtr,
                                           stripe* stripePtr,
                                           const size_t rmvIndStart,
                                           const size_t rmvIndEnd,
                                           F&& check)
            {
                bool removesuccess   = false;
                size_t removeamount  = 0;
                while ( stripePtr != nullptr )
                {
                    if ( stripePtr->is_empty() )
                        goto next;
                {
                    size_t stripestart = stripePtr->get_start();
                    if ( stripestart < rmvIndStart )
                        stripestart = rmvIndStart;

                    size_t stripepos = stripePtr->get_position();
                    if ( stripepos > rmvIndEnd )
                        stripepos = rmvIndEnd;

                    for ( size_t i = stripestart; i < stripepos; )
                    {
                        if ( check(itemsPtr[i]) )
                        {
                            removesuccess |= erase_item_from_stripe(itemsPtr,
                                                                    stripePtr,
                                                                    i);
                            stripepos--;
                            removeamount++;
                            //Check index again as item may have been swapped
                            continue;
                        }
                        i++;
                    }

                }   //start of next
                next:

                    stripePtr = stripePtr->get_next();
                };

                return Prs::tpsPr<const bool, const size_t>(removesuccess,
                                                            removeamount);
            };
            ///Removes all items from ite
            ///Sets stripe position to start - effectively clearing the stripe of it's items
            static inline auto
                clear_entire_stripe(stripe* stripePtr)
            {
                //Return whether erase succeeded or not from stripePtr (SHOULD indicate empty)
                return stripePtr->clear_stripe();
            };
            ///Sets all stripe positions to start - effectively clearing all stripes of their items
            static inline auto
                clear_stripe_all(stripe* stripePtr)
            {
                bool clearsuccess  = false;
                size_t clearamount = 0;
                while ( stripePtr != nullptr )
                {
                    const auto clearstripe = stripePtr->clear_stripe();

                    clearsuccess |= clearstripe._1;
                    clearamount  += clearstripe._2;

                    stripePtr = stripePtr->get_next();
                }

                return Prs::tpsPr<const bool, const size_t>(clearsuccess,
                                                            clearamount);
            };

        };  //end of item array control functions namespace

    };  //end of helper namespace

    ///-------------------------------------------------------------------------------------------------------
    ///BEGIN OF STRIPE_MAP CLASS
    template <typename V>
    class stripe_map
    {
        struct iterator;

        public:
            ///MAKE STRIPE_MAP
            stripe_map(const size_t depthMax = implem::SMAP_INIT_MAX_DEPTH,
                       const size_t stripeAmnt = implem::SMAP_INIT_STRIPE_AMOUNT,
                       const size_t stripeWdth = implem::SMAP_INIT_WIDTH)

            {
                init(stripeAmnt,
                     stripeWdth,
                     depthMax);
            };
            ///CLEANUP
            virtual ~stripe_map()
            {
                (this->*_smap_close_func_)();
            };

            ///-------------------------------------------------------------------------------------------------------
            ///OPERATIONAL FUNCTIONS

            ///Add to the stripe_map
            ///--- ( COPY ADD )
            inline bool
                add(const Prs::tpsPr<size_t, V>& aItem)
            {
                auto attemptadd = attempt_add(aItem);

                if ( !attemptadd._1 )
                    return false;

                _smap_items[attemptadd._2] = aItem;

                return true;
            };
            ///Add to the stripe_map
            ///--- ( MOVE ADD )
            inline bool
                add(Prs::tpsPr<size_t, V>&& aItem)
            {
                auto attemptadd = attempt_add(aItem);

                if ( !attemptadd._1 )
                    return false;

                _smap_items[attemptadd._2] = std::move(aItem);

                return true;
            };
            ///Shrink stripes to remove gaps and provide array of contiguous memory
            ///--- ( Any add or remove/clear operations will lose performance gain )
            inline void
                shrink()
            {
                (this->*_smap_shrink_func_)();
            };
            ///Resizes the size of stripe_map to given values and resets
            inline void
                resize(const size_t depthMax = implem::SMAP_INIT_MAX_DEPTH,
                       const size_t stripeAmnt = implem::SMAP_INIT_STRIPE_AMOUNT,
                       const size_t stripeWdth = implem::SMAP_INIT_WIDTH)
            {
                (this->*_smap_close_func_)();

                init(stripeAmnt,
                     stripeWdth,
                     depthMax);
            };
            ///Resets entire stripe_map to default values
            inline void
                reset()
            {
                (this->*_smap_close_func_)();

                reset_values();
            };
            ///Iterator position erase
            inline auto
                erase(const iterator& eraseIt)
            {
                const auto erasesucc = erase_item(eraseIt.sm_index);

                return Prs::tpsPr<const bool, const size_t>(erasesucc,
                                                            _smap_items_count);
            };
            ///Clears all stripes of their items yet retains current size of stripe_map
            inline auto
                clear()
            {   using namespace implem;

                const auto clearsucc = clear_stripe_all(_smap_stripes);

                if ( clearsucc._1 )
                    _smap_items_count -= clearsucc._2;

                return clearsucc;
            };
            ///Clears entire stripe at given depth
            inline auto
                clear_depth(const size_t depthMatch)
            {
                const auto clearsucc = clear_stripe(depthMatch);

                return Prs::tpsPr<const bool, const size_t>(clearsucc,
                                                            _smap_items_count);
            };
            ///Removes all items matching given value from given range
            inline auto
                remove(const iterator& removeBegin,
                       const iterator& removeEnd,
                       const V& value)
            {
                const auto removesuccess = remove_items(removeBegin.sm_index,
                                                        removeEnd.sm_index,
                                                        [value](const Prs::tpsPr<size_t, V>& item)
                                                            { return item._2 == value; });

                return Prs::tpsPr<const bool, const size_t>(removesuccess,
                                                            _smap_items_count);
            };
            ///Removes all items matching given key from given range
            inline auto
                remove_key(const iterator& removeBegin,
                           const iterator& removeEnd,
                           const size_t depthKey)
            {
                const auto removesuccess = remove_items(removeBegin.sm_index,
                                                        removeEnd.sm_index,
                                                        [depthKey](const Prs::tpsPr<size_t, V>& item)
                                                            { return item._1 == depthKey; });

                return Prs::tpsPr<const bool, const size_t>(removesuccess,
                                                            _smap_items_count);
            };
            ///Removes all items matching given predicate function from given range
            template <typename F>
            inline auto
                remove_if(const iterator& removeBegin,
                          const iterator& removeEnd,
                          F&& predicate)
            {
                const auto removesuccess = remove_items(removeBegin.sm_index,
                                                        removeEnd.sm_index,
                                                        predicate);

                return Prs::tpsPr<const bool, const size_t>(removesuccess,
                                                            _smap_items_count);
            };

            ///-------------------------------------------------------------------------------------------------------
            ///LOOKUP FUNCTIONS

            ///Returns item aligned with non-empty stripes ( or direct index to stripe_map while shrunk )
            inline Prs::tpsPr<size_t, V>&
                operator[](const size_t smIndex)
            {   using namespace implem;

                if ( _smap_is_shrunk )
                    return _smap_items[smIndex];

                return _smap_items[find_adjd_index(_smap_stripes,
                                                   smIndex)];
            };

            ///-------------------------------------------------------------------------------------------------------
            ///INFO FUNCTIONS

            ///Current stored items(slots used) in stripe_map
            inline size_t
                size(){
                return _smap_items_count;
            };
            ///Current total slot count in stripe_map
            inline size_t
                slots(){
                return _smap_slots_count;
            };
            ///Current total stripe amount in stripe_map
            inline size_t
                stripes(){
                return _smap_stripe_stripes;
            };
            ///Current depth per stripe of stripe_map
            inline size_t
                depth(){
                return _smap_stripe_depth;
            };
            ///Current max depth of stripe_map
            inline size_t
                depthmax(){
                return _smap_depth_max;
            }
            ///Stripe_map begin iterator
            auto
                begin(){
                return iterator(this, 0);
            };
            ///Stripe_map end iterator
            auto
                end(){
                return iterator(this, _smap_items_count);
            };
            ///Returns iterator to the begin index of stripe matching given depthMatch
            auto
                begin(const size_t depthMatch)
            {   using namespace implem;

                const size_t adjindex = find_start_index_adj_match_depth(_smap_stripes,
                                                                         depthMatch);

                return iterator(this, adjindex);
            };
            ///Returns iterator to the end index of stripe matching given depthMatch
            auto
                end(const size_t depthMatch)
            {   using namespace implem;

                const size_t adjindex = find_end_index_adj_match_depth(_smap_stripes,
                                                                       depthMatch);

                return iterator(this, adjindex);
            };

        private:
            ///Stripe_map custom iterator struct
            struct iterator
            {
                friend class stripe_map;

                using iterator_category = std::random_access_iterator_tag;
                using difference_type   = std::ptrdiff_t;
                using controller        = stripe_map*;
                using value_type        = Prs::tpsPr<size_t, V>;
                using pointer           = Prs::tpsPr<size_t, V>*;
                using reference         = Prs::tpsPr<size_t, V>&;

                reference
                    operator*(){
                    return (*sm_ctrl)[sm_index];
                };
                pointer
                    operator->(){
                    return &(*sm_ctrl)[sm_index];
                };
                iterator&
                    operator--()
                {
                    sm_index--;
                    return *this;
                };
                iterator
                    operator--(int)
                {
                    iterator tmp = *this;
                    --(*this);
                    return tmp;
                };
                iterator&
                    operator++()
                {
                    sm_index++;
                    return *this;
                };
                iterator
                    operator++(int)
                {
                    iterator tmp = *this;
                    ++(*this);
                    return tmp;
                };
                iterator&
                    operator+=(int i)
                {
                    iterator tmp = *this;
                    (*this)+= i;
                    return tmp;
                };
                iterator
                    operator+(int i)
                {
                    sm_index += i;
                    return *this;
                };
                iterator&
                    operator-=(int i)
                {
                    iterator tmp = *this;
                    (*this)-= i;
                    return tmp;
                };
                iterator
                    operator-(int i)
                {
                    sm_index -= i;
                    return *this;
                };
                reference
                    operator[](int i){
                    return (*sm_ctrl)[sm_index + i];
                }

                friend bool
                    operator==(const iterator& a,
                               const iterator& b){
                    return a.sm_index == b.sm_index;
                };
                friend bool
                    operator!=(const iterator& a,
                               const iterator& b){
                    return a.sm_index != b.sm_index;
                };
                friend bool
                    operator>(const iterator& a,
                              const iterator& b)
                {
                    return a.sm_index > b.sm_index;
                };
                friend bool
                    operator<(const iterator& a,
                              const iterator& b)
                {
                    return a.sm_index < b.sm_index;
                };

                iterator(controller ctrl,
                        const size_t index = 0):
                    sm_ctrl(ctrl),
                    sm_index(index)
                {};
                virtual ~iterator(){};

                protected:
                    controller sm_ctrl;
                    size_t sm_index;
            };

            ///Returns bool for success and
            ///--- ( WILL PERFORM RESTRIPE ON STRIPE ADD ATTEMPT FAILURE )
            inline const auto
                attempt_add(const Prs::tpsPr<size_t, V>& aItem)
            {
                (this->*_smap_init_func_)();

                const auto attempt1 = find_attempt_add(aItem);

                //Add to stripe slot index viable
                if ( attempt1._1 )
                    return attempt1;

                //If add fails, attempt to re-stripe
                restripe();

                return find_attempt_add(aItem);
            };
            ///Attempts to find stripe and request add
            ///--- ( DOES NOT ATTEMPT TO RESTRIPE )
            inline const auto
                find_attempt_add(const Prs::tpsPr<size_t, V>& aItem)
            {   using namespace implem;

                auto stripefind = find_stripe_jump_depth(_smap_stripes,
                                                         aItem._1,
                                                         _smap_depth_max);

                #if DEBUG_SMAP > 0
                    assert(stripefind != nullptr);
                #endif

                const auto attemptadd = stripefind->add();

                if ( attemptadd._1 )
                {
                    _smap_items_count++;

                    #if DEBUG_SMAP > 1
                        std::cout << "ADDED TO [" << attemptadd._2 << "]" << std::endl;
                    #endif
                };

                //Returns <bool, size_t> pair for success and viable add index
                return attemptadd;
            };
            ///Delete entire item array
            inline void
                delete_items()
            {
                delete[] _smap_items;
            };
            ///Delete entire stripe array
            inline void
                delete_stripes()
            {
                delete[] _smap_stripes;
            };
            ///Delete entirety of both item and stripe array
            inline void
                delete_both()
            {
                delete_items();
                delete_stripes();
            };
            ///All is lost and internal item pointers and stripes are rebuilt to init state
            inline void
                init(size_t stripeAmnt,
                     size_t stripeWdth,
                     size_t depthMax)
            {   using namespace implem;

                if ( stripeAmnt < SMAP_INIT_STRIPE_AMOUNT )
                    stripeAmnt = SMAP_INIT_STRIPE_AMOUNT;
                else if ( stripeAmnt > depthMax )
                    stripeAmnt = depthMax;
                if ( depthMax == 0 )
                    depthMax = SMAP_INIT_MAX_DEPTH;
                if ( stripeWdth < 1 )
                    stripeWdth = 1;

                set_values(stripeAmnt, stripeWdth, depthMax);
            };
            ///Creates enough slots+stripes to accommodate initialization size ( performed on first add to empty map )
            inline void
                reserve()
            {   using namespace implem;

                make_slots();
                _smap_stripes = make_init_stripes(_smap_stripe_stripes,
                                                  _smap_slots_width,
                                                  _smap_stripe_depth);
            };
            ///Initializes 'newSz' amount of slots given input
            ///--- ( THIS EXPECTS SMAP_ITEMS IS CLEAR OR BEEN DELETED BEFORE USE )
            inline void
                make_slots()
            {
                _smap_items = new Prs::tpsPr<size_t, V>[_smap_slots_count];
            };
            ///Set internal initial values to allow space for first item insertions
            ///--- ( Allows space for items to be inserted before resize and )
            ///--- ( reallocation is required. Initial size can be set via constructor )
            ///--- ( or reset(). Default init sets width to be same as total stripes )
            inline void
                set_values(const size_t stripeAmnt,
                           const size_t stripeWdth,
                           const size_t depthMax)
            {   using namespace implem;

                _smap_stripe_stripes = stripeAmnt;
                _smap_slots_width    = stripeWdth;
                _smap_depth_max      = depthMax;
                _smap_stripe_depth   = calc_init_depth_increm(_smap_stripe_stripes,
                                                            _smap_depth_max);

                reset_values();

                #if DEBUG_SMAP > 0
                    std::cout << "Init values ------------" << std::endl;
                    std::cout << "Slot count: " << _smap_slots_count << std::endl;
                    std::cout << "Depth maximum: " << _smap_depth_max << std::endl;
                    std::cout << "Stripe amount: " << _smap_stripe_stripes << std::endl;
                    std::cout << "Stripe depth: " << _smap_stripe_depth << std::endl;
                #endif
            };
            ///Set item and slot count to init values & set internal function pointers
            inline void
                reset_values()
            {
                _smap_items_count = 0;
                _smap_slots_count = _smap_stripe_stripes * _smap_slots_width;
                _smap_init_func_   = &stripe_map::init_reserve;
                _smap_shrink_func_ = &stripe_map::func_void;
            };
            ///Restripes stripe_map
            inline void
                restripe()
            {   using namespace implem;

                auto newstripes = setup_new_stripes(_smap_stripes,
                                                    _smap_stripe_stripes,
                                                    _smap_slots_count);

                auto newitems   = setup_new_items(_smap_items,
                                                  newstripes,
                                                  _smap_stripes,
                                                  _smap_slots_count);

                #if DEBUG_SMAP > 2
                    std::cout << "New slot count:    " << _smap_slots_count << std::endl << std::endl;
                #endif // DEBUG_SMAP

                //Delete items and stripe previous arrays
                delete_both();
                //Assign new items and stripes to stripe_map members
                _smap_stripes = newstripes;
                _smap_items   = newitems;

                _smap_index_search_ = find_stripe_from_adjd_index;
                _smap_shrink_func_  = &stripe_map::shrink_map;
                _smap_is_shrunk     = false;
            };
            ///Shrinks entire stripe_map to single array of contiguous memory
            inline void
                shrink_map()
            {   using namespace implem;

                auto newitems = shrink_items(_smap_items,
                                             _smap_stripes,
                                             _smap_items_count);

                #if DEBUG_SMAP > 2
                    std::cout << "before slot count: " << _smap_slots_count
                              << " after slot count: " << _smap_items_count << std::endl;
                #endif

                _smap_slots_count   = _smap_items_count;

                delete_items();
                _smap_items = newitems;

                _smap_index_search_ = find_stripe_nogap_index;
                _smap_shrink_func_  = &stripe_map::func_void;
                _smap_is_shrunk     = true;
            };
            ///Erases item at given adjusted index
            inline bool
                erase_item(const size_t eraseIndex)
            {   using namespace implem;

                //Get correct stripe and aligned index to
                auto stripefind = _smap_index_search_(_smap_stripes,
                                                      eraseIndex);

                const auto destroysucc = erase_item_from_stripe(_smap_items,
                                                                stripefind._1,
                                                                stripefind._2);
                //Erase attempt failed
                if ( !destroysucc )
                    return false;

                //Decrement count of total held items
                _smap_items_count--;

                #if DEBUG_SMAP > 1
                    std::cout << "Erased item at index [" << eraseIndex << "]"
                              << " new item count is: " << _smap_items_count << std::endl;
                #endif

                _smap_index_search_  = find_stripe_from_adjd_index;
                _smap_is_shrunk     = false;

                //Erase attempt succeeded
                return true;
            };
            ///Clears items from stripe matching given depthMatch
            inline bool
                clear_stripe(const size_t depthMatch)
            {   using namespace implem;

                auto stripefind = find_stripe_jump_depth(_smap_stripes,
                                                         depthMatch,
                                                         _smap_depth_max);

                const auto clearsucc = clear_entire_stripe(stripefind);

                //Stripe erase failed
                if ( !clearsucc._1 )
                    return false;

                //Update total items count from what was removed
                _smap_items_count -= clearsucc._2;

                return true;
            };
            ///Removes all items matching value from stripes
            template <typename F>
            inline bool
                remove_items(const size_t rmvIndStart,
                              const size_t rmvIndEnd,
                              F&& check)
            {
                auto stripefindstart = _smap_index_search_(_smap_stripes,
                                                           rmvIndStart);
                auto stripefindend   = _smap_index_search_(_smap_stripes,
                                                           rmvIndEnd);

                size_t endfix = stripefindend._2;
                //Index search will return 0 if requesting past max usable index
                if ( rmvIndEnd == _smap_items_count )
                    endfix = _smap_slots_count;

                auto removesuccess = remove_values_from_stripes(_smap_items,
                                                                stripefindstart._1,
                                                                stripefindstart._2,
                                                                endfix,
                                                                check);

                if ( !removesuccess._1 )
                    return false;

                _smap_items_count -= removesuccess._2;

                return true;
            };

            ///-------------------------------------------------------------------------------------------------------
            ///SWAPPABLE FUNCTIONS

            ///Empty void function
            inline void
                func_void()
            {};
            ///Performs initial reserve of stripe_map allocations and updates swappable functions to post-reserve
            inline void
                init_reserve()
            {
                reserve();

                _smap_close_func_  = &stripe_map::close_used;
                _smap_init_func_   = &stripe_map::func_void;
                _smap_shrink_func_ = &stripe_map::shrink_map;
            };
            ///Performs deletion of item and stripe arrays if stripe_map is post-reserve
            inline void
                close_used()
            {
                delete_both();

                _smap_close_func_ = &stripe_map::func_void;
            }

            ///-------------------------------------------------------------------------------------------------------
            ///INTERNAL STRIPE_MAP VARIABLES
            typedef void
                (stripe_map::*initFunc)();
            typedef void
                (stripe_map::*closeFunc)();
            typedef void
                (stripe_map::*shrinkFunc)();
            typedef const Prs::tpsPr<implem::stripe*, size_t>
                (*findFunc)(implem::stripe*, const size_t);

            Prs::tpsPr<size_t, V>* _smap_items;                             ///< internal stripe_map items
            implem::stripe* _smap_stripes;                                          ///< stripe_map stripe information

            size_t _smap_items_count      = 0;                              ///< total items currently held in stripe_map
            size_t _smap_slots_count      = implem::SMAP_INIT_SLOT_COUNT;           ///< total slots including empty in stripe_map

            size_t _smap_depth_max        = implem::SMAP_INIT_MAX_DEPTH;            ///< maximum depth search value for stripe_map
            size_t _smap_stripe_stripes   = implem::SMAP_INIT_STRIPE_AMOUNT;        ///< total stripes held within stripe_map
            size_t _smap_slots_width      = implem::SMAP_INIT_WIDTH;
            size_t _smap_stripe_depth     = _smap_depth_max;                ///< search depth increment (granularity)

            bool _smap_is_shrunk          = false;                          ///< determines if stripe_map currently shrunk

            initFunc _smap_init_func_     = &stripe_map::init_reserve;      ///< function used to init stripe_map if uninit
            closeFunc _smap_close_func_   = &stripe_map::func_void;         ///< function for cleaning stripe_map if in use
            shrinkFunc _smap_shrink_func_ = &stripe_map::func_void;
            findFunc _smap_index_search_  = implem::find_stripe_from_adjd_index;    ///< stripe_map index search func pointer
    };

};  //end of qmap namespace

#endif // STRIPE_MAP_HPP
