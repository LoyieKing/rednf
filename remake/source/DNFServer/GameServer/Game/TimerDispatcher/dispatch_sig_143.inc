// ORIG T 0x0863b6c4 sz 0x1ba（_ZN33Timer_NoticeEmergencyPatchMessage12dispatch_sigEiij）
// 报告 docs/class_func_reports/Timer_NoticeEmergencyPatchMessage/dispatch_sig.md 全文已读：
// PacketGuard；put_header(0,0xc)；put_byte(0)；put_short(0)；put_byte(0)；
// local_43c[0x100] rep stos 清零；sprintf("%d분뒤 서버점검이 있습니다. %s",
//   this+0x04, GameserverDemonManagerInstance()->GetNoticeMessage())；
// len=strlen；put_int(len)；put_str(local_43c,len)；finalize(true)；
// G_GameWorld()->send_all；this+0x04 -= 1；
// ==0 → GetInstanceShutdowManager()->SendShutdownSignal() +
//   cMyTrace(PRETTY,0x1849,0)("Emergency Patch Message Received")；
// !=0 → RegistNextTimer()；return 1。
// [b28d-30] 复核 DIFF：push edi 缺失 + 帧差 4（0x43c vs 0x440，N82/P210 族），ORIG 0x0863bxxx。
// [b28d-solo-TimerDispatcher2] P167 差集 EXTRA memset×1 ⇒ ORIG 的 0x400 字节清零是
// **内联 rep stos**（N78/P214③/P312 突破口：`char buf[N] = {0}` 复现 4.1 rep stos，
// 显式 memset 调用在 4.4 恒为 call）⇒ 改数组零初始化形。残差（push edi/帧差/槽平移）
// 为 N82/P210 族。
bool Timer_NoticeEmergencyPatchMessage::dispatch_sig(int /*p1*/, int /*p2*/,
                                                    memberIdentificationNumber_t /*p3*/)
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
    // [Main 授权 2026-09-11 · §6.5 盲区] ORIG .rodata 该串实字节 = EUC-KR(cp949)
    // （`%d` BA D0 B5 DA 20 BC AD B9 F6 C1 A1 B0 CB C0 CC 20 C0 D6 BD C0 B4 CF B4 D9 2E 20 `%s`），
    // 原 UTF-8 字面量发射 3 字节/字 ⇒ 运行期乱码。改字节转义发射（源码文件仍 UTF-8）。
    sprintf(local_43c, "\x25\x64\xba\xd0\xb5\xda\x20\xbc\xad\xb9\xf6\xc1\xa1\xb0\xcb\xc0\xcc\x20\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9\x2e\x20\x25\x73", m_remainingNotiCount,
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
}