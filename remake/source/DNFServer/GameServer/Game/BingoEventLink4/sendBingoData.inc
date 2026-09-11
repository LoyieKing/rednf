// ---------------------------------------------------------------------------
// sendBingoData @ 0x0869134e：取 repeat 事件 0x6a（BingoEvent）当日棋盘号，
// 连同用户 bingo 数据（棋盘值 +0x00、连线数 +0x05）打包 0x1c2 发送。
void CUser::sendBingoData()
{
    int today = 0;
    BingoEvent* event = reinterpret_cast<BingoEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x6a));
    if (event != 0)
    {
        // ORIG +0x03b：getBingoSystem → boost::shared_ptr<CBingo>::operator->()（内部
        // pt()）→ getToday 链式内联。用 shared_ptr 的 operator-> 复现 pt() 调用，
        // 不裸转换 CBingo** 绕过。
        today = reinterpret_cast<boost::shared_ptr<CBingo>*>(
                    event->getBingoSystem())
                    ->operator->()
                    ->getToday();
    }

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1c2);
    packet.put_byte(static_cast<int>(static_cast<char>(today)));
    packet.put_int(static_cast<int>(
        static_cast<BingoData*>(getBingoData())->m_eventId));
    // ORIG (uint)*(byte*)(iVar2+5) movzbl 直读 +0x05；bool `?1:0` 会产 setcc 归一。
    packet.put_byte(static_cast<int>(
        static_cast<unsigned char>(
            static_cast<BingoData*>(getBingoData())->m_isWin)));
    packet.finalize(true);
    Send(packet);
}