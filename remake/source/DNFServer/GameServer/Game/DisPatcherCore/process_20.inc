int DisPatcher_SortItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG +0x06：check_error 最先（err→-0x14），msg(-0x10)/param(-0xc) 视图
    // 绑定发生在错误分支之后 —— 声明序决定 O0 槽深。
    const int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    const MSG_SORT_ITEM& req = *reinterpret_cast<const MSG_SORT_ITEM*>(&msg);
    stOnSortItemParam& out = static_cast<stOnSortItemParam&>(param);

    out.m_invenType = req.m_invenType;
    out.m_result = 0;
    switch (out.m_invenType)
    {
    case 0:
        out.m_result = user->getCurCharacInvenW()->sort((INVEN_TYPE)1);
        break;
    case 1:
        out.m_result = user->getCurCharacInvenW()->sort((INVEN_TYPE)2);
        break;
    case 2:
        user->getCurCharacCargoW()->sort();
        break;
    case 7:
        out.m_result = user->getCurCharacInvenW()->sort((INVEN_TYPE)3);
        break;
    default:
        break;
    }
    return 0;
