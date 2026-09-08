#ifndef DNF_GAME_POD_TYPES_H_
#define DNF_GAME_POD_TYPES_H_

#include <cstddef>   // offsetof（static_assert 用）

#pragma pack(push, 1)
struct stAvatarEmblemInfo_t
{
    void init();
    struct
    {
        unsigned short emblemSocketType_;
        int emblemItemIdx_;
    } stEmblemSocket[5];
};
#pragma pack(pop)

struct stAvatarExpansionInfo_t
{
    void init();
    short m_color[2];
};

// ORIG 布局（CAvatarItemMgr::GetItem 0x82f92a3 步长 0x4b、DB_LoadInventory::_GetUserAvatar
// 写点实证）：+0x0 ui_id/+0x4 expire/+0x8 reg/+0xc ipg(0x19)/+0x25 clearAvatarId/
// +0x29 jewelSocket(0x1e)/+0x47 color1/+0x49 color2；整体 pack(1)，sizeof 0x4b。
#pragma pack(push, 1)
struct AvatarDetailInfo
{
    int m_key;                 // +0x00
    int m_expireDate;          // +0x04
    int m_registTime;          // +0x08
    char m_ipgAgencyNo[25];    // +0x0c..+0x24
    int m_clearAvatarId;       // +0x25..+0x28
    stAvatarEmblemInfo_t m_jewelSocket;      // +0x29..+0x46
    stAvatarExpansionInfo_t m_expansionInfo; // +0x47..+0x4a（color1/color2）
};
#pragma pack(pop)
static_assert(sizeof(AvatarDetailInfo) == 0x4b, "AvatarDetailInfo size");

// SIG_CREATURE_ITEM_DETAIL_INFO 唯一权威定义（自 CInventory.h 顶部纯移动，
// 2026-09-04 DBLoad：轻量 DB TU 免拉 CInventory.h 的 iostream 链——ORIG
// DB_LoadInventory TU 无 __static_init(ios_base)。pack(1)，0x38；布局证据
// = CCreature.cpp 局部镜像/CreatureOnlyMgrRest.cpp 镜像交叉锁定 +
// DB_LoadInventory/GetUserCreature.md 14×movl 写点）。
#ifndef DNF_GAME_SIG_CREATURE_ITEM_DETAIL_INFO_DEFINED
#define DNF_GAME_SIG_CREATURE_ITEM_DETAIL_INFO_DEFINED
#pragma pack(push, 1)
struct SIG_CREATURE_ITEM_DETAIL_INFO
{
    int           m_slotNo;           // +0x00（0xee=装备中槽标记）
    int           m_uid;              // +0x04（RegisterCreatureItem 的 map key）
    char          m_name[0xc];        // +0x08（memcpy 12 字节，无终止符）
    unsigned char m_pad14[4];         // +0x14..0x17
    int           m_stomachValue;     // +0x18
    unsigned char m_pad1c[4];         // +0x1c..0x1f
    int           m_exp;              // +0x20
    unsigned char m_creatureItemType; // +0x24（低字节；Load 以 (signed char) 还原）
    unsigned char m_pad25;            // +0x25
    unsigned char m_noCharge;         // +0x26
    unsigned char m_stat;             // +0x27（兼作删除标记）
    int           m_expireTime;       // +0x28
    unsigned char m_pad2c[4];         // +0x2c..0x2f
    unsigned char m_growExpFlag;      // +0x30
    bool          m_isMaxLevel;       // +0x31（ORIG CCreature::LoadCreatureItem
                                      // 0x83386xx `mov %al,0x31(%edx)` bool→bool
                                      // 直存形，P219/P211 同型判据；sizeof 不变）
    unsigned char m_pad32[6];         // +0x32..0x37
};
#pragma pack(pop)
static_assert(sizeof(SIG_CREATURE_ITEM_DETAIL_INFO) == 0x38,
              "SIG_CREATURE_ITEM_DETAIL_INFO ABI drift");
static_assert(offsetof(SIG_CREATURE_ITEM_DETAIL_INFO, m_stomachValue) == 0x18,
              "SIG_CREATURE_ITEM_DETAIL_INFO m_stomachValue offset drift");
#endif

#endif
