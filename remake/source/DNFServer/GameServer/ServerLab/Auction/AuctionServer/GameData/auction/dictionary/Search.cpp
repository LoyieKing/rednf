// Rebuilt from df_auction_r DWARF (Search.cpp CU), 2026-08-08
#include <algorithm>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Search.h"
#include "Auction.h"
#include "AuctionDictionary.h"
#include "AveragePriceDictionary.h"
#include "DBTransactionDesign.h"
#include "TraceLog.h"

enum ENUM_ITEM_CATEGORY_LIST
{
    CATEGORY_ALL = 0,
    CATEGORY_WEAPON = 10000,
    CATEGORY_SWORD_MAN = 10101,
    CATEGORY_SWORD_MAN_LAST = 10107,
    CATEGORY_FIGHTER = 10201,
    CATEGORY_FIGHTER_LAST = 10207,
    CATEGORY_GUNNER = 10301,
    CATEGORY_GUNNER_LAST = 10307,
    CATEGORY_MAGE = 10401,
    CATEGORY_MAGE_LAST = 10407,
    CATEGORY_PRIEST = 10501,
    CATEGORY_PRIEST_LAST = 10507,
    CATEGORY_THIEF = 10601,
    CATEGORY_THIEF_LAST = 10605,
    CATEGORY_AT_FIGHTER = 10701,
    CATEGORY_AT_FIGHTER_LAST = 10707,
    CATEGORY_WEAPON_LAST = 10708,
    CATEGORY_DEFENCE = 11000,
    CATEGORY_CLOTH = 11001,
    CATEGORY_CLOTH_LAST = 11007,
    CATEGORY_LEATHER = 11100,
    CATEGORY_LEATHER_LAST = 11106,
    CATEGORY_LIGHT = 11200,
    CATEGORY_LIGHT_LAST = 11206,
    CATEGORY_HEAVY = 11300,
    CATEGORY_HEAVY_LAST = 11306,
    CATEGORY_METAL = 11400,
    CATEGORY_METAL_LAST = 11406,
    CATEGORY_DEFENCE_LAST = 11407,
    CATEGORY_ACCESSARY = 12000,
    CATEGORY_ACCESSARY_LAST = 12005,
    CATEGORY_STACKABLE = 13000,
    CATEGORY_STACKABLE_LAST = 13007,
    CATEGORY_CREATURE = 14000,
    CATEGORY_CREATURE_LAST = 14005,
    CATEGORY_AVATAR = 15000,
    CATEGORY_AVATAR_SWORDMAN = 15001,
    CATEGORY_AVATAR_LAST = 15812,
    CATEGORY_CLONE_AVATAR = 17000,
    CATEGORY_CLONE_AVATAR_SWORDMAN = 17001,
    CATEGORY_CLONE_AVATAR_LAST = 17812,
    CATEGORY_EMBLEM_AVATAR = 19000,
    CATEGORY_EMBLEM_AVATAR_SWORDMAN = 19001,
    CATEGORY_EMBLEM_AVATAR_LAST = 19812,
    CATEGORY_CLONE_EMBLEM_AVATAR = 21000,
    CATEGORY_CLONE_EMBLEM_AVATAR_SWORDMAN = 21001,
    CATEGORY_CLONE_EMBLEM_AVATAR_LAST = 21812,
    CATEGORY_NORMAL_EMBLEM_AVATAR = 23000,
    CATEGORY_NORMAL_EMBLEM_AVATAR_SWORDMAN = 23001,
    CATEGORY_NORMAL_EMBLEM_AVATAR_LAST = 23812,
    CATEGORY_CLONE_NORMAL_EMBLEM_AVATAR = 25000,
    CATEGORY_CLONE_NORMAL_EMBLEM_AVATAR_SWORDMAN = 25001,
    CATEGORY_CLONE_NORMAL_EMBLEM_AVATAR_LAST = 25812,
    CATEGORY_STACKABLE_EMBLEM = 30000,
    CATEGORY_STACKABLE_EMBLEM_M = 30100,
    CATEGORY_STACKABLE_EMBLEM_M_LAST = 30104,
    CATEGORY_STACKABLE_EMBLEM_LAST = 30105,
    CATEGORY_RECIPE = 31000,
    CATEGORY_RECIPE_WEAPON = 31001,
    CATEGORY_RECIPE_WEAPON_LAST = 31010,
    CATEGORY_RECIPE_DEFENCE = 31101,
    CATEGORY_RECIPE_DEFENCE_LAST = 31107,
    CATEGORY_RECIPE_ACCESSARY = 31201,
    CATEGORY_RECIPE_ACCESSARY_LAST = 31205,
    CATEGORY_RECIPE_SPECIAL_EQUIPMENT = 31301,
    CATEGORY_RECIPE_SPECIAL_EQUIPMENT_LAST = 31304,
    CATEGORY_RECIPE_LAST = 31306,
    CATEGORY_SPECIAL_EQUIPMENT = 32000,
    CATEGORY_SPECIAL_EQUIPMENT_SUPPORT = 32001,
    CATEGORY_SPECIAL_EQUIPMENT_SUPPORT_LAST = 32013,
    CATEGORY_SPECIAL_EQUIPMENT_MAGIC_STONE = 32100,
    CATEGORY_SPECIAL_EQUIPMENT_MAGIC_STONE_LAST = 32112,
    CATEGORY_SPECIAL_EQUIPMENT_LAST = 32113,
    CATEGORY_EXPERT = 33000,
    CATEGORY_EXPERT_LAST = 33005,
    CATEGORY_ETC = 34000,
    CATEGORY_ETC_LAST = 34001,
    CATEGORY_NONE = 40120,
};

Search::Search()
{
    InitializeCategoryNextContainerData();
}

Search::~Search()
{
    mCategoryNextContainer.clear();
}

bool Search::IsFindEnough(TSearchResult* pSearchResult)
{
    return 0x3b < pSearchResult->numberOfFound;
}

bool Search::IsRequireStopSearch(TSearchResult* pSearchResult)
{
    return IsFindEnough(pSearchResult) &&
           (pSearchResult->isFirstSearch != true ||
            (pSearchResult->isFirstSearch != false &&
             9999 < pSearchResult->totalNumberOfFound));
}

bool Search::IsAvatar()
{
    return mIsAvatar;
}

void Search::SetIsAvatar(int category)
{
    mIsAvatar = mpAuction->IsAvatarCategory(category);
}

bool Search::IsCategoryHasSocket(int category)
{
    return (category > 18999 && category <= 19811) ||
           (category > 20999 && category <= 21811);
}

bool Search::IsValidUpgrade(BYTE upgrade)
{
    return upgrade < 0x20;
}

bool Search::IsValidUpgradeRange(BYTE upgradeStart, BYTE upgradeEnd)
{
    return IsValidUpgrade(upgradeStart) && IsValidUpgrade(upgradeEnd) &&
           upgradeStart <= upgradeEnd;
}

bool Search::IsValidRefine(BYTE refine)
{
#ifdef POINT_SERVER
    return refine <= 0x07;
#else
    return refine <= 0x7f;
#endif
}

