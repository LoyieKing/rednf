bool DBTask::DBProcess(Stream* stream, int& signal)
{
    int msgId = 0;
    // 技巧18 反向：ORIG 两处共用单 bool 槽 -0xd（lifetime 合并）⇒
    // 单具名变量跨段复用（双 bool = 4.4 恒分两槽不可合并）。
    bool ok;
    ok = (*stream >> msgId);                      // ORIG Stream::rsERi
    signal = msgId;
    if (!ok)
    {
        // 技巧3：临时链式（ORIG 0x08443…? trace1=-0x30 介于 msgId(-0x34)
        // 与后续槽之间；具名块对象使 4.4 槽外推 20B+）。
        cMyTrace(__PRETTY_FUNCTION__, 0x94d, 5)("[%s][%d]",
                                                __PRETTY_FUNCTION__, 0x94d);
        return false;
    }

    int userIdx = 0;
    ok = (*stream >> userIdx);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x95f, 5)("[%s][%d]",
                                                __PRETTY_FUNCTION__, 0x95f);
        return false;
    }

    // ORIG 0x83ff4f1..0x83ff533：`test %eax,%eax; jns <body>` 与
    // `cmp $0x353,%eax; jle <body>` 两跳同落主体、失败块居中 ⇒ 原码是
    // **&& 失败链** `if (msgId < 0 && msgId > 851) { fail; return false; }`
    // （探针 /tmp/l4_probe_dbp.cpp formB/formC 逐条复现；|| 正极性包裹
    // formA 反产 `jns body; jg fail` 块序 DIFF）。负值经有符号 jle 亦落
    // 主体 ⇒ ORIG 的 "Invalid signal received" 实为死代码，P171 照抄。
    if (msgId < 0 && msgId > 851)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0x967,
                              "Invalid signal received");
        return false;
    }

    DBDispatcher* dispatcher = m_dispatchers[msgId];
    if (dispatcher == 0)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0x970,
                              "Can't find the db dispatcher : signal(%d)", msgId);
        return false;
    }

    if (m_curQueueIdx == MsgQueueMgr::DB_Q)
    {
        GlobalData::s_db_mgr->setUserIdx(userIdx);
    }
    dispatcher->dispatch(msgId, userIdx, stream); // ORIG vtable slot0 (this,msgId,userIdx,stream)
    if (m_curQueueIdx == MsgQueueMgr::DB_Q)
    {
        GlobalData::s_db_mgr->setUserIdx(-1);
    }
    return true;
