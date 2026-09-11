// ORIG 0x085887c8 T
// _ZN18online_preliminary25COnlinePreliminaryTeamMgr11OnEnterUserEP5CUserPc
//（报告 online_preliminary__COnlinePreliminaryTeamMgr/OnEnterUser.md）。
// 键语义：teamData 头部 int teamId —— find/make_pair 的 const int& 参数在
// ABI 上直接复用 teamData 指针解引用（ORIG 0x01a 传 edx=teamData 作引用）。
// [b28d-54 N] ORIG 0x085887c8：帧/槽整体平移（N82）+ map 迭代器临时槽（N86）；
//   调用序列全等。
bool online_preliminary::COnlinePreliminaryTeamMgr::OnEnterUser(CUser* user, char* teamData)
{
    // [P1T4 #284-303] ORIG 0x085887c8 序言 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`
    // （保存 char* 本体、单槽）+ find 处 `mov -0xc(%ebp),%edx`（不解引用即作
    // `const int&` 实参）⇒ ORIG 无 `int teamId` 值局部：teamId 的 4 个使用点全部写成
    // `*(int*)data` 解引用式。旧 `int teamId = *(int*)teamData` 生成
    // `mov (%eax),%eax; mov %eax,-0x28(%ebp)` 值槽，致 end 迭代器临时槽上移 4
    // （ORIG -0x28 vs 旧 -0x24）、make_pair/insert 临时槽整体上移 4。
    // 探针 /tmp/tid_probe.cpp f4 逐条命中：序言存指针、值参数解引用、引用参数直传
    // 槽值、`data+4` 走槽。语义等价（data 即 teamData 别名）。loadDataPreliminaryTeam
    // 与其它 ORIG 点仍走参数（ORIG 0x08588af `mov 0x10(%ebp),%edx`），故保持 teamData。
    char* data = teamData;
    std::map<int, COnlinePreliminaryTeam*>::iterator it =
        m_teamMap.find(*reinterpret_cast<int*>(data));
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
        m_teamMap.insert(std::make_pair(*reinterpret_cast<int*>(data), team));  // 0x10a..0x13b

        // 日志流：StreamPool::Acquire("OnlinePreliminaryTeam.cpp",0x106)，
        // 流序 << 0x1ec << -1 << teamId → MsgQueueMgr::put(DB_Q, guard)。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("OnlinePreliminaryTeam.cpp", 0x106), true);
        // [L6 round3 · p167 MISSING `_ZN12CStreamGuarddeEv`×3] ORIG 三个 << 各先调
        // `CStreamGuard::operator*()`（P132：`(*guard) << x` 代理链），成员直调 `guard<<x`
        // 不发 de() 调用点 ⇒ 显式 operator*()->operator<< 形。
        guard.operator*()->operator<<(0x1ec);
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<(*reinterpret_cast<int*>(data));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        Timer_UpdateOnlinePreliminaryTeam::RegistNextTimer(  // 0x0863977e
            *reinterpret_cast<int*>(data));
    }

    // 全服广播：header(0,0x118) | short(uniqueId&0xffff) | int(teamId)
    //          | int(strlen(data+4)) | str(队名) | finalize(true)。
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x118);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_int(*reinterpret_cast<int*>(data));
    packet.put_int(static_cast<int>(strlen(data + 4)));
    packet.put_str(data + 4, static_cast<int>(strlen(data + 4)));
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return true;
}