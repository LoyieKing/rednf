int Dispatcher_CheckAssaultMotionHack::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08221f3c..0x0822226f (0x334 字节)。
    // 双精度常量：_DAT_08bd6320=0.5、_DAT_08bd6328=0.8（已 objdump 核实）。

    if (user == 0)
        return LineFunc(0xdb73, __PRETTY_FUNCTION__, 0, 0);

    if (user->getCurCharacR() == 0)
        return 0;

    unsigned int moveCnt = 0;    // -0x18
    unsigned int limitCnt = 0;   // -0x1c
    if (packet.get_int(moveCnt) != 1)
        return LineFunc(0xdb7e, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(limitCnt) != 1)
        return LineFunc(0xdb7f, __PRETTY_FUNCTION__, 0, 0);

    // Power War 事件开启且当前村庄 == 7 → power war 场景
    // [b28d-solo-S162] ORIG 0x0822204e：条件物化于 %al 后**分支写常量**
    // （`test %al,%al; je; movb $1,-0x11; jmp; movb $0,-0x11`）⇒ 原码是 if/else
    // 赋值；直接初始化 `bool x = expr;` 产单条 `mov %al,-0x11`（P194 变体）。
    bool inPowerWar;
    if (GlobalData::s_power_manager->IsPowerWarEventOn()
        && user->getCurCharacVill() == 7)
        inPowerWar = true;
    else
        inPowerWar = false;

    if (10000 < moveCnt)   // cmp $0x2710 / jbe
    {
        // [b28d-solo-S162] ORIG 0x0822206a/71 先 `movl $0,-0x10/-0xc` 预清再算
        // （P198① 两句式：初始化式会丢预清行）。
        unsigned int overCnt = 0;   // -0x10
        unsigned int overRem = 0;   // -0xc
        overCnt = moveCnt / 10000;
        overRem = moveCnt % 10000;
        if (inPowerWar)
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x7c, 1, overCnt, 0);
        else
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x7a, 1, overCnt, 0);
        moveCnt = overRem;
    }

    // 职业 == 3，或 ==4 且初生 growtype == 2 → 系数 0.5，否则 0.8（截断转换）
    // ORIG 每次测试重读 get_charac_job（P48 无 CSE）。
    // [b28d-solo-S162] P226/P0：ORIG 该条件物化于 %al（`test %al,%al`，无槽往返）
    // ⇒ 原码无具名 bool 局部，改条件表达式直用（具名版落 -0x12 并挤帧）。
    if ((user->get_charac_job() == 3) ||
        (user->get_charac_job() == 4 && user->getCurCharFirstGrowType() == 2))
        moveCnt = (unsigned int)((double)(unsigned long long)moveCnt * 0.5);
    else
        moveCnt = (unsigned int)((double)(unsigned long long)moveCnt * 0.8);

    if (limitCnt < moveCnt)
    {
        if (!inPowerWar)
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x79, 1, moveCnt, 0);
        else
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x7b, 1, moveCnt, 0);
    }
    return 0;
