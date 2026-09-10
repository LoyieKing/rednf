// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 SeriaRoomDecoObjectList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// SeriaRoomDecoObjectList.cpp — seriaDecoObjectInfo 四方法 +
// importSeriaRoomDecoEventObjectList
//
// ORIG（df_game_r，elf32-i386）：
//   seriaDecoObjectInfo::clear()                    W 0x08962422
//   seriaDecoObjectInfo::seriaDecoObjectInfo()      W 0x08962656
//   seriaDecoObjectInfo::~seriaDecoObjectInfo()     W 0x089626e8
//   seriaDecoObjectInfo::seriaDecoObjectInfo(const&) W 0x08962912
//   importSeriaRoomDecoEventObjectList              T 0x0895e79b
//     （_Z34importSeriaRoomDecoEventObjectListP34seriaRoomDecoEventObjectListScriptPKc）
// 报告：docs/class_func_reports/_lt_global_gt_/importSeriaRoomDecoEventObjectList.md
// 语义：脚本 "Event/SeriaRoomDecoEventObjectList.evt"（推断路径，ORIG 无
// 直接调用者）：
//   [event object list] 段内每条 "[object index] <key>" 后跟
//   [stay animation]/[walk animation]/[run animation] 单行 ScanStr，
//   [ramdom animation] 后多行 ScanStr 全部 push 入 m_ramdomAnimations，
//   "[/object index]" 结束并把 (key, info) insert 入 map；EOF 返回 true，
//   打开失败/空指针返回 false。
// ============================================================================
#include "SeriaRoomDecoObjectList.h"

#include <utility>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanStr

// ---- seriaDecoObjectInfo ----
#include "SeriaRoomDecoObjectList/seriaDecoObjectInfo.inc"

#include "SeriaRoomDecoObjectList/seriaDecoObjectInfo_1.inc"
}

#include "SeriaRoomDecoObjectList/dtor_seriaDecoObjectInfo.inc"

#include "SeriaRoomDecoObjectList/clear.inc"
}

// ============================================================================
// importSeriaRoomDecoEventObjectList —— ORIG T 0x0895e79b
//   bool (seriaRoomDecoEventObjectListScript*, const char*)
// ============================================================================
// [b28d-L8] N82 | 槽平移 + ScanType 调用序（ORIG 0x08xxxxxx）
#include "SeriaRoomDecoObjectList/importSeriaRoomDecoEventObjectList.inc"
}
