#ifndef SEARCH_H_
#define SEARCH_H_

#include <map>
#include <set>
#include <vector>

class Auction;
struct DnfItemInfo;
struct ROI_Category;
struct AuctionItemInfo;
struct TSearchByItemId_;
struct TSearchByCategory_;
struct STItemInfo;

typedef unsigned char BYTE;
typedef unsigned short WORD;

class Search
{
public:
    struct AuctionId
    {
        unsigned long long auctionId;
        int instantPrice;

        bool operator==(const AuctionId& _rhs) const
        {
            return auctionId == _rhs.auctionId;
        }
        bool operator<(const AuctionId& _rhs) const
        {
            return auctionId < _rhs.auctionId;
        }
    };

    typedef std::multiset<AuctionId> PAuctionIdContainer;
    typedef std::map<unsigned char, PAuctionIdContainer> PRefineContainer;
    typedef std::map<unsigned char, PRefineContainer> PUpgradeContainer;
    typedef std::map<unsigned long, PUpgradeContainer> PItemIdUpgradeContainer;
    typedef std::map<unsigned char, PAuctionIdContainer> TLvContainer;
    typedef TLvContainer PLvContainer;
    typedef std::map<unsigned char, PLvContainer> PRefineLvContainer;
    typedef std::map<unsigned char, PRefineLvContainer> PUpgradeLvContainer;
    typedef std::map<unsigned char, PUpgradeContainer> PRarityUpgradeContainer;
    typedef std::map<unsigned char, PUpgradeLvContainer> PRarityUpgradeLvContainer;
    typedef std::map<unsigned short, PUpgradeContainer> PCategoryUpgradeContainer;
    typedef std::map<unsigned short, PUpgradeLvContainer> PCategoryUpgradeLvContainer;
    typedef std::map<unsigned short, PRarityUpgradeContainer>
        PCategoryRarityUpgradeContainer;
    typedef std::map<unsigned short, PRarityUpgradeLvContainer>
        PCategoryRarityUpgradeLvContainer;
    typedef PUpgradeContainer TUpgradeContainer;
    typedef PUpgradeLvContainer TUpgradeLvContainer;
    typedef PRarityUpgradeLvContainer TRarityUpgradeLvContainer;
    typedef PCategoryRarityUpgradeLvContainer TCategoryRarityUpgradeLvContainer;

    enum STATE_SEARCH_MODULE_OPERATION
    {
        STATE_INSERT = 0,
        STATE_DELETE = 1,
    };

    struct TOperate
    {
        unsigned long itemId;
        BYTE upgrade;
        unsigned long long auctionId;
        STItemInfo* pItemInfo;
        STATE_SEARCH_MODULE_OPERATION operation;
        bool hasSocket;
        int instantPrice;
        BYTE refine;
        BYTE rarity;
        unsigned short category;
        BYTE lv;
        bool isAlternativeAvater;
    };

    struct TItemIdParameter
    {
        unsigned long itemId;
    };

    struct TCategoryParameter
    {
        unsigned short category;
    };

    struct TSearchResult
    {
        unsigned int totalNumberOfFound;
        unsigned int startIdx;
        bool isFirstSearch;
        AuctionItemInfo* pOutAuctionItemInfoArray;
        unsigned short* pNumberOfFound;
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

    Auction* mpAuction;
    std::map<unsigned short, unsigned short> mCategoryNextContainer;   // @8
    PItemIdUpgradeContainer mItemIdUpgradeContainer;                   // @0x20
    TCategoryRarityUpgradeLvContainer mCategoryRarityUpgradeLvContainer;  // @0x38
    PCategoryRarityUpgradeContainer mCategoryRarityUpgradeContainer;   // @0x50
    PCategoryUpgradeLvContainer mCategoryUpgradeLvContainer;           // @0x68
    PCategoryUpgradeContainer mCategoryUpgradeContainer;               // @0x80
    bool mIsAvatar;                                                    // @0x98
};

#endif // SEARCH_H_
