void TimerUpdateItemGenerateStatistics::registNextTimer(long pointSec)
{
    std::tm timePoint;
    localtime_r(&pointSec, &timePoint);
    timePoint.tm_hour = 0x17;
    timePoint.tm_min = 0;
    timePoint.tm_sec = 0;
    // [b28d-30] ORIG 0x086373xx：mktime 结果写回形参槽 0x8(%ebp)（`mov %eax,0x8(%ebp)`）
    // ⇒ 原码给形参 pointSec 赋值（值传递副本），4.4 直写形参槽复现（旧具名 base 局部
    // 落 -0x10 槽使 candidates 整体上移 8）。
    pointSec = mktime(&timePoint);
    // [b28d-30] ORIG 086373xx：两值落独立槽 -0x10/-0xc 且实参取址为单 lea（无 add）
    // ⇒ 原码是两个独立 int 局部（数组形 &arr[1] 4.4 恒折 lea+add，P30/N4）；
    // max 结果留 %ebx 跨 G_TimerQueue call（P231 纯 register）。
    // N93 例外（标量可翻）+ P197③：声明序定槽（candMin 先声明占 -0xc、candBase
    // 后声明占 -0x10），赋值语句序独立（ORIG 先算 candBase 再 candMin=1）⇒ 声明与
    // 赋值拆两句。
    int candMin;
    int candBase;
    candBase = static_cast<int>(pointSec - GlobalData::s_systemTime_.getCurSec());
    candMin = 1;
    register const int delay = std::max(candMin, candBase);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x6b, delay, 0, 0);
