void CSharedServerMessageManager::LoadServerMessageFromDB()
{
    // b28c-L20: AE — ndiff 30→0（fresh orig=68 our=68）。两处归因：
    // ①本 TU 局部 GlobalData 声明原为 object 形（`s_stream_pool.Acquire` / `s_msgq_mgr.put`
    //   生 `movl $&sym,(%esp)` 取址立即数），与权威头 GlobalData.h 的指针声明矛盾；ORIG
    //   +0x08 `mov _ZN11GlobalData13s_stream_poolE,%eax`、+0xb1 `mov …s_msgq_mgrE,%eax` 是
    //   32 位**值装载**作 this ⇒ 声明改指针 + 调用点 `->`（同 TU 的 UpdateServerMessage 一并
    //   受益：ndiff 395→385）。
    // ②P0：ORIG Acquire 返回值经 %eax 直落 CStreamGuard ctor 实参（+0x25 `movl $0x1` /
    //   +0x2d `mov %eax,0x4(%esp)`，无 Stream* 槽往返）⇒ 原码无具名 pStream 局部，改内联
    //   实参后 guard 槽回正 -0x14(%ebp)（原 -0x18）。
    // 消费侧 P132/P103（`**guard <<` 两级 operator*、`guard->GetInBuffer<T>()`）与
    // m_server=(short)m_field378（32 位读 + `movw` 写）、m_channel=(short)get_channel_no()
    // （`mov %eax,%edx; mov %dx,0x2(%eax)`）改前已同形，未动。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("SharedServerMessageManager.cpp", 0x21b),
                       true);
    **guard << 0x16d;
    **guard << -1;

    SIG_SELECT_SERVER_MESSAGE* pOut = guard->GetInBuffer<SIG_SELECT_SERVER_MESSAGE>();
    pOut->m_server = (short)G_CEnvironment()->m_field378;
    pOut->m_channel = (short)G_CEnvironment()->get_channel_no();

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}