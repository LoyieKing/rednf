Inven_Item CTitleBook::_getTitleR(ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    // ORIG 0x8641db0：无 out 构造（POD 聚合返回=单 memmove 0x3d）；Inven_Item
    // 现含 union 位域后 out 声明会引入构造调用（新头基线实测）。
    return m_category[(int)category][slot];
}