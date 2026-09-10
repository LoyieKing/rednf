int Dispatcher_RequestDisjointItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cfcac：check_error(三参) 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x2a1f)；err==0 → 按 msg 字段分解物品。
    stRequestDisjointItemParam& p = static_cast<stRequestDisjointItemParam&>(param);
    p.m_4 = check_error(user, msg, param);
    if (p.m_4 > 0)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        LineFunc(0x2a1f,
                 __PRETTY_FUNCTION__,
                 p.m_4, (unsigned int)user->get_acc_id());
        return 0;
    }

    // msg+0xe：ushort 对象 uid → find_from_world；msg+0x10：short 槽；
    // msg+0x12：char（有符号）背包类型。命令字 0xef，区域 0xffff。
    MSG_REQUEST_DISJOINT_ITEM& mref =
        reinterpret_cast<MSG_REQUEST_DISJOINT_ITEM&>(msg);
    unsigned short uid =
        *reinterpret_cast<const unsigned short*>(&mref.m_field0e);
    CUser* target = G_GameWorld()->find_from_world(uid);

    DisPatcher_DisJointItem::_disjoint(
        user,
        (int)mref.m_field10,
        (int)(signed char)*reinterpret_cast<const unsigned char*>(&mref.m_field12),
        (ENUM_CMDPACKET)0xef,
        target,
        0xffff);
    return 0;
