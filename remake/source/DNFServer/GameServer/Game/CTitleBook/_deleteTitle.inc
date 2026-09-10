void CTitleBook::_deleteTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    alter();
    m_category[(int)category][slot].reset();
