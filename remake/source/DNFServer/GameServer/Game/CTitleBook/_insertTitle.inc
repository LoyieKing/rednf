void CTitleBook::_insertTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot,
                              Inven_Item* item)
{
    alter();
    m_category[(int)category][slot].setCopy(*item);
}