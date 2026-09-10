void CMailBoxHelper::SetPeriodCheckTimer(CUser* user)
{
    // ORIG 0x085523e2
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    long pad24 = 0x151800; // P31：ORIG 首槽死写（movl $0x151800,-0x24）
    long deadline = 0x151800;
    long nowSec = GlobalData::s_systemTime_.getCurSec();
    deadline = deadline + nowSec;
    long minPeriod = deadline;
    MailMap::iterator it = mail_map(box).begin();
    // 与 ORIG 槽阶梯（0x855242x：it@-0x3c / end() 临时@-0x34 / 转换 ctor @-0x40）
    // 对齐（P326 右值临时形）。
    MailMap::const_iterator endIt = MailMap::const_iterator(mail_map(box).end());
    for (; it != endIt; ++it)
    {
        // [本波 2026-09-09，isUnlimit 全链 bool 化后翻正] ORIG 0x08552486 条件为
        // `call; test %al,%al; jne <循环增量>`（2 条）= 正极 `if (f()) continue;`
        // （bool callee；旧 uchar 时代 continue 形曾 +2 条，bool 化后与 ORIG 同形）。
        if (it->second->isUnlimit())
            continue;
        long period = it->second->GetPeriod();
        minPeriod = std::min(minPeriod, period);
    }
    if (minPeriod != deadline)
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                    (TIMER_MESSAGE)8, (int)user->GetLoginTick(),
                                    (int)user->getCurCharacNo(), 0);
    }
