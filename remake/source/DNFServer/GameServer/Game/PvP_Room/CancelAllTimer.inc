int PvP_Room::CancelAllTimer()
{
    // P31：ORIG 帧 0x18 vs gcc4.4 0x8，补 4 个未用局部对齐帧深（ORIG 0x085d6c42）
    int d1;
    int d2;
    int d3;
    int d4;
    gen_timer_key((TIMER_MESSAGE)0x32);
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}