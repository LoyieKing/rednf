void CAddItemCountDB::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810e064。
    // [b28d-solo-CEventScriptMng_entities] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<AddItemCountDBScript*>(script);
