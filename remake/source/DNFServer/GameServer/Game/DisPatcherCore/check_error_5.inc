// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮：返回 unsigned（与 int 字段异型，
// 复现 process 调用点 RHS-first+edx 中转）；返回类型不入 mangled/ABI。
unsigned int DisPatcher_SetPartyInfo::check_error(CUser* /*user*/, MSG_BASE& msg)

{
    MSG_PARTY_SET_INFO& req = *reinterpret_cast<MSG_PARTY_SET_INFO*>(&msg);
    std::string title(reinterpret_cast<char*>(&req.m_title), std::allocator<char>());
    if (CSyncSlangFilter::GetInstance()->HasSlang(title))
    {
        return 0x9f;
    }
    return 0;
}
