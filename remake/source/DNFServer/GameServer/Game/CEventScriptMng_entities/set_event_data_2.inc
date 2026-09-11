// [b28d-solo-CEventScriptMng_entities] AE（4.4.6 路由，17/17）：P211 档③——m_script1c
//   具型化（AttendanceScript*，S12 先例）+ 赋值点 static_cast ⇒ store RHS-first；
//   4.4.6 后端消 N7（vfn %ecx / this %eax 与 ORIG 同形）。4.4.7 下仍 NEAR（纯 N7
//   后端差，Main 已路由 4.4.6）（ORIG 0x0810eadc sz 0x0000002a）
void CAttendance::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810eadc。
    // [b28d-solo-CEventScriptMng_entities] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<AttendanceScript*>(script);
}