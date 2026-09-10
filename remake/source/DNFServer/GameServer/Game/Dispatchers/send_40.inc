void Dispatcher_PvpMissionHPPercent::send(CUser* user, ParamBase& param)
{
    // ORIG：&param 基址入槽 -0x10，每次用点 `mov -0x10,%eax` + `lea/add $0x4` 现址；
    // 结构字段视图（技巧14/32）复现 lea 折叠形。
    struct HPParamView { int m_pad0; std::vector<int> m_vals; };
    HPParamView* p = reinterpret_cast<HPParamView*>(&param);
    PacketGuard guard;
    guard.put_header(0, 0x141);
    guard.put_byte((int)p->m_vals.size());
    for (unsigned int i = 0; i < p->m_vals.size(); ++i)
    {
        guard.put_short(p->m_vals[i]);
    }
    guard.finalize(true);
    user->Send(guard);
