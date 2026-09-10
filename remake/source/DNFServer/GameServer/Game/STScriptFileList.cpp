// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 STScriptFileList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— STScriptFileList（ORIG T 0x88bad3a 族）。
//   FindIndexByFullScan(char const*) const（ORIG T 0x88bae56，mangled
//   _ZNK16STScriptFileList19FindIndexByFullScanEPKc）：
//     遍历 std::map<int,std::string> m_fileMap（+0x00），
//     返回 value==path 的 key；未找到返回 0xffffffff（-1）。
//   （证据见 docs/class_func_reports/STScriptFileList/FindIndexByFullScan.md）

#include "NpcScript.h"
#include "LogManager.h"
#include "DNFLexWrapper.h"

// ORIG 0x0949b284 BSS：为 0 时重复索引报错返回 0；非 0 时重复项以负键覆盖插入。
extern char g_isAllowScriptDuplication;


// ORIG 0x088bacf8：map 默认构造后调 Clear。
#include "STScriptFileList/STScriptFileList.inc"
}

#include "STScriptFileList/Clear.inc"
#include "STScriptFileList/GetFileName.inc"
}

//   ORIG 循环形态：begin() 一次、end() 缓存于循环外（P13）、后缀 it++（ppEi）。
#include "STScriptFileList/FindIndexByFullScan.inc"
}

// ORIG T 0x088badba，mangled _ZN16STScriptFileList8GetIndexEPKc：
//   按文件名遍历 map（与 FindIndexByFullScan 不同：非 const 迭代器、end()
//   每次循环调用、后缀 it++），命中返回索引，未命中返回 -1。
//   （证据见 docs/class_func_reports/STScriptFileList/GetIndex.md）
#include "STScriptFileList/GetIndex.inc"
}

// ORIG 0x088bbd77
// _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 用 DNFLex 扫描列表文件，逐行 ScanInt+ScanStr 填充 m_fileMap。
// 第 4 参 map<int,int>* 在 ORIG 中未被使用（可由调用方传 0）。
// b28d-31: N 封顶（N82）——ORIG 0x088bbd77；134 vs 136（帧/槽）。
// [b28d-Main 代落·S248 P292] int→bool：ORIG 调用点 0x08903edb 为 `test %al,%al; je`
// （无 setne）；返回域 0/1。另两处声明（OnlyOrigBatch12.cpp:53 / CtorProducers_1.cpp:133）
// 本已为 bool ⇒ 本次消除跨 TU 返回类型 ODR 分歧。返回类型不入 mangled。
#include "STScriptFileList/loadRDARScriptListFile.inc"
}
