// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPowerWarPacketLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CPowerWarPacketLog 实现。
// 报告：docs/class_func_reports/CPowerWarPacketLog/（CPowerWarPacketLog /
// InsertPacketLog / PrintPowerWarPacketLogReport / Reset / ~CPowerWarPacketLog）。
#include <algorithm>
#include <vector>

#include "CPowerWarPacketLog.h"
#include "GameTypes.h"   // cMyTrace

// ORIG C1 0x0848085a：map 成员默认构造 + Reset（无 vptr 写入）。
#include "CPowerWarPacketLog/CPowerWarPacketLog.inc"
}

// ORIG D1 0x0848089c：Reset() + map 成员析构（Reset 已 clear，析构为空操作）。
#include "CPowerWarPacketLog/dtor_CPowerWarPacketLog.inc"
}

// ORIG Reset 0x084808de：遍历 map，delete 每个 STPacketLog（析构+释放），
// 最后 map::clear()。
#include "CPowerWarPacketLog/Reset.inc"
}

// ORIG InsertPacketLog 0x08480970：以 (unsigned int)t 为 key find；
// 命中 → ++(元素->m_count)；未命中 → new STPacketLog(0x14) 初始化
// （m_count=1、m_field8=t、m_sendType=type、m_field4=i、m_name.assign(name)）
// 后 make_pair(t, log) insert。
#include "CPowerWarPacketLog/InsertPacketLog.inc"
}

// ORIG PrintPowerWarPacketLogReport 0x08480ac0：
//   cMyTrace 三段头 → 收集 vector<STPacketLog*> → sort(Compare) →
//   逐条 cMyTrace 打印 → 尾分隔线。
#include "CPowerWarPacketLog/PrintPowerWarPacketLogReport.inc"
}
