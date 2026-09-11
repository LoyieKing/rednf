void CBoosterGage::send_error_state_packet(CUser* pUser, unsigned long dwItemIdx)
{
    // [K7 2026-09-11 §6.5/P174] ORIG 0x080dd16c `mov %al,-0xd(%ebp)`（直落 1 字节，无归一化）
    // + 0x080dd176 `cmpb $0x0,-0xd(%ebp); je` ⇒ 局部亦为 bool（旧 `unsigned char` 会补 setne）。
    bool cResult = pUser->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_17)
                       ->check_max_booster_gage(dwItemIdx);
    int nMaxCount = 2;
    if (cResult != 0)
        nMaxCount = G_CDataManager()->m_boosterGage.m_cMultiboxCount * nMaxCount;
    PacketGuard packetGuard;
    packetGuard.put_header(1, 0xd3);
    packetGuard.put_byte(0);
    packetGuard.put_byte(0x13);
    packetGuard.put_byte(nMaxCount);
    packetGuard.finalize(true);
    pUser->Send(packetGuard);
}