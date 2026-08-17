// 本 TU 由 GCC 4.1.2 编译（ORIG .comment 中 4.1.2 x2 之一；A/B 实测 4.1.2
// 对 register bool 比较生成 ORIG 的 setle %al + test %al,%al 形态，
// 4.4.x 各版本均为 setle %bl / jg，无法复现）。
// 其余函数留在 DNFProhibitUser.cpp（c6444r 编译，保持已 identical 不回退）。
#include "DNFProhibitUser.h"

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0041 | monitor | 与ORIG差异=DIFF | CDNFProhibitUser::IsTimeOutConnectable | 详见 function_reports/monitor/_ZN16CDNFProhibitUser20IsTimeOutConnectableEv.md
bool CDNFProhibitUser::IsTimeOutConnectable()
{
    m_remain--;
    if ((short)m_remain <= 0)
    {
        return true;
    }
    return false;
}
