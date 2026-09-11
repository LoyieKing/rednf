// [b28d-solo-CEventScriptMng_entities] AE（4.4.6 路由，17/17）：P211 档③——m_script1c
//   具型化（LevelUpRewardScript*）+ 赋值点 static_cast 异型化 ⇒ store 复现 ORIG
//   RHS-first；4.4.6 后端同时消 N7（vfn %ecx / this %eax 与 ORIG 同形）。4.4.7 下
//   该件仍 NEAR（纯 N7 后端差；Main 已路由 4.4.6，dnf_game_cxx.sh b28d-S157 条目）
//   （ORIG 0x0810e168 sz 0x0000002a）
void CLevelUpReward::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810e168：call vtable+0x18（set_event_record_type(key)），再写 +0x1c。
    // [b28d-solo-CEventScriptMng_entities] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<LevelUpRewardScript*>(script);
}