#ifndef GAME_STPACKETLOG_H_
#define GAME_STPACKETLOG_H_

// ============================================================================
// df_game_r 还原 —— STPacketLog（PowerWar 包日志条目，CPowerWarPacketLog 的
// map<int,STPacketLog*> 值类型；本头为唯一声明点）。
// 布局依据 ORIG C1 0x08481100 / D1 0x084810ea / Compare 0x08481030 及
// CPowerWarPacketLog::InsertPacketLog 0x08480970 写入序实证，sizeof 0x14
// （ORIG _Znwj(0x14) 分配实证）：
//   +0x00 eSendType（4B，InsertPacketLog 第 3 参）
//   +0x04 int（4B，InsertPacketLog 第 1 参 i —— ORIG movl 写 +4）
//   +0x08 ushort（2B，InsertPacketLog 第 2 参 t 低 16 位）
//   +0x0a pad（2B）
//   +0x0c int m_count（新元素置 1；重复 key 时 ++；Compare 排序键）
//   +0x10 std::string（assign(char const*)）
// 2026-08-28：m_field4 由 unsigned short 改 int（ORIG InsertPacketLog 0x08480a52
// 以 movl 四字节写 +0x04 实证；原 ushort 声明使布局漂移至 sizeof 0x10）。
// ============================================================================

#include <string>

enum eSendType
{
    ESENDTYPE_0 = 0
};

struct STPacketLog
{
    STPacketLog();                 // ORIG W 0x08481100：string@+0x10 默认构造
    ~STPacketLog();                // ORIG W 0x084810ea：析构 string@+0x10
    // ORIG W 0x08481030：返回 this->m_count > other->m_count。
    static bool Compare(const STPacketLog* a, const STPacketLog* b);

    eSendType m_sendType;          // +0x00（InsertPacketLog 第 3 参写 +0）
    int m_field4;                  // +0x04（InsertPacketLog 第 1 参 i 写 +4）
    int m_field8;                   // +0x08（InsertPacketLog 第 2 参 t 零扩展写 +8——ORIG 0x08480a4c movl 实证）
    int m_count;                   // +0x0c（新元素置 1；重复 key 时 ++；Compare 排序键）
    std::string m_name;            // +0x10（assign(char const*)）
};   // sizeof 0x14

#endif  // GAME_STPACKETLOG_H_
