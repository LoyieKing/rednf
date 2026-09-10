// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EventCharacterParameterScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "EventCharacterParameterScript.h"
#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt
#include "LogManager.h"
#include "NpcScript.h"       // STScriptFileList（g_eventCharacterScriptFileList 权威类型）

// ORIG 全局（定义于 InitScriptProducers_5.cpp；本 TU 仅 extern 引用）：
//   g_eventCharacterScriptBaseDirectory  B 0x094f73c0 (char[0x100])
//   g_eventCharacterScriptFileList      B 0x094f74c0 (STScriptFileList, 0x18)
extern char g_eventCharacterScriptBaseDirectory[0x100];
extern STScriptFileList g_eventCharacterScriptFileList;

// ORIG 0848ecce：m_growType/m_secondGrowType 清零 + 两个 vector 构造后各自 clear()
#include "EventCharacterParameterScript/STEventCharacterInfo.inc"
}
// ORIG 083ce3ee：先析构 m_itemList(+0x10) 再 m_skillList(+0x04)（成员声明逆序，编译器自动生成）
#include "EventCharacterParameterScript/dtor_STEventCharacterInfo.inc"
#include "EventCharacterParameterScript/STEventCharacterInfo_level.inc"
}

// ORIG C1 0x8371fde：map 构造 + clear()。
#include "EventCharacterParameterScript/EventCharacterParameterScript.inc"
}

#include "EventCharacterParameterScript/dtor_EventCharacterParameterScript.inc"
#include "EventCharacterParameterScript/clear.inc"
}

// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0895b006 orig/ours=423/421 ndiff=376。
// 残差为巨件帧/槽号平移（局部表/缓冲）与 std::map/string 内联差（N89）；
// P167 调用点已核，语义按报告逐条对齐。
#include "EventCharacterParameterScript/importEventCharacterParameterScript.inc"
}

#include "EventCharacterParameterScript/load_event_character_list.inc"
}

#include "EventCharacterParameterScript/GetEventCharacterInfo.inc"
}
