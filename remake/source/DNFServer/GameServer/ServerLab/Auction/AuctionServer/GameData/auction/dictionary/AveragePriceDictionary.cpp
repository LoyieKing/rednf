// Rebuilt from df_auction_r DWARF (AveragePriceDictionary.cpp CU), 2026-08-08
#include <stdlib.h>

#include "AveragePriceDictionary.h"
#include "TraceLog.h"

using namespace nsl;

AveragePriceDictionary::AveragePriceDictionary()
    : mAvrgPriceDicDataPool(0x20)
{
}

AveragePriceDictionary::~AveragePriceDictionary()
{
    // size_t / unsigned => setbe on bound (orig), not signed setle
    for (unsigned int j = 0; j < 8; j = j + 1)
    {
        for (unsigned int i = 0; i < 0x20; i = i + 1)
        {
            mAvrgPriceDicTable[i][j].clear();
            mAvrgPrice_ROI_DicTable[i][j].clear();
        }
    }
}

bool ROI_Average_Constraint::isVaildRange() const
{
    if ((inf_max_price <= 0) || (inf_max_price > 0x64))
    {
        return false;
    }
    if ((inf_min_price <= 0) || (inf_min_price > 0x2710))
    {
        return false;
    }
    if ((inf_prob <= 0) || (inf_prob > 0x64))
    {
        return false;
    }
    if ((inf_limit_count <= 0) || (inf_limit_count > 0x64))
    {
        return false;
    }
    if ((inf_base_mul_min_a <= 0) || (inf_base_mul_min_a > 0x3e8))
    {
        return false;
    }
    if ((inf_base_mul_max_b <= 0) || (inf_base_mul_max_b > 0x3e8))
    {
        return false;
    }
    return true;
}

int AveragePriceDictionary::makeItemAveragePrice(unsigned long itemId,
                                                 unsigned char itemUpgradeValue,
                                                 int averagePrice,
                                                 unsigned int _purchase_cnt,
                                                 const ROI_AverageKey& _roi_average_key,
                                                 unsigned char itemRefineValue)
{
    if (!isValidUpgradeValue(itemUpgradeValue))
    {
        return 0x20;
    }
    if (!isValidRefineValue(itemRefineValue))
    {
        return 0x20;
    }
    AveragePriceDictionaryData* ptr_data = mAvrgPriceDicDataPool.malloc();
    if (ptr_data == NULL)
    {
        return 9;
    }
    if (averagePrice < 0)
    {
        return 0x21;
    }
    ptr_data->average_price = averagePrice;
    ptr_data->average_price_notice = averagePrice;
    ptr_data->added_cnt = 0x0f;
    if (_roi_average_key.option_category.isEmpty())
    {
        std::pair<std::map<unsigned long, AveragePriceDictionaryData*>::iterator, bool> ret_pair;
        ret_pair = mAvrgPriceDicTable[itemUpgradeValue][itemRefineValue].insert(
            std::make_pair<const unsigned long&, AveragePriceDictionaryData*&>(
                itemId, ptr_data));
        if (ret_pair.second != true)
        {
            return 0xd;
        }
    }
    else
    {
        ptr_data->real_purchase_cnt = _purchase_cnt;
        if (!mAvrgPrice_ROI_DicTable[itemUpgradeValue][itemRefineValue].insert(
                std::make_pair<const ROI_AverageKey&, AveragePriceDictionaryData*&>(
                    _roi_average_key, ptr_data)).second)
        {
            return 0xd;
        }
    }
    return 0;
}

bool AveragePriceDictionary::aver_Set_ROI_Constraint(
    const ROI_Average_Constraint& _constraint)
{
    // Single struct assign: this in %eax, constraint in %edx, field stores (orig)
    mRoiAverageConstraint = _constraint;
    return true;
}

int AveragePriceDictionary::GetItemAveragePrice(
    unsigned long itemId, unsigned char itemUpgradeValue,
    const ROI_AverageKey& roi_average_key, unsigned char itemRefineValue,
    int* pOutAveragePrice)
{
    if (!isValidUpgradeValue(itemUpgradeValue))
    {
        return 0x20;
    }
    // Orig: !isEmpty fallthrough = ROI map first; empty is jump target (callset order).
    // Each branch ends with its own `return 0` (mov $0; jmp/ret), matching orig.
    if (!roi_average_key.option_category.isEmpty())
    {
        std::map<ROI_AverageKey, AveragePriceDictionaryData*>::iterator find_iter =
            mAvrgPrice_ROI_DicTable[itemUpgradeValue][itemRefineValue].find(
                roi_average_key);
        if (find_iter != mAvrgPrice_ROI_DicTable[itemUpgradeValue][itemRefineValue].end())
        {
            *pOutAveragePrice = find_iter->second->average_price_notice;
        }
        else
        {
            *pOutAveragePrice = -1;
        }
        return 0;
    }
    else
    {
        AveragePriceDictionaryData* ptr_data;
        std::map<unsigned long, AveragePriceDictionaryData*>::iterator find_iter =
            mAvrgPriceDicTable[itemUpgradeValue][itemRefineValue].find(itemId);
        if (find_iter == mAvrgPriceDicTable[itemUpgradeValue][itemRefineValue].end())
        {
            *pOutAveragePrice = -1;
        }
        else
        {
            // intermediate ptr_data (orig stores then reloads)
            ptr_data = find_iter->second;
            *pOutAveragePrice = ptr_data->average_price_notice;
        }
        return 0;
    }
}

// ORIG 的 FP 比较用 fucompp+fnstsw（i586 形态）；函数级 target 覆盖。
__attribute__((target("arch=i586"))) int AveragePriceDictionary::AddItemAveragePrice(
    unsigned long itemId, unsigned char itemUpgradeValue, int price,
    bool& rFristAdd, int& rAveragePrice, bool isStack,
    const ROI_AverageKey& _roi_average_key, unsigned int& _real_purchase_cnt,
    unsigned char itemRefineValue)
{
    AveragePriceDictionaryData* ptr_data;
    bool is_first_add = false;
    unsigned long long average_price_temp = 0;

    if (!isValidUpgradeValue(itemUpgradeValue))
    {
        return 0x20;
    }
    if (!isValidRefineValue(itemRefineValue))
    {
        return 0x20;
    }
    // Find path: empty first (test/je to ROI). Per-branch error_code check (orig).
    if (_roi_average_key.option_category.isEmpty())
    {
        std::map<unsigned long, AveragePriceDictionaryData*>::iterator find_iter =
            mAvrgPriceDicTable[itemUpgradeValue][itemRefineValue].find(itemId);
        if (find_iter != mAvrgPriceDicTable[itemUpgradeValue][itemRefineValue].end())
        {
            ptr_data = find_iter->second;
            goto continue_update;
        }
        is_first_add = true;
        int error_code = makeItemAveragePrice(itemId, itemUpgradeValue, price, 1,
                                              _roi_average_key, itemRefineValue);
        if (error_code != 0)
        {
            return error_code;
        }
    }
    else
    {
        std::map<ROI_AverageKey, AveragePriceDictionaryData*>::iterator find_iter =
            mAvrgPrice_ROI_DicTable[itemUpgradeValue][itemRefineValue].find(
                _roi_average_key);
        if (find_iter != mAvrgPrice_ROI_DicTable[itemUpgradeValue][itemRefineValue].end())
        {
            ptr_data = find_iter->second;
            // orig: real_purchase_cnt++ then assign out-param
            ptr_data->real_purchase_cnt = ptr_data->real_purchase_cnt + 1;
            _real_purchase_cnt = ptr_data->real_purchase_cnt;
            goto continue_update;
        }
        is_first_add = true;
        int error_code = makeItemAveragePrice(itemId, itemUpgradeValue, price,
                                              _real_purchase_cnt, _roi_average_key,
                                              itemRefineValue);
        if (error_code != 0)
        {
            return error_code;
        }
    }
continue_update:
    if (is_first_add)
    {
        rFristAdd = true;
        rAveragePrice = price;
    }
    else
    {
        unsigned long long average_max_limit = 0;
        __int64 average_min_limit = 0;
        bool submit = false;
        bool roi_unseal_average_submit = true;
        if (isStack)
        {
            average_max_limit =
                (unsigned long long)ptr_data->average_price * 3;
            average_min_limit =
                (__int64)((double)ptr_data->average_price * (1.0 / 3.0));
            submit = rand() % 10 == 0;
        }
        else
        {
            // Orig update block: !isEmpty (ROI) fallthrough first; empty is jump
            // target so call order is rand(stack), isEmpty, rand(ROI), guard, rand(empty)
            if (!_roi_average_key.option_category.isEmpty())
            {
                average_max_limit =
                    (unsigned long long)ptr_data->average_price *
                    (unsigned long long)mRoiAverageConstraint.inf_max_price;
                average_min_limit =
                    (__int64)((double)(__int64)ptr_data->average_price *
                              ((double)mRoiAverageConstraint.inf_min_price /
                               10000.0));
                submit = rand() % 100 + 1 <= mRoiAverageConstraint.inf_prob;
                static ROI_AverageKey _static_UnsealRandomOptionItem;
                static bool _isInit = false;
                if (!_isInit)
                {
                    for (int i = 0; i < 3; i = i + 1)
                    {
                        _static_UnsealRandomOptionItem.option_category.field_0
                            ._hcv._high_category_value[i] = 30000;
                        _static_UnsealRandomOptionItem.option_category.field_1
                            ._lcv._low_category_value[i] = 0;
                    }
                    _static_UnsealRandomOptionItem.option_index_key = 0;
                    _isInit = true;
                }
                _static_UnsealRandomOptionItem.baseItem_index = itemId;
                std::map<ROI_AverageKey, AveragePriceDictionaryData*>::iterator
                    base_find_iter =
                        mAvrgPrice_ROI_DicTable[itemUpgradeValue][itemRefineValue]
                            .find(_static_UnsealRandomOptionItem);
                if (base_find_iter !=
                    mAvrgPrice_ROI_DicTable[itemUpgradeValue][itemRefineValue]
                        .end())
                {
                    if (base_find_iter->second != NULL)
                    {
                        if ((double)price <
                            (double)base_find_iter->second->average_price_notice *
                                ((double)mRoiAverageConstraint.inf_base_mul_min_a /
                                 100.0))
                        {
                            roi_unseal_average_submit = false;
                        }
                    }
                }
            }
            else
            {
                average_max_limit =
                    (unsigned long long)ptr_data->average_price * 3;
                average_min_limit =
                    (__int64)((double)ptr_data->average_price * 0.2);
                submit = rand() % 5 == 0;
            }
        }
        if (((unsigned long long)price < average_max_limit) &&
            ((__int64)average_min_limit < (__int64)price) &&
            (submit != 0) && (roi_unseal_average_submit != 0))
        {
            average_price_temp =
                (unsigned int)ptr_data->added_cnt *
                    (unsigned long long)ptr_data->average_price +
                (__int64)price;
            ptr_data->added_cnt = (unsigned char)(ptr_data->added_cnt + 1);
            average_price_temp =
                average_price_temp /
                (unsigned long long)ptr_data->added_cnt;
            if (ptr_data->added_cnt > 0xf)
            {
                ptr_data->added_cnt = 0xf;
            }
            ptr_data->average_price = (int)average_price_temp;
        }
        rAveragePrice = ptr_data->average_price;
    }
    return 0;
}

