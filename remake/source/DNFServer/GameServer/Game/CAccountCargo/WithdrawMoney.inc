bool CAccountCargo::WithdrawMoney(unsigned int money)
{
    // [b28d-solo-CAccountCargo] ORIG 0x0828a2f6：无 user/mgr/inven 具名局部
    // （isRestricted/gain_money 实参先物化 %ebx 跨 receiver call 保活，P52；
    // 其余调用点重读 m_pUser，P0）。
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            m_pUser, (RestrictCategory::Enum)1, 0x1a)) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, 0xd1);
        return 0;
    }
    unsigned int err =
        GlobalData::s_pSecuProtectionField->Check(m_pUser, (SECURITY_PROTCTION)3);
    if (err != 0) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, (unsigned char)err);
        return 0;
    }
    // ORIG 0x0828a3a4：GetMoney() 访问器比较（ORIG 非内联调用）
    if (GetMoney() < money || (int)money < 0) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, 10);
        return 0;
    }
    if (!m_pUser->CheckMoney(money)) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 0x5e);
        return 0;
    }
    SubMoney(money);
    if (((CInventory*)m_pUser->getCurCharacInvenW())
            ->gain_money(money, (eMoneyAddReason)0x1b, 1, 0) == 0) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, 10);
        return 0;
    }
    SendNotifyMoney((ENUM_CMDPACKET)0x135);
    m_alter = 1;
    return 1;
