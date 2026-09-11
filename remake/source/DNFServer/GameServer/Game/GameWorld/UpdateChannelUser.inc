void GameWorld::UpdateChannelUser()
{
    // 删具名指针中转变量（技巧 36）：ORIG 0x086cb… 区是
    // `call StreamPool::Acquire; movl $0x1,0x8(%esp); mov %eax,0x4(%esp);
    // lea -0x14(%ebp),%eax; call CStreamGuard::C1` —— Acquire 结果 %eax 直落
    // arg2，无 spill 槽；我们多一个 `Stream* stream` 局部 ⇒ 4.4 插入
    // `mov %eax,-0x10(%ebp)` + `mov -0x10(%ebp),%eax` 两条，且该 4 字节把
    // 后续所有槽号（guard -0x14→-0x18、buf -0x4e→-0x52）整体推偏。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x1146), true);
    **guard << 0x2e;
    **guard << -1;

    // DB 0x2e 载荷（ORIG 0x086cb72a 逐指令偏移实证：+0x00 int ←env+0x198、
    // +0x04 word ←map size、+0x06..+0x31 11×int ←this+0x164、
    // +0x32 int ←this+0x194（hangame）、+0x36 int ←this+0x190）。
    struct __attribute__((packed)) stChannelUserCountInfo
    {
        int m_gcNo;                  // +0x00
        unsigned short m_worldUsers; // +0x04
        int m_channelUserCount[11];  // +0x06..+0x31
        int m_hangameUserCount;      // +0x32
        int m_userCount;             // +0x36
    };
    stChannelUserCountInfo buf;
    memset(&buf, 0, 0x3a);
    buf.m_gcNo = G_CEnvironment()->m_serverEnvir.m_gcNo;
    buf.m_worldUsers = (unsigned short)m_UserInWorld.size();
    for (int i = 0; i < 0xb; i++)
    {
        buf.m_channelUserCount[i] = m_channelUserCount[i];
    }
    buf.m_hangameUserCount = m_hangameUserCount;
    buf.m_userCount = m_userCount;

    guard->put_binary(&buf, 0x3a);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}