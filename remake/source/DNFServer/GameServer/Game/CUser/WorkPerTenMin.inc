// ORIG _ZN5CUser13WorkPerTenMinEv T 0x08653196：无角色时 trace；有激活角色
// 则向监视代理上报角色在线（characNo）。
void CUser::WorkPerTenMin()
{
    if (m_chState <= 2)
    {
        return;
    }
    if (getCurCharacR() == 0)
    {
        // ORIG 0x86531f4-：NumberToString 两 call 先执行、结果暂 %ebx，cMyTrace
        // 临时构造成其后（0x8653215 mov %eax,%ebx 证据）→ 临时链式（技巧 3）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1d71, 5)(
            "CUser::WorkPerTenMin() m_id(%s) character - m_selected is null",
            NumberToString(get_acc_id(), 0));
        return;
    }
    if (get_charac_memberkey() != 0)
    {
        GlobalData::s_monitor_proxy_mgr
            ->GetServerProxy(GetServerGroup())
            ->SendNoticeCharLive(getCurCharacNo());
    }
}