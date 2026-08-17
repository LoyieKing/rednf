// df_game_r STItemScript 还原（2026-08-16）。
// 成员布局/赋值顺序以 ORIG Clear（0x898d718）逐条对照。

#include "STItemScript.h"

// ===================== SEXPERTJOB 嵌套结构构造（ORIG weak 0x849f34c / 0x850d0e8） =====================

STItemScript::SEXPERTJOB::stExpertJobAdditionalExp::stExpertJobAdditionalExp()
{
    m_field0 = 0.0f;
    m_field4 = 0;
}

STItemScript::SEXPERTJOB::stExpertCompoundResultVariation::stExpertCompoundResultVariation()
{
    m_field0 = 0.0f;
    m_field4 = 0.0f;
}

STItemScript::STItemScript()
{
    Clear();
}

STItemScript::~STItemScript()
{
}

void STItemScript::Clear()
{
    m_field0 = 0;
    m_field4 = 1;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field18 = 0;
    m_field1c = 0;
    m_field20 = 0;
    m_field24 = -1;
    m_pair28.first = -1;
    m_pair28.second = 0;
    m_field30 = 0;
    m_field31 = 0;
    m_field34 = 0;
    for (int i = 0; i < 0xb; ++i)
    {
        m_usable[i] = 0;
    }
    for (int i = 0; i < 0xb; ++i)
    {
        m_set44.insert(i);
    }
    m_field5c = 0;
    m_field60 = 0;
    m_field64 = 0;
    m_field68 = 0;
    m_str6c = "";
    m_field70 = 0;
    m_str74 = "";
    m_field78 = 0;
    m_str84 = "";
    m_vec88.clear();
    m_vec94.clear();
    m_fielda0 = 0;
    m_fielda4 = 0;
    m_fielda8 = -1;
    m_strac = "";
    m_strb0 = "";
    m_strb4 = "";
    m_strb8 = "";
    m_strbc = "";
    m_strc0 = "";
    m_fieldc4 = -1;
    m_fieldc8 = 0;
    m_fieldcc = 0;
    m_fieldd0 = 0;
    m_fieldda = 0;
    m_fieldd9 = 0;
    m_fieldd1 = 0;
    m_setdc.clear();
    m_setdc.insert(0);
    m_fieldf4 = 0;
    m_fieldf8 = 0;
    m_fieldfc = 1;
    m_field100 = 0;
    m_field104 = 1;
    m_field10e = 0;
    m_field10f = 0;
    m_vec110.clear();
    m_set11c.clear();
    m_field134 = 0;
    m_expertJob.m_field18 = 0;
    m_expertJob.m_field1c = 0;
    m_expertJob.m_usable.clear();
    m_expertJob.m_field4 = 0;
    m_expertJob.m_field0 = 0;
    m_expertJob.m_field8 = 0;
    m_expertJob.m_fieldc = 0;
    m_expertJob.m_field10 = 0;
    m_expertJob.m_field14 = 0;
    m_fieldd4 = 0;
    m_field80 = 0;
    m_dyeInfo.clear();
    m_str174.clear();
    m_fieldd8 = 0;
}

bool STItemScript::hasCategory(ENUM_ITEM_CATEGORY category) const
{
    std::set<ENUM_ITEM_CATEGORY>::const_iterator it = m_set11c.find(category);
    return it != m_set11c.end();
}
