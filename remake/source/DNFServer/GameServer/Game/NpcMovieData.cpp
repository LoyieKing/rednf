// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 NpcMovieData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "NpcMovieData.h"

#include "NpcMovieData/NpcMovieData.inc"


#include "NpcMovieData/clear.inc"


#include "NpcMovieData/getNextMoviePath.inc"
{
    std::string result;
    if (m_movies.empty())
    {
        return result;
    }
    if ((int)m_movies.size() > m_movieIdx)
    {
        result = m_movies[m_movieIdx];
        m_movieIdx = (unsigned)(m_movieIdx + 1) % (unsigned)m_movies.size();
    }
    return result;
}

// ---- ORIG W 弱符号发射段（D1/D2 @0x08581d96，头内联空析构）----
// 本 TU 既有函数不 odr-use 析构（无 NpcMovieData 实例化），头内联空析构的
// 弱符号不会发射；此处真实构造/析构强制发射，不引入导出符号。
namespace
{
#include "NpcMovieData/emitNpcMovieDataDtor.inc"

}
