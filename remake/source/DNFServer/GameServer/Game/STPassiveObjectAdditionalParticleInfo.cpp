// ============================================================================
// STPassiveObjectAdditionalParticleInfo.cpp — C1/clear 实现
//
// 来源（df_game_r，elf32-i386）：
//   C1   W 0x08a3dad0：初始化列表 3 × string 默认构造 → 函数体 call clear()
//     （P69：ctor 内 call clear 必须外联定义；含 unwind 清理路径）
//   clear W 0x08a3da5a：3 × string::operator=(const char*) 赋 ""（0x8e1dc00）→
//     +0x0c/+0x10/+0x14/+0x18 movl $0 直写 → +0x1c movb $0 直写。
// ============================================================================
#include "STPassiveObjectAdditionalParticleInfo.h"

STPassiveObjectAdditionalParticleInfo::STPassiveObjectAdditionalParticleInfo()
    : m_s00(), m_s04(), m_s08()
{
    clear();
}

void STPassiveObjectAdditionalParticleInfo::clear()
{
    m_s00 = "";
    m_s04 = "";
    m_s08 = "";
    m_i0c = 0;
    m_i10 = 0;
    m_i14 = 0;
    m_i18 = 0;
    m_b1c = 0;
}
