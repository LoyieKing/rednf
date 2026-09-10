void CHandlePremium::handleSetUserPCRoom(CUser* user, long start, long end)
{
    // [b28d-56 判词·N] NEAR(ndiff 8)：ORIG 0x086b02fe 179 条 vs 我们 179 条，
    // 助记符序列全等，仅两处多实参 call 首值 callee-saved 互换（%esi↔%ebx，
    // N82附「多实参 call 首值寄存器对调」源形不可择）。语义完成。
    // [b23S3C] P48 去缓存：无 remain 槽——ORIG 每使用点重取 (end-curSec)（7 条/处
    // ×5）；InsertTimer 实参全内联（4.1 右→左求值序 acc_id→diff→uid→G_TQ 实证，
    // 具名局部改序即偏差）；affected bool 槽 movb 直落（ORIG `mov %al,-0x1d` 无
    // 三目物化）+ `if (!affected)` 让 effect 块走 body、timer 块成 je 岛（P191）；
    // put_byte 双分支常实参复制（ORIG 两 call 序列，非 setne 汇聚）。
    bool affected = user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc);
    handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)0xc, start, end, 0, true);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    if ((int)(end - curSec) < 0x93a80 && 600 < (int)(end - curSec))
    {
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, (int)user->GetUID(),
                                    (TIMER_MESSAGE)7,
                                    (int)(end - curSec) - 600, 0xc,
                                    user->get_acc_id());
    }
    if (user->get_state() > 2)
    {
        handleNotifyPremiumInfo(user, 0xc, (int)(end - curSec));
        if (!affected)
        {
            handlePremiumEffect(user, (ENUM_PREMIUM_TYPE)0xc);
            user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
        }
        else
        {
            if ((int)(end - curSec) < 0x93a80 && 600 < (int)(end - curSec))
            {
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0,
                                            (int)user->GetUID(),
                                            (TIMER_MESSAGE)4,
                                            (int)(end - curSec), 0xc,
                                            user->get_acc_id());
            }
        }
    }
    PacketGuard packet;
    packet.put_header(0, 0x1aa);
    if (user->HasCurCharacRentalItem(1))
        packet.put_byte(1);
    else
        packet.put_byte(0);
    packet.finalize(true);
    user->Send(packet);
