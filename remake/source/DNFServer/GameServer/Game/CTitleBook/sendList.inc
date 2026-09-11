void CTitleBook::sendList()
{
    for (int category = 0; category < 4; ++category)
    {
        _sendCategoryData(m_user, 0, (ENUM_TITLE_BOOK_CATEGORY)category,
                          ENUM_TITLE_BOOK_INFO_TYPE_0);
    }
}