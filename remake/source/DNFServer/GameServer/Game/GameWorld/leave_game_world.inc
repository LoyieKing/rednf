// [b28d-sweep] 298/296 → **298/294**：g_guildAgitMgr 对象化（原指针建模 ⇒ 调用点
// `mov g,%eax; mov %eax,(%esp)`；ORIG 0x086c55e5 `movl $0x943e0e0,(%esp)`）。
// 余 N7/N104：get_area 结果 %esi vs %ebx + EH 尾块相对序。
// [b28d-solo-GameWorld] _ZN9GameWorld16leave_game_worldEP5CUser verdict=DIFF 298/294
// 本轮修法：P50 声明点 = 发射点 —— 两个 iterator 声明移到 key（uniqueId/accId）
// 之前，复现 ORIG 0x086c539c 段 C1Ev 先于 get_unique_id/get_acc_id（旧序差 2 块）。
// 余 N7/N82：get_area 结果 %esi vs %ebx（ORIG 保 %esi 供 check_valid_area 第2实参）
// + 槽组平移；p167 CLEAR 54=54。
// 既有修法（保留）：P50 复活
// （两个 iterator C1Ev 先声明后赋值；p167 MISSING 实证）+ 具名 key + **真 try/catch
// 补缺**（ORIG 0x086c568f `__cxa_begin_catch` + 0x086c569e `strerror(errno)` +
// 0x086c56ce logFormat 0x5a7 "[EXCEPTION Leave GameWorld] - code = %s" +
// 0x086c56d8 `__cxa_end_catch`；原实现缺 try ⇒ p167 MISSING）。
// [H8B 2026-09-11 复核·终判] 298/294（少 4）+ p167 54/54（仅 PacketGuard C1/C2·D1/D2 与
// Rb_tree_iterator C1Ev/C2Ev 别名）⇒ 无真缺口。3 hunk：①槽组平移（PacketGuard -0x34 vs
// -0x38、find 结果/it -0x24/-0x20 vs -0x2c/-0x24、两个 key -0x1a/-0x3c vs -0x1e/-0x1c）；
// ②get_area 结果 ORIG 0x086c53xx `mov %eax,%esi`（保 %esi 供 check_valid_area 第 2 实参）
// vs 我方 %ebx（N7）；③EH 尾块：ORIG 0x086c5676 / 0x086c5681 两处各自 `lea -0x38;
// mov %eax,(%esp); call PacketGuard::D1Ev`，我方共享一处 `mov %eax,(%esp)`（P325 共享尾出口）
// ⇒ 少 2 条 lea + 相对块序差。try/catch 段逐条同形（__cxa_begin_catch/__errno_location/
// strerror/logFormat/__cxa_end_catch 全有）⇒ 判 N7/N82/N104。
bool GameWorld::leave_game_world(CUser* user)
{
    if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()))
    {
        user->SetGameMasterMode(false);
        GlobalData::s_GM_Manager->TurnGmMode(user->get_acc_id());
    }

    // P50 复活：ORIG 迭代器 C1Ev 在 key 求值之前发射（声明点即发射点）。
    std::map<unsigned short, CUser*>::iterator userIt;
    unsigned short uniqueId = (unsigned short)user->get_unique_id();
    userIt = m_UserInWorld.find(uniqueId);
    if (userIt == m_UserInWorld.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x56a,
                              "m_UsersInWorld.find(%d)",
                              (unsigned short)user->get_unique_id());
        return false;
    }

    // ORIG 0x086c539c 再次 get_unique_id 存 -0x1e 后 erase(&key)（非复用 uniqueId）
    m_UserInWorld.erase((unsigned short)user->get_unique_id());

    if (check_valid_area(user->getCurCharacVill(), user->get_area(false)))
    {
        Village* village = getUserVillage(user);
        if (village != NULL)
        {
            Area* area = village->getArea(user->get_area(false));
            if (area != NULL)
            {
                area->erase_user(user);
            }
        }
    }

    PacketGuard packet;
    packet.put_header(0, 6);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.finalize(true);
    send_all(packet);

    std::map<unsigned int, unsigned short>::iterator sessionIt;
    unsigned int accId = user->get_acc_id();
    sessionIt = m_sessionList.find(accId);
    if (sessionIt == m_sessionList.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x588,
                              "session_list.find(%s)",
                              NumberToString(user->get_acc_id(), 0));
        return false;
    }

    m_sessionList.erase(user->get_acc_id());
    // ORIG 0x086c568f..0x086c56d8 真 try/catch（[EXCEPTION Leave GameWorld]）。
    try
    {
        g_schoolMgr.DelUser(user);
        if (user->get_charac_guildkey() != 0)
        {
            g_guildAgitMgr.ReleaseGuildAgitArea(user->get_charac_guildkey());
        }
        if (user->isJoinPowerWar())
        {
            GlobalData::s_power_manager->DecreasePowerUserCount((char)user->getPowerSide());
        }
        if (user->isHangameUser())
        {
            m_hangameUserCount--;
        }
        else
        {
            m_userCount--;
        }
    }
    catch (...)
    {
        LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x5a7,
                              "[EXCEPTION Leave GameWorld] - code = %s",
                              strerror(errno));
        return false;
    }
    return true;
}