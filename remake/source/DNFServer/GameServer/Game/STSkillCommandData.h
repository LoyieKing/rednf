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
        // §P336（拟）packed 位域 ctor 的「读一次再写 0」形 —— 复现 ORIG(4.1) 的逐字段
        // read-modify-write，同时保持清零语义。
        // ORIG W 0x0822da04 该 ctor 122 条指令、零 call，全是 +0x02/+0x03 两字节上的
        // 「movzbl 读 → and 掩码清位 → （移位/or 合并）→ 回写」逐字段往返，如
        // `movzbl 0x3(%eax),%edx; and $0x1f,%edx; mov %dl,0x3(%eax)`（= m_count 清零）、
        // `shr $0x5; and $7; lea 0x0(,%edx,4); and $0xffffffe3; or; mov %dl,0x3(%eax)`。
        // 四形指令数实测（/tmp/fdp_probe/{p4,p5,p6}.cpp，route 与 c6root 4.4.7 逐位一致）：
        //   成员初始化列表        = 41 条（本方旧形；CUser 侧 ourN=41 精确吻合，gap 81）
        //   体内 `bf = 0;`         = 41 条（4.4 把 packed 位域清零折叠 ⇒ 无效）
        //   体内 `bf = bf;`        = 128 条（≈ ORIG 122，但**不清零** ⇒ 改可观察行为，弃）
        //   体内 `t = bf; bf = 0;` = 93 条（清零语义与旧形逐位一致 ⇒ **采用**）
        //   清+自赋值交替          = 165 条（超 ORIG，弃）
        // 读出的旧值一律丢弃（非 volatile ⇒ 无可观察副作用），最终每位都被写 0，
        // 与成员初始化列表版完全等价；不给 .h 加 volatile（避免污染全部消费 TU）。
        STCommand()
        {
            unsigned int norm;
            norm = m_command; m_command = 0;
            norm = m_key0;    m_key0 = 0;
            norm = m_key1;    m_key1 = 0;
            norm = m_key2;    m_key2 = 0;
            norm = m_key3;    m_key3 = 0;
            norm = m_key4;    m_key4 = 0;
            norm = m_count;   m_count = 0;
            (void)norm;
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
