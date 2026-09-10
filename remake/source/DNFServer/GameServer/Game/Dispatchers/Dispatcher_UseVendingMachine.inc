Dispatcher_UseVendingMachine::Dispatcher_UseVendingMachine()
{
    // ORIG 0x0821c25c：基类 IPacketDispatcher + vtable + this+0x04 = new CMTRand(time(0))
    // （ORIG D1 0x082415f0 不 delete 该指针，忠实保留）
    unsigned long seed = time(0);
    // P52/register：ORIG 在 operator new **之前**把 &seed 物化进 %esi 跨调用缓存，
    // ctor 实参直接从 %esi 落 0x4(%esp)；4.4 默认在 new 后重算 lea(edx)。
    register unsigned long* pSeed = &seed;
    m_rand = new CMTRand(*pSeed);
