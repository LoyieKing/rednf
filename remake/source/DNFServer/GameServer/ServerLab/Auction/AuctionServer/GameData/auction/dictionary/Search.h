#ifndef SEARCH_H_
#define SEARCH_H_

#include <map>
#include <set>

#include "AuctionPacket.h"
#include "RDARScriptItemInfo.h"

class Auction;

typedef unsigned char BYTE;
typedef unsigned short WORD;

class Search
{
public:
    struct AuctionId
    {
        unsigned long long auctionId;   // @0
        int instantPrice;               // @8

        bool operator==(const AuctionId& _rhs) const
        {
            return auctionId == _rhs.auctionId;
        }
        bool operator<(const AuctionId& _rhs) const
        {
            return auctionId < _rhs.auctionId;
        }
    };

    typedef std::multiset<AuctionId> TAuctionIdContainer;
    typedef TAuctionIdContainer* PAuctionIdContainer;

    typedef std::map<unsigned char, PAuctionIdContainer> TRefineContainer;
    typedef TRefineContainer* PRefineContainer;

    typedef std::map<unsigned char, PRefineContainer> TUpgradeContainer;
    typedef TUpgradeContainer* PUpgradeContainer;

    typedef std::map<unsigned char, PAuctionIdContainer> TLvContainer;
    typedef TLvContainer* PLvContainer;

    typedef std::map<unsigned char, PRefineContainer> TRefineLvContainer;
    typedef TRefineLvContainer* PRefineLvContainer;

    typedef std::map<unsigned char, PRefineLvContainer> TUpgradeLvContainer;
    typedef TUpgradeLvContainer* PUpgradeLvContainer;

    typedef std::map<unsigned char, PRefineLvContainer> TRarityUpgradeContainer;
    typedef TRarityUpgradeContainer* PRarityUpgradeContainer;

    typedef std::map<unsigned char, PUpgradeLvContainer> TRarityUpgradeLvContainer;
    typedef TRarityUpgradeLvContainer* PRarityUpgradeLvContainer;

    typedef std::map<unsigned short, PUpgradeContainer> TCategoryUpgradeContainer;
    typedef TCategoryUpgradeContainer* PCategoryUpgradeContainer;

    typedef std::map<unsigned short, PUpgradeLvContainer> TCategoryUpgradeLvContainer;
    typedef TCategoryUpgradeLvContainer* PCategoryUpgradeLvContainer;

    typedef std::map<unsigned short, PUpgradeLvContainer> TCategoryRarityUpgradeContainer;
    typedef TCategoryRarityUpgradeContainer* PCategoryRarityUpgradeContainer;

    typedef std::map<unsigned short, PRarityUpgradeLvContainer> TCategoryRarityUpgradeLvContainer;
    typedef TCategoryRarityUpgradeLvContainer* PCategoryRarityUpgradeLvContainer;

    typedef std::map<unsigned long, PUpgradeContainer> TItemIdUpgradeContainer;
    typedef TItemIdUpgradeContainer* PItemIdUpgradeContainer;

    enum STATE_SEARCH_MODULE_OPERATION
    {
        STATE_INSERT = 0,
        STATE_DELETE = 1,
        STATE_SEARCH = 2,
    };

    struct TOperate
    {
        unsigned long itemId;              // @0
        WORD category;                     // @4
        BYTE rarity;                       // @6
        BYTE upgrade;                      // @7
        BYTE lv;                           // @8
        unsigned long long auctionId;      // @12
        int instantPrice;                  // @20
        BYTE refine;                       // @24
        STATE_SEARCH_MODULE_OPERATION operation;  // @28
    };

    struct TSearchResult
    {
        TSearchResult()
        {
        }

        unsigned int startIdx;                   // @0
        bool isFirstSearch;                      // @4
        unsigned int totalNumberOfFound;         // @8
        unsigned short numberOfFound;            // @12
        AuctionItemInfo* pOutAuctionItemInfoArray;  // @16
        int category;                            // @20
        ROI_Category roi_search_category;        // @24
        BYTE refine;                             // @36
    };

    struct TItemIdParameter
    {
        TItemIdParameter()
        {
        }

        WORD categoryStart;                      // @0
        BYTE itemIdNum;                          // @2
        unsigned long* pItemIdArray;             // @4
        BYTE upgradeStart;                       // @8
        BYTE upgradeEnd;                         // @9
        BYTE refineStart;                        // @10
        BYTE refineEnd;                          // @11
        TSearchResult searchResult;              // @12
    };

    struct TCategoryParameter
    {
        TCategoryParameter()
        {
        }

        WORD categoryStart;                      // @0
        WORD categoryEnd;                        // @2
        BYTE rarity;                             // @4
        BYTE upgradeStart;                       // @5
        BYTE upgradeEnd;                         // @6
        BYTE refineStart;                        // @7
        BYTE refineEnd;                          // @8
        int lvStart;                             // @12
        int lvEnd;                               // @16
        TSearchResult searchResult;              // @20
    };

    Search();
    virtual ~Search();

    int Insert(unsigned long itemId, BYTE upgrade, unsigned long long auctionId,
               bool hasSocket, int instantPrice, BYTE refine);
    int Delete(unsigned long long auctionId);
    int FindByItem(TSearchByItemId_* pSearchByItemId, unsigned long* pItemIdArray,
                   unsigned int* pTotalNumberOfFound, unsigned short* pNumberOfFound,
                   AuctionItemInfo* pOutAuctionItemInfoArray);
    int FindByCategory(TSearchByCategory_* pSearchByCategory,
                       unsigned int* pTotalNumberOfFound, unsigned short* pNumberOfFound,
                       AuctionItemInfo* pOutAuctionItemInfoArray);

    int OperateByRefine(TOperate* pOperate, PRefineContainer pParent);
    int OperateByUpgrade(TOperate* pOperate, PUpgradeContainer pParent);
    int OperateByLv(TOperate* pOperate, PLvContainer pParent);
    int OperateByRefineLv(TOperate* pOperate, PRefineLvContainer pParent);
    int OperateByUpgradeLv(TOperate* pOperate, PUpgradeLvContainer pParent);
    int OperateByRarityUpgrade(TOperate* pOperate, PRarityUpgradeContainer pParent);
    int OperateByRarityUpgradeLv(TOperate* pOperate, PRarityUpgradeLvContainer pParent);
    int OperateByItemIdUpgrade(TOperate* pOperate, PItemIdUpgradeContainer pParent);
    int OperateByCategoryUpgrade(TOperate* pOperate, PCategoryUpgradeContainer pParent);
    int OperateByCategoryUpgradeLv(TOperate* pOperate, PCategoryUpgradeLvContainer pParent);
    int OperateByCategoryRarityUpgrade(TOperate* pOperate, PCategoryRarityUpgradeContainer pParent);
    int OperateByCategoryRarityUpgradeLv(TOperate* pOperate,
                                         PCategoryRarityUpgradeLvContainer pParent);

    bool IsValidCategory(WORD category, STATE_SEARCH_MODULE_OPERATION operation);
    bool IsValidUpgrade(BYTE upgrade);
    bool IsValidUpgradeRange(BYTE upgradeStart, BYTE upgradeEnd);
    bool IsValidRefine(BYTE refine);
    bool IsValidRefineRange(BYTE refineStart, BYTE refineEnd);
    bool IsValidRarity(BYTE rarity);
    bool IsAlternativeAvater(int category);
    int SetOperateParameter(TOperate* pOperate, unsigned long itemId, BYTE upgrade,
                            unsigned long long auctionId,
                            const CNRDItemInfoList::STItemInfo* pItemInfo,
                            STATE_SEARCH_MODULE_OPERATION operation, bool hasSocket,
                            int instantPrice, BYTE refine);
    bool IsSpecificRarity(BYTE rarity);
    bool IsSpecificLv(TCategoryParameter* pParameter);
    bool IsAvatarCategory(int category);
    bool IsFindEnough(TSearchResult* pSearchResult);
    bool IsRequireStopSearch(TSearchResult* pSearchResult);
    int GetAuctionItemInfo(unsigned long long auctionId, const ROI_Category& roi_search_category,
                           AuctionItemInfo* pAuctionItemInfo);
    WORD FindNextSameLevelCategory(WORD category);
    bool IsCategoryHasSocket(int category);
    int SetSearchResult(TSearchResult* pSearchResult, PAuctionIdContainer pContainer);
    int SearchByLv(TCategoryParameter* pParameter, PLvContainer pParent);
    int ROI_SetSearchResult(TSearchResult* pSearchResult, PAuctionIdContainer pContainer);
    int SearchByRefineLv(TCategoryParameter* pParameter, PRefineLvContainer pParent);
    int SearchByRefineWrapper(BYTE refineFrom, BYTE refineTo, TSearchResult* pSearchResult,
                              PRefineContainer pParent);
    int SearchByUpgradeWrapper(TItemIdParameter* pParameter, TSearchResult* pSearchResult,
                               PUpgradeContainer pParent);
    int SearchByUpgradeWrapper(TCategoryParameter* pParameter, TSearchResult* pSearchResult,
                               PUpgradeContainer pParent);
    int SearchByUpgrade(TItemIdParameter* pParameter, PUpgradeContainer pParent);
    int SearchByUpgrade(TCategoryParameter* pParameter, PUpgradeContainer pParent);
    int SearchByUpgradeLv(TCategoryParameter* pParameter, PUpgradeLvContainer pParent);
    int SearchByRarityUpgrade(TCategoryParameter* pParameter, PRarityUpgradeContainer pParent);
    int SearchByRarityUpgradeLv(TCategoryParameter* pParameter, PRarityUpgradeLvContainer pParent);
    int SearchByItemIdUpgrade(TItemIdParameter* pParameter, PItemIdUpgradeContainer pParent);
    int SearchByCategoryUpgrade(TCategoryParameter* pParameter, PCategoryUpgradeContainer pParent);
    int SearchByCategoryUpgradeLv(TCategoryParameter* pParameter,
                                  PCategoryUpgradeLvContainer pParent);
    int SearchByCategoryRarityUpgrade(TCategoryParameter* pParameter,
                                      PCategoryRarityUpgradeContainer pParent);
    int SearchByCategoryRarityUpgradeLv(TCategoryParameter* pParameter,
                                        PCategoryRarityUpgradeLvContainer pParent);
    int GetItemInfoByItemId(unsigned long* pItemIdArray, BYTE* pUpgradeStart,
                            BYTE* pUpgradeEnd, BYTE* pRefine);
    int GetRegisteredInfo(unsigned long long auctionId, unsigned long* pItemId,
                          BYTE* pUpgrade, BYTE* pRefine);
    void InitializeCategoryNextContainerData();
    bool IsAvatar();
    void SetIsAvatar(int category);

    Auction* mpAuction;                                    // @4
    std::map<unsigned short, unsigned short> mCategoryNextContainer;   // @8
    TItemIdUpgradeContainer mItemIdUpgradeContainer;       // @32
    TCategoryRarityUpgradeLvContainer mCategoryRarityUpgradeLvContainer;  // @56
    TCategoryRarityUpgradeContainer mCategoryRarityUpgradeContainer;  // @80
    TCategoryUpgradeLvContainer mCategoryUpgradeLvContainer;  // @104
    TCategoryUpgradeContainer mCategoryUpgradeContainer;   // @128
    bool mIsAvatar;                                        // @152
};

#endif // SEARCH_H_
