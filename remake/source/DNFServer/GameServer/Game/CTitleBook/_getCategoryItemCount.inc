int CTitleBook::_getCategoryItemCount(ENUM_TITLE_BOOK_CATEGORY category)
{
    int count = 0;
    for (int i = 0; i < TITLE_BOOK_MAX[(int)category]; ++i)
    {
        if (m_category[(int)category][i].m_addInfo != 0)
        {
            ++count;
        }
    }
    return count;
}