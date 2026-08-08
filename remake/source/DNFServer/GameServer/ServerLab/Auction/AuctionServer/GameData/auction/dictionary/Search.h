// Rebuilt from df_auction_r DWARF (Search.cpp CU) — pending full reconstruction.
// Only the members referenced by Auction/AuctionDictionary are declared here.
#ifndef SEARCH_H_
#define SEARCH_H_

class Auction;

class Search
{
public:
    Search();
    virtual ~Search();
    int Insert(unsigned long itemId, unsigned char upgrade, unsigned long long auctionId,
               bool bIsStackable, int price, unsigned char refine);
    int Delete(unsigned long long auctionId);

private:
    char m_pad[0x9c];   // 156B layout placeholder (pending Search TU)
};

#endif // SEARCH_H_
