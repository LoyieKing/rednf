// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StatisticsPvP/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— StatisticsPvP（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/StatisticsPvP.md + ORIG 反汇编。
// 布局：+0x00 Mutex | +0x18 map<PvP_Key,PvP_Room>。
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
}

#include "StatisticsPvP/dtor_StatisticsPvP.inc"

#include "StatisticsPvP/reset.inc"
}

// [b28d-solo-StatisticsPvP] incPvPRoomResult DIFF(ndiff=69) -> AE（96=96，p167 10/10 CLEAR）。
// 修法（三处，均为源码形态差异，无 ABI 变更）：
//  ① else 累加分支：ORIG 单次 operator*（deEv）+ `&second` 入具名槽 -0xc（P0 槽-局部律），
//     原两次 `it->second.m_fieldN += x` 被 4.4 折成两次 deEv 且无槽；改具名
//     `PvP_Room* pRoom = &(*it).second;`（P167：`(*it).m` 才实例化 operator*），
//     累加写 `= pRoom->m_fieldN + n`（复现 ORIG `mov mem,%eax; lea/add %edx; mov`）。
//  ② insert 分支：`std::make_pair(key, room)` 额外生成 make_pair 临时 + 转换构造；
//     ORIG 为 value_type 直构 ⇒ 改 `std::pair<const PvP_Key, PvP_Room>(key, room)`
//     （与 ORIG `_ZNSt4pairIKS0_S1_EC1IRS0_RS2_EEOT_OT0_` 同符号）。
//  ③ PvP_Room 头 8 字节改为具名 PvP_Key 成员整体赋值（`room.m_key = key;`）：
//     ORIG 读-读-写-写 8 字节块拷贝（P246），两条标量赋值 4.4 恒 load-store 交错。
// 复验：check_tu_routed.sh COMPILE OK（identical=27 ae=47 near=0 diff=0）。
#include "StatisticsPvP/incPvPRoomResult.inc"
}
