#ifndef GAME_CITEMDICTIONARY_H_
#define GAME_CITEMDICTIONARY_H_

#include <vector>
class CUser;

// df_game_r：charac_expand::CData 基类之后为 5 字节头和 5000 字节位图。
class CItemDictionary
{
public:
    CItemDictionary();
    ~CItemDictionary();
    void _reset();
    int _saveData(CUser* user);
    void getData(char* out) const;
    int getGainItemCount() const;
    std::vector<int>& getGainItemList();
    bool getItemCheckFlag(unsigned int item, unsigned int flag) const;
    int getLengthBinary() const;
    int loadData(CUser* user, char* data);
    int sendItemDictionaryInfo(CUser* user, unsigned int item) const;
    void sendItemDictionaryInfoTotal(CUser* user) const;
    void setGainItemFlag(CUser* user, unsigned int item);
    void setItemCheckFlag(CUser* user, unsigned int item, unsigned int flag);
private:
    unsigned char m_data[5005];
    std::vector<int> m_gainItemList;
};

#endif
