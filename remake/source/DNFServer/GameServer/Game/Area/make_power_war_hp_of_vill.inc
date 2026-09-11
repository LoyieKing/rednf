// ---------------------------------------------------------------------------
// make_power_war_hp_of_vill：收集在线用户 PowerWar HP 到包并计数
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 10=10）。修法：while+continue 真循环形 +
// 主流程尾序照 ORIG put_short → it++ → ++count（086c2ccc ppEi 先于 086c2ce9 ++count）。
void Area::make_power_war_hp_of_vill(PacketGuard& packet, int& count)
{
    // [b28d-solo-Area] while+continue 真循环形（ORIG 086c2c1e）：user 预清 +
    // P50 先声明后赋值 + P28 后置 it++；ppEi/count++ 段槽号与顺序已对齐。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UsersInArea.begin();
    while (it != m_UsersInArea.end())
    {
        user = it->second;
        if (user == 0 || user->get_state() <= 2)
        {
            it++;
            continue;
        }
        // [b28d-solo-Area] ORIG 086c2ccc 先 it++（ppEi -0x10）再 086c2ce9 ++count。
        packet.put_short(user->getPowerWarHP());
        it++;
        ++count;
    }
}