// STAgitUpgradeInfo 定义（ORIG gcc4.1.2，W 符号三件套）。
// C1 0x8979978 = sub $0x18 + call clear；clear 0x897994c = 4×movl $0
// （每条重载 this）；D1/D2 0x897a678 = 用户空析构（push/pop/ret）。
#include "CDataManager.h"

STAgitUpgradeInfo::STAgitUpgradeInfo()
{
    clear();
}

STAgitUpgradeInfo::~STAgitUpgradeInfo() {}

void STAgitUpgradeInfo::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
}
