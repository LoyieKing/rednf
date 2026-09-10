void StrikerScript::importSkillDataUpVector()
{
    std::string token;
    m_skillDataUp.clear();
    for (;;)
    {
        if (!ScanType(token, true))          // ORIG xor $1/test/jne 反相形
            break;
        if (token == "[level]")              // ORIG test/je 直测形
        {
            StrikerSkillDataUp up;               // ORIG C1 W 0x08a9ed4a
            up.m_level = ScanInt((bool*)0);      // 局部 -0x20
            up.m_field04 = ScanInt((bool*)0);    // 局部 -0x1c
            importSkillDataUp(up.m_dataUps);     // 参数=&局部+8
            m_skillDataUp.push_back(up);         // vector@this+0x40
        }
        else
        {
            break;
        }
    }
