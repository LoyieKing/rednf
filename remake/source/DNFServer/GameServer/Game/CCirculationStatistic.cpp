// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCirculationStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "CCirculationStatistic.h"
#include "CUser.h"
#include <string.h>
#include "Packet_Circulation_Statistic.h"   // ORIG 包布局（LinkClosureD 补 include）
#include "CStatisticServerProxy.h"          // SendPacket
#include "CServerProxyMgr.h"                // GetServerProxy 模板
#include "GlobalData.h"                     // s_statistic_proxy_mgr


#include "CCirculationStatistic/reset.inc"
}

CCirculationStatistic* GetInstanceCirculationStatistic()
{
    static CCirculationStatistic instance;
    return &instance;
}

// ORIG 0x0860daaa：无 user 判空、无 level 局部缓存（get_charac_level 在条件里被
// 调两次、错误分支再调一次、命中分支各调一次），且槽位不是 map::operator[] 而是
// find/end 比较 + 未命中时栈上零初始化记录 insert(make_pair(...))。
#include "CCirculationStatistic/AddCirculationStatistic.inc"
}

// ORIG T 0x0860dccc sz 0x184（InsertCirculationStatistic.md 全文已读）：
// 遍历 m_data（map<ENUM_SERVER_GROUP,stCirculationStatistic>）；每条目 level 1..0x46
// 有任一 field 值非 0 → 构造 Packet_Circulation_Statistic（0xd0 字节）：
//   ctor(0x086100f8)→m_channel=get_channel_no()→m_level=level→memcpy(数据, level*0xc0)
//   → GetServerProxy(group).SendPacket(buf,0xd0)；全部 level 发送完 → reset。
// 注意：局部包对象在栈上，ctor 置 m_level/m_channel 为 0（Packet_Circulation_Statistic.h
// 初始化列表已对位）。
#include "CCirculationStatistic/InsertCirculationStatistic.inc"
}
