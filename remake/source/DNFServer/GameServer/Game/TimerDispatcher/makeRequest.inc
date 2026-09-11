// ORIG T 0x0815ba94（_ZN26DBSelectCreateDnfEventInfo11makeRequestEv）
// 报告：docs/class_func_reports/DBSelectCreateDnfEventInfo/makeRequest.md：
// 事件未在进行中（或 receiver 空）时发 Taiwan DbThread 0x9 选择请求。
// [b28d-solo-TimerDispatcher2] P277 边界复核：ORIG 虚调用段 `movl $0x0,0x4(%esp)`
// （**立即数**实参）先于 `mov -0xc(%ebp),%eax`（this）。P277 探针的实参是复杂
// 表达式（引用解引用）才成立；具名派生指针形与点号形 `(*event).IsEventing(0)`
// 实测均 this-first（本件实参为立即数 0）⇒ 装载序不可翻，判 N7/P75 封顶（69/69）。
// [TimerDispatcher-3 2026-09-10 判据①] 全函数分支目标序列全等（无语义缺口）。
void DBSelectCreateDnfEventInfo::makeRequest()
{
    // [MiscLane 2026-09-11 翻正] 旧形把 GetRepeatEvent 结果 static_cast 成 CEventCreateDnf*
    // 后作虚调用接收者；探针 /tmp/lane_MiscLane/probe_vord2.cpp 实证：**带 static_cast 的
    // 接收者**使 4.4 把 this 装载提前到立即数实参之前（f6 this-first），而**基类指针直用**
    // （f7）产 ORIG 的 const-first（`movl $0x0,0x4(%esp)` 先、`mov -0xc(%ebp),%eax` 后，
    // ORIG 0x0815bac4/0x0815bacc 直证）。本函数只用 IsEventing（CEventBase vtable+0x34
    // 虚函数，CGuildServerProxy.h:145），无需派生类型 ⇒ 改用 CEventBase* 直用。
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0xa0);
    // [b28d-30] 真语义 bug 修正：ORIG 0815bab4..0815bae9 语义 = body 执行当且仅当
    // `event != 0 && IsEventing(0)==1`（Ghidra C：cVar2==1 → bVar1=false → !bVar1 进 body）。
    // 旧码 `event==0 || IsEventing(0)` 极性相反（IsEventing==0 才进 body）。修法 =
    // 早返形 `if (event==0 || !IsEventing(0)) return;`——探针 /tmp/probe30_f.cpp m1
    // 逐条复现 ORIG（je L_true / xor $1 / je L_false / test;jne 跳过 body 岛）。
    if (event == 0 || !event->IsEventing(0))
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(
            "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x1e4),
        true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(9), -1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}