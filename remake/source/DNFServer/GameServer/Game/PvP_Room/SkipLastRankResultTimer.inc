int PvP_Room::SkipLastRankResultTimer()
{
    // P31：ORIG 帧 0x18 vs gcc4.4 0x8，补 4 个未用局部对齐帧深（ORIG 0x085d9ba4）
    int d1;
    int d2;
    int d3;
    int d4;
    gen_timer_key((TIMER_MESSAGE)0x32);
}