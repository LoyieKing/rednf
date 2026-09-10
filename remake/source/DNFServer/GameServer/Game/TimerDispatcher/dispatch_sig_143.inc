int Timer_NoticeEmergencyPatchMessage::dispatch_sig(int /*p1*/, int /*p2*/,
                                                    unsigned int /*p3*/)
{
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    // ORIG 0863bxxx：guard ctor + 4 次 put 之后才 rep stos 清 0x400 字节 ⇒
    // 数组声明落在 put 段之后（旧形前置致缓冲/guard 槽序互换 + 帧差）。
    char local_43c[0x400] = {0};        // ORIG rep stos ecx=0x100（dword）→ 清 0x400 字节
    // P0：ORIG 无 notice 槽（`call GetNoticeMessage; mov %eax,0xc(%esp)` 直传）⇒ 内联。
    sprintf(local_43c, "%d분뒤 서버점검이 있습니다. %s", m_remainingNotiCount,
            GameserverDemonManagerInstance()->GetNoticeMessage());
    unsigned int len = strlen(local_43c);
    packet.put_int((int)len);
    packet.put_str(local_43c, (int)len);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    m_remainingNotiCount -= 1;
    if (m_remainingNotiCount == 0)
    {
        GetInstanceShutdowManager()->SendShutdownSignal();
        cMyTrace(__PRETTY_FUNCTION__,
                 0x1849, 0)("Emergency Patch Message Received");
        // ORIG 0863bxxx：两分支各发 `mov $1,%ebx; jmp <共享尾>`（独立 return 1）。
        return 1;
    }
    RegistNextTimer();
    return 1;
