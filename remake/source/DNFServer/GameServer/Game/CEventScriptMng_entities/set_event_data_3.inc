void CPcRoomPlayTime::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x8110872。
    // [b28d-solo-CEventScriptMng_entities] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<PcBangPlayTimeScript*>(script);
