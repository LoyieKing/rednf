// Rebuilt from df_auction_r DWARF (AuctionDictionary.cpp CU), 2026-08-08
#ifndef AUCTIONDICTIONARY_H_
#define AUCTIONDICTIONARY_H_

#include <map>

#include "ObjectPoolByBoostPool.h"
#include "AuctionPacket.h"
#include "ExpireTimeDictionary.h"
#include "AveragePriceDictionary.h"
#include "CharacterDictionary.h"

class Auction;

enum AUCTION_HISTORY_EVENT_TYPE
{
    AUCTION_HISTORY_EXPIRE_EVENT = 0,
    AUCTION_HISTORY_INSTANT_BUYING_EVENT = 1,
    AUCTION_HISTORY_REGIST_CANCEL_EVENT = 2,
    AUCTION_HISTORY_APIECE_EVENT = 3,
};

enum ENUM_AUCTION_TYPE
{
    ENUM_AUCTION_USER = 0,
    ENUM_AUCTION_SYSTEM = 1,
};

typedef unsigned long long __int64;

class AuctionDictionary
{
public:
    struct CharacterNameStruct
    {
        char char_name[13];
    };

    struct AuctionDictionaryData
    {
        int price;                          // @0
        int instant_price;                  // @4
        int owner_id;                       // @8
        int buyer_id;                       // @12
        long expire_time;                   // @16
        DnfItemInfo item_info;              // @20
        unsigned char owner_type;           // @73
        unsigned int black_point;           // @76
        int unit_price;                     // @80
        ROI_Category _reg_roi_category_key; // @84
        char owner_nexon_id[32];            // @96
        ExpireTimeDictionary::ExpireTimeDictionaryData* expire_table_ptr;  // @128

        void PrintOut(__int64 auctionId);
    };

    AuctionDictionary();
    virtual ~AuctionDictionary();
    void UpdateAveragePrice()
    {
        mAvrgPriceDic.UpdateAveragePirce();
    }
    int AddItemAveragePrice(unsigned long itemId, unsigned char itemUpgradeValue,
                            int price, bool& rFristAdd, int& rAveragePrice, bool isStack,
                            const ROI_AverageKey& _roi_average_key,
                            unsigned int& _real_purchase_cnt, unsigned char itemRefineValue);
    int GetAveragePrice(unsigned long itemId, unsigned char itemUpgradeValue,
                        const ROI_AverageKey& _roi_average_key, unsigned char itemRefineValue,
                        int* pOutAveragePrice);
    int dic_Set_ROI_Constraint(const ROI_Average_Constraint& _roi_average_constraint);
    int RegistItem(__int64 auctionId, int ownerId, const char* ownerName, int price,
                   int instantPrice, unsigned int blackPoint, int unitPrice,
                   DnfItemInfo& itemInfo, long expireTime, int buyerId, const char* buyerName,
                   const ROI_Category& _roi_category_key, char userType, char* owner_nexon_id);
    int RegistCancel(int ownerId, __int64 auctionId);
    int Bidding(const __int64& auctionId, int buyerId, const char* buyerName, int price,
                char* owner_id, int& charge_point);
    int ProcessMostRecentExpireItem(bool& one_processing, int auction_type);
    int GetNowRegistedItemNum(int ownerId);
    int GetRegistedItemInfo(int ownerId, int* pInOutItemNum,
                            MyRegistedItemInfo* pOutMyRegistedItemInfoArray);
    int GetBiddingInfo(int buyerId, int* pInOutItemNum,
                       MyBiddingItemInfo* pOutMyBiddingItemInfoArray);
    AuctionDictionaryData* GetAuctionDicData(__int64 auctionId) const;
    int BuyItemApiece(__int64& auctionId, int buyerId, int price, int count);
    int Purchase(__int64 auctionId, int buyerId, AuctionDictionaryData* pAucDicData,
                 int price, int count, bool entire);
    void PutDBExpireHistory(__int64 auctionId, int buyerId, AuctionDictionaryData* pAucDicData,
                            int price, int count, AUCTION_HISTORY_EVENT_TYPE event_type);
    void PutDBBuyerHistory(__int64 auctionId, int pre_buyer_id, int buyer_id,
                           int pre_price, int price);
    void PutDBUpdateItem(__int64 auctionId, int instant_price, int add_info);
    void PutDBDeleteItem(__int64 auctionId);
    void PutDBSendPackageByExpire(__int64 auctionId, int buyerId,
                                  AuctionDictionaryData* pAucDicData, int money,
                                  int commission, int send_money, int count,
                                  AUCTION_HISTORY_EVENT_TYPE package_type, bool entire);
    int makeSuccessfulBid(__int64 auctionId, AuctionDictionaryData* pAucDicData,
                          bool isInstantBuying, int& charge_point);
    unsigned char getExpiringTime(long expirationTime, long nowTime);
    char* getCharacterName(int characterId);
    int getTotalItemNum();
    bool isInvalidPrice(int price) const
    {
        return price >= 0;
    }
    unsigned char GetOwnerIsVIP(__int64 auctionid, OwnerInfo& info);

private:
public:
    Auction* mpAuction;       // @4
private:
    nsl::object_pool_by_boost_pool<AuctionDictionaryData, int, int, int>
        mAuctionDicDataPool;  // @8
    std::map<__int64, AuctionDictionaryData*> mAuctionDicTable;  // @40
    ExpireTimeDictionary mExprTimeDic;                          // @64
    ExpireTimeDictionary mExprTimeDicForSystemAuction;          // @140
    AveragePriceDictionary mAvrgPriceDic;                       // @216
    CharacterDictionary mRegisterDic;                           // @12564
    CharacterDictionary mBidderDic;                             // @12592
    char mpSzBuffer[4096];                                      // @12620
    nsl::object_pool_by_boost_pool<CharacterNameStruct, int, int, int>
        mCharacterNamePool;                                     // @16716
    std::map<const int, CharacterNameStruct*> mCharacterNameTable;  // @16748
};

#endif // AUCTIONDICTIONARY_H_
