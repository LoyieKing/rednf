void STAIInfoScript::clear()
{
    // ORIG 0x083673d4（objdump 逐条转写）。
    m_thinkTerm = 0x3e8;
    m_destinationChangeTerm = 0x3e8;
    m_keepRange0.clear();               // ORIG 0x083673f9..0x08367439 逐条
    m_keepRange1.clear();
    m_keepRange2.clear();
    m_keepRange3.clear();
    m_keepRange4.clear();
    m_dir.clear();
    for (int i = 0; i <= 4; ++i)
        m_patterns[i].clear();
    for (int i = 0; i <= 21; ++i)
    {
        // ORIG 0x08367487..0x083674b0：两分支各自重装载 this+下标写常量
        // （三目会被 4.4 合并公共地址，-2 insn）。
        if (i == 0)
            m_targetingBonus[i] = 0x32;
        else
            m_targetingBonus[i] = 0;
        m_targetingBonusStr[i].clear();
    }
    m_keepDistance = 0x64;
    m_intelligence = 0x64;
    m_field1f0 = 0x64;
    m_field1f4 = 0x1388;
    m_vision = 0x5dc;
    m_backVision = 0x5dc;
    m_noChangeTarget = true;
}