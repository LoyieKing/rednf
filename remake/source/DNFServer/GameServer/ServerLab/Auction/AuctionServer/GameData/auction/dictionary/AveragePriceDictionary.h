// Rebuilt from df_auction_r DWARF (AveragePriceDictionary.cpp CU), 2026-08-08
#ifndef AVERAGEPRICEDICTIONARY_H_
#define AVERAGEPRICEDICTIONARY_H_

#include <map>

#include "ObjectPoolByBoostPool.h"
#include "DBTransactionDesign.h"

typedef unsigned long long __int64;

class AveragePriceDictionary
{
public:
    struct AveragePriceDictionaryData
    {
        unsigned char added_cnt;          // @0
        int average_price;                // @4
        int average_price_notice;         // @8
        unsigned int real_purchase_cnt;   // @12
    };

    AveragePriceDictionary();
    virtual ~AveragePriceDictionary();
    void UpdateAveragePirce();
    int AddItemAveragePrice(unsigned long itemId, unsigned char itemUpgradeValue,
                            int price, bool& rFristAdd, int& rAveragePrice, bool isStack,
                            const ROI_AverageKey& _roi_average_key,
                            unsigned int& _real_purchase_cnt, unsigned char itemRefineValue);
    int GetItemAveragePrice(unsigned long itemId, unsigned char itemUpgradeValue,
                            const ROI_AverageKey& roi_average_key,
                            unsigned char itemRefineValue, int* pOutAveragePrice);
    int makeItemAveragePrice(unsigned long itemId, unsigned char itemUpgradeValue,
                             int averagePrice, unsigned int _purchase_cnt,
                             const ROI_AverageKey& _roi_average_key,
                             unsigned char itemRefineValue);
    bool isValidUpgradeValue(unsigned char itemUpgradeValue)
    {
        // false-first (cmp/jbe true) to match auction binary
        if (itemUpgradeValue > 0x1f)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    bool isValidRefineValue(unsigned char itemRefineValue)
    {
        // POINT_SERVER may change refine max (0x07 vs 0x7f) — preserve macros.
        // Same false-first shape as isValidUpgradeValue. Note: auction's 0x7f
        // bound is folded by gcc4.4 to movzbl/test/jns (semantic == cmpb $0x7f).
#ifdef POINT_SERVER
        if (itemRefineValue > 0x07)
#else
        if (itemRefineValue > 0x7f)
#endif
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    bool aver_Set_ROI_Constraint(const ROI_Average_Constraint& _constraint);

private:
    nsl::object_pool_by_boost_pool<AveragePriceDictionaryData, int, int, int>
        mAvrgPriceDicDataPool;                                    // @4
    std::map<unsigned long, AveragePriceDictionaryData*>
        mAvrgPriceDicTable[0x20][8];                              // @36
    ROI_Average_Constraint mRoiAverageConstraint;                 // @6180
    std::map<ROI_AverageKey, AveragePriceDictionaryData*>
        mAvrgPrice_ROI_DicTable[0x20][8];                         // @6204
};

#endif // AVERAGEPRICEDICTIONARY_H_