bool Search::IsValidRefineRange(BYTE refineStart, BYTE refineEnd)
{
    return IsValidRefine(refineStart) && IsValidRefine(refineEnd) &&
           refineStart <= refineEnd;
}

bool Search::IsValidRarity(BYTE rarity)
{
    return rarity < 7;
}

bool Search::IsSpecificRarity(BYTE rarity)
{
    return rarity < 6;
}

bool Search::IsSpecificLv(TCategoryParameter* pParameter)
{
#ifdef POINT_SERVER
    return pParameter->lvStart >= 1 && pParameter->lvStart <= 0x46;
#else
    return pParameter->lvStart >= 1 && pParameter->lvStart <= 0x55;
#endif
}

bool Search::IsAlternativeAvater(int category)
{
    return (category > 0x59d8 && category <= 0x5d03) ||
           (category > 0x61a8 && category <= 0x64d3);
}

WORD Search::FindNextSameLevelCategory(WORD category)
{
    WORD next_same_level_category = category;
    std::map<unsigned short, unsigned short>::iterator iter;
    iter = mCategoryNextContainer.find(next_same_level_category);
    if (iter == mCategoryNextContainer.end())
    {
        return next_same_level_category;
    }
    next_same_level_category = iter->second;
    return next_same_level_category;
}

int Search::SetSearchResult(TSearchResult* pSearchResult, PAuctionIdContainer pContainer)
{
    std::multiset<AuctionId>::const_iterator i;
    AuctionItemInfo* pAuctionItemInfo;
    int result;

    if (pContainer == (PAuctionIdContainer)0x0)
    {
        return 0;
    }
    if (pSearchResult->isFirstSearch != false)
    {
        pSearchResult->totalNumberOfFound =
            pSearchResult->totalNumberOfFound + pContainer->size();
    }
    if (IsFindEnough(pSearchResult))
    {
        return 0;
    }
    if (pContainer->size() <= pSearchResult->startIdx)
    {
        pSearchResult->startIdx = pSearchResult->startIdx - pContainer->size();
        return 0;
    }
    else
    {
        i = pContainer->begin();
        while (i != pContainer->end())
        {
            if (pSearchResult->startIdx != 0)
            {
                pSearchResult->startIdx = pSearchResult->startIdx - 1;
            }
            else
            {
                pAuctionItemInfo =
                    pSearchResult->pOutAuctionItemInfoArray + pSearchResult->numberOfFound;
                result = GetAuctionItemInfo(i->auctionId, pSearchResult->roi_search_category,
                                            pAuctionItemInfo);
                if (result == 0)
                {
                    pSearchResult->numberOfFound = pSearchResult->numberOfFound + 1;
                }
            }
            if (IsFindEnough(pSearchResult))
            {
                return 0;
            }
            ++i;
        }
    }
    return 0;
}

int Search::ROI_SetSearchResult(TSearchResult* pSearchResult, PAuctionIdContainer pContainer)
{
    AuctionItemInfo _tempSearchItemInfo;
    AuctionItemInfo* pAuctionItemInfo;
    std::multiset<AuctionId>::const_iterator i;
    int result;

    if (pContainer == (PAuctionIdContainer)0x0)
    {
        return 0;
    }
    i = pContainer->begin();
    while (i != pContainer->end())
    {
        pAuctionItemInfo = &_tempSearchItemInfo;
        result = GetAuctionItemInfo(i->auctionId, pSearchResult->roi_search_category,
                                    pAuctionItemInfo);
        if (result == 0)
        {
            if (pSearchResult->startIdx != 0)
            {
                pSearchResult->startIdx = pSearchResult->startIdx - 1;
            }
            else
            {
                if (pSearchResult->isFirstSearch != false)
                {
                    pSearchResult->totalNumberOfFound = pSearchResult->totalNumberOfFound + 1;
                }
                if (IsFindEnough(pSearchResult))
                {
                }
                else
                {
                    memcpy(pSearchResult->pOutAuctionItemInfoArray +
                           pSearchResult->numberOfFound, pAuctionItemInfo, 0x89);
                    pSearchResult->numberOfFound = pSearchResult->numberOfFound + 1;
                }
            }
        }
        ++i;
    }
    return 0;
}

