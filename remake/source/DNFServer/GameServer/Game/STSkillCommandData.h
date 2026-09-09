#ifndef DNF_GAME_ST_SKILL_COMMAND_DATA_H_
#define DNF_GAME_ST_SKILL_COMMAND_DATA_H_

#include <string.h>   // memset（ctor 体内使用）

// ============================================================================
// STSkillCommandData —— 技能命令数据（自 CUser.h 抽出）
// SigTypes_C.h 的 SIG_LOAD_SKILL 数组成员 STCommand[0x31] 构造需完整类型，
// 且 CUser.h 依赖 SigTypes.h 存在包含环。布局证据见 CUser.h 原文。
// STCommand ctor 为 ORIG W 0x0822da04（_ZN18STSkillCommandData9STCommandC1Ev）。
// ============================================================================

#pragma pack(push, 1)
struct STSkillCommandData
{
    // 32 位紧凑编码（GCC little-endian 位域，自 bit0 起，与 ORIG 打包一致）：
    //   bits 0-13  m_command（14）   bits 14-16 m_key0（3）
    //   bits 17-19 m_key1（3）       bits 20-22 m_key2（3）
    //   bits 23-25 m_key3（3）       bits 26-28 m_key4（3）
    //   bits 29-31 m_count（3）
    struct STCommand
    {
        // §P336 修正（DualCUOO 实测 + Main 裁定）：packed 位域 ctor 的**链式赋值**形。
        // ORIG W 0x0822da04 `_ZN18STSkillCommandData9STCommandC1Ev` = size 0x161 / 122 条
        // 指令、零 call，全是 +0x02/+0x03 两字节上的「movzbl 读 → and 掩码清位 →
        // 移位/or 合并 → 回写」逐字段 RMW（如 `movzbl 0x3(%eax),%edx; and $0x1f,%edx;
        // mov %dl,0x3(%eax)` = m_count 清零、`shr $0x5; and $7; lea 0x0(,%edx,4);
        // and $0xffffffe3; or; mov %dl,0x3(%eax)`）。
        // 探针实测（/tmp/exp/varF/STSkillCommandData.h + /tmp/exp/stcmd_exp.cpp，路由与
        // check_tu_routed 同源 4.4.6）：
        //   成员初始化列表 `: m_command(0), …` = 0x7c B / 41 条（旧形，gap 81）
        //   体内 `bf = 0;`                      = 41 条（4.4 把 packed 位域清零折叠 ⇒ 无效）
        //   体内 `bf = bf;` 自赋值              = 0x170 / 128 条（≈ 但不等于 ORIG，且不清零）
        //   体内 `t = bf; bf = 0;`              = 93 条（清零等价，但不匹配）
        //   **链式赋值 `a = b = … = 0;`**       = 0x161 B / 123 行（122 条指令）——
        //     与 ORIG 0x0822da04 size 相同，objdump 逐行 diff 仅首行地址标签不同 ⇒ 采用。
        // 语义：七字段全部清零，与原成员初始化列表逐位等价。
        STCommand()
        {
            m_command = m_key0 = m_key1 = m_key2 = m_key3 = m_key4 = m_count = 0;
        }

        unsigned int m_command : 14;
        unsigned int m_key0    : 3;
        unsigned int m_key1    : 3;
        unsigned int m_key2    : 3;
        unsigned int m_key3    : 3;
        unsigned int m_key4    : 3;
        unsigned int m_count   : 3;
    } __attribute__((packed));  // ORIG 数组基址 +0x703db 非对齐 → align=1
                               //   （CUser ctor 0x864e410 循环步长 4 实证）

    // ORIG W 0x0822db66：先自动构造成员 m_commandPacked（declaration order，
    // 调 STCommand ctor 0x0822da04），随后函数体写 m_command(movw)/m_count(movb)/
    // memset(&m_key0, 0, 5)。字段初始化若走 init-list 会变成逐字节 movb，不符。
    STSkillCommandData()
    {
        m_command = 0;
        m_count = 0;
        memset(&m_key0, 0, 5);
    }

    unsigned short m_command;       // +0x00
    unsigned char m_count;          // +0x02
    union
    {
        struct
        {
            unsigned char m_key0;
            unsigned char m_key1;
            unsigned char m_key2;
            unsigned char m_key3;
            unsigned char m_key4;
        };
        unsigned char m_keyBytes[5];
    };
    STCommand      m_commandPacked; // +0x08
};
#pragma pack(pop)

#endif  // DNF_GAME_ST_SKILL_COMMAND_DATA_H_
