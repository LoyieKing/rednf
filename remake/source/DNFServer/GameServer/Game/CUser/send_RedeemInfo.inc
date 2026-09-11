// ============================================================================
// ORIG T 0x086624f8 sz 0x30 _ZN5CUser15send_RedeemInfoEv：
//   GetCharacExpandData(6) 取 Redeem_Item::CRedeemItem 后 Send_RedeemList。
// ============================================================================
void CUser::send_RedeemInfo()
{
    Redeem_Item::CRedeemItem* redeem =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)6);
    redeem->Send_RedeemList(this);
}