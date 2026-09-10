void STAIInfoScript::destroy()
{
    // ORIG 0x0836753a 逐块转写：外层 p≤4（setle）；内层每轮重取 size()
    // （setg 有符号比较）；body 内 ix() 三次独立装载（P48 无 CSE）：
    // 判空 → 取值 → 置 0；死块重复判空照抄（P184）。
    for (int p = 0; p <= 4; ++p)
    {
        for (int i = 0; i < (int)m_patterns[p].size(); ++i)
        {
            if (m_patterns[p][i] != 0)
                delete m_patterns[p][i];   // ORIG 0x083675a1 第 2 次 ix() 直读实参
            m_patterns[p][i] = 0;
        }
        m_patterns[p].clear();
    }
