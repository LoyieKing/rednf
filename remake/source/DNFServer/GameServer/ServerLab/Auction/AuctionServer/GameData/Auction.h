#ifndef AUCTION_AUCTION_H_
#define AUCTION_AUCTION_H_

#include "GlobalInstance.h"
#include "AuctionItem.h"
#include "DBTransactionDesign.h"
#include "RDARScriptItemInfo.h"
#include "AuctionPacket.h"
#include "Search.h"
#include "AuctionDictionary.h"
#include "RDARScriptAvatarColorInfo.h"
#include "TCPSocket.h"
#include <map>
#include <set>
#include <vector>

enum PAY_TYPE
{
    PAY_TYPE_GOLD = 0,
    PAY_TYPE_POINT = 1,
};

enum EnumAuctionItemType
{
    AUCTION_ITEM_TYPE_UNKNOWN = -1,
    AUCTION_ITEM_TYPE_PLAIN = 1,
    AUCTION_ITEM_TYPE_CREATURE = 2,
    AUCTION_ITEM_TYPE_AVATAR = 3,
};

void PrintDnfItemInfo(DnfItemInfo& itemInfo, char* out);

class Auction
{
public:
    Auction();
    virtual ~Auction();
    void UpdateAveragePrice()
    {
        mAuctionDic.UpdateAveragePrice();
    }
    int ProcessMostRecentExpireItem();
    PAY_TYPE GetPayType()
    {
        return mPayType;
    }
    void SetPayType(PAY_TYPE payType)
    {
        mPayType = payType;
    }
    int GetExpireTime()
    {
        return mAUCTION_EXPIRE_TIME;
    }
    int GetSystemAuctionExpireTime()
    {
        return mSYSTEM_AUCTION_EXPIRE_TIME;
    }
    double GetCommission()
    {
        return mAUCTION_COMMISSION;
    }
    double GetVIPCommission()
    {
        return mAUCTION_VIP_COMMISSION;
    }

    CNRDItemInfoList::STItemInfo* GetItemInfo(unsigned long item_id) const;
    bool IsAvatarCategory(int category);
    int AddAvatarEmblemInfo(int ui_id, const stAvatarEmblemInfo_t* pInfo);
    int AddAvatarExpansionInfo(int ui_id, const stAvatarExpansionInfo_t* pInfo);
    void SetMaxHistoryAuctionId(unsigned long long id);
    void SendMessageToMonitor(unsigned int characId, unsigned char type, unsigned long itemId,
                              const RandomOption& randomOption);
    int RegistItem(int ownerId, const char* ownerName, char ownerType,
                   int price, int instantPrice, unsigned int black_point, int unitPrice,
                   const ROI_Category& roiKey, char* ownerNexonId, DnfItemInfo itemInfo,
                   unsigned long long& auctionId, long expireTime, int buyerId,
                   const char* buyerName, bool bFlag);
    int AddItemAveragePrice(unsigned long item_id, unsigned char upgrade,
                            int average_price, unsigned int purchase_cnt,
                            const ROI_AverageKey& roiKey, unsigned char refine, bool bFlag);
    int Set_ROI_Constraint(const ROI_Average_Constraint& constraint);
    int TransErrToReason(int err);
    int GetAvatarEmblemInfo(int ui_id, stAvatarEmblemInfo_t* pInfo);
    int GetAvatarExpansionInfo(int ui_id, stAvatarExpansionInfo_t* pInfo);
    AuctionDictionary::AuctionDictionaryData* GetAuctionDicData(unsigned long item_id) const;
    int GetAveragePrice(unsigned long item_id, unsigned char upgrade,
                        const ROI_AverageKey& roiKey, unsigned char refine, int* out);
    int SearchByItemId(PSearchByItemId pSearch, unsigned long* pItemIdArray,
                       unsigned int* pTotalNumberOfFound, unsigned short* pNumberOfFound,
                       AuctionItemInfo* pOutAuctionItemInfoArray);
    int SearchByCategory(PSearchByCategory pSearch, unsigned int* pTotalNumberOfFound,
                         unsigned short* pNumberOfFound,
                         AuctionItemInfo* pOutAuctionItemInfoArray);
    int RegistCancel(int owner_id, unsigned long long auction_id);
    void OpenPrivateStore(unsigned int m_id, int character_num, int* character_ids);
    int GetMyRegistedItemInfo(int owner_id, int* pNum, MyRegistedItemInfo* pOut);
    int GetMyBiddingInfo(int buyer_id, int* pNum, MyBiddingItemInfo* pOut);
    void ClosePrivateStore(unsigned int m_id);
    int BuyItemApiece(unsigned long long auction_id, int buyer_id, int price, int count);
    int Bidding(int buyer_id, const char* buyer_name, unsigned long long auction_id,
                int price, char* out, int& ret);
    bool IsOwnerVIP(unsigned long long auction_id, OwnerInfo& ownerInfo);
    char* GetAvatarColorName(int ui_id);
    EnumAuctionItemType CheckItemType(unsigned long item_id);
    void UnregistChkMapForAvatarCreature(bool bCreature, int add_info);
    int SubAvatarEmblemInfo(int ui_id);
    int SubAvatarExpansionInfo(int ui_id);
    bool IsPrivateStoreOpen(int owner_id);
    bool IsStackableCategory(unsigned short category) const
    {
        // ORIG：单条 OR 链 + 共享 return true（jbe 逐区间跳过）
        if ((0x32c7 < category && category <= 0x32ce)
            || (0x752f < category && category <= 0x7598)
            || (0x7917 < category && category <= 0x7a49)
            || (0x80e7 < category && category <= 0x80ec))
        {
            return true;
        }
        return false;
    }
    int GetNowRegistedItemNum(int owner_id);
    char* GetAuctionServerName();
    bool RegistChkMapForAvatarCreature(bool isAvatar, int ui_id);
    bool isEmblemAvatar(int category);
    bool isValidEmblemAvatar(int category, avatarEmblemType_t emblemSocketType_);

    // Layout from df_auction_r DWARF (Auction, 21396B):
    nsl::UDPSocket mSocketForMonitor;  // @4
    __int64 mMaxAuctionId;      // @0x2c
    double mAUCTION_COMMISSION; // @0x34
    double mAUCTION_VIP_COMMISSION;    // @0x3c
    double mAUCTION_PRIVATE_STORE_COMISSION;  // @0x44
    int mAUCTION_EXPIRE_TIME;   // @0x4c
    int mSYSTEM_AUCTION_EXPIRE_TIME;  // @0x50
    AuctionDictionary mAuctionDic;  // @0x54
    char mpSzBuffer[0x1000];    // @0x41d8
    Search mSearch;             // @0x51d8
    CNRDItemInfoList mItemInfo; // @0x5274
    AvatarVariation::AvatarColorInfo avatarColorInfo;  // @0x5290
    PAY_TYPE mPayType;          // @0x52c0
    std::map<unsigned int, std::vector<int> > mIdMap;   // @0x52c4
    std::set<int> mCidSet;      // @0x52dc
    nsl::object_pool_by_boost_pool<stAvatarEmblemInfo_t, int, int, int>
        mEmblemInfoStructPool;  // @0x52f4
    std::map<int, stAvatarEmblemInfo_t*> mEmblemInfoMap;  // @0x5314
    nsl::object_pool_by_boost_pool<stAvatarExpansionInfo_t, int, int, int>
        mExpansionInfoStructPool;  // @0x532c
    std::map<int, stAvatarExpansionInfo_t*> mExpansionInfoMap;  // @0x534c
    std::map<int, char> mAvatarDupChkMap;  // @0x5364
    std::map<int, char> mCreatureDupChkMap;  // @0x537c

    static char msAuctionServerName[20];

    unsigned long long getNextAuctionId();
    void checkMaxAuctionId(unsigned long long auctionId);
};

inline Auction* G_Auction()
{
    return nsl::GlobalInstance<Auction>::inst_ptr();
}

#endif // AUCTION_AUCTION_H_
