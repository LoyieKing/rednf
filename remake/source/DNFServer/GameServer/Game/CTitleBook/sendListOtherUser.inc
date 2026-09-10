void CTitleBook::sendListOtherUser(CUser* user,
                                   ENUM_TITLE_BOOK_INFO_TYPE type)
{
    for (int category = 0; category < 4; ++category)
    {
        // P0/P200：ORIG `call get_unique_id; movzwl %ax,%eax; …` 返回值全程
        // 寄存器直传（无 unsigned short 局部槽），category 先入 %ebx 跨 call
        // 保活 ⇒ 表达式直传形。
        _sendCategoryData(user, (unsigned short)m_user->get_unique_id(),
                          (ENUM_TITLE_BOOK_CATEGORY)category, type);
    }
