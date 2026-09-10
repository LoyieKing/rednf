void DBSelectCreateDnfUserInfo::makeRequest(int userIdx, unsigned int accId)
{
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0xa0);
    // ORIG 0x0842a3??：enabled 无槽——%eax 物化（`mov $1/$0` 双常数岛 +
    // vcall 后 `xor $1,%eax; test`）。真虚调 IsEventing(vtable+0x34) 复现
    // `mov (%eax),%edx … call` 形（手写 SlotFn 三中转指针 = 多 4 槽 = 帧
    // 外推 16B；§4.3 裸偏移改具名虚函数，repo 先例 CConditionEventManager/
    // CBossPlay）。
    // ORIG 0x0815bee0：早退条件 = event==0 || !IsEventing()（`cmpl $0,-0x10; je
    // <mov $1岛>`；vcall 后 `xor $1; test; je <mov $0岛>`；`test %al; jne 尾`）
    // ⇒ P226/P316 语句表达式零槽短路物化 + 正极性早返（P59/P214①）。
    if (({ event == 0 || !event->IsEventing((CUser*)0); }))
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x227),
                       true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)10, userIdx);
    MSG* msg = guard->GetInBuffer<MSG>();
    if (msg == 0)
    {
        // 技巧3：临时链式（ORIG guard=-0x28/trace=-0x20，具名块对象把
        // trace 外推到 -0x28 顶移 guard）。
        cMyTrace(__PRETTY_FUNCTION__, 0x22c, 0)("[Taiwan, Event] msg is null");
    }
    else
    {
        // ORIG：*(uint*)msg = accId（MSG +0x00 account id 字段）
        msg->m_id = accId;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
