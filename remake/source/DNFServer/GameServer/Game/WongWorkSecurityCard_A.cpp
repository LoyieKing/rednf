// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkSecurityCard_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// WongWorkSecurityCard_A.cpp — WongWork::CSecurityCard ORIG-only 成员（worklist A 段）
// 逐函数自 ORIG df_game_r 反汇编忠实转写（地址见各处注释）。
// 布局依赖：CUser.h CSecurityCard（+0x40 字节与 +0x44 requestState 由
// OnlyOrig_CUser 批次按 ORIG 补正后，本 TU 与真机偏移一致）。
#include "CUser.h"
#include <string.h>

namespace WongWork
{

#include "WongWorkSecurityCard_A/init.inc"


#include "WongWorkSecurityCard_A/getCertFlag.inc"


#include "WongWorkSecurityCard_A/getCertCnt.inc"


#include "WongWorkSecurityCard_A/incFailCnt.inc"


#include "WongWorkSecurityCard_A/resetFailCnt.inc"


#include "WongWorkSecurityCard_A/setActivate.inc"
{
    m_activate2f = flag;
}

#include "WongWorkSecurityCard_A/setCertified.inc"
{
    m_certified30 = flag;
}

#include "WongWorkSecurityCard_A/setCertFlag.inc"
{
    m_certFlag31 = static_cast<unsigned char>(flag);
}

// W 0x0822ef64：int 参数写入 +0x44。
namespace {
struct CSecurityCard44View { char m_pad[0x44]; int m_requestState44; };
}
#include "WongWorkSecurityCard_A/setRequestState.inc"


#include "WongWorkSecurityCard_A/isCertFlagOn.inc"


#include "WongWorkSecurityCard_A/incCancelCnt.inc"


#include "WongWorkSecurityCard_A/setSecurityCardInfo.inc"


} // namespace WongWork
