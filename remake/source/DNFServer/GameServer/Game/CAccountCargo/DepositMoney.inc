// CUser.h:820 声明已同步为 bool（caller 0x0822508c test %al 直判证据，
// Main 代落头）；定义体返回类型随之改 bool，return 0/1 字面量语义不变。
bool CAccountCargo::DepositMoney(unsigned int money)
{
    // [b28d-solo-CAccountCargo] ORIG 0x0828a12a：无 user/mgr 具名局部——isRestricted
    // 实参 m_pUser 先物化 %ebx 跨 Get() 保活（P52），其余调用点重读 m_pUser（P0）。
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            m_pUser, (RestrictCategory::Enum)1, 0x1a)) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 0xd1);
        return 0;
    }
    unsigned int err =
        GlobalData::s_pSecuProtectionField->Check(m_pUser, (SECURITY_PROTCTION)2);
    if (err != 0) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, (unsigned char)err);
        return 0;
    }
    if ((int)money > m_pUser->getCurCharacMoney() || (int)money < 0) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 10);
        return 0;
    }
    if (!CheckMoneyLimit(money)) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 0x5f);
        return 0;
    }
    // ORIG：money 实参先物化 %ebx 跨 getCurCharacInvenW 保活（P52）；无 inven 槽。
    if (!((CInventory*)m_pUser->getCurCharacInvenW())
             ->use_money(money, (eMoneySubReason)0x28, 1)) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 10);
        return 0;
    }
    AddMoney(money);
    SendNotifyMoney((ENUM_CMDPACKET)0x134);
    m_alter = 1;
    return 1;
}