void GameWorld::DeleteChannel(int gcNo, ENUM_SERVER_GROUP group)
{
    // N82/P112 封顶：指令序/call 序/EH 清理段逐条全等（78/78），残差全为
    // guard/buf/trace 槽序（4.1 声明正序 buf→guard→trace；4.4 实测声明逆序
    // 分配，trace 构造时机不可前移，buf 提前声明已试无收益）。帧 0x40/0x30
    // 同源。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x11bc),
                       true);
    **guard << 0x2f;
    **guard << -1;

    struct stChannelDeleteInfo
    {
        int m_gcNo;     // +0x00
        int m_group;    // +0x04
        int m_reserved; // +0x08  ORIG sizeof=0xc（memset 0xc / put_binary 0xc 证据），
                        //        该字段仅由 memset 清零，ORIG 无独立写入
    };
    stChannelDeleteInfo buf;
    memset(&buf, 0, 0xc);
    buf.m_gcNo = gcNo;
    buf.m_group = (int)group;

    guard->put_binary(&buf, 0xc);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

    // b28c-lane3：P221 全表达式临时链——具名 trace 使 4.4 把 trace 放最深槽
    // (-0x28)，临时链后 4.4 复现 ORIG 槽序 trace -0x18 / guard -0x20 / buf -0x2c。
    cMyTrace(__PRETTY_FUNCTION__, 0x11c7, 0)(
        "put db message to db thread DeleteChannel()");