int Search::OperateByRefine(TOperate* pOperate, PRefineContainer pParent)
{
    TAuctionIdContainer* p_container = NULL;
    BYTE key = pOperate->refine;
    TRefineContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PAuctionIdContainer* pp_container = new PAuctionIdContainer;
        if (pp_container == (PAuctionIdContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TAuctionIdContainer();
        if (*pp_container == (TAuctionIdContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0xe;
        }
    }
    p_container = pos->second;
    if (pOperate->operation == STATE_INSERT)
    {
        AuctionId id;
        id.auctionId = pOperate->auctionId;
        id.instantPrice = pOperate->instantPrice;
        p_container->insert(id);
    }
    else if (pOperate->operation == STATE_DELETE)
    {
        AuctionId id;
        id.auctionId = pOperate->auctionId;
        id.instantPrice = pOperate->instantPrice;
        std::multiset<AuctionId>::const_iterator find_pos = std::find_if(
            p_container->begin(), p_container->end(),
            std::bind2nd(std::equal_to<AuctionId>(), id));
        if (find_pos != p_container->end())
        {
            p_container->erase(find_pos);
        }
    }
    return 0;
}

int Search::OperateByUpgrade(TOperate* pOperate, PUpgradeContainer pParent)
{
    PRefineContainer p_container = NULL;
    unsigned long key = pOperate->upgrade;
    TUpgradeContainer::iterator pos;
    pos = pParent->find((unsigned char)key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PRefineContainer* pp_container = new PRefineContainer;
        if (pp_container == (PRefineContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TRefineContainer();
        if (*pp_container == (TRefineContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find((unsigned char)key);
        if (pos == pParent->end())
        {
            return 0xe;
        }
    }
    p_container = pos->second;
    if (OperateByRefine(pOperate, p_container) != 0)
    {
        return 0xe;
    }
    else
    {
        return 0;
    }
}

int Search::OperateByLv(TOperate* pOperate, PLvContainer pParent)
{
    TAuctionIdContainer* p_container = NULL;
    BYTE key = pOperate->lv;
    TLvContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PAuctionIdContainer* pp_container = new PAuctionIdContainer;
        if (pp_container == (PAuctionIdContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TAuctionIdContainer();
        if (*pp_container == (TAuctionIdContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0x11;
        }
    }
    p_container = pos->second;
    if (pOperate->operation == STATE_INSERT)
    {
        AuctionId id;
        id.auctionId = pOperate->auctionId;
        id.instantPrice = pOperate->instantPrice;
        p_container->insert(id);
    }
    else if (pOperate->operation == STATE_DELETE)
    {
        AuctionId id;
        id.auctionId = pOperate->auctionId;
        id.instantPrice = pOperate->instantPrice;
        std::multiset<AuctionId>::const_iterator find_pos = std::find_if(
            p_container->begin(), p_container->end(),
            std::bind2nd(std::equal_to<AuctionId>(), id));
        if (find_pos != p_container->end())
        {
            p_container->erase(find_pos);
        }
    }
    return 0;
}

int Search::OperateByRefineLv(TOperate* pOperate, PRefineLvContainer pParent)
{
    PLvContainer p_container = NULL;
    BYTE key = pOperate->refine;
    TRefineLvContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PLvContainer* pp_container = new PLvContainer;
        if (pp_container == (PLvContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TLvContainer();
        if (*pp_container == (TLvContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0x11;
        }
    }
    p_container = pos->second;
    if (OperateByLv(pOperate, p_container) != 0)
    {
        return 0x11;
    }
    else
    {
        return 0;
    }
}

int Search::OperateByUpgradeLv(TOperate* pOperate, PUpgradeLvContainer pParent)
{
    PRefineLvContainer p_container = NULL;
    BYTE key = pOperate->upgrade;
    TUpgradeLvContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PRefineLvContainer* pp_container = new PRefineLvContainer;
        if (pp_container == (PRefineLvContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TRefineLvContainer();
        if (*pp_container == (TRefineLvContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0x11;
        }
    }
    p_container = pos->second;
    if (OperateByRefineLv(pOperate, p_container) != 0)
    {
        return 0x11;
    }
    else
    {
        return 0;
    }
}

int Search::OperateByRarityUpgrade(TOperate* pOperate, PRarityUpgradeContainer pParent)
{
    PUpgradeContainer p_container = NULL;
    BYTE key = pOperate->rarity;
    TRarityUpgradeContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PUpgradeContainer* pp_container = new PUpgradeContainer;
        if (pp_container == (PUpgradeContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TUpgradeContainer();
        if (*pp_container == (TUpgradeContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0x10;
        }
    }
    p_container = pos->second;
    if (OperateByUpgrade(pOperate, p_container) != 0)
    {
        return 0x10;
    }
    else
    {
        return 0;
    }
}

int Search::OperateByRarityUpgradeLv(TOperate* pOperate, PRarityUpgradeLvContainer pParent)
{
    PUpgradeLvContainer p_container = NULL;
    BYTE key = pOperate->rarity;
    TRarityUpgradeLvContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PUpgradeLvContainer* pp_container = new PUpgradeLvContainer;
        if (pp_container == (PUpgradeLvContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TUpgradeLvContainer();
        if (*pp_container == (TUpgradeLvContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0xf;
        }
    }
    p_container = pos->second;
    if (OperateByUpgradeLv(pOperate, p_container) != 0)
    {
        return 0xf;
    }
    else
    {
        return 0;
    }
}

int Search::OperateByItemIdUpgrade(TOperate* pOperate, PItemIdUpgradeContainer pParent)
{
    PUpgradeContainer p_container = NULL;
    unsigned long key = pOperate->itemId;
    TItemIdUpgradeContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PUpgradeContainer* pp_container = new PUpgradeContainer;
        if (pp_container == (PUpgradeContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TUpgradeContainer();
        if (*pp_container == (TUpgradeContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0xe;
        }
    }
    p_container = pos->second;
    if (OperateByUpgrade(pOperate, p_container) != 0)
    {
        return 0xe;
    }
    else
    {
        return 0;
    }
}

int Search::OperateByCategoryUpgrade(TOperate* pOperate, PCategoryUpgradeContainer pParent)
{
    PUpgradeContainer p_container = NULL;
    WORD key = pOperate->category;
    TCategoryUpgradeContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PUpgradeContainer* pp_container = new PUpgradeContainer;
        if (pp_container == (PUpgradeContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TUpgradeContainer();
        if (*pp_container == (TUpgradeContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0x12;
        }
    }
    p_container = pos->second;
    if (OperateByUpgrade(pOperate, p_container) != 0)
    {
        return 0x12;
    }
    else
    {
        return 0;
    }
}

int Search::OperateByCategoryUpgradeLv(TOperate* pOperate, PCategoryUpgradeLvContainer pParent)
{
    PUpgradeLvContainer p_container = NULL;
    WORD key = pOperate->category;
    TCategoryUpgradeLvContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PUpgradeLvContainer* pp_container = new PUpgradeLvContainer;
        if (pp_container == (PUpgradeLvContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TUpgradeLvContainer();
        if (*pp_container == (TUpgradeLvContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0x11;
        }
    }
    p_container = pos->second;
    if (OperateByUpgradeLv(pOperate, p_container) != 0)
    {
        return 0x11;
    }
    else
    {
        return 0;
    }
}

int Search::OperateByCategoryRarityUpgrade(TOperate* pOperate,
                                           PCategoryRarityUpgradeContainer pParent)
{
    PRarityUpgradeContainer p_container = NULL;
    WORD key = pOperate->category;
    TCategoryRarityUpgradeContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PRarityUpgradeContainer* pp_container = new PRarityUpgradeContainer;
        if (pp_container == (PRarityUpgradeContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TRarityUpgradeContainer();
        if (*pp_container == (TRarityUpgradeContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0x10;
        }
    }
    p_container = pos->second;
    if (OperateByRarityUpgrade(pOperate, p_container) != 0)
    {
        return 0x10;
    }
    else
    {
        return 0;
    }
}

int Search::OperateByCategoryRarityUpgradeLv(TOperate* pOperate,
                                             PCategoryRarityUpgradeLvContainer pParent)
{
    PRarityUpgradeLvContainer p_container = NULL;
    WORD key = pOperate->category;
    TCategoryRarityUpgradeLvContainer::iterator pos;
    pos = pParent->find(key);
    if (pos == pParent->end())
    {
        if (pOperate->operation == STATE_DELETE)
        {
            return 0x13;
        }
        PRarityUpgradeLvContainer* pp_container = new PRarityUpgradeLvContainer;
        if (pp_container == (PRarityUpgradeLvContainer*)0x0)
        {
            return 9;
        }
        *pp_container = new TRarityUpgradeLvContainer();
        if (*pp_container == (TRarityUpgradeLvContainer*)0x0)
        {
            return 9;
        }
        pParent->insert(std::make_pair(key, *pp_container));
        pos = pParent->find(key);
        if (pos == pParent->end())
        {
            return 0xf;
        }
    }
    p_container = pos->second;
    if (OperateByRarityUpgradeLv(pOperate, p_container) != 0)
    {
        return 0xf;
    }
    else
    {
        return 0;
    }
}

int Search::SetOperateParameter(TOperate* pOperate, unsigned long itemId, BYTE upgrade,
                                unsigned long long auctionId,
                                const CNRDItemInfoList::STItemInfo* pItemInfo,
                                STATE_SEARCH_MODULE_OPERATION operation, bool hasSocket,
                                int instantPrice, BYTE refine)
{
    WORD category_before_convert;
    WORD category_after_convert;
    WORD gap;

    if (pItemInfo == (CNRDItemInfoList::STItemInfo*)0x0)
    {
        return 0x27;
    }
    if (!IsValidCategory(pItemInfo->category_, operation))
    {
        return 0x19;
    }
    if (!IsSpecificRarity(pItemInfo->rarity_))
    {
        return 0x1a;
    }
    if (!IsValidUpgrade(upgrade))
    {
        return 0x1c;
    }
    if (!IsValidRefine(refine))
    {
        return 0x33;
    }
#ifdef POINT_SERVER
    if (pItemInfo->nOriginalUsableLevel_ > 0x46)
#else
    if (pItemInfo->nOriginalUsableLevel_ > 0x55)
#endif
    {
        return 0x1b;
    }
    pOperate->itemId = itemId;
    pOperate->category = pItemInfo->category_;
    if (IsAlternativeAvater(pOperate->category))
    {
        category_before_convert = pOperate->category;
        gap = pOperate->category - (pOperate->category < 25000 ? 0x59d8 : 0x61a8);
        if (hasSocket)
        {
            pOperate->category = gap + (pOperate->category < 25000 ? 19000 : 21000);
        }
        else
        {
            pOperate->category = gap + (pOperate->category < 25000 ? 15000 : 17000);
        }
        category_after_convert = pOperate->category;
        nsl::G_TraceLog()->sysLog(
            5,
            "category converted(auction Id:%llu, has socket:%d), before:%d, after:%d",
            pOperate->auctionId, hasSocket, category_before_convert, category_after_convert);
    }
    pOperate->rarity = pItemInfo->rarity_;
    pOperate->upgrade = upgrade;
    pOperate->lv = pItemInfo->nOriginalUsableLevel_;
    pOperate->auctionId = auctionId;
    pOperate->operation = operation;
    pOperate->instantPrice = instantPrice;
    pOperate->refine = refine;
    return 0;
}

int Search::Insert(unsigned long itemId, BYTE upgrade, unsigned long long auctionId,
                   bool hasSocket, int instantPrice, BYTE refine)
{
    static TOperate search_regist_parameter;
    static TOperate* p_parameter = &search_regist_parameter;

    CNRDItemInfoList::STItemInfo* pItemInfo;
    int result;

    pItemInfo = mpAuction->GetItemInfo(itemId);
    result = SetOperateParameter(p_parameter, itemId, upgrade, auctionId, pItemInfo,
                                 STATE_INSERT, hasSocket, instantPrice, refine);
    if (result == 0)
    {
        result = OperateByItemIdUpgrade(p_parameter, &mItemIdUpgradeContainer);
        if (result == 0)
        {
            result = OperateByCategoryRarityUpgradeLv(p_parameter,
                                                      &mCategoryRarityUpgradeLvContainer);
            if (result == 0)
            {
                result = OperateByCategoryRarityUpgrade(p_parameter,
                                                        &mCategoryRarityUpgradeContainer);
                if (result == 0)
                {
                    result = OperateByCategoryUpgradeLv(p_parameter,
                                                        &mCategoryUpgradeLvContainer);
                    if (result == 0)
                    {
                        result = OperateByCategoryUpgrade(p_parameter, &mCategoryUpgradeContainer);
                        if (result != 0)
                        {
                            p_parameter->operation = STATE_DELETE;
                            OperateByItemIdUpgrade(p_parameter, &mItemIdUpgradeContainer);
                            OperateByCategoryRarityUpgradeLv(p_parameter,
                                                             &mCategoryRarityUpgradeLvContainer);
                            OperateByCategoryRarityUpgrade(p_parameter,
                                                           &mCategoryRarityUpgradeContainer);
                            OperateByCategoryUpgradeLv(p_parameter, &mCategoryUpgradeLvContainer);
                            OperateByCategoryUpgrade(p_parameter, &mCategoryUpgradeContainer);
                        }
                    }
                    else
                    {
                        p_parameter->operation = STATE_DELETE;
                        OperateByItemIdUpgrade(p_parameter, &mItemIdUpgradeContainer);
                        OperateByCategoryRarityUpgradeLv(p_parameter,
                                                         &mCategoryRarityUpgradeLvContainer);
                        OperateByCategoryRarityUpgrade(p_parameter,
                                                       &mCategoryRarityUpgradeContainer);
                        OperateByCategoryUpgradeLv(p_parameter, &mCategoryUpgradeLvContainer);
                    }
                }
                else
                {
                    p_parameter->operation = STATE_DELETE;
                    OperateByItemIdUpgrade(p_parameter, &mItemIdUpgradeContainer);
                    OperateByCategoryRarityUpgradeLv(p_parameter,
                                                     &mCategoryRarityUpgradeLvContainer);
                    OperateByCategoryRarityUpgrade(p_parameter, &mCategoryRarityUpgradeContainer);
                }
            }
            else
            {
                p_parameter->operation = STATE_DELETE;
                OperateByItemIdUpgrade(p_parameter, &mItemIdUpgradeContainer);
                OperateByCategoryRarityUpgradeLv(p_parameter,
                                                 &mCategoryRarityUpgradeLvContainer);
            }
        }
        else
        {
            p_parameter->operation = STATE_DELETE;
            OperateByItemIdUpgrade(p_parameter, &mItemIdUpgradeContainer);
        }
    }
    return result;
}

int Search::Delete(unsigned long long auctionId)
{
    static TOperate search_regist_parameter;
    static TOperate* p_parameter = &search_regist_parameter;

    unsigned long itemId;
    BYTE upgrade;
    BYTE refine;
    CNRDItemInfoList::STItemInfo* pItemInfo;
    bool hasSocket;
    int sub_result;
    int result;

    memset(&search_regist_parameter, 0, 0x20);
    result = GetRegisteredInfo(auctionId, &itemId, &upgrade, &refine);
    if (result != 0)
    {
        nsl::G_TraceLog()->sysLog(
            7, "Search::Delete GetRegisteredInfo error(insert auction_id:%llu) result:%d",
            auctionId, result);
        return result;
    }
    hasSocket = upgrade != 0;
    pItemInfo = mpAuction->GetItemInfo(itemId);
    result = SetOperateParameter(p_parameter, itemId, upgrade, auctionId, pItemInfo,
                                 STATE_DELETE, hasSocket, 0, refine);
    if (result != 0)
    {
        nsl::G_TraceLog()->sysLog(
            7,
            "Search::Delete SetOperateParameter error(insert auction_id:%llu, itemdId:%u), (delete auction_id:%llu, itemdId:%u) result:%d",
            auctionId, itemId, p_parameter->auctionId, p_parameter->itemId, result);
        return result;
    }
    result = OperateByItemIdUpgrade(p_parameter, &mItemIdUpgradeContainer);
    sub_result = OperateByCategoryRarityUpgradeLv(p_parameter, &mCategoryRarityUpgradeLvContainer);
    result = sub_result + result;
    sub_result = OperateByCategoryRarityUpgrade(p_parameter, &mCategoryRarityUpgradeContainer);
    result = sub_result + result;
    sub_result = OperateByCategoryUpgradeLv(p_parameter, &mCategoryUpgradeLvContainer);
    result = sub_result + result;
    sub_result = OperateByCategoryUpgrade(p_parameter, &mCategoryUpgradeContainer);
    result = sub_result + result;
    return result;
}

int Search::FindByItem(TSearchByItemId_* pSearchByItemId, unsigned long* pItemIdArray,
                       unsigned int* pTotalNumberOfFound, unsigned short* pNumberOfFound,
                       AuctionItemInfo* pOutAuctionItemInfoArray)
{
    static TItemIdParameter search_by_item_id_parameter;
    static TItemIdParameter* p_parameter = &search_by_item_id_parameter;

    int result;

    if (pSearchByItemId->itemIdNum > 0x14)
    {
        return 0x1d;
    }
    if (!IsValidUpgradeRange(pSearchByItemId->upgradeStart, pSearchByItemId->upgradeEnd))
    {
        return 0x1c;
    }
    if (!IsValidRefineRange(pSearchByItemId->refineStart, pSearchByItemId->refineEnd))
    {
        return 0x33;
    }
    if (mpAuction->IsAvatarCategory(pSearchByItemId->category))
    {
        p_parameter->categoryStart = pSearchByItemId->category;
    }
    else
    {
        p_parameter->categoryStart = 0;
    }
    p_parameter->itemIdNum = pSearchByItemId->itemIdNum;
    p_parameter->pItemIdArray = pItemIdArray;
    p_parameter->upgradeStart = pSearchByItemId->upgradeStart;
    p_parameter->upgradeEnd = pSearchByItemId->upgradeEnd;
    p_parameter->searchResult.startIdx = pSearchByItemId->startIndex;
    p_parameter->searchResult.isFirstSearch = pSearchByItemId->startIndex == 0;
    p_parameter->searchResult.pOutAuctionItemInfoArray = pOutAuctionItemInfoArray;
    p_parameter->searchResult.numberOfFound = 0;
    p_parameter->searchResult.totalNumberOfFound = 0;
    p_parameter->searchResult.roi_search_category = pSearchByItemId->ROI_Search_category_key;
    p_parameter->refineStart = pSearchByItemId->refineStart;
    p_parameter->refineEnd = pSearchByItemId->refineEnd;
    result = SearchByItemIdUpgrade(p_parameter, &mItemIdUpgradeContainer);
    if (result == 0)
    {
        *pNumberOfFound = p_parameter->searchResult.numberOfFound;
        *pTotalNumberOfFound = p_parameter->searchResult.totalNumberOfFound;
        result = 0;
    }
    return result;
}

int Search::FindByCategory(TSearchByCategory_* pSearchByCategory,
                           unsigned int* pTotalNumberOfFound, unsigned short* pNumberOfFound,
                           AuctionItemInfo* pOutAuctionItemInfoArray)
{
    static TCategoryParameter search_by_category_parameter;
    static TCategoryParameter* p_parameter = &search_by_category_parameter;

    int result;

    if (!IsValidCategory(pSearchByCategory->category, STATE_SEARCH))
    {
        return 0x19;
    }
    if (!IsValidRarity(pSearchByCategory->rarity))
    {
        return 0x1a;
    }
    if (!IsValidUpgradeRange(pSearchByCategory->upgradeStart, pSearchByCategory->upgradeEnd))
    {
        return 0x1c;
    }
    if (!IsValidRefineRange(pSearchByCategory->refineStart, pSearchByCategory->refineEnd))
    {
        return 0x33;
    }
    if (pSearchByCategory->lvStart > pSearchByCategory->lvEnd)
    {
        return 0x1b;
    }
#ifdef POINT_SERVER
    if (pSearchByCategory->lvStart > 0x46)
#else
    if (pSearchByCategory->lvStart > 0x55)
#endif
    {
        return 0x1b;
    }
#ifdef POINT_SERVER
    if (pSearchByCategory->lvEnd > 0x46)
#else
    if (pSearchByCategory->lvEnd > 0x55)
#endif
    {
        return 0x1b;
    }
    p_parameter->categoryEnd = FindNextSameLevelCategory(pSearchByCategory->category);
    p_parameter->categoryStart = pSearchByCategory->category;
    p_parameter->rarity = pSearchByCategory->rarity;
    p_parameter->upgradeStart = pSearchByCategory->upgradeStart;
    p_parameter->upgradeEnd = pSearchByCategory->upgradeEnd;
    p_parameter->lvStart = pSearchByCategory->lvStart;
    p_parameter->lvEnd = pSearchByCategory->lvEnd;
    p_parameter->refineStart = pSearchByCategory->refineStart;
    p_parameter->refineEnd = pSearchByCategory->refineEnd;
    p_parameter->searchResult.startIdx = pSearchByCategory->startIndex;
    p_parameter->searchResult.isFirstSearch = pSearchByCategory->startIndex == 0;
    p_parameter->searchResult.pOutAuctionItemInfoArray = pOutAuctionItemInfoArray;
    p_parameter->searchResult.numberOfFound = 0;
    p_parameter->searchResult.totalNumberOfFound = 0;
    p_parameter->searchResult.roi_search_category = pSearchByCategory->ROI_Search_category_key;
    if (IsCategoryHasSocket(p_parameter->categoryStart))
    {
        p_parameter->upgradeStart = 1;
        p_parameter->upgradeEnd = 1;
    }
    if (IsSpecificRarity(p_parameter->rarity))
    {
        if (IsSpecificLv(p_parameter))
        {
            result = SearchByCategoryRarityUpgradeLv(p_parameter,
                                                     &mCategoryRarityUpgradeLvContainer);
        }
        else
        {
            result = SearchByCategoryRarityUpgrade(p_parameter,
                                                   &mCategoryRarityUpgradeContainer);
        }
    }
    else
    {
        if (IsSpecificLv(p_parameter))
        {
            result = SearchByCategoryUpgradeLv(p_parameter, &mCategoryUpgradeLvContainer);
        }
        else
        {
            result = SearchByCategoryUpgrade(p_parameter, &mCategoryUpgradeContainer);
        }
    }
    if (result == 0)
    {
        *pNumberOfFound = p_parameter->searchResult.numberOfFound;
        *pTotalNumberOfFound = p_parameter->searchResult.totalNumberOfFound;
        result = 0;
    }
    return result;
}

int Search::SearchByRefineWrapper(BYTE refineFrom, BYTE refineTo, TSearchResult* pSearchResult,
                                  PRefineContainer pParent)
{
    TRefineContainer::iterator pos;

    pos = pParent->lower_bound(refineFrom);
    while (pos != pParent->upper_bound(refineTo))
    {
        if (!pSearchResult->roi_search_category.isEmpty())
        {
            ROI_SetSearchResult(pSearchResult, pos->second);
        }
        else
        {
            SetSearchResult(pSearchResult, pos->second);
        }
        if (IsRequireStopSearch(pSearchResult))
        {
            break;
        }
        ++pos;
    }
    return 0;
}

int Search::SearchByUpgradeWrapper(TItemIdParameter* pParameter, TSearchResult* pSearchResult,
                                   PUpgradeContainer pParent)
{
    TUpgradeContainer::iterator pos;

    pos = pParent->lower_bound(pParameter->upgradeStart);
    while (pos != pParent->upper_bound(pParameter->upgradeEnd))
    {
        SearchByRefineWrapper(pParameter->refineStart, pParameter->refineEnd,
                              &pParameter->searchResult, pos->second);
        if (IsRequireStopSearch(pSearchResult))
        {
            break;
        }
        ++pos;
    }
    return 0;
}

int Search::SearchByUpgradeWrapper(TCategoryParameter* pParameter, TSearchResult* pSearchResult,
                                   PUpgradeContainer pParent)
{
    TUpgradeContainer::iterator pos;

    pos = pParent->lower_bound(pParameter->upgradeStart);
    while (pos != pParent->upper_bound(pParameter->upgradeEnd))
    {
        SearchByRefineWrapper(pParameter->refineStart, pParameter->refineEnd,
                              &pParameter->searchResult, pos->second);
        if (IsRequireStopSearch(pSearchResult))
        {
            break;
        }
        ++pos;
    }
    return 0;
}

int Search::SearchByUpgrade(TItemIdParameter* pParameter, PUpgradeContainer pParent)
{
    return SearchByUpgradeWrapper(pParameter, &pParameter->searchResult, pParent);
}

int Search::SearchByUpgrade(TCategoryParameter* pParameter, PUpgradeContainer pParent)
{
    return SearchByUpgradeWrapper(pParameter, &pParameter->searchResult, pParent);
}

int Search::SearchByLv(TCategoryParameter* pParameter, PLvContainer pParent)
{
    TSearchResult* pSearchResult;
    BYTE lvStart;
    TLvContainer::iterator pos;

    pSearchResult = &pParameter->searchResult;
    pSearchResult->category = -1;
    lvStart = pParameter->lvStart;
    pos = pParent->lower_bound(lvStart);
    while (pos != pParent->upper_bound(pParameter->lvEnd))
    {
        if (!pSearchResult->roi_search_category.isEmpty())
        {
            ROI_SetSearchResult(pSearchResult, pos->second);
        }
        else
        {
            SetSearchResult(pSearchResult, pos->second);
        }
        if (IsRequireStopSearch(pSearchResult))
        {
            break;
        }
        ++pos;
    }
    return 0;
}

int Search::SearchByRefineLv(TCategoryParameter* pParameter, PRefineLvContainer pParent)
{
    TRefineLvContainer::iterator pos;

    pos = pParent->lower_bound(pParameter->refineStart);
    while (pos != pParent->upper_bound(pParameter->refineEnd))
    {
        if (SearchByLv(pParameter, pos->second) != 0)
        {
            return 0x34;
        }
        if (IsRequireStopSearch(&pParameter->searchResult))
        {
            break;
        }
        ++pos;
    }
    return 0;
}

int Search::SearchByUpgradeLv(TCategoryParameter* pParameter, PUpgradeLvContainer pParent)
{
    TUpgradeLvContainer::iterator pos;

    pos = pParent->lower_bound(pParameter->upgradeStart);
    while (pos != pParent->upper_bound(pParameter->upgradeEnd))
    {
        if (SearchByRefineLv(pParameter, pos->second) != 0)
        {
            return 0x18;
        }
        if (IsRequireStopSearch(&pParameter->searchResult))
        {
            break;
        }
        ++pos;
    }
    return 0;
}

int Search::SearchByRarityUpgrade(TCategoryParameter* pParameter, PRarityUpgradeContainer pParent)
{
    BYTE key = pParameter->rarity;
    TRarityUpgradeContainer::iterator pos;
    pos = pParent->find(key);
    if (pos != pParent->end())
    {
        if (SearchByUpgrade(pParameter, pos->second) != 0)
        {
            return 0x16;
        }
    }
    return 0;
}

int Search::SearchByRarityUpgradeLv(TCategoryParameter* pParameter,
                                    PRarityUpgradeLvContainer pParent)
{
    BYTE key = pParameter->rarity;
    TRarityUpgradeLvContainer::iterator pos;
    pos = pParent->find(key);
    if (pos != pParent->end())
    {
        if (SearchByUpgradeLv(pParameter, pos->second) != 0)
        {
            return 0x15;
        }
    }
    return 0;
}

int Search::SearchByCategoryUpgrade(TCategoryParameter* pParameter,
                                    PCategoryUpgradeContainer pParent)
{
    TCategoryUpgradeContainer::iterator pos;

    pos = pParent->lower_bound(pParameter->categoryStart);
    while (pos != pParent->upper_bound(pParameter->categoryEnd))
    {
        SetIsAvatar(pos->first);
        if (SearchByUpgrade(pParameter, pos->second) != 0)
        {
            return 0x18;
        }
        if (IsRequireStopSearch(&pParameter->searchResult))
        {
            break;
        }
        ++pos;
    }
    return 0;
}

int Search::SearchByCategoryUpgradeLv(TCategoryParameter* pParameter,
                                      PCategoryUpgradeLvContainer pParent)
{
    TCategoryUpgradeLvContainer::iterator pos;

    pos = pParent->lower_bound(pParameter->categoryStart);
    while (pos != pParent->upper_bound(pParameter->categoryEnd))
    {
        SetIsAvatar(pos->first);
        if (SearchByUpgradeLv(pParameter, pos->second) != 0)
        {
            return 0x17;
        }
        if (IsRequireStopSearch(&pParameter->searchResult))
        {
            break;
        }
        ++pos;
    }
    return 0;
}

int Search::SearchByCategoryRarityUpgrade(TCategoryParameter* pParameter,
                                          PCategoryRarityUpgradeContainer pParent)
{
    TCategoryRarityUpgradeContainer::iterator pos;

    pos = pParent->lower_bound(pParameter->categoryStart);
    while (pos != pParent->upper_bound(pParameter->categoryEnd))
    {
        SetIsAvatar(pos->first);
        if (SearchByRarityUpgrade(pParameter, pos->second) != 0)
        {
            return 0x16;
        }
        if (IsRequireStopSearch(&pParameter->searchResult))
        {
            break;
        }
        ++pos;
    }
    return 0;
}

int Search::SearchByCategoryRarityUpgradeLv(TCategoryParameter* pParameter,
                                            PCategoryRarityUpgradeLvContainer pParent)
{
    TCategoryRarityUpgradeLvContainer::iterator pos;

    pos = pParent->lower_bound(pParameter->categoryStart);
    while (pos != pParent->upper_bound(pParameter->categoryEnd))
    {
        SetIsAvatar(pos->first);
        if (SearchByRarityUpgradeLv(pParameter, pos->second) != 0)
        {
            return 0x15;
        }
        if (IsRequireStopSearch(&pParameter->searchResult))
        {
            break;
        }
        ++pos;
    }
    return 0;
}

int Search::SearchByItemIdUpgrade(TItemIdParameter* pParameter, PItemIdUpgradeContainer pParent)
{
    unsigned long key;
    CNRDItemInfoList::STItemInfo* p_item_info;
    bool has_category_selected;
    BYTE upgrade_start_backup;
    BYTE upgrade_end_backup;
    int i;
    TItemIdUpgradeContainer::iterator pos;

    upgrade_start_backup = pParameter->upgradeStart;
    upgrade_end_backup = pParameter->upgradeEnd;
    has_category_selected = pParameter->categoryStart < CATEGORY_NONE;
    i = 0;
    while (i < pParameter->itemIdNum)
    {
        if (has_category_selected &&
            mpAuction->IsAvatarCategory(pParameter->categoryStart))
        {
            if (IsCategoryHasSocket(pParameter->categoryStart))
            {
                pParameter->upgradeStart = 1;
                pParameter->upgradeEnd = 1;
            }
            else
            {
                pParameter->upgradeStart = 0;
                pParameter->upgradeEnd = 0;
            }
        }
        key = pParameter->pItemIdArray[i];
        pos = pParent->find(key);
        if (pos != pParent->end())
        {
            p_item_info = mpAuction->GetItemInfo(key);
            if (p_item_info == (CNRDItemInfoList::STItemInfo*)0x0)
            {
                return 0x27;
            }
            SetIsAvatar(p_item_info->category_);
            if (!has_category_selected && IsAvatar())
            {
                pParameter->upgradeStart = 0;
                pParameter->upgradeEnd = 1;
            }
            if (SearchByUpgrade(pParameter, pos->second) != 0)
            {
                return 0x14;
            }
            if (IsRequireStopSearch(&pParameter->searchResult))
            {
                return 0;
            }
        }
        pParameter->upgradeStart = upgrade_start_backup;
        pParameter->upgradeEnd = upgrade_end_backup;
        i = i + 1;
    }
    return 0;
}

int Search::GetRegisteredInfo(unsigned long long auctionId, unsigned long* pItemId,
                              BYTE* pUpgrade, BYTE* pRefine)
{
    AuctionDictionary* p_auction_dictionary = &mpAuction->mAuctionDic;
    AuctionDictionary::AuctionDictionaryData* p_auction_dictionary_data;
    std::map<unsigned long long, AuctionDictionary::AuctionDictionaryData*>::iterator iter =
        p_auction_dictionary->mAuctionDicTable.find(auctionId);
    int result;

    if (iter != p_auction_dictionary->mAuctionDicTable.end())
    {
        p_auction_dictionary_data = iter->second;
        if (p_auction_dictionary_data != (AuctionDictionary::AuctionDictionaryData*)0x0)
        {
            *pItemId = p_auction_dictionary_data->item_info.GetItemId();
            *pUpgrade = p_auction_dictionary_data->item_info.GetUpgradeValue();
            *pRefine = p_auction_dictionary_data->item_info.separate_info.GetUpgradeSeparate();
            result = 0;
        }
        else
        {
            result = 0x24;
        }
    }
    else
    {
        result = 0x24;
    }
    return result;
}

int Search::GetAuctionItemInfo(unsigned long long auctionId,
                               const ROI_Category& roi_search_category,
                               AuctionItemInfo* pAuctionItemInfo)
{
    AuctionDictionary* p_auction_dictionary = &mpAuction->mAuctionDic;
    memset(pAuctionItemInfo, 0, 0x89);
    std::map<unsigned long long, AuctionDictionary::AuctionDictionaryData*>::iterator iter =
        p_auction_dictionary->mAuctionDicTable.find(auctionId);
    AuctionDictionary::AuctionDictionaryData* p_auction_dictionary_data;
    unsigned long itemId;
    unsigned char itemUpgradeValue;
    unsigned char itemRefineValue;
    int result;
    if (iter != p_auction_dictionary->mAuctionDicTable.end())
    {
        p_auction_dictionary_data = iter->second;
        if (p_auction_dictionary_data == (AuctionDictionary::AuctionDictionaryData*)0x0)
        {
            result = 0x24;
        }
        else if (roi_search_category.isEmpty() ||
                 p_auction_dictionary_data->_reg_roi_category_key.isMatching(
                     roi_search_category))
        {
            ROI_AverageKey roi_average_key;

            pAuctionItemInfo->auction_id = auctionId;
            pAuctionItemInfo->price = p_auction_dictionary_data->price;
            pAuctionItemInfo->instant_price = p_auction_dictionary_data->instant_price;
            roi_average_key.baseItem_index = p_auction_dictionary_data->item_info.GetItemId();
            roi_average_key.option_category = p_auction_dictionary_data->_reg_roi_category_key;
            for (int i = 0; i < 3; i = i + 1)
            {
                roi_average_key._oiv.option_index_value[i] =
                    (unsigned short)p_auction_dictionary_data->item_info.random_option_
                        .option_[i].option_index_;
            }
            std::sort((short*)&roi_average_key.option_index_key,
                      (short*)((int)&roi_average_key.option_index_key + 6));
            itemRefineValue =
                p_auction_dictionary_data->item_info.separate_info.GetUpgradeSeparate();
            itemUpgradeValue = p_auction_dictionary_data->item_info.GetUpgradeValue();
            itemId = p_auction_dictionary_data->item_info.GetItemId();
            result = p_auction_dictionary->mAvrgPriceDic.GetItemAveragePrice(
                itemId, itemUpgradeValue, roi_average_key, itemRefineValue,
                &pAuctionItemInfo->average_price);
            if (result == 0)
            {
                strncpy(pAuctionItemInfo->owner_name,
                        p_auction_dictionary->getCharacterName(
                            p_auction_dictionary_data->owner_id),
                        0xc);
                pAuctionItemInfo->expire_time = p_auction_dictionary->getExpiringTime(
                    p_auction_dictionary_data->expire_time, 0);
                pAuctionItemInfo->item_info = p_auction_dictionary_data->item_info;
                pAuctionItemInfo->black_point = p_auction_dictionary_data->black_point;
                pAuctionItemInfo->unit_price = p_auction_dictionary_data->unit_price;
                pAuctionItemInfo->item_info.abilityType_ =
                    p_auction_dictionary_data->item_info.getAbilityType();
                pAuctionItemInfo->item_info.abilityValue_ =
                    p_auction_dictionary_data->item_info.getAbilityValue();
                result = 0;
            }
        }
        else
        {
            result = 0x24;
        }
    }
    else
    {
        result = 0x24;
    }
    return result;
}

bool Search::IsValidCategory(WORD category, STATE_SEARCH_MODULE_OPERATION operation)
{
    bool is_valid_category;

    is_valid_category = category < 0x277b ||
                        ((0x27d8 < category) && (category < 0x27df)) ||
                        ((0x283c < category) && (category < 0x2843)) ||
                        ((0x28a0 < category) && (category < 0x28a7)) ||
                        ((0x2904 < category) && (category < 0x290b)) ||
                        ((0x2968 < category) && (category < 0x296d)) ||
                        ((0x29cc < category) && (category < 0x29d3)) ||
                        ((10999 < category) && (category < 0x2aff)) ||
                        ((0x2b5b < category) && (category < 0x2b62)) ||
                        ((0x2bbf < category) && (category < 0x2bc6)) ||
                        ((0x2c23 < category) && (category < 0x2c2a)) ||
                        ((0x2c87 < category) && (category < 0x2c8e)) ||
                        ((11999 < category) && (category < 0x2ee5)) ||
                        ((12999 < category) && (category < 0x32cf)) ||
                        ((13999 < category) && (category < 0x36b5)) ||
                        ((14999 < category) && (category < 0x3dc4)) ||
                        ((16999 < category) && (category < 0x4594)) ||
                        ((18999 < category) && (category < 0x4d64)) ||
                        ((20999 < category) && (category < 0x5534)) ||
                        ((22999 < category) && (category < 0x5d04)) ||
                        ((24999 < category) && (category < 0x64d4)) ||
                        ((29999 < category) && (category < 0x7599)) ||
                        ((30999 < category) && (category < 0x7a4a)) ||
                        ((31999 < category) && (category < 0x7d71)) ||
                        ((32999 < category) && (category < 0x80ed)) ||
                        ((33999 < category) && (category < 0x84d1));
    if ((operation != STATE_SEARCH) && (is_valid_category != false))
    {
        is_valid_category = !(category == 10000 ||
                              category == 0x2775 ||
                              category == 0x27d9 ||
                              category == 0x283d ||
                              category == 0x28a1 ||
                              category == 0x2905 ||
                              category == 11000 ||
                              category == 0x2af9 ||
                              category == 0x2b5c ||
                              category == 0x2bc0 ||
                              category == 0x2c24 ||
                              category == 0x2c88 ||
                              category == 12000 ||
                              category == 13000 ||
                              category == 14000 ||
                              category == 15000 ||
                              category == 0x3a99 ||
                              category == 0x3afd ||
                              category == 0x3b61 ||
                              category == 0x3bc5 ||
                              category == 0x3c29 ||
                              category == 0x3cf1 ||
                              category == 0x3d55 ||
                              category == 0x3db9 ||
                              category == 17000 ||
                              category == 0x4269 ||
                              category == 0x42cd ||
                              category == 0x4331 ||
                              category == 0x4395 ||
                              category == 0x43f9 ||
                              category == 0x44c1 ||
                              category == 0x4525 ||
                              category == 0x4589 ||
                              category == 19000 ||
                              category == 0x4a39 ||
                              category == 0x4a9d ||
                              category == 0x4b01 ||
                              category == 0x4b65 ||
                              category == 0x4bc9 ||
                              category == 0x4c91 ||
                              category == 0x4cf5 ||
                              category == 0x4d59 ||
                              category == 21000 ||
                              category == 0x5209 ||
                              category == 0x526d ||
                              category == 0x52d1 ||
                              category == 0x5335 ||
                              category == 0x5399 ||
                              category == 0x5461 ||
                              category == 0x54c5 ||
                              category == 0x5529 ||
                              category == 23000 ||
                              category == 0x59d9 ||
                              category == 0x5a3d ||
                              category == 0x5aa1 ||
                              category == 0x5b05 ||
                              category == 0x5b69 ||
                              category == 0x5c31 ||
                              category == 0x5c95 ||
                              category == 0x5cf9 ||
                              category == 25000 ||
                              category == 0x61a9 ||
                              category == 0x620d ||
                              category == 0x6271 ||
                              category == 0x62d5 ||
                              category == 0x6339 ||
                              category == 0x6401 ||
                              category == 0x6465 ||
                              category == 0x64c9);
    }
    return is_valid_category;
}

void Search::InitializeCategoryNextContainerData()
{
    static const int NUMBER_OF_AVATAR_TYPE = 6;
    static const int NUMBER_OF_CLASS = 9;
    static const int GAP_OF_BEGIN_AND_END_IN_SAME_CLASS = 10;
    static const int GAP_OF_NEXT_CLASS_AVATAR = 100;
    static const int AVATAR_CATEGORY_START_INDEXES[NUMBER_OF_AVATAR_TYPE] = {
        CATEGORY_AVATAR_SWORDMAN,
        CATEGORY_CLONE_AVATAR_SWORDMAN,
        CATEGORY_EMBLEM_AVATAR_SWORDMAN,
        CATEGORY_CLONE_EMBLEM_AVATAR_SWORDMAN,
        CATEGORY_NORMAL_EMBLEM_AVATAR_SWORDMAN,
        CATEGORY_CLONE_NORMAL_EMBLEM_AVATAR_SWORDMAN,
    };

    mCategoryNextContainer.insert(std::make_pair(CATEGORY_WEAPON, CATEGORY_WEAPON_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_SWORD_MAN, CATEGORY_SWORD_MAN_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_FIGHTER, CATEGORY_FIGHTER_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_GUNNER, CATEGORY_GUNNER_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_MAGE, CATEGORY_MAGE_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_PRIEST, CATEGORY_PRIEST_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_THIEF, CATEGORY_THIEF_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_AT_FIGHTER, CATEGORY_AT_FIGHTER_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_DEFENCE, CATEGORY_DEFENCE_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_CLOTH, CATEGORY_CLOTH_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_LEATHER, CATEGORY_LEATHER_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_LIGHT, CATEGORY_LIGHT_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_HEAVY, CATEGORY_HEAVY_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_METAL, CATEGORY_METAL_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_ACCESSARY, CATEGORY_ACCESSARY_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_STACKABLE, CATEGORY_STACKABLE_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_CREATURE, CATEGORY_CREATURE_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_AVATAR, CATEGORY_AVATAR_LAST));

    for (int i = 0; i < NUMBER_OF_AVATAR_TYPE; i = i + 1)
    {
        int first_element = AVATAR_CATEGORY_START_INDEXES[i];
        for (int j = 0; j < NUMBER_OF_CLASS; j = j + 1)
        {
            mCategoryNextContainer.insert(
                std::make_pair(first_element,
                               first_element + GAP_OF_BEGIN_AND_END_IN_SAME_CLASS));
            first_element = first_element + GAP_OF_NEXT_CLASS_AVATAR;
        }
    }

    mCategoryNextContainer.insert(std::make_pair(CATEGORY_CLONE_AVATAR, CATEGORY_CLONE_AVATAR_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_EMBLEM_AVATAR, CATEGORY_EMBLEM_AVATAR_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_CLONE_EMBLEM_AVATAR,
                                                 CATEGORY_CLONE_EMBLEM_AVATAR_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_NORMAL_EMBLEM_AVATAR,
                                                 CATEGORY_NORMAL_EMBLEM_AVATAR_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_CLONE_NORMAL_EMBLEM_AVATAR,
                                                 CATEGORY_CLONE_NORMAL_EMBLEM_AVATAR_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_STACKABLE_EMBLEM,
                                                 CATEGORY_STACKABLE_EMBLEM_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_STACKABLE_EMBLEM_M,
                                                 CATEGORY_STACKABLE_EMBLEM_M_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_ETC, CATEGORY_ETC_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_RECIPE, CATEGORY_RECIPE_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_RECIPE_WEAPON,
                                                 CATEGORY_RECIPE_WEAPON_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_RECIPE_DEFENCE,
                                                 CATEGORY_RECIPE_DEFENCE_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_RECIPE_ACCESSARY,
                                                 CATEGORY_RECIPE_ACCESSARY_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_RECIPE_SPECIAL_EQUIPMENT,
                                                 CATEGORY_RECIPE_SPECIAL_EQUIPMENT_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_SPECIAL_EQUIPMENT,
                                                 CATEGORY_SPECIAL_EQUIPMENT_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_EXPERT, CATEGORY_EXPERT_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_SPECIAL_EQUIPMENT_SUPPORT,
                                                 CATEGORY_SPECIAL_EQUIPMENT_SUPPORT_LAST));
    mCategoryNextContainer.insert(std::make_pair(CATEGORY_SPECIAL_EQUIPMENT_MAGIC_STONE,
                                                 CATEGORY_SPECIAL_EQUIPMENT_MAGIC_STONE_LAST));
}
