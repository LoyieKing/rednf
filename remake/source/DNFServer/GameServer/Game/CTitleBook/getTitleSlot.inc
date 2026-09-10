Inven_Item* CTitleBook::getTitleSlot(ENUM_TITLE_BOOK_CATEGORY category,
                                     int slot)
{
    alter();
    return m_category[(int)category] + slot;
