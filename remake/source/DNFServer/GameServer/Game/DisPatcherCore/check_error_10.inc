unsigned int DisPatcher_DeleteItem::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG：msg 指针先落 -0xc，call 后 reload + `mov 0x4bf(%edx),%edx`。
    const MSG_DELETE_ITEM* pMsg =
        reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);   // ORIG -0xc(%ebp)
    if (user->getCurCharacInvenCheckSum() != (*(&pMsg))->m_checkSum)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25b, 1u, 0u, 0u);
    }
    return 0;
