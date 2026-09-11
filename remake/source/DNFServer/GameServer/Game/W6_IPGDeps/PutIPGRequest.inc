// ORIG T 0x08104754 (0x95)：queued 且 workerCount!=0 → Guard(reqMutex) 入队，
// 返回 0；否则同步调 workers[0].ProcIPGSync 并透传其结果。
// [b28d-solo-W6_IPGDeps] verdict=AE（52=52）。修法：把 `Guard<Mutex> guard` 包进
// 内层复合块、`return 0` 移到 Guard 作用域外 ⇒ 4.4 复现 ORIG 0x081047c7 在
// Guard dtor 之后物化 `mov $0x0,%eax`（原形提前到 %ebx 缓存）。其余（Guard EH 段、
// Proc 尾）逐条同形。
SIPGData* CIPGMgr::PutIPGRequest(SIPGData* data, bool queued)
{
    // ORIG 块序：queued==0/count==0 的 Proc 块为 fall-through 共享出口
    //（Proc 段在函数尾，je 直跳汇入）——P63 共享出口形。
    if (queued != 0 && m_workerCount != 0)
    {
        // [b28d-solo-W6_IPGDeps] ORIG 0x081047c7 `mov $0x0,%eax` 在 Guard dtor
        // 之后物化（return 0 在 Guard 作用域外）；4.4 原形提前到 %ebx 缓存。
        {
            Guard<Mutex> guard(&m_reqMutex);
            m_reqQueue.push(data);
        }
        return 0;
    }
    return m_workers->ProcIPGSync(data);
}
