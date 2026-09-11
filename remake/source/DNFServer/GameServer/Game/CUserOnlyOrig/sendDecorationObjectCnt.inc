// ---------------------------------------------------------------------------
// CUser::sendDecorationObjectCnt(short count) T 0x08690d9a sz0x96
// 报告缺失（nm 0x08690d9a 定址后 objdump 忠实转写）：
// put_header(0,0x1ac)→put_short(count)→finalize(true)→Send。
// ---------------------------------------------------------------------------
void CUser::sendDecorationObjectCnt(short count)
{
    PacketGuard packet;
    packet.put_header(0, 0x1ac);
    packet.put_short(static_cast<int>(count));
    packet.finalize(true);
    Send(packet);
}