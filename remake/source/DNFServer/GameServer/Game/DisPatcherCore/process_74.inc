// ORIG 0x0826481a T _ZN34DisPatcher_CheckSecurityProtection7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_CheckSecurityProtection/process.md 全文转写）：
// sub $0x28 帧；将 CSecu_ProtectionField::Check(GlobalData::s_pSecuProtectionField,user,0x38)
// 返回值低字节写入 param_3[4]（CheckSecurityProtection 首数据字节 @+0x4 = m_pad[0]），恒返回 0。
int DisPatcher_CheckSecurityProtection::process(CUser* user, MSG_BASE& /*msg*/, ParamBase& param)

{
    CheckSecurityProtection& out = static_cast<CheckSecurityProtection&>(param);
    out.m_4 = static_cast<unsigned char>(
        GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x38)));
    return 0;
}
