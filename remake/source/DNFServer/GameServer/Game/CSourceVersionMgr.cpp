// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSourceVersionMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN17CSourceVersionMgrC2Ev`（ORIG 0x86b2388）；ctor 已是首强符号。
#include <iostream>

#include "CSourceVersionMgr.h"

#include "CSourceVersionMgr/CSourceVersionMgr.inc"
    : source_version_list_()
{
    InsertSourceVersion(".", 0x16bdc);
    InsertSourceVersion("../ServerCommon", 0x16bdc);
    InsertSourceVersion("../../DNFShared", 0x16bdc);
}

#include "CSourceVersionMgr/InsertSourceVersion.inc"
{
    source_version_list_.push_back(SourceVersion(source_file_name, version));
}

#include "CSourceVersionMgr/SourceVersion.inc"
    : source_file_name_()
{
    source_file_name_ = source_file_name;
    version_ = version;
}

#include "CSourceVersionMgr/SourceVersion_1.inc"
    : source_file_name_(other.source_file_name_)
    , version_(other.version_)
{
}

#include "CSourceVersionMgr/dtor_SourceVersion.inc"
{
}

CSourceVersionMgr::SourceVersion& CSourceVersionMgr::SourceVersion::operator=(
    const SourceVersion& other)
{
    source_file_name_ = other.source_file_name_;
    version_ = other.version_;
    return *this;
}
