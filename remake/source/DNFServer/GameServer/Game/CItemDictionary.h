#ifndef GAME_CITEMDICTIONARY_H_
#define GAME_CITEMDICTIONARY_H_

#include <vector>
#include "CBoosterGage.h"   // charac_expand::CData（vptr+0、m_bIsSave+4）

class CUser;

// df_game_r：CItemDictionary 继承 charac_expand::CData。
// 布局：vptr(+0) + m_bIsSave(+4) + 位图(+5..+5004) + m_gainItemList。
// 实证：ORIG _reset 0x0811d774 = memset(this+5,0,0x1388)；CData 虚表
// 0x08b56908 起 8 槽 = CData 声明序（ResetDailyMidnight/loadData/getData/
// ResetDaily/~D1/~D0/_saveData/_reset），loadData/getData/_saveData/_reset
// 为 CItemDictionary 覆盖；~CData 槽 → D1/D0 由此类的虚析构提供。
#pragma pack(push, 1)
class CItemDictionary : public charac_expand::CData
{
public:
    CItemDictionary();                        // ORIG 0x0811d5d4：CDataC1 + 置 vptr
    virtual ~CItemDictionary();               // ORIG 0x0811d5f0 D1 / 0x0811d620 D0
    void _reset();                            // override（ORIG 0x0811d774）
    bool _saveData(CUser* user);              // override（ORIG 0x0811d63e）
    void getData(char* out) const;            // override（ORIG 0x0811d7e0）
    bool loadData(CUser* user, char* data);   // override（ORIG 0x0811d79a）
    int getGainItemCount() const;
    std::vector<int>& getGainItemList();
    bool getItemCheckFlag(unsigned int item, unsigned int flag) const;
    int getLengthBinary() const;
    void sendItemDictionaryInfo(CUser* user, unsigned int item);
    void sendItemDictionaryInfoTotal(CUser* user);
    void setGainItemFlag(CUser* user, unsigned int item);
    void setItemCheckFlag(CUser* user, unsigned int item, unsigned int flag);
private:
    unsigned char m_data[5000];               // +0x05 起位图（5000 字节）
    std::vector<int> m_gainItemList;
};
#pragma pack(pop)

#endif
