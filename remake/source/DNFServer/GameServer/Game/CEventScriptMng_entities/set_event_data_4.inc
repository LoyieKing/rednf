// [b28d-solo-CEventScriptMng_entities] AE（4.4.6 路由，17/17）：同 CLevelUpReward
//   ::set_event_data——P211 档③具型化（AccountFirstLoginScript*）+ 4.4.6 消 N7；
//   4.4.7 下仍 NEAR（纯 N7 后端差，Main 已路由 4.4.6）（ORIG 0x0810deda sz 0x0000002a）
void CAccountFirstLogin::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810deda。
    // [b28d-solo-CEventScriptMng_entities] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<AccountFirstLoginScript*>(script);
}