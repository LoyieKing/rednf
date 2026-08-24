#ifndef GAME_STEXPREWARD_H_
#define GAME_STEXPREWARD_H_

// ============================================================================
// STExpReward —— 权威唯一定义点（ORIG pack(1) 布局，sizeof 0x17）。
// 原 CDataManager.h 内定义；因 CUser.h 按值成员需要完整类型，而
// CDataManager.h 经 CItemGeneratorMgr→CItemGenerator→PvPTypes→CUser.h 存在
// 重入环（CDataManager.h 解析中途 CUser.h 被再次展开），故下沉为本叶子头，
// 供 CDataManager.h 与 CUser.h 直接 include。无任何反向依赖。
// ============================================================================

#pragma pack(push, 1)
struct STExpReward
{
    STExpReward() {}

    int m_reward;      // +0x00
    char m_field4;     // +0x04
    char m_field5;     // +0x05
    char m_powerUp;    // +0x06（SetPowerUp）
    int m_localIp;     // +0x07（0x8d254）
    int m_publicIp;    // +0x0b（0x8d258）
    char m_padf[4];    // +0x0f..0x12
    bool m_hangameUser;  // +0x13（0x8d260）
    char m_pad14[0x17 - 0x14];  // +0x14..0x17
};                                     // sizeof 0x17（pack(1)；CUser 内 +0x8d24d..+0x8d263）
#pragma pack(pop)

typedef char _assert_stexpreward_size[(sizeof(STExpReward) == 0x17) ? 1 : -1];
#endif  // GAME_STEXPREWARD_H_
