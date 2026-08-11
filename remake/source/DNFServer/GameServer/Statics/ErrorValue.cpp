// df_statics_r — ErrorValue 构造函数单独 TU（2026-08-11）
// ORIG 中 ErrorValue::ErrorValue() 与 STErrorStatic 构造不在同一编译单元：
// STErrorStatic 构造看到的是纯声明，GCC -O0 为其保留 0x18 的调用区
// （sub $0x18），与 ORIG 0x806372c 逐字节一致；若同 TU 内联可见则只留 0x4。
#include "HWSpecResearcher.h"

ErrorValue::ErrorValue()
{
    m_field0 = 0xffff;
    m_field4 = 0xffffffff;
}
ErrorValue::ErrorValue(unsigned short value, unsigned int param)
{
    m_field0 = value;
    m_field4 = param;
}
