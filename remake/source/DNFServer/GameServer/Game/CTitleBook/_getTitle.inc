Inven_Item* CTitleBook::_getTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    alter();
    return m_category[(int)category] + slot;
}