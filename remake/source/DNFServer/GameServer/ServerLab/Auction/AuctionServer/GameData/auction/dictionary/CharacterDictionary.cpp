// Rebuilt from df_auction_r DWARF (CharacterDictionary.cpp CU), 2026-08-08
#include "CharacterDictionary.h"

CharacterDictionary::CharacterDictionary()
    : mCharacterDicTable()
{
}

CharacterDictionary::~CharacterDictionary()
{
    mCharacterDicTable.clear();
}

int CharacterDictionary::AddAuctionId(int characterId, __int64 auctionId)
{
    std::map<const int, CharacterDictionaryData*>::iterator find_iter =
        mCharacterDicTable.find(characterId);
    CharacterDictionaryData* ptr_data;
    if (find_iter != mCharacterDicTable.end())
    {
        ptr_data = find_iter->second;
    }
    else
    {
        ptr_data = new CharacterDictionaryData;
        if (ptr_data == NULL)
        {
            return 9;
        }
        std::pair<std::map<const int, CharacterDictionaryData*>::iterator, bool> ch_pair;
        ch_pair = mCharacterDicTable.insert(
            std::make_pair<const int&, CharacterDictionaryData*&>(characterId, ptr_data));
        if (ch_pair.second != true)
        {
            return 0xc;
        }
    }
    ptr_data->auction_id_vector.push_back(auctionId);
    return 0;
}

int CharacterDictionary::SubAuctionId(int characterId, __int64 auctionId)
{
    std::map<const int, CharacterDictionaryData*>::iterator find_iter =
        mCharacterDicTable.find(characterId);
    if (find_iter == mCharacterDicTable.end())
    {
        return 0x22;
    }
    CharacterDictionaryData* ptr_data = find_iter->second;
    std::vector<unsigned long long>::iterator id_list_iterator;
    id_list_iterator = ptr_data->auction_id_vector.begin();
    while (id_list_iterator != ptr_data->auction_id_vector.end())
    {
        if (auctionId == *id_list_iterator)
        {
            ptr_data->auction_id_vector.erase(id_list_iterator);
            break;
        }
        ++id_list_iterator;
    }
    if (ptr_data->auction_id_vector.empty())
    {
        mCharacterDicTable.erase(characterId);
    }
    return 0;
}

int CharacterDictionary::GetAuctionIdList(int characterId,
                                          CharacterDictionaryData*& rpOutCharacterDicData)
{
    std::map<const int, CharacterDictionaryData*>::iterator find_iter =
        mCharacterDicTable.find(characterId);
    if (find_iter != mCharacterDicTable.end())
    {
        rpOutCharacterDicData = find_iter->second;
    }
    else
    {
        rpOutCharacterDicData = NULL;
    }
    return 0;
}

int CharacterDictionary::NowItemNum(int ownerId)
{
    std::map<const int, CharacterDictionaryData*>::iterator find_iter =
        mCharacterDicTable.find(ownerId);
    if (find_iter != mCharacterDicTable.end())
    {
        return (int)find_iter->second->auction_id_vector.size();
    }
    return 0;
}
