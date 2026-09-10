CraneMinigameScript::~CraneMinigameScript()
{
    std::vector<CraneItemInfo*>::iterator it = m_items.begin();
    std::vector<CraneItemInfo*>::iterator end = m_items.end();
    for (; it != end; ++it)
    {
        if (*it != 0)
            delete *it;
        *it = 0;
    }
    m_items.clear();
