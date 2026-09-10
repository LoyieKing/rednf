void CExchangeRandomItemReward::set_event_data(int key, InGameEventScript* script)
{
    // [b28d-solo-CEventScriptMng_entitiesB] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<ExchangeRandomItemRewardScript*>(script);
