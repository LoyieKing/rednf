// STGuildStorageUpgradeInfo 定义（ORIG gcc4.1.2，W 符号三件套）。
// C1 0x89799c2 = sub $0x18 + call clear；clear 0x897998c = 5×movl $0
// （每条重载 this）；D1/D2 0x897a88a = 用户空析构（push/pop/ret）。
#include "CDataManager.h"

STGuildStorageUpgradeInfo::STGuildStorageUpgradeInfo()
{
    clear();
}

STGuildStorageUpgradeInfo::~STGuildStorageUpgradeInfo() {}

void STGuildStorageUpgradeInfo::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_field10 = 0;
}
