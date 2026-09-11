// [b28d-lane2] verdict=DIFF ndiff=127（ORIG 0x084d1ab6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=117（ORIG 0x84d1ab6）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_SaveClientHackLog::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SaveClientHackLogPktView* pPkt = reinterpret_cast<Inter_SaveClientHackLogPktView*>(data);
    // §P0/P52：ORIG 084d1ac6 把 m_0 读入 %ebx 跨 G_GameWorld call（无槽）⇒ 不设具名 accId。
    CUser* target = G_CGameManager()->getUser(
        static_cast<int>(G_GameWorld()->find_session(pPkt->m_0)), 0x30cd);
    if (target == 0)
        return 0;

    if (!ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            RestrictCategory::Enum_3, 1))
        return 0;                                    // asm xor $0x1,test 取反提前返回

    // [2026-09-11] ORIG 084d1b35/-0x1c、084d1bce/-0x18、084d1c02/-0x14 三槽连续 ==
    //   Ghidra `int local_20[3]`（含 100 常量物化槽 -0x18）⇒ 源形是局部数组而非三个独立
    //   标量（旧 `int stdDropRate` 落 -0x20，与 ORIG -0x14 差 0xc；`penalty` 落 -0x18 差 4）。
    // [G00 2026-09-11 P-g03-b 反向试形 A1] ORIG 三处对同区分别发**单条 lea**
    //   （084d1bce `lea -0x18(%ebp),%eax`、084d1c02 `lea -0x14(%ebp),%eax`）⇒ 独立具名
    //   标量；数组形则恒生 `lea -0x1c(%ebp),%eax; add $0x4/%eax`（我方旧读数）。
    //   声明序按槽位高→低书写（dropRateRes 最高 -0x14）。
    int dropRateRes;
    int rate100;
    int acc;
    acc = 0;                                         // acc/asm -0x1c(%ebp)
    // ORIG 084d1b46..084d1b69：二分决策树（cmp $0x1f7 / jg / cmp $0x193 / cmp $0x326 /
    //   cmp $0x3ba）⇒ 原码是 switch；case 体物理序实测 0x3ba(084d1b71 cmp $0x4b0)、
    //   0x1f7(084d1b89 cmp $0x32)、0x193(084d1b9f cmp $0xfa)、0x326(084d1bb7 cmp $0x19)
    //   ⇒ gcc O0 按源码顺序布 case 体，按该序书写；m_8/m_e 均无槽往返。
    switch (pPkt->m_8)
    {
    case 0x3ba:
        if (pPkt->m_e > 0x4b0)
            acc += 0x14;
        break;
    case 0x1f7:
        if (pPkt->m_e > 0x32)
            acc += 0x28;
        break;
    case 0x193:
        if (pPkt->m_e > 0xfa)
            acc += 0x14;
        break;
    case 0x326:
        if (pPkt->m_e > 0x19)
            acc += 0x1e;
        break;
    }

    // [b23S3C] P48：ORIG 两次 getStdDropRate（local_20[2] 落槽供 min 取址；
    // 比较点重新 call 入寄存器 iVar5），勿具名缓存复用。
    // P183：ORIG 084d1bce movl $0x64,-0x18(%ebp) 后 min 返回值经 mov (%eax),%eax
    //   直接进减法（无 penalty 落槽回读）⇒ 原码是单表达式，不设中间具名。
    // [H2·2026-09-11 laneC P0 负结论·已回滚] ORIG 084d1bf4 `mov %eax,-0x1c(%ebp)` 复用 penalty
    //   槽（084d1b35 同址）看似同一变量；实测删 `int rate` 合形后 nd 28→**40**（n 仍 137）
    //   ⇒ 反向劣化（4.4 的 min 取址对 `penalty` 槽序另有约束）⇒ 回滚，判 N。
    rate100 = 100;
    acc = 100 - std::min(rate100, acc);
    dropRateRes = target->getStdDropRate();          // dropRateRes/asm -0x14(%ebp)
    acc = std::min(acc, dropRateRes);                // std::min(&acc,&dropRateRes)
    if (target->getStdDropRate() != acc)
    {
        DB_SaveUserHandicap::makeRequest(
            target->get_acc_id(), 0x9c41, 0x65, acc, 3, 0);
        target->setStdDropRate(acc);
    }
    return 0;
}