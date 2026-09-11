#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 QuestMovieInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r QuestMovieInfo 还原（G3 任务域）。
// 依据 docs/class_func_reports/QuestMovieInfo.md 的完整函数体与布局：
//   ctor 0x089173f0 / operator= 0x08917458 / operator== 0x0891748e / clear 0x089174d4
// 布局：两个连续 std::string（+0x00 / +0x04），类大小 8 字节。
// 注意 ORIG mangled：operator= 参数为非 const 引用（_aSERS_），
// operator== 为非 const 成员（_ZN14QuestMovieInfoeqERKS_）。
// C1ERKS 0x08921fbc / D1 0x0891cbd0 为 ORIG 模板实例化弱符号（隐式生成形态）：
//   拷贝构造=初始化列表逐成员 string 拷贝（P29），析构=成员逆序 + EH 清理。
#include "QuestMovieInfo.h"
#include "DNFLexWrapper.h"   // ScanType / ScanInt / ScanStr
#include "NpcScript.h"       // loadRDARScriptFile

#include "QuestMovieInfo/importQuestMovieList.inc"


#include "QuestMovieInfo/QuestMovieInfo.inc"


#include "QuestMovieInfo/QuestMovieInfo_1.inc"


#include "QuestMovieInfo/dtor_QuestMovieInfo.inc"


#include "QuestMovieInfo/clear.inc"


QuestMovieInfo& QuestMovieInfo::operator=(QuestMovieInfo& other)
{
    m_movieName = other.m_movieName;
    m_moviePath = other.m_moviePath;
    return *this;
}

bool QuestMovieInfo::operator==(const QuestMovieInfo& other)
{
    return m_movieName == other.m_movieName &&
           m_moviePath == other.m_moviePath;
}