// ORIG 的 FP 比较用 fucompp+fnstsw（i586 形态）；函数级 target 覆盖。
__attribute__((target("arch=i586"))) void AveragePriceDictionary::UpdateAveragePirce()
{
    AveragePriceDictionaryData* ptr_data;
    __int64 average_max_limit;
    __int64 average_min_limit;
    for (int j = 0; j < 8; j = j + 1)
    {
        for (int i = 0; i < 0x20; i = i + 1)
        {
            std::map<unsigned long, AveragePriceDictionaryData*>::iterator iter_pos =
                mAvrgPriceDicTable[i][j].begin();
            while (iter_pos != mAvrgPriceDicTable[i][j].end())
            {
                ptr_data = iter_pos->second;
                // use __int64 for max to match orig multi+sign-extend form
                average_max_limit =
                    (__int64)ptr_data->average_price_notice * 3;
                average_min_limit =
                    (__int64)((double)ptr_data->average_price_notice * 0.01);
                if (((__int64)ptr_data->average_price < average_max_limit) &&
                    (average_min_limit < (__int64)ptr_data->average_price))
                {
                    G_TraceLog()->sysLog(
                        5, "item_id:%u, count:%u, befor_average:%u, after_average:%u",
                        iter_pos->first, (unsigned int)ptr_data->added_cnt,
                        (unsigned int)ptr_data->average_price_notice,
                        (unsigned int)ptr_data->average_price);
                    ptr_data->average_price_notice = ptr_data->average_price;
                }
                ++iter_pos;
            }
        }
    }
    for (int j = 0; j < 8; j = j + 1)
    {
        for (int i = 0; i < 0x20; i = i + 1)
        {
            std::map<ROI_AverageKey, AveragePriceDictionaryData*>::iterator iter_pos =
                mAvrgPrice_ROI_DicTable[i][j].begin();
            while (iter_pos != mAvrgPrice_ROI_DicTable[i][j].end())
            {
                ptr_data = iter_pos->second;
                average_max_limit =
                    (__int64)ptr_data->average_price_notice * 3;
                average_min_limit =
                    (__int64)((double)ptr_data->average_price_notice * 0.01);
                if (((__int64)ptr_data->average_price < average_max_limit) &&
                    (average_min_limit < (__int64)ptr_data->average_price))
                {
                    ptr_data->average_price_notice = ptr_data->average_price;
                }
                ++iter_pos;
            }
        }
    }
}
