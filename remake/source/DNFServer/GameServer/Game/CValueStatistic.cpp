// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CValueStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CValueStatistic 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/CValueStatistic.md：
//   AddValueStatistic 0x0860d682 / InsertValueStatistic 0x0860d8a4
#include "CInventory.h"
// [Lane37CUser 2026-09-10 解环] CInventory.h 的 `#include "CUser.h"` 已改前向声明
//   （解环）⇒ 依赖传递获得 CUser 完整定义的 TU 需自行直补。
#include "CUser.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include <string.h>

// ---- CStatisticServerProxy（定义见 GameStubs_remaining.cpp） ----
#include "CStatisticServerProxy.h"

#include "Packet_Value_Statistic.h"

CValueStatistic* GetInstanceValueStatistic()
{
    static CValueStatistic instance;
    return &instance;
}

#include "CValueStatistic/reset.inc"
{
    memset(m_values, 0, sizeof(m_values));
}

#include "CValueStatistic/AddValueStatistic.inc"
                                        unsigned int value)
{
    // [b28d-59] 封 N82/N93（ORIG 0x0860d682）：已还原——无 fieldIdx/level 具名局部（P0：形参直读 +
    // 索引处内联 get_charac_level()，0x0860d7a5 每次重调）；条件为
    // field<0(js) / field>0x1d(jg) / level<=0(jle) / level>0x46(jle) 四项短路，
    // 汇聚 bool 后 `test %al; je`。插入路径也是 `+= value`（读-加-写，非直接赋值）。
    if (field < 0 || field > 0x1d ||
        user->get_charac_level() <= 0 || user->get_charac_level() > 0x46)
    {
        // [b28d-59] 全表达式临时链（P227）：ORIG 0x0860d6d2 在 cMyTrace ctor 之前
        // 求 varargs（get_charac_level → %ebx、field → %esi 跨 ctor 保活），具名
        // trace 会把它挪到 ctor 之后。实参序 (field, level)。
        cMyTrace(__PRETTY_FUNCTION__, 0x306, false)(
            "VALUE_STATISTIC wrong index (%d,%d)",
            field, user->get_charac_level());
        return false;
    }

    // [A24TailBatch2] P236/P0：ORIG 0x0860d6a3 `call GetServerGroup; mov %eax,-0x42c8(%ebp);
    // lea -0x42c8(%ebp),%ecx`（匿名临时槽绑定 const&，非具名 group 深槽）⇒ find 实参内联。
    // 旧具名 group 被 4.4 排入深区（-0x42d8）挤占 cMyTrace 槽位（ORIG cMyTrace@-0x42d8）。
    std::map<ENUM_SERVER_GROUP, stValueStatistic>::iterator it =
        m_data.find(user->GetServerGroup());
    // [A24TailBatch2] 判据①：ORIG 0x0860d788 `call ne; test %al; je 0860d7e8` ⇒
    // ne 为假（it==end）跳插入块 ⇒ 原码是 `if (it != end()) { 更新 } else { 插入 }`
    // （fall-through=更新 0x10f..0x161、je 目标=插入 0x166）；旧写 `== end()` 使
    // 4.4 反转块序（je→更新 / fall-through=插入）⇒ POLARITY 差。
    if (it != m_data.end())
    {
        // [A24TailBatch2] P0：ORIG 0x0860d79a `call ptEv; add $0x4,%eax; mov %eax,-0x1c(%ebp)`
        // ⇒ 原码有具名引用局部（槽往返），两次使用均 `mov -0x1c(%ebp),%ecx` 重读。
        stValueStatistic& stat = it->second;
        stat.m_values[user->get_charac_level()][field] += value;
    }
    else
    {
        // [A24TailBatch2] ORIG 0x0860d801 直接 `call memset`（非 reset() 转发；
        // 实参 lea stat / 0 / 0x2148）+ 0x0860d822 **纯 store**（无 load-add）⇒
        // 插入路径是 `= value`（旧 `+=` 多 5~6 条重算索引+读改写）。
        stValueStatistic stat;
        memset(&stat, 0, sizeof(stat));
        stat.m_values[user->get_charac_level()][field] = value;
        // [A24TailBatch2] ORIG 0x0860d82f 插入路径**再调一次** GetServerGroup 且结果落
        // 独立临时槽 -0x20（0x0860d806 索引用、0x0860d82f make_pair 用）⇒ 原码写
        // `make_pair(user->GetServerGroup(), stat)`（右值实参 ⇒ 引用绑定临时槽），
        // 而非复用前面已具名的 group（旧形少 3 条 call 序列 + 少一槽）。
        m_data.insert(std::make_pair(user->GetServerGroup(), stat));
    }
    return true;
}

#include "CValueStatistic/InsertValueStatistic.inc"
{
    // [b28d-59] AE（ORIG 0x0860d8a4）：修掉 group 局部缺失（恢复具名 group 槽 -0x1c）、
    // 删 proxy 判空（P171）、条件改 `if (hasValue == false) continue;`（P168a② 取反形）
    // 后 105/105 逐条一致。
    for (std::map<ENUM_SERVER_GROUP, stValueStatistic>::iterator it = m_data.begin();
         it != m_data.end(); ++it)
    {
        // [b28d-59] ORIG 0x0860d8d3/d8e6：group 槽 -0x1c、stat 引用槽 -0x18
        // （均具名局部，P0 槽往返实证）。
        ENUM_SERVER_GROUP group = it->first;
        stValueStatistic& stat = it->second;
        for (int level = 1; level < 0x47; ++level)
        {
            bool hasValue = false;
            for (int f = 0; f < 0x1e; ++f)
            {
                if (stat.m_values[level][f] != 0)
                {
                    hasValue = true;
                    break;
                }
            }
            // [b28d-59] ORIG 0x0860d933 `movzbl hasValue; xor $1; test; jne` ⇒
            // 原码取反形（P168a②直分支形）。
            if (hasValue == false)
            {
                continue;
            }
            {
                // ORIG 0x0860d8a4：ctor（PacketHeader(0x17ca,0x86)+m_level=0）构造，
                // 随后 m_level=level 与 memcpy m_values；不再整体 memset。
                Packet_Value_Statistic packet;
                packet.m_level = level;
                memcpy(packet.m_values, stat.m_values[level], 0x78);
                // [b28d-59] ORIG 0x0860d999..d9ad 无 proxy 判空（P171：原码不写即删；
                // GetServerProxy 返回值经 %eax 直传 SendPacket），旧判空多 1 槽 +3 条。
                GlobalData::s_statistic_proxy_mgr->GetServerProxy(group)->SendPacket(
                    (char*)&packet, 0x86);
            }
        }
        stat.reset();
    }
    return 1;
}
