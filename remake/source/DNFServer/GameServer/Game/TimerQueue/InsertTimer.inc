// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _ZN10TimerQueue11InsertTimerER10TimerEntry
// ⇒ 该重载定义须为本 TU 首个强符号（此前 TimerEntry::TimerEntry 定义在前 ⇒ 键名错为
// _GLOBAL__I__ZN10TimerEntryC2Ev）。纯前置函数定义，符号集合/构造序零变化。
void TimerQueue::InsertTimer(TimerEntry& entry) throw()
{
    // b28d-solo-TimerQueue 判词：AE（94=94 逐条，P167 CLEAR）。两处关键：
    // ① `int msg;` 声明前置到 Guard 之前、赋值留在 push 之后 —— 4.4 栈槽按声明序
    //    「先声明浅」（P212/N98），据此把 ORIG 的 Guard(-0x34 深)/msg(-0x30 浅) 翻正；
    // ② ORIG 的 -0x10/-0xc 两槽不是具名 one/msg2，而是 `std::make_pair((int)entry.msg, 1)`
    //    右值引用参数的临时物化 —— 具名左值版选错 make_pair<int&,int&> 实例（ORIG 为
    //    make_pair<int,int>），直接实参形调用点差集归零且逐条 AE（P167 判据②）。
    // 残余具名 mp/cp 形（ndiff 38）与 hoist 形（32）均劣。@0x8630cec
    int msg;
    Guard<Mutex> guard(&m_mutex);
    m_queue.push(entry);

    msg = entry.msg;
    std::map<int, int>::iterator it = m_map.find(msg);
    if (it != m_map.end())
    {
        it->second += 1;
    }
    else
    {
        m_map.insert(std::make_pair((int)entry.msg, 1));
    }
}