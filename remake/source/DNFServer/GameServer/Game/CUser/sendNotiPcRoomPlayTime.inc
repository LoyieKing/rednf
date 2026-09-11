// ORIG _ZN5CUser22sendNotiPcRoomPlayTimeEv T 0x0868f83c：包 0x1a8（网吧时长通知）。
void CUser::sendNotiPcRoomPlayTime()
{
    PacketGuard pg;
    pg.put_header(0, 0x1a8);
    pg.put_int(getTotalPcRoomPlayTime());
    pg.finalize(true);
    Send(pg);
}