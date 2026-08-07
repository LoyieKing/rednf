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
