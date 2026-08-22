#ifndef DNF_GAME_REDEEM_ITEM_CREDEEMITEM_H_
#define DNF_GAME_REDEEM_ITEM_CREDEEMITEM_H_

// df_game_r Redeem_Item::CRedeemItem —— 唯一声明点（Redeem_Item__CRedeemItem.cpp 实现）。
// ORIG 符号（nm -S --defined-only）：
//   _ZN11Redeem_Item11CRedeemItem15isAddableFilterERK10Inven_Item  0x085f76f0 T
//   _ZN11Redeem_Item11CRedeemItem13AddRedeemListERK10Inven_Itemib  0x085f71a8 T
//   _ZNK11Redeem_Item11CRedeemItem9_isFPItemERK10Inven_Item        0x085f7c04 T（私有辅助）
// 布局：charac_expand::CData 基类（vptr +0 / m_bIsSave +4，pack(1)，5 字节）
//       + std::list<Inven_Item_Expand_RedeemInfo> @ +0x08（ORIG AddRedeemList 以
//         this+8 作 list 操作基址）。对象由 charac_expand::CDataMgr 槽位 6 持有，
//         CUser::Add_RedeemInfo 经 GetCharacExpandData(6) 取得后调用本类方法。
// 其余 ORIG 方法（ctor/dtor/loadData/_saveData/RedeemItem/GetItemPrice 等）暂未建模。

#include <cstring>
#include <list>

#include "CBoosterGage.h"   // charac_expand::CData
#include "Inven_Item.h"

namespace Redeem_Item
{

// Inven_Item_Expand_RedeemInfo：0x44 字节。
// ORIG AddRedeemList：ctor 清零 + 0x00..0x3c 复制 Inven_Item 前 15 dword +
//   +0x3c 复制 Inven_Item 末字节 + +0x40 写入 int（param2+1000 或 1000）。
struct Inven_Item_Expand_RedeemInfo
{
    Inven_Item_Expand_RedeemInfo() { std::memset(this, 0, sizeof(*this)); }

    unsigned int m_data[0xf];  // +0x00..0x3c（15 dword）
    unsigned char m_byte3c;    // +0x3c
    char m_pad3d[3];           // +0x3d..0x40
    int m_count40;             // +0x40
};

class CRedeemItem : public charac_expand::CData
{
public:
    bool isAddableFilter(const Inven_Item& item);
    int AddRedeemList(const Inven_Item& item, int param2, bool param3);

private:
    bool _isFPItem(const Inven_Item& item) const;

    std::list<Inven_Item_Expand_RedeemInfo> m_redeemList;  // +0x08
};

}  // namespace Redeem_Item

#endif  // DNF_GAME_REDEEM_ITEM_CREDEEMITEM_H_
