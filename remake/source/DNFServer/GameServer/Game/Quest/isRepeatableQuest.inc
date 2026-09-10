bool Quest::isRepeatableQuest() const
{
    switch (m_type)
    {
    case 4:
    case 8:
        return 1;
    default:
        return 0;
    }
