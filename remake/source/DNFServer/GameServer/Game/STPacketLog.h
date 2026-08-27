#ifndef GAME_STPACKETLOG_H_
#define GAME_STPACKETLOG_H_

// ============================================================================
// df_game_r 还原 —— STPacketLog（PowerWar 包日志条目，CPowerWarPacketLog 的
// map<int,STPacketLog*> 值类型；本头为唯一声明点）。
// 布局依据 ORIG C1 0x08481100 / D1 0x084810ea / Compare 0x08481030 及
// CPowerWarPacketLog::InsertPacketLog 0x08480970 写入序实证，sizeof 0x14
// （ORIG _Znwj(0x14) 分配实证）。
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
    unsigned short m_field4;       // +0x04（InsertPacketLog 第 2 参 t 写 +4）
    unsigned short m_field8;       // +0x08（InsertPacketLog 第 1 参 i 低 16 位写 +8；语义未证【推断】）
    int m_count;                   // +0x0c（新元素置 1；重复 key 时 ++；Compare 排序键）
    std::string m_name;            // +0x10（assign(char const*)）
};   // sizeof 0x14

#endif  // GAME_STPACKETLOG_H_
