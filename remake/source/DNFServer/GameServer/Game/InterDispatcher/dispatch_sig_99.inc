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

    int penalty = 0;                                 // local_20[0]/asm -0x1c(%ebp)
    // ORIG 084d1b46..084d1b69：二分决策树（cmp $0x1f7 / jg / cmp $0x193 / cmp $0x326 /
    //   cmp $0x3ba）⇒ 原码是 switch；case 体物理序实测 0x3ba(084d1b71 cmp $0x4b0)、
    //   0x1f7(084d1b89 cmp $0x32)、0x193(084d1b9f cmp $0xfa)、0x326(084d1bb7 cmp $0x19)
    //   ⇒ gcc O0 按源码顺序布 case 体，按该序书写；m_8/m_e 均无槽往返。
    switch (pPkt->m_8)
    {
    case 0x3ba:
        if (pPkt->m_e > 0x4b0)
            penalty += 0x14;
        break;
    case 0x1f7:
        if (pPkt->m_e > 0x32)
            penalty += 0x28;
        break;
    case 0x193:
        if (pPkt->m_e > 0xfa)
            penalty += 0x14;
        break;
    case 0x326:
        if (pPkt->m_e > 0x19)
            penalty += 0x1e;
        break;
    }

    // [b23S3C] P48：ORIG 两次 getStdDropRate（local_20[2] 落槽供 min 取址；
    // 比较点重新 call 入寄存器 iVar5），勿具名缓存复用。
    // P183：ORIG 084d1bce movl $0x64,-0x18(%ebp) 后 min 返回值经 mov (%eax),%eax
    //   直接进减法（无 penalty 落槽回读）⇒ 原码是单表达式，不设中间具名。
    int rate = 100 - std::min(100, penalty);
    int stdDropRate = target->getStdDropRate();      // local_20[2]
    rate = std::min(rate, stdDropRate);              // std::min(&local_20[0],&local_20[2])
    if (target->getStdDropRate() != rate)
    {
        DB_SaveUserHandicap::makeRequest(
            target->get_acc_id(), 0x9c41, 0x65, rate, 3, 0);
        target->setStdDropRate(rate);
    }
    return 0;
