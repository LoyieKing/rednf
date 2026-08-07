// Rebuilt from df_auction_r, 2026-08-08 — ctor/dtor scaffold; Auction TU pending
#include "Auction.h"

Auction::Auction()
{
}

Auction::~Auction()
{
}

// Temporary stubs for methods referenced by HandlerFor_DB_; replaced when the
// Auction TU is reconstructed.
void Auction::UpdateAveragePrice()
{
}

void Auction::ProcessMostRecentExpireItem()
{
}

CNRDItemInfoList::STItemInfo* Auction::GetItemInfo(unsigned long item_id) const
{
    return NULL;
}

bool Auction::IsAvatarCategory(int category)
{
    return false;
}

int Auction::AddAvatarEmblemInfo(int ui_id, const stAvatarEmblemInfo_t* pInfo)
{
    return 0;
}

int Auction::AddAvatarExpansionInfo(int ui_id, const stAvatarExpansionInfo_t* pInfo)
{
    return 0;
}

void Auction::SetMaxHistoryAuctionId(unsigned long long id)
{
}

void Auction::SendMessageToMonitor(unsigned int characId, unsigned char type, unsigned long itemId,
                                   const RandomOption& randomOption)
{
}

int Auction::RegistItem(int ownerId, const char* ownerName, char ownerType,
                        int price, int instantPrice, unsigned int black_point, int unitPrice,
                        const ROI_Category& roiKey, char* ownerNexonId, DnfItemInfo itemInfo,
                        unsigned long long& auctionId, long expireTime, int buyerId,
                        const char* buyerName, bool bFlag)
{
    return 0;
}

void Auction::AddItemAveragePrice(unsigned long item_id, unsigned char upgrade,
                                  int average_price, unsigned int purchase_cnt,
                                  const ROI_AverageKey& roiKey, unsigned char refine, bool bFlag)
{
}

void Auction::Set_ROI_Constraint(const ROI_Average_Constraint& constraint)
{
}

int Auction::TransErrToReason(int err)
{
    return 0;
}

int Auction::GetAvatarEmblemInfo(int ui_id, stAvatarEmblemInfo_t* pInfo)
{
    return 0;
}

int Auction::GetAvatarExpansionInfo(int ui_id, stAvatarExpansionInfo_t* pInfo)
{
    return 0;
}

int Auction::GetAveragePrice(unsigned long item_id, unsigned char upgrade,
                             const ROI_AverageKey& roiKey, unsigned char refine, int* out)
{
    return 0;
}

int Auction::SearchByItemId(PSearchByItemId pSearch, unsigned long* pItemIdArray,
                            unsigned int* pTotalNumberOfFound, unsigned short* pNumberOfFound,
                            AuctionItemInfo* pOutAuctionItemInfoArray)
{
    return 0;
}

int Auction::SearchByCategory(PSearchByCategory pSearch, unsigned int* pTotalNumberOfFound,
                              unsigned short* pNumberOfFound,
                              AuctionItemInfo* pOutAuctionItemInfoArray)
{
    return 0;
}

int Auction::RegistCancel(unsigned int owner_id, unsigned long long auction_id)
{
    return 0;
}

int Auction::OpenPrivateStore(unsigned int m_id, int character_num, int* character_ids)
{
    return 0;
}

int Auction::GetMyRegistedItemInfo(int owner_id, int* pNum, MyRegistedItemInfo* pOut)
{
    return 0;
}

int Auction::GetMyBiddingInfo(int buyer_id, int* pNum, MyBiddingItemInfo* pOut)
{
    return 0;
}

int Auction::ClosePrivateStore(unsigned int m_id)
{
    return 0;
}

int Auction::BuyItemApiece(unsigned long long auction_id, int buyer_id, int price, int count)
{
    return 0;
}

int Auction::Bidding(int buyer_id, const char* buyer_name, unsigned long long auction_id,
                     int price, char* out, int& ret)
{
    return 0;
}

// AuctionDictionary TU pending; provide the two methods Auction inline wrappers link against.
int AuctionDictionary::GetNowRegistedItemNum(int owner_id)
{
    return 0;
}

AuctionDictionaryData* AuctionDictionary::GetAuctionDicData(unsigned long long auction_id) const
{
    return NULL;
}

void PrintDnfItemInfo(DnfItemInfo& itemInfo, char* out)
{
    out[0] = '\0';
}
