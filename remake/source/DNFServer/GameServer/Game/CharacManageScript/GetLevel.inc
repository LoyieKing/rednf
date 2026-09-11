// ---------------------------------------------------------------------------
// CharacManageScript::GetLevel —— ORIG T 0x088e0ed4
// point 为 0 返 0；自高向低找首个不高于 point 的门槛，等级取其下标加一，
// 最后 clamp 到 m_manageLevelMax。
// b28c-28：cursor 声明 int（非 unsigned）⇒ 4.4 复现 ORIG 实参求值序
// `mov slot,%eax; mov this,%edx; add $0xc,%edx`（探针 /tmp/l28_probe2.cpp fD/fE）。
// ---------------------------------------------------------------------------
int CharacManageScript::GetLevel(int point)
{
    if (point == 0)
        return 0;

    int level = 0;
    int cursor = (int)m_manageLevelPoint.size() - 1;
    while (cursor >= 0)
    {
        if (m_manageLevelPoint.operator[]((unsigned int)cursor) <= point)
        {
            level = (int)(cursor + 1);
            break;
        }
        --cursor;
    }

    if (m_manageLevelMax < level)
        return m_manageLevelMax;
    return level;
}