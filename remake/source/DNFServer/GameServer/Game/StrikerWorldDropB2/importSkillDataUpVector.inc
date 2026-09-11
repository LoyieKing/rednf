// ORIG T 0x08a9e4d6：clear this+0x40；顶测循环两段条件各自独立判退：
// ScanType(token,true)!=true → 退出；token!="[level]" → 退出；
// 循环体 { StrikerSkillDataUp 局部(0x14)；+0=ScanInt、+4=ScanInt；
// importSkillDataUp(&局部+8)；push_back }。for(;;)+break 形态与 ORIG 的
// 「条件在顶部、无前置 jmp、两个 break 各自跳出」布局同构。
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
}