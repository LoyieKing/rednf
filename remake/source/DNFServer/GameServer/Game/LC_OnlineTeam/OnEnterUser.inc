bool online_preliminary::COnlinePreliminaryTeamMgr::OnEnterUser(CUser* user, char* teamData)
{
    int teamId = *reinterpret_cast<int*>(teamData);
    std::map<int, COnlinePreliminaryTeam*>::iterator it = m_teamMap.find(teamId);
    // [L6 round3 · p167 + Ghidra 证据] ORIG 0x0858881a 用
    // `_Rb_tree_iterator::ne`（!=）+ `test %al,%al; je <+0x93>`（Ghidra：
    // `cVar2 = operator!=; if (cVar2 == '\0') { Acquire 新建分支 }`）⇒ 原码是
    // **`if (it != end) { 已存在分支 } else { 新建分支 }`**（块序 = 已存在在前）。
    // 旧 `if (it == end) { 新建 } else { 已存在 }` 产 `eq` + 反向块序。
    if (it != m_teamMap.end())
    {
        user->SetCurCharacOnlinePreliminaryTeam(it->second);   // 0x073
        it->second->IncreaseCurrentUserCount();                // 0x08588d96
    }
    else
    {
        COnlinePreliminaryTeam* team = m_factory->Acquire();   // 0x08588dd8（this+0）
        if (team == 0)
            return false;                                      // ORIG ebx=0 返回
        team->Reset();                                         // 0x0858838a
        user->SetCurCharacOnlinePreliminaryTeam(team);         // 0x08588dba（this=user）
        team->SetCurrentUserCount(1);                          // 0x08588d88
        team->loadDataPreliminaryTeam(teamData);               // 0x085882b8
        m_teamMap.insert(std::make_pair(teamId, team));        // 0x10a..0x13b

        // 日志流：StreamPool::Acquire("OnlinePreliminaryTeam.cpp",0x106)，
        // 流序 << 0x1ec << -1 << teamId → MsgQueueMgr::put(DB_Q, guard)。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("OnlinePreliminaryTeam.cpp", 0x106), true);
        // [L6 round3 · p167 MISSING `_ZN12CStreamGuarddeEv`×3] ORIG 三个 << 各先调
        // `CStreamGuard::operator*()`（P132：`(*guard) << x` 代理链），成员直调 `guard<<x`
        // 不发 de() 调用点 ⇒ 显式 operator*()->operator<< 形。
        guard.operator*()->operator<<(0x1ec);
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<(teamId);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        Timer_UpdateOnlinePreliminaryTeam::RegistNextTimer(teamId);  // 0x0863977e
    }

    // 全服广播：header(0,0x118) | short(uniqueId&0xffff) | int(teamId)
    //          | int(strlen(teamData+4)) | str(队名) | finalize(true)。
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x118);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_int(teamId);
    packet.put_int(static_cast<int>(strlen(teamData + 4)));
    packet.put_str(teamData + 4, static_cast<int>(strlen(teamData + 4)));
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return true;
