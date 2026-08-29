// Sanicova::CPad::setPassword(char*) — ORIG _ZN8Sanicova4CPad11setPasswordEPc W @0x0828713a (size 0x22)
// 忠实转写自 ORIG 反汇编（推断：无独立报告，docs/class_func_reports/Sanicova__CPad/ 无 setPassword.md）。
// ORIG 仅一条调用：strncpy(this /*+0x00*/, pwd, 0x24)；无 memset。
#include "CUser.h"

namespace Sanicova
{

void CPad::setPassword(char* pwd)
{
    // ORIG: strncpy(&m_pad[0], pwd, 0x24)
    strncpy(m_pad, pwd, 0x24);
}

} // namespace Sanicova
