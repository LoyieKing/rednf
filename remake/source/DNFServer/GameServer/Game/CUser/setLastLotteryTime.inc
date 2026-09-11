void CUser::setLastLotteryTime(unsigned long t)
{
    // ORIG 0x82307cb-0x82307d1：this-先 uint 直写、无帧。b28d-solo-CUser AE：P211 同型律——
    //   ORIG LHS-first ⇒ 字段与形参 `unsigned long t` 同型；CMCAPManager.h:81 已改
    //   int→unsigned long（ia32 4B，偏移 0x548/sizeof 不变），探针 /tmp/probe_store.cpp 双形实证。
    m_mcap.m_lastLotteryTime = t;
}