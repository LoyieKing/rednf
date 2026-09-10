void CBoosterGage::send_error_state_packet(CUser* pUser, unsigned long dwItemIdx)
{
    unsigned char cResult = pUser->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_17)
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
