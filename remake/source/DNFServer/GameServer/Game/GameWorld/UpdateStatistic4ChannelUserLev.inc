// [b28d-solo-GameWorld] _ZN9GameWorld30UpdateStatistic4ChannelUserLevEv verdict=AE
// b28d-solo 修法（两处真修）：①P13 —— ORIG 0x086cd18b 先调 end()（last=-0x1c）、
// 0x086cd1a6 后调 begin()（it=-0x20），条件块 0x086cd20d 直接用 last 比较（无每轮
// end call）⇒ 原码缓存 end 迭代器（旧实现每轮 `it != m_UserInWorld.end()`；p167
// 多重集同计数故 CLEAR 掩盖了位置差）；②P0 —— `int level=get_charac_level();
// sig->m_count[level]++` 具名版多 `mov %eax,槽/mov 槽,%eax` 两条，ORIG 0x086cd1ec
// call 结果 %eax 直作 scaled 索引 ⇒ 内联表达式 `sig->m_count[get_charac_level()]++`。
// 余 N82/N93 槽组整体平移（guard/it/sig/info，帧 0x30 vs 0x40）不再试。
void GameWorld::UpdateStatistic4ChannelUserLev()
{
    // [b28d-solo-GameWorld] P259：Acquire 内联进 CStreamGuard 实参（消 stream 具名局部，
    // ORIG 0x086cd0b0 段无 stream 槽、帧 0x30 vs 具名形 0x50）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x14ab), true);
    **guard << 0x6b;
    **guard << -1;
    SIG_UPDATE_STATISTIC4LEV* sig =
        guard.operator->()->GetInBuffer<SIG_UPDATE_STATISTIC4LEV>();
    memset(sig, 0, 0x124);
    sig->m_gcNo = G_CEnvironment()->m_serverEnvir.m_channelNo;   // +0x1b0（0x086cd158 movl）
    sig->m_time = GlobalData::s_systemTime_.getCurSec();

    // [b28d-solo-GameWorld] P50：ORIG begin sret 直落 -0x20（无 iterator::C1Ev + 拷贝）
    // ⇒ for-init 内直构，勿先声明后赋值。
    // b28d-solo 修：P13 —— ORIG 0x086cd18b 先调 end()（last=-0x1c）、0x086cd1a6 后调
    // begin()（it=-0x20），条件块 0x086cd20d 直接用 last 与 it 比较（无每轮 end call）
    // ⇒ 原码缓存 end 迭代器（旧实现每轮调 end，p167 多重集同计数故 CLEAR 掩盖）。
    std::map<unsigned short, CUser*>::iterator last = m_UserInWorld.end();
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != last; ++it)
    {
        CUserCharacInfo* info = it->second;
        if (info != NULL && info->getCurCharacR() != NULL)
        {
            // b28d-solo：ORIG 0x086cd1ec-0x086cd1fe 无 level 槽往返（call 结果 %eax
            // 直作 scaled 索引）⇒ P0 内联表达式（旧具名版多 mov %eax,槽/mov 槽,%eax 两条）。
            sig->m_count[info->get_charac_level()]++;
        }
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}