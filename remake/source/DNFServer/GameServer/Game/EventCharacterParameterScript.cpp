#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventCharacterParameterScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "EventCharacterParameterScript/STEventCharacterInfo.inc"

#include "EventCharacterParameterScript/dtor_STEventCharacterInfo.inc"

#include "EventCharacterParameterScript/STEventCharacterInfo_level.inc"


#include "EventCharacterParameterScript/EventCharacterParameterScript.inc"


#include "EventCharacterParameterScript/dtor_EventCharacterParameterScript.inc"

#include "EventCharacterParameterScript/clear.inc"


#include "EventCharacterParameterScript/importEventCharacterParameterScript.inc"


#include "EventCharacterParameterScript/load_event_character_list.inc"


#include "EventCharacterParameterScript/GetEventCharacterInfo.inc"

