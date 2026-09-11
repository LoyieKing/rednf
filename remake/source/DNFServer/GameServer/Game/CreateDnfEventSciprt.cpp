// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CreateDnfEventSciprt/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CreateDnfEventSciprt 实现（独立新 TU，全局命名空间类）。
// 依据 docs/class_func_reports/CreateDnfEventSciprtServer/{Load,Destroy}.md、
// docs/class_func_reports/CreateDnfEventSciprt/{loadScript,clearScript,
// GetItemInfo}.md 与 ORIG 反汇编逐段转写。
//
// 注册链（ARAD.cpp Init #8，待 ARAD 域批次接入）：
//   RegistScript(Singleton<CreateDnfEventSciprtServer>::Get(), 0x13)
#include "CreateDnfEventSciprt.h"

#include <cstdio>

#include "DNFLexWrapper.h"    // loadRDARScriptFile / unloadRDARScriptFile /
                              // ScanType / ScanInt / ScanTypeOrInt
#include "GameTypes.h"        // cMyTrace

namespace
{
typedef std::vector<CreateDnfEventSciprt::ItemInfo> ItemInfoVector;
}  // namespace

// ============================================================================
// CreateDnfEventSciprt
// ============================================================================

#include "CreateDnfEventSciprt/CreateDnfEventSciprt.inc"


#include "CreateDnfEventSciprt/dtor_CreateDnfEventSciprt.inc"


#include "CreateDnfEventSciprt/loadScript.inc"


#include "CreateDnfEventSciprt/clearScript.inc"


#include "CreateDnfEventSciprt/GetItemInfo.inc"


#include "CreateDnfEventSciprt/GetItemInfo_1.inc"


// ============================================================================
// CreateDnfEventSciprtServer
// ============================================================================

#include "CreateDnfEventSciprt/CreateDnfEventSciprtServer.inc"


#include "CreateDnfEventSciprt/dtor_CreateDnfEventSciprtServer.inc"


#include "CreateDnfEventSciprt/Load.inc"


#include "CreateDnfEventSciprt/Destroy.inc"
{
    return 1;
}

// ============================================================================
// ORIG 单例链完整类族：_ZN4ARAD9SingletonI26CreateDnfEventSciprtServerEC1Ev（W
// 0x08164902）、Get（W 0x08164907）、guard var/obj（0x09404630/0x09404638）、
// GetPtr。本 TU 经下列可执行引用完成隐式实例化（inline 模板各 TU COMDAT 合并，
// 与 ARAD.cpp 将来的 #8 RegistScript 调用不冲突）；勿在他处重复显式实例化。
// ============================================================================
namespace
{
ARAD::ScriptInterface* instantiateCreateDnfEventSciprtServerSingleton()
{
    return ARAD::Singleton<CreateDnfEventSciprtServer>::Get();
}
}  // namespace
