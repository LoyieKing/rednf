#ifndef RDARSCRIPTITEMINFO_H_
#define RDARSCRIPTITEMINFO_H_

#include <map>
#include <string>

#include "stdafx.h"

enum ENUM_RARITY
{
    RARITY_COMMON = 0,
    RARITY_UNCOMMON = 1,
    RARITY_RARE = 2,
    RARITY_UNIQUE = 3,
    RARITY_EPIC = 4,
    RARITY_CHRONICLE = 5,
    RARITY_MAX = 6,
    RARITY_AUCTION_LEGACY = 4,
    RARITY_MONSTER_CARD_MAX = 4,
};

char* NextToken(const char** ppszBuff, char chStringMark, char chDelimiter);
bool findFileWithServerTokenInAuction(TCHAR* fullPath);

class CNRDItemInfoList
{
public:
    struct STItemInfo
    {
        STItemInfo();
        ~STItemInfo();

        int nItemIndex_;
        int rarity_;
        bool xaUsableCharacter_[11];
        int nOriginalUsableLevel_;
        std::string sName_;
        std::string sEnglishName_;
        unsigned short category_;
    };

    typedef std::map<int, STItemInfo*> ItemInfoMap;
    typedef bool (*ConvertFunction)(char* pszSrc, char* pszDst);

    CNRDItemInfoList();
    virtual ~CNRDItemInfoList();
    ItemInfoMap& GetItemInfoMap() { return itemInfoMap_; }
    bool Load(const TCHAR* szFilePath, ConvertFunction convertor);
    bool Save(const TCHAR* pszFilePath);
    void Clear();
    STItemInfo* GetItemInfo(int nIndex) const;
    unsigned short ConvertAvatarCategory(unsigned short category, bool bEmblemAvatar) const;

private:
    ItemInfoMap itemInfoMap_;
};

#endif // RDARSCRIPTITEMINFO_H_
