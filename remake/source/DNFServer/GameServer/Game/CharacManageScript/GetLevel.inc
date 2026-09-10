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
