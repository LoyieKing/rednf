// ---- CIPGMgr::Initialize —— ORIG T 0x08104494：
//   workers 已存在 → -1；count!=0 时 new CIPGWorker[count]，逐个
//   SetIPGMgr(this)+ThreadInterface::begin()；写 param2@+8、count@+4；
//   InitBillingCash(billingCash) 失败 → logFormat("IPGMgr.cpp",0x98) 后 -1。----
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x08104494 orig/ours=116/121
//   p167 CLEAR（8/8）⇒ 语义完成；残差为槽/寄存器分配跨版本差（new[] 展开 + EH 段）。
// [P2S6 2026-09-11 N82 语义轴] ORIG +0x0d0..+0x100 循环体逐条：
//   `mov 0x8(%ebp),%eax; mov (%eax),%eax; mov %eax,(%esp); call SetIPGMgr` /
//   `mov 0x8(%ebp),%eax; mov (%eax),%eax; mov %eax,(%esp); call ThreadInterface::begin`
//   —— **两次都重读 this->m_workers（+0），全程无 i*16 索引计算**（我方原
//   `m_workers[i]` 生成 `mov -0x1c(%ebp),%edx; shl $0x4,%edx; lea (%eax,%edx,1),%edx`
//   ⇒ 每块 +3 条，共 gap=-6）。ORIG 语义 = 循环 workerCount 次、每次都作用于
//   m_workers[0]（原版行为，非"逐个"）。
int CIPGMgr::Initialize(unsigned int workerCount, unsigned int param2)
{
    if (m_workers != 0)
    {
        return -1;
    }
    if (workerCount != 0)
    {
        m_workers = new CIPGWorker[workerCount];
        for (unsigned int i = 0; i < workerCount; ++i)
        {
            m_workers[0].SetIPGMgr(this);
            m_workers[0].begin();
        }
    }
    m_param2 = param2;
    m_workerCount = workerCount;

    if (!InitBillingCash(m_billingCash))
    {
        LogManager::logFormat(
            1, "IPGMgr.cpp",
            __PRETTY_FUNCTION__,
            0x98, "Global billing cash initialize failed.");
        return -1;
    }
    return 0;
}