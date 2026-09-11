// ORIG 0x0815bee0 sz 0x153（_ZN25DBSelectCreateDnfUserInfo11makeRequestEij）：
// 先查 GlobalData::s_event_manager GetRepeatEvent(0xa0)，事件存在且其 vtable
// +0x34 虚槽（IsEventing 口径）返回真时跳过请求；Acquire(
// "localtaiwan/Dispatcher/Taiwan_DbThread.cpp",0x227) → CStreamGuard(stream,true)
// → Taiwan::internal_stream(guard,T(10),uid) → GetInBuffer<MSG>()，null 走
// cMyTrace("[Taiwan, Event] msg is null")，否则 *(uint*)msg(+0x00)=accId；
// MsgQueueMgr::put(DB_Q(2), guard)。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0815bee0
typedef unsigned int memberIdentificationNumber_t;
void DBSelectCreateDnfUserInfo::makeRequest(int userIdx, memberIdentificationNumber_t accId)
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
}