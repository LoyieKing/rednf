// ORIG STSecondGrowTypeInfo::clear W 0x08373a1e（objdump 转写）：
// i=0..1 { m_addInfo[i].clear(); m_skills[i].clear(); } 后 memset(&m_fieldbc,0,2)
// —— 仅清 m_fieldbc 低 2 字节，高 2 字节保留（ORIG 固有）。
void STSecondGrowTypeInfo::clear()
{
    for (int i = 0; i <= 1; ++i)
    {
        m_addInfo[i].clear();
        m_skills[i].clear();
    }
    memset(&m_fieldbc, 0, 2);
}