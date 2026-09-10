bool DBTask::HandleEvents(MsgQueueMgr::QUEUE_IDX queueIdx, int& signal)
{
    Stream* stream = GlobalData::s_msgq_mgr->get(queueIdx);
    if (stream == 0)
    {
        OS_API::uSleep(100u);
        return true;
    }

    m_curQueueIdx = static_cast<int>(queueIdx);
    const int length = stream->length();
    const int poolType = stream->m_poolType;      // Stream +0x20（Stream.h:73）

    DBProcess(stream, signal);

    if (signal == 0x3f)
    {
        // P227：ORIG 0x083ff24e 在 ctor 前 call get_queue_name 存 %ebx 保活。
        cMyTrace(__PRETTY_FUNCTION__, 0x8e6, 0)(
            "Terminate Server db queue: %s(%d)\n",
            MsgQueueMgr::get_queue_name(queueIdx), static_cast<int>(queueIdx));
        return false;
    }

    // [SweepIntervalSet] P251：ORIG 0x083ff29d `mov -0xc(%ebp),%eax; cmp $0x1,%eax;
    // je; cmp $0x2,%eax; je; test %eax,%eax; je; jmp`（值留寄存器、无槽直比）⇒
    // 源形为 switch(poolType)（if-else 链在 4.4 恒产 `cmpl $K,mem; jne`）。
    // 三处 trace 同时改 P227 全表达式临时链（ORIG 各在 ctor 前读 *signal/length）。
    switch (poolType)
    {
    case 1:                                       // SmallStreamPool
        if (length > 0xfff)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x8f2, 5)(
                "Small Stream Size over code(%d), size(%u)", signal,
                static_cast<unsigned int>(length));
        }
        GlobalData::s_small_stream_pool->Free(stream);        // ORIG 0x08290004
        break;
    case 2:                                       // BigStreamPool
        if (length > 0x3ffff)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x908, 5)(
                "Big Stream Size over code(%d), size(%u)", signal,
                static_cast<unsigned int>(length));
        }
        GlobalData::s_big_stream_pool->Free(stream);          // ORIG 0x08290304
        break;
    case 0:                                       // StreamPool
        if (length > 0x3fff)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x8fd, 5)(
                "Default Stream Size over code(%d), size(%u)", signal,
                static_cast<unsigned int>(length));
        }
        GlobalData::s_stream_pool->Free(stream);              // ORIG 0x0828fb28
        break;
    }
    return true;
