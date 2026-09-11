int CTitleBook::putTitleOnly(CUser* user, int itemIdx)
{
    ENUM_TITLE_BOOK_CATEGORY category;
    int slot;
    return putTitleOnly(user, itemIdx, category, slot);
}