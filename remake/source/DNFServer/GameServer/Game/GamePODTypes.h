#ifndef DNF_GAME_POD_TYPES_H_
#define DNF_GAME_POD_TYPES_H_

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

#endif
