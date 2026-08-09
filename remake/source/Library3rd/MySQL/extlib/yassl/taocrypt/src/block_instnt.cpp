/*
 * 强制发出原版二进制中存在、但 4.4.7 -O3 下被内联消失的弱符号：
 *   _ZN8TaoCrypt5BlockIjNS_20AllocatorWithCleanupIjEEE8CleanNewEj
 * 原版（GCC 4.4.4）在 Integer::Decode(Source&) 等处以 out-of-line 调用它。
 * 显式实例化不会改变调用点的内联决策，只补足符号面，语义不变。
 */
#include "runtime.hpp"
#include "block.hpp"

namespace TaoCrypt {
template void Block<word32>::CleanNew(word32);
}
