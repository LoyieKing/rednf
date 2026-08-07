// Rebuilt from df_auction_r DWARF (CharacterDictionary.cpp CU), 2026-08-08
#ifndef CHARACTERDICTIONARY_H_
#define CHARACTERDICTIONARY_H_

#include <map>
#include <vector>

typedef unsigned long long __int64;

class CharacterDictionary
{
public:
    typedef std::vector<unsigned long long> AuctionIdListType;

    struct CharacterDictionaryData
    {
        AuctionIdListType auction_id_vector;    // @0, total 12B

        CharacterDictionaryData()
        {
        }
    };

    CharacterDictionary();
    virtual ~CharacterDictionary();
    int AddAuctionId(int characterId, __int64 auctionId);
    int SubAuctionId(int characterId, __int64 auctionId);
    int GetAuctionIdList(int characterId, CharacterDictionaryData*& rpOutCharacterDicData);
    int NowItemNum(int ownerId);

private:
    std::map<const int, CharacterDictionaryData*> mCharacterDicTable;   // @4
};

#endif // CHARACTERDICTIONARY_H_
