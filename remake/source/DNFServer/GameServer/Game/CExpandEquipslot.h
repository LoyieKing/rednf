#ifndef DNF_GAME_CEXPANDEQUIPSLOT_H_
#define DNF_GAME_CEXPANDEQUIPSLOT_H_

// ============================================================================
// df_game_r 还原 —— CExpandEquipslot（G1-2 扩展装备槽）
// 布局依据 ORIG 构造（0x84992c0）/ 访问器（0x84993aa 族）：
//   charac_expand::CData 基类（pack(1)：vptr +0x0 / char m_bIsSave +0x4，
//   sizeof 5）+ 3 组 Inven_Item[12]（每组 0x2dc 字节，步长 0x3d）：
//     Inven_Item m_equipSlot[12]        @ +0x05
//     Inven_Item m_expandEquipSlot[12]  @ +0x2e1
//     Inven_Item m_expandAvatarSlot[12] @ +0x5bd
//   sizeof = 0x899（pack(1)）。
// 虚表 0x8c7b208 槽位：ResetDailyMidnight @0 / loadData @1 / getData @2 /
//   ResetDaily @3 / ~CExpandEquipslot @4,5 / _saveData @6 / _reset @7。
// 依赖 CUser/CInventory/CDataManager 等暂无权威头，本头仅声明符号形态。
// ============================================================================

#include <cstring>

#include "CBoosterGage.h"
#include "Inven_Item.h"

class CUser;
class CInventory;

// ---- 枚举（值以 ORIG 立即数为准；与 CInventory.h 同名枚举待主 agent 合并）----
#ifndef DNF_ENUM_INVEN_TYPE_DEFINED
#define DNF_ENUM_INVEN_TYPE_DEFINED
enum INVEN_TYPE
{
    INVEN_TYPE_INVEN = 0,
    INVEN_TYPE_EQUIP = 1,
    INVEN_TYPE_AVATAR = 2,
    INVEN_TYPE_CREATURE = 3,
    INVEN_TYPE_5 = 5,
    INVEN_TYPE_6 = 6
};
#endif

#ifndef DNF_ENUM_EQUIPSLOT_DEFINED
#define DNF_ENUM_EQUIPSLOT_DEFINED
enum ENUM_EQUIPSLOT
{
    ENUM_EQUIPSLOT_0 = 0,
    ENUM_EQUIPSLOT_1 = 1,
    ENUM_EQUIPSLOT_2 = 2,
    ENUM_EQUIPSLOT_3 = 3
};
#endif

#ifndef DNF_ENUM_ITEMSPACE_DEFINED
#define DNF_ENUM_ITEMSPACE_DEFINED
enum ENUM_ITEMSPACE
{
    ENUM_ITEMSPACE_0 = 0,
    ENUM_ITEMSPACE_0x12 = 0x12
};
#endif

#ifndef DNF_ENUM_USERINFO_DEFINED
#define DNF_ENUM_USERINFO_DEFINED
enum ENUM_USERINFO
{
    ENUM_USERINFO_0 = 0
};
#endif

#ifndef DNF_ENUM_SECURITY_PROTCTION_DEFINED
#define DNF_ENUM_SECURITY_PROTCTION_DEFINED
enum SECURITY_PROTCTION
{
    SECURITY_PROTCTION_0 = 0,
    SECURITY_PROTCTION_0x16 = 0x16
};
#endif

// ---- CExpandEquipslot（0x899 字节，pack(1)）----
#pragma pack(push, 1)

class CExpandEquipslot : public charac_expand::CData
{
public:
    CExpandEquipslot();
    virtual ~CExpandEquipslot();

    virtual bool loadData(CUser* pUser, char* pData);
    virtual void getData(char* pData) const;
    virtual bool _saveData(CUser* pUser);
    virtual void _reset();

    int Change_Equip(CUser* pUser, int type, int slot, int space,
                     int expandSlot);
    int EquipslotSwitch(CUser* pUser, char flag, char channelSwitch,
                        char expandSwitch);
    bool FindItemLock(unsigned char lock, ENUM_ITEMSPACE& space,
                      int& slot) const;
    Inven_Item* GetInvenRefR(INVEN_TYPE type, int slot) const;
    Inven_Item* GetInvenRefW(INVEN_TYPE type, int slot);
    Inven_Item GetInvenSlot(INVEN_TYPE type, int slot) const;
    int Move_Item(CUser* pUser, int type, int slot, int space,
                  int expandSlot);
    int Send_Equip_Info(CUser* pUser);
    int Send_Expand_Equip_Info(CUser* pUser);
    void SetExpandEquipslot(int type, void* data, int size);
    Inven_Item* getExpandEquipslotR(ENUM_EQUIPSLOT slot) const;
    Inven_Item* getExpandEquipslotW(ENUM_EQUIPSLOT slot);

    Inven_Item m_equipSlot[12];        // +0x05
    Inven_Item m_expandEquipSlot[12];  // +0x2e1
    Inven_Item m_expandAvatarSlot[12]; // +0x5bd
};

#pragma pack(pop)

#endif  // DNF_GAME_CEXPANDEQUIPSLOT_H_
