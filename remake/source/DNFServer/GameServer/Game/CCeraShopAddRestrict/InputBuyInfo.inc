bool Manager::InputBuyInfo(CUser* user, unsigned int ipgNo, unsigned int itemIdx,
                           DBInputData& out)
{
    InfoDaily* info = m_loader.GetRestrictInfo(ipgNo);
    if (info == 0)
        return false;
    out.m_pInfo = info;
    UserInfo& ui = user->m_ceraUserInfo;
    // b28d-45 P251/P261：ORIG 0x080e12d0 为无槽 cmp 链
    // `test %eax,%eax; je case0; cmp $0x1,%eax; je case1; jmp default` ⇒ switch。
    switch (info->m_type) {
    case 0: {
        std::map<unsigned int, unsigned int>::iterator it = ui.m_field30.find(ipgNo);
        if (it != ui.m_field30.end())
            ui.m_field30.erase(it);
        // [b28d-solo P211档③] m_result 具型化为 paramDaily*（与 callee 返回同型）⇒
        // ORIG 0x080e1375 `mov 0x18(%ebp),%edx; mov %eax,(%edx)` LHS-first 直存；
        // 原 int 字段异型恒产 `mov %eax,%edx; mov 0x18(%ebp),%eax; mov %edx,(%eax)`。
        out.m_result = InputBuyInfoDetail(ui.m_accountDaily, info,
                                          ipgNo, itemIdx);
        break;
    }
    case 1: {
        std::map<unsigned int, unsigned int>::iterator it = ui.m_field48.find(ipgNo);
        if (it != ui.m_field48.end())
            ui.m_field48.erase(it);
        out.m_result = InputBuyInfoDetail(ui.m_characDaily, info,
                                          ipgNo, itemIdx);
        break;
    }
    default:
        return false;
    }
    if (out.m_result == 0)
        return false;
    return true;
