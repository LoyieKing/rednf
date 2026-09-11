// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StatisticsPvP/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>

#include "GameTypes.h"

class StatisticsPvP
{
public:
    struct PvP_Key
    {
        int m_a;  // +0x00
        int m_b;  // +0x04

        bool operator<(const PvP_Key& other) const
        {
            if (m_a < other.m_a)
            {
                return true;
            }
            if (m_a == other.m_a)
            {
                if (m_b < other.m_b)
                {
                    return true;
                }
                return false;
            }
            return false;
        }
    };

    struct PvP_Room
    {
        // 2026-08-31：ORIG sizeof 16（map 节点 40 = 16+pair(8+16)；incPvPRoomResult
        // 0x860c653 构造 4 dword：{keyA, keyB, 1, value}）——头 8 字节为 PvP_Key 整体
        // 赋值（ORIG 读-读-写-写 8 字节块拷贝，P246），故具名结构成员而非两个标量。
        PvP_Key m_key;  // +0x00
        int m_field8;  // +0x08
        int m_fieldC;  // +0x0c
    };

    StatisticsPvP();
    ~StatisticsPvP();

    void reset();
    void incPvPRoomResult(int keyA, int keyB, int value);

    Mutex m_mutex;                         // +0x00
    std::map<PvP_Key, PvP_Room> m_map;     // +0x18
};

#include "StatisticsPvP/StatisticsPvP.inc"


#include "StatisticsPvP/dtor_StatisticsPvP.inc"

#include "StatisticsPvP/reset.inc"


#include "StatisticsPvP/incPvPRoomResult.inc"

