// ==== Dispatcher_MonstercardBind::Dispatcher_MonstercardBind (ORIG 0x08595348 W) ====
Dispatcher_MonstercardBind::Dispatcher_MonstercardBind()
{
    // ORIG：call IPacketDispatcher<MSG_MONSTERCARD_BIND,stMonstercardBind,
    // ch_state3>::IPacketDispatcher(); 置 vtable; time(0); new(0x9c8) 后
    // CMTRand(const unsigned long&) 种子构造，指针存 this+0x04。
    unsigned long seed = (unsigned long)time(0);
    register unsigned long* pSeed = &seed;   // ORIG：new 前 &seed 物化 %esi 跨调用
    m_rand = new CMTRand(*pSeed);
}