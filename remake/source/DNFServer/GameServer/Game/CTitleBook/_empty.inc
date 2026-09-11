bool CTitleBook::_empty()
{
    for (int category = 0; category < 4; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            if (m_category[category][i].m_addInfo != 0)
            {
                return 0;
            }
        }
    }
    return 1;
}