// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x08635xxx 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82/N93 封顶：帧/槽位跨版本差（106/109）。
bool TimerCheckMailTimeOut::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x08635d0f：单出口 result(%ebx) + 四段判错，各 setXX %al 物化
    // （0x08635d9x `cmp $0x1,%eax; setle` / `cmp 0x10,%eax; setne` / `cmp 0x14,%eax; setne`）
    // → `test %al,%al; je` 前跳；result=1 在 PacketGuard dtor(0x08635e64) 之前。
    // [b28d-solo-TimerDispatcher] P231 纯 register：ORIG result 驻 %ebx 跨全部 call
    // 无栈槽；P0：ORIG 无 bad 槽 ⇒ 条件内联（旧具名 bool 生 mov %al,-0x11; cmpb）。
    // [P7HeaderAxis 2026-09-11] bool 化：ORIG 整族 dispatch_sig 返回 bool（prett 直证
    // `virtual bool X::dispatch_sig(int, int, memberIdentificationNumber_t)`）；ORIG
    // 0x08635d4d/0x08635e5f `mov $0x0/$0x1,%ebx` 产 result、0x08635e8c `mov %ebx,%eax`
    // 直返、**无** test/setne 物化 ⇒ result 本体即 bool。
    register bool result;
    CUser* user = G_CGameManager()->getUser(p1, 7);   // ORIG 第二实参为常量 7
    if (user == 0) {
        result = 0;
    } else if (user->get_state() <= 1) {
        result = 0;
    } else if (user->GetLoginTick() != p2) {
        result = 0;
    } else if (static_cast<unsigned int>(user->getCurCharacNo()) != p3) {
        result = 0;
    } else {
        PacketGuard packet;
        packet.put_header(0, 0x62);
        int countIndex = packet.get_index();
        packet.put_int(0);
        int expired = static_cast<int>(WongWork::CMailBoxHelper::CheckMailTimeout(user, &packet));
        if (expired != 0)
        {
            packet.put_int(countIndex, expired);
            packet.finalize(true);
            user->Send(packet);
        }
        WongWork::CMailBoxHelper::SetPeriodCheckTimer(user);
        result = 1;
    }
    return result;
}