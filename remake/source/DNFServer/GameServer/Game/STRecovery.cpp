// STRecovery：恢复数据。布局见 STRecovery.h。
// ORIG：C1 0x089c3098（仅调用 clear）、clear 0x089c30ac。
#include "STRecovery.h"

STRecovery::STRecovery()
{
    clear();
}

void STRecovery::clear()
{
    m_bAddMode = 0;
    m_nValue1 = 0;
    m_nValue2 = 0;
    m_nTarget = 0;
    m_bEnable = 0;
}
