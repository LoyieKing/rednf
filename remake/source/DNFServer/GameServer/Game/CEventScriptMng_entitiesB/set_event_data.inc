void CFatigueQuantity::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810f95e：call vtable+0x18（set_event_record_type(key)），再写 +0x1c。
    // [b28d-solo-CEventScriptMng_entitiesB] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<FatigueQuantityScript*>(script);
