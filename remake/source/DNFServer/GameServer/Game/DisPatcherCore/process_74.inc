int DisPatcher_CheckSecurityProtection::process(CUser* user, MSG_BASE& /*msg*/, ParamBase& param)

{
    CheckSecurityProtection& out = static_cast<CheckSecurityProtection&>(param);
    out.m_4 = static_cast<unsigned char>(
        GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x38)));
    return 0;
}
