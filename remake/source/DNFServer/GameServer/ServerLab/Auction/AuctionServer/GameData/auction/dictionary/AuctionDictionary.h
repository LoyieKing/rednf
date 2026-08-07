// Rebuilt from df_auction_r DWARF (AuctionDictionary.cpp CU) — pending full reconstruction.
// Only the members referenced by Auction inline wrappers are declared here.
#ifndef AUCTIONDICTIONARY_H_
#define AUCTIONDICTIONARY_H_

struct AuctionDictionaryData;

class AuctionDictionary
{
public:
    int GetNowRegistedItemNum(int owner_id);
    AuctionDictionaryData* GetAuctionDicData(unsigned long long auction_id) const;
};

#endif // AUCTIONDICTIONARY_H_
