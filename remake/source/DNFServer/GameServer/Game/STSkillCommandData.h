#ifndef DNF_GAME_ST_SKILL_COMMAND_DATA_H_
#define DNF_GAME_ST_SKILL_COMMAND_DATA_H_

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
        STCommand() : m_command(0), m_key0(0), m_key1(0), m_key2(0),
                      m_key3(0), m_key4(0), m_count(0) {}

        unsigned int m_command : 14;
        unsigned int m_key0    : 3;
        unsigned int m_key1    : 3;
        unsigned int m_key2    : 3;
        unsigned int m_key3    : 3;
        unsigned int m_key4    : 3;
        unsigned int m_count   : 3;
    } __attribute__((packed));  // ORIG 数组基址 +0x703db 非对齐 → align=1
                               //   （CUser ctor 0x864e410 循环步长 4 实证）

    STSkillCommandData() : m_command(0), m_count(0), m_key0(0), m_key1(0),
                           m_key2(0), m_key3(0), m_key4(0), m_commandPacked() {}

    unsigned short m_command;       // +0x00
    unsigned char  m_count;         // +0x02
    unsigned char  m_key0;          // +0x03
    unsigned char  m_key1;          // +0x04
    unsigned char  m_key2;          // +0x05
    unsigned char  m_key3;          // +0x06
    unsigned char  m_key4;          // +0x07
    STCommand      m_commandPacked; // +0x08
};
#pragma pack(pop)

#endif  // DNF_GAME_ST_SKILL_COMMAND_DATA_H_
