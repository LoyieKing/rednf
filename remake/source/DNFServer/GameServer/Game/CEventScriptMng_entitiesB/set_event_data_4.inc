// ORIG 0x0810f996：同 CFatigueQuantity 模式。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：同
//   CFatigueQuantity::set_event_data——P211 档③ 具型化
//   （PurchaseCashItemBonusScript*）⇒ store RHS-first；4.4.6 消 N7。4.4.7 下仍
//   NEAR（纯 N7 后端差）。路由已落 dnf_game_cxx.sh:370（ORIG 0x0810f996 sz 0x0000002a）
void CPurchaseCashItemBonus::set_event_data(int key, InGameEventScript* script)
{
    // [b28d-solo-CEventScriptMng_entitiesB] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<PurchaseCashItemBonusScript*>(script);
}