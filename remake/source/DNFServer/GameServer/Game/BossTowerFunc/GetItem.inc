// ----------------------------------------------------------------------------
// ORIG T 0x08146994 _ZN6Global14CBossTowerFunc7GetItemEP5CUseribc
// getBossTower() 判空 → cMyTrace("bossTower not find : charac_no - %u") 恒返 1；
// 否则虚槽[10] handlePickupItem(user,item,flag,type)；非零 →
// PacketGuard: put_header(1,0x2e)+put_byte(1)+finalize(true)+user->Send。
// ----------------------------------------------------------------------------
// [b28d-D9] P227/N82：cMyTrace 临时链 %ebx 保活 + PacketGuard 槽位差（ORIG 0x08146994 sz 0x00000144）
bool Global::CBossTowerFunc::GetItem(CUser* user, int item, bool flag, char type)
{
    WongWork::CBossTower* tower = user->getBossTower();
    if (tower != 0)
    {
        const char result =
            (char)tower->handlePickupItem(user, item, flag, type);
        if (result != 0)
        {
            PacketGuard packet;
            packet.put_header(1, 0x2e);
            packet.put_byte(1);
            packet.finalize(true);
            user->Send(packet);
        }
    }
    else
    {
        // [P227/P221] ORIG 在 ctor call 前求 vararg（`call getCurCharacNo;
        // mov %eax,%ebx`）且 cMyTrace 槽浅于 PacketGuard ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xb7, 5)(
            "bossTower not find : charac_no - %u", user->getCurCharacNo());
    }
    return 1;
}