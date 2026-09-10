int Dispatcher_ItemDictionary::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stItemDictionary* p = static_cast<stItemDictionary*>(&param);
    p->m_4 = check_error(user, msg);
    if (p->m_4 > 0)
        return 0;
    if (p->m_4 < 0)
        return LineFunc(0x4e52,
                        __PRETTY_FUNCTION__,
                        p->m_4, 0);
    MSG_BASE* pm = &msg;
    (void)pm;
    return 0;
