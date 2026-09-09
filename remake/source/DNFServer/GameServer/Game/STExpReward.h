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

// [b28d-sweep2-InitScriptProducers_2] pack(1)：ORIG importGrowthPower 0x08911fe5
// `lea -0x21(%ebp),%eax` 把局部落**非 4 对齐**槽 ⇒ 类型对齐 1（P250）；4.4 未 packed
// 时给 -0x24（该件残余 37 行全为 +3 槽平移）。sizeof 仍 4、成员偏移/mangled 不变。
struct __attribute__((packed)) STExpReward
{
    // ORIG C1/C2 0x085bed52（W）：级联清零 +0x0..+0x3（4 字节）。
    // union 双视图：m_reward 整字（get_growth_power_exp_reward 按 dword 读
    // CUser+0x8d24d 整体返回）；m_reward8/m_field4/m_field5/m_powerUp 字节视图
    // （InitScriptProducers [exp reward] 解析逐字节写 +0x00..+0x03）。
    // ORIG C1/C2 0x085bed52：级联清零 +0x0..+0x3（movb $0,0x3 → movzbl 传递
    // → movb 高位逐个下移）。`m_reward = 0` 生成单 movl；逐字节赋值复现
    // 4.1 的 movzbl 级联链。
    // ORIG C1/C2 0x085bed52：4.1 对同值字节字段发**传播链**（movb $0,+3 → movzbl
    // +3 → movb +2 → movzbl +2 → movb +1 → movzbl +1 → movb +0）；4.4 对独立
    // 逐字段赋值恒发 4 条 movb $0（b28c-lane25 探针 /tmp/l25probe/se.cpp fA/fB）。
    // 手写传播链语义等价（全零）且逐条复现 ORIG。
    STExpReward() { m_powerUp = 0; m_field5 = m_powerUp; m_field4 = m_field5; m_reward8 = m_field4; }

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
