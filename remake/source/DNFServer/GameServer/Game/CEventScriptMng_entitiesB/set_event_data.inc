// ORIG 0x0810f95e：虚调度 set_event_record_type（vtable+0x18）写 m_recordType=key，
// 然后把绑定脚本存到 +0x1c。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：P211 档③——m_script1c
//   具型化（FatigueQuantityScript*）+ 赋值点 static_cast 异型化 ⇒ store 复现 ORIG
//   RHS-first（0x810f97d 三条逐条一致）；4.4.6 后端同时消 N7（vfn %ecx / this %eax
//   与 ORIG 同形）。4.4.7 下该件仍 NEAR（纯 N7 后端差：vfn %eax / this %edx）；
//   实测 TU identical 30→35 / ae 42 / near 5→0 / diff 1（N123 撞名假 DIFF）零回归。
//   路由已落 dnf_game_cxx.sh:370（b28d-S188）（ORIG 0x0810f95e sz 0x0000002a）
void CFatigueQuantity::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810f95e：call vtable+0x18（set_event_record_type(key)），再写 +0x1c。
    // [b28d-solo-CEventScriptMng_entitiesB] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<FatigueQuantityScript*>(script);
}