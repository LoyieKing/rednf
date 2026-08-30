#ifndef GAME_STEXPREWARD_H_
#define GAME_STEXPREWARD_H_

// ============================================================================
// STExpReward —— 权威唯一定义点（ORIG sizeof 0x4）。
// 原 CDataManager.h 内定义；因 CUser.h 按值成员需要完整类型，而
// CDataManager.h 经 CItemGeneratorMgr→CItemGenerator→PvPTypes→CUser.h 存在
// 重入环（CDataManager.h 解析中途 CUser.h 被再次展开），故下沉为本叶子头，
// 供 CDataManager.h 与 CUser.h 直接 include。无任何反向依赖。
// 2026-08-31 修正：原实现 pack(1) 0x17 字节（含 m_field4/m_field5/m_powerUp/
// m_localIp/m_publicIp/m_padf/m_hangameUser）——影子测试实测 ORIG
// vector<STExpReward> 元素分配仅 4 字节；ORIG C1（0x85bed52）只级联清零
// +0x0..+0x3——STExpReward 仅 int m_reward。其余字段实为 CUser 的
// +0x8d251..+0x8d263 独立字段（误塞），已移回 CUser.h。
// ============================================================================

struct STExpReward
{
    // ORIG C1/C2 0x085bed52（W）：级联清零 +0x0..+0x3（4 字节）。
    // union 双视图：m_reward 整字（get_growth_power_exp_reward 按 dword 读
    // CUser+0x8d24d 整体返回）；m_reward8/m_field4/m_field5/m_powerUp 字节视图
    // （InitScriptProducers [exp reward] 解析逐字节写 +0x00..+0x03）。
    STExpReward() { m_reward = 0; }

    union
    {
        int m_reward;              // +0x00（dword 视图）
        struct
        {
            char m_reward8;        // +0x00 低字节
            char m_field4;         // +0x01
            char m_field5;         // +0x02
            char m_powerUp;        // +0x03
        };
    };
};                                     // sizeof 0x4

typedef char _assert_stexpreward_size[(sizeof(STExpReward) == 0x4) ? 1 : -1];
#endif  // GAME_STEXPREWARD_H_
