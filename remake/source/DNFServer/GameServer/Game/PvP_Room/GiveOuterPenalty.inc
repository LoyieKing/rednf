// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::GiveOuterPenalty（原 TU 第 2355 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N95] ORIG 0x085dcaf2：对象槽 -0xb0 与局部 -0xc，4.4 整体深移 8（-0xb8/-0x14）；P167 CLEAR ⇒ N95 槽基封顶。
// [b28d-solo-PvP_Room] 本轮：P0 —— 去 `void* ev` 具名局部 + 裸函数指针虚调用改具型化
// `GetRepeatEvent(0x38)->IsEventing((CUser*)0)`（ORIG 值全程 %eax 无槽往返）⇒ 176→172；
// 残余 N95 槽基差 8（帧同 0xc0，P190 撑帧法不可上移）。
// [SweepPvPRoom] 172→176（ORIG 175）：P226 扩展合并短路 && —— ORIG 0x085dcba4..0x085dcbf0
// 是 `IsIntegratedPvPBaseChannel && IsEventing` 的零槽物化岛（5 条），旧形拆嵌套 if 缺该岛；
// 合并后槽基差 8→4（-0xb4 vs ORIG -0xb0）。残余 = 上述槽基差 + 1 条 `lea ...%ebx` 取址缓存
// 与 1 条 ORIG 尾 jmp（N95/P231 族）。
// [L10] 176→174（ORIG 175）：P52/P183 —— 删 `CStatisticServerProxy* proxy` 具名局部改
// 单语句链式（ORIG 0x085dcc8x 段 GetServerProxy 结果经 %eax 直传 SendPacket receiver，
// 无 -0xc store/reload）。实测差异行 129→66。**当前残余已收敛为两项**：
//  ① N104 EH 尾块相对序：ORIG 157..166 `jmp +0x274`（正常路径跳出）→ unwind 段
//     （`mov %edx,%ebx; mov %eax,%esi; lea -0xb0; call PvpResultType::D1Ev; ... ;
//     call _Unwind_Resume`）居中；4.4 把同一 epilogue 就地内联于正常路径
//     （`add $0xc0,%esp; pop %ebx/%esi/%ebp; ret`）而 unwind 压尾 ⇒ 块互换、内容逐条同。
//  ② N132 数据地址显示差：ORIG `mov 0x941f730,%eax`（s_event_manager）/`mov 0x940be30,%edx`
//     （s_statistic_proxy_mgr）vs .o `mov 0x0`（未重定位）。
// ⇒ 语义完成（p167 CLEAR、call 多重集相等），byte 层 N104/N132 封顶。
// [L10] p167 复验（2026-09-10）：差集仅 C1/C2、D1/D2 同址别名（P93 豁免）⇒ CLEAR。
void PvP_Room::GiveOuterPenalty(CUser* user)   // [b28c-16 SIGFIX] ORIG 串 void
{
    PvpResultType local_b4;
    local_b4.Clear();
    user->update_pvp_rank(local_b4);
    int local_10 = GetTeamIndex(user);
    if (m_userTable.IsValidTeam(local_10) != 1)
    {
        cMyTrace local_20(__PRETTY_FUNCTION__, 0x10f8, 5);
        local_20("Invalid Team(%d)", local_10);
        goto LAB_085dcd66;
    }
    // [SweepPvPRoom] P226 扩展（旧「外层 if + 内层语句表达式」判词作废）：ORIG
    // 0x085dcba4..0x085dcbf0 是**合并短路 &&** 的零槽物化岛 —— `call
    // IsIntegratedPvPBaseChannel; test %al; je <false>; … call *%edx (IsEventing);
    // test %al; je <false>; mov $1,%eax; jmp; <false>: mov $0,%eax; test %al; je
    // <else>`；拆成嵌套 if 缺 5 条物化岛。语句表达式包住整个 &&（内不含具名变量）。
    // P0：GetRepeatEvent 返回值全程 %eax（无槽往返），虚调用是 vptr+0x34 具型化。
    if (({ G_GameWorld()->IsIntegratedPvPBaseChannel() != 0
           && GlobalData::s_event_manager->GetRepeatEvent(0x38)->IsEventing((CUser*)0) != 0; }))
    {
        if ((int)(long)user->GetServerGroup() != 0)
        {
            Packet_Server_Match_data local_2d;
            local_2d.m_serverGroup = (char)(long)user->GetServerGroup();
            local_2d.m_lose = 1;
            // [L10] P52/P183 实测：ORIG 0x085dcc8x 段无 proxy 具名槽（GetServerProxy
            // 结果经 %eax 直传 SendPacket receiver，无 -0xc store/reload）⇒ 保链式。
            // 实测：具名 proxy 形 176 条/129 差异行 → 链式 174 条/66 差异行
            //（ORIG 175 条）。残余 = ORIG 0x085dcca0 `lea -0x29(%ebp),%ebx` 实参
            // 预物化（N18a/P52）与槽基差 4（N95）。
            GlobalData::s_statistic_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)(long)user->GetServerGroup())
                ->SendPacket((char*)&local_2d, 0xd);
        }
    }
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if ((G_GameWorld()->GetChannelType() == 4) ||
            (G_GameWorld()->GetChannelType() == 8))
        {
            user->update_pvp_point(m_userTable.GetLeavePenalty(local_10));
        }
        else if (G_GameWorld()->GetChannelType() == 6)
        {
            user->add_guild_pvp_result(-1);
            goto LAB_085dcd66;
        }
    }
    if (G_GameWorld()->IsWinPointPvPChannel() != 0)
    {
        user->add_pvp_result(false, 0);
    }
    ((IMatch*)m_matching)->OnGiveup(user);
LAB_085dcd66:
    ;
}
