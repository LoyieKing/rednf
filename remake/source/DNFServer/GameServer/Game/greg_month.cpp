// ============================================================================
// [SplitC 2026-09-11 · TU 迁移（ORIG GI 锚点）] 本文件由 OnlyOrigBatch11.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN5boost9gregorian10greg_month17get_month_map_ptrEv
//   ORIG TU     ：greg_month.cpp
//   .inc 为原样搬移（零文本改动）；月名常量表（匿名 namespace，内部链接，原属
//   本 CU）随迁。本壳只承载 TU 级外壳 + 依赖头。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include <string>
#include <cctype>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace boost
{
namespace gregorian
{
namespace
{
const char* const kLongMonthNames[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
const char* const kShortMonthNames[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
}  // namespace

#include "greg_month/get_month_map_ptr.inc"

}  // namespace gregorian
}  // namespace boost
