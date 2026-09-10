// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LevelupSupportLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// LevelupSupportLeafC3.cpp —— LevelupSupport 域收尾 TU（独占；LevelupSupportLeafC3 批次）。
//
// 指派符号（全部逐份完整阅读 ## 汇编 + ## 反编译 C 后转写，报告路径见各函数）：
//   1. LevelupSupportEventManger::Load      T 0x08146e3c
//      docs/class_func_reports/LevelupSupportEventManger/Load.md
//   2. LevelupSupportEventManger::Destroy   T 0x08146fda
//      docs/class_func_reports/LevelupSupportEventManger/Destroy.md
//   3. LevelupSupport2ndEventManger::Load    T 0x081477e4
//      docs/class_func_reports/LevelupSupport2ndEventManger/Load.md
//   4. LevelupSupport2ndEventManger::Destroy T 0x08147864
//      docs/class_func_reports/LevelupSupport2ndEventManger/Destroy.md
//   5. LevelupSupportSciprt::loadScript      T 0x08ab514c
//      docs/class_func_reports/LevelupSupportSciprt/loadScript.md
//   6. LevelupSupportSciprt::clearScript     T 0x08ab5442
//      docs/class_func_reports/LevelupSupportSciprt/clearScript.md
//   7. LevelupSupport2ndScript::loadScript   T 0x08ab55a4
//      docs/class_func_reports/LevelupSupport2ndScript/loadScript.md
//   8. LevelupSupport2ndScript::clearScript  T 0x08ab5964
//      docs/class_func_reports/LevelupSupport2ndScript/clearScript.md
//
// 分工边界（查重后确认，防 ODR）：
//   - 两 manager C1/D1 已由 EventLevelupA2.cpp 定义；
//   - 两脚本 C1/D1 已由 EventScriptsCompleteB2.cpp 定义；
//   - CheckNewStep/UpdateEventInfo/SendCharacState/Process/
//     DB_SaveEventLevelupSupport::makeRequest 已由 CUserDepsB.cpp 定义。
//   本 TU 不重复上述符号。
//
// vtable：两 manager 的 key function 为槽0 Load，本 TU 定义后产生
// _ZTV25LevelupSupportEventManger / _ZTV28LevelupSupport2ndEventManger
// （ORIG V 0x08b73c80 / 0x08b73c70），使 EventLevelupA2.cpp 中 C1/D1 的
// vptr 写入可链接。Destroy 返回 void 系 ARAD::ScriptInterface 槽1 约定
// （ORIG 体尾 mov $0x1,%eax 为 void 返回约定的寄存器残留，与
// AvatarConvertServer::Destroy 同例）。
// ============================================================================
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "GameTypes.h"                 // cMyTrace
#include "LevelupSupportEventManger.h" // 四类权威声明

// RDAR 脚本扫描 helper（真实符号，定义在 DNFLexWrapperHelpers.cpp / CDataManager.cpp；
// 此处仅原型。不 include DNFLexWrapper.h：其 DNFLex.h→<iostream> 会引入
// std::ios_base::Init 静态初始化块，ORIG 本域 TU 无该块——经 TU 对比实证）。
#include "LevelupSupportLeafC3/loadRDARScriptFile.inc"
#include "LevelupSupportLeafC3/unloadRDARScriptFile.inc"
#include "LevelupSupportLeafC3/ScanType.inc"
#include "LevelupSupportLeafC3/ScanInt.inc"
#include "LevelupSupportLeafC3/ScanTypeOrInt.inc"
#include "LevelupSupportLeafC3/ScanStr.inc"
#include "LevelupSupportLeafC3/getCharacterJobToEnumValue.inc"
typedef char lvl_manger_size_assert[sizeof(LevelupSupportEventManger) == 0x40 ? 1 : -1];
typedef char lvl_2nd_manger_size_assert[sizeof(LevelupSupport2ndEventManger) == 0x28 ? 1 : -1];
typedef char lvl_sciprt_size_assert[sizeof(LevelupSupportSciprt) == 0x18 ? 1 : -1];
typedef char lvl_2nd_sciprt_size_assert[sizeof(LevelupSupport2ndScript) == 0x24 ? 1 : -1];

// ============================================================================
// LevelupSupportEventManger::Load —— 报告 Load.md @0x08146e3c。
// cMyTrace(pretty,0x18,9,true,false) → 清脚本 map → loadScript()==0 才继续：
// 清 m_stepIndexMap/m_events → 按 map 升序写入 level→档位序号 与
// EventInfo(level 截断 ushort, 0)。迭代器递增为 ORIG 的 operator++(int) 形态。
// [b28d-N82] fresh 复验 110/114：残差 = 迭代器三槽阶梯（ORIG -0x3c/-0x40/-0x44
// 深→浅，4.4 -0x18/-0x2c/-0x44）与 4.4 缺 push %ebx（callee-saved 需求差）
// —— P28/N82 槽方向族；块序/调用点全等，语义等价。
// ============================================================================
#include "LevelupSupportLeafC3/Load.inc"
}

