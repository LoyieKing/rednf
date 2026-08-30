#ifndef DNF_GAME_REDEEM_ITEM_CREDEEMITEM_H_
#define DNF_GAME_REDEEM_ITEM_CREDEEMITEM_H_

// df_game_r Redeem_Item::CRedeemItem —— 唯一声明点（Redeem_Item__CRedeemItem.cpp 实现；
// loadData/_saveData/_reset 定义于 StubS3A_deps.cpp）。
//   _ZN11Redeem_Item11CRedeemItem15isAddableFilterERK10Inven_Item  0x085f76f0 T
//   _ZN11Redeem_Item11CRedeemItem13AddRedeemListERK10Inven_Itemib  0x085f71a8 T
//   _ZNK11Redeem_Item11CRedeemItem9_isFPItemERK10Inven_Item        0x085f7c04 T（私有辅助）
// 布局：charac_expand::CData 基类（vptr +0 / m_bIsSave +4，pack(1)，5 字节）
//       + std::list<Inven_Item_Expand_RedeemInfo> @ +0x08（ORIG AddRedeemList 以
//         this+8 作 list 操作基址）。对象由 charac_expand::CDataMgr 槽位 6 持有，
//         CUser::Add_RedeemInfo 经 GetCharacExpandData(6) 取得后调用本类方法。
// ctor/dtor（0x085f7040/0x085f708e）由编译器隐式生成（CData ctor + list
// 构造/析构，与 ORIG 逐指令等价）。

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
    Inven_Item_Expand_RedeemInfo() : m_item(), m_count40(0x7fffffff)
    {
        // ORIG C1 0x085f75ea：Inven_ItemC1 构造 +0x00..0x3c，+0x40 = 0x7fffffff。
    }

    Inven_Item m_item;         // +0x00..0x3c（0x3d 字节，pack(1)）
    char m_pad3d[3];           // +0x3d..0x40
    int m_count40;             // +0x40
};

class CRedeemItem : public charac_expand::CData
{
public:
    bool isAddableFilter(const Inven_Item& item);
    int AddRedeemList(const Inven_Item& item, int param2, bool param3);
    // ---- charac_expand::CData 纯虚覆盖（StubS3A 批；ORIG 0x085f7830 /
    // 0x085f7998 / 0x085f7b6e，定义 StubS3A_deps.cpp）----
    virtual bool loadData(CUser* pUser, char* pData);
    virtual void getData(char* pData) const;   // T 0x085f78bc _ZNK11Redeem_Item11CRedeemItem7getDataEPc
    virtual bool _saveData(CUser* pUser);
    virtual void _reset();
    int Send_RedeemList(CUser* user);    // ORIG T 0x085f73ee（返 1） _ZN11Redeem_Item11CRedeemItem15Send_RedeemListEP5CUser
    // ---- UserMiscLink 批补（ORIG 实证；定义 Game/UserMiscLink.cpp）----
    int GetItemPrice(int idx);                                     // T 0x085f7386（越界 -1，否则 m_count40）
    Inven_Item_Expand_RedeemInfo GetRedeemInfoR(int idx) const;    // T 0x085f7668（越界返回默认副本）
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator
        _CheckVaildIndex_N_GetIter(int idx) const;   // T 0x085f710e（越界返回 end()）
    Inven_Item_Expand_RedeemInfo RedeemItem(int idx);              // T 0x085f728a（取副本+erase；alter 置脏）

private:
    bool _isFPItem(const Inven_Item& item) const;

    std::list<Inven_Item_Expand_RedeemInfo> m_redeemList;  // +0x08
};

}  // namespace Redeem_Item

#endif  // DNF_GAME_REDEEM_ITEM_CREDEEMITEM_H_
