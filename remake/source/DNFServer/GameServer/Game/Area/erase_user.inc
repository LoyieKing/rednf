void Area::erase_user(CUser* user)
{
    // [b28c-21] ORIG 尾部 jmp+nop 中继岛（P223）：goto 尾空标签复现。
    if (m_areaType == 1)
        goto END;
    m_UsersInArea.erase(user->get_unique_id());
END:;