// ============================================================================
// LevelupSupportEventManger::Destroy —— 报告 Destroy.md @0x08146fda。
// clearScript（释放堆上 vector）→ 清 m_stepIndexMap → 清 m_events。返回 void
// 见文件头说明。
// ============================================================================
#include "LevelupSupportLeafC3/Destroy.inc"
}
// ============================================================================
// LevelupSupportEventManger::GetDefaultEventInfo —— 报告 GetDefaultEventInfo.md
// @0x08147012（_ZNK25LevelupSupportEventManger19GetDefaultEventInfoEv，sz 0xb）：
// 仅返回 &m_events（this+0x34）的 const 引用。ORIG 唯一调用点
// Inter_LoadEventLevelupSupport::dispatch_sig @0x84e8d5f 把返回值直接作为
// vector<EventInfo>::operator=(const vector&) 的源实参（0x84f16ae），实证
// 引用返回 ABI；故头声明为 const std::vector<EventInfo>&（本次修正）。
// ============================================================================
const std::vector<LevelupSupportEventManger::EventInfo>&
#include "LevelupSupportLeafC3/GetDefaultEventInfo.inc"
}

// ============================================================================
// LevelupSupport2ndEventManger::Load —— 报告 Load.md @0x081477e4。
// cMyTrace(pretty,0xb7,9,true,false) → 清脚本 map（manager+0x8 =
// m_script.m_stepItems）→ 返回 loadScript()==0。
// [b28d-N82] 同 EventManger::Load（0x08146e3c）：帧 0x?? 与迭代器槽方向差。
// ============================================================================
#include "LevelupSupportLeafC3/Load_1.inc"
}

// ============================================================================
// LevelupSupport2ndEventManger::Destroy —— 报告 Destroy.md @0x08147864。
// 仅 clearScript。
// ============================================================================
#include "LevelupSupportLeafC3/Destroy_1.inc"
}

// ============================================================================
// LevelupSupportSciprt::loadScript —— 报告 loadScript.md @0x08ab514c。
// "Event/"+"tw_LevelupSupport.evt"；[step] → [level](ScanInt) → [reward]
// （循环 ScanTypeOrInt 取 item index、ScanInt 取数量，遇 TYPE 退出）→ [/step]
// （find 查重 → duplicate printf+return -1【此路径不 unloadRDARScriptFile，
// 忠实】/ new vector + swap 转移 + insert）。任一节内 ScanType 失败视为正常
// 结束：unloadRDARScriptFile 后返回 0；打开失败直接返回 -1。
// ============================================================================
// [b28d-N86] fresh 复验 210/213：残差 = std::string 局部（-0x44 vs -0x4c）+
// vector<ItemInfo> 构造/析构/swap 的 libstdc++ 外联槽整体平移 8B（P112/N86），
// 解析分支与全部调用点一一对应 ⇒ 语义等价、byte 层封顶。
#include "LevelupSupportLeafC3/loadScript.inc"
}

// ============================================================================
// LevelupSupportSciprt::clearScript —— 报告 clearScript.md @0x08ab5442。
// begin/end 遍历 delete 堆上 vector 后 map.clear。注意：ORIG 循环体【无迭代器
// 递增】（汇编 +0x33..+0x55 无 operator++ 调用）——忠实复刻；map 非空时行为与
// ORIG 一致（首元素反复析构路径）。
// ============================================================================
#include "LevelupSupportLeafC3/clearScript.inc"
}

// ============================================================================
// LevelupSupport2ndScript::loadScript —— 报告 loadScript.md @0x08ab55a4。
// 同上状态机，另含 [job type]（ScanStr → getCharacterJobToEnumValue 写 m_jobType）
// 与 [mail title]/[mail content]（ScanStr 直写两个 string 成员）。重复 [level]
// 路径同样不 unloadRDARScriptFile。
// ============================================================================
// [b28d-N86] fresh 复验 264/267（ORIG 0x08ab55a4）：同 LevelupSupportSciprt::
// loadScript（0x08ab514c）——string/vector/map 局部槽整体平移 + swap 形差异。
#include "LevelupSupportLeafC3/loadScript_1.inc"
}

// ============================================================================
// LevelupSupport2ndScript::clearScript —— 报告 clearScript.md @0x08ab5964。
// 与 LevelupSupportSciprt::clearScript 同构（同样无迭代器递增，忠实复刻），
// 目标 map 为 +0x04 的 m_stepItems。
// ============================================================================
// [b28c-31] AE：`delete it->second` 复现 ORIG 0x08ab59c6 无槽往返形（43=43）。
#include "LevelupSupportLeafC3/clearScript_1.inc"
}
