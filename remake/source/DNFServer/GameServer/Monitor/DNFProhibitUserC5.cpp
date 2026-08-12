// 本 TU 由 GCC 4.1.2 编译（ORIG .comment 中 4.1.2 x2 之一；A/B 实测 4.1.2
// 对 register bool 比较生成 ORIG 的 setle %al + test %al,%al 形态，
// 4.4.x 各版本均为 setle %bl / jg，无法复现）。
// 其余函数留在 DNFProhibitUser.cpp（c6444r 编译，保持已 identical 不回退）。
#include "DNFProhibitUser.h"

bool CDNFProhibitUser::IsTimeOutConnectable()
{
    m_remain--;
    register bool b = ((short)m_remain <= 0);
    if (b)
    {
        return true;
    }
    return false;
}
