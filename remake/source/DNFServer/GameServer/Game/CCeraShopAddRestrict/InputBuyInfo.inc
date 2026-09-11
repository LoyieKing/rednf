// ORIG 0x080e129a（报告 InputBuyInfo.md 全文转写）：
//   GetRestrictInfo 未命中 → 0；out.m_pInfo=info；m_type==0/1 时先在
//   m_field30/m_field48 中 find(ipgNo) 命中则 erase，再
//   InputBuyInfoDetail(...) 并把返回值写入 out.m_result；
//   尾判 out.m_result==0 → 0，否则 1；其它 m_type → 0。
// [b28c-4 档①] 返回类型 int→bool（Main 代落声明 CUser.h:1522）：ORIG 0x080e129a
// 出口仅 `mov $0x0/$0x1,%eax`，且唯一调用点 UpdateBuyableRestrictItem 0x080e0d14 为
// `call; test %al,%al; je` 单字节直判——int callee 在 4.4 恒多 setne 归一段
//（探针 /tmp/l4/al.cpp m1-m4 四形全败），唯 bool callee 复现。
// b28d-solo-CCeraShopAddRestrict: AE（121=121，仅 .o 重定位显示差；P211档③
// m_result 具型化 paramDaily* + P251/P261 switch）。
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
}