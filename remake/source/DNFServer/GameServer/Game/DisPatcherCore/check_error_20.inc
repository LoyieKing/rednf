// ORIG 0x081cad80 T _ZN19DisPatcher_UseSkill11check_errorEP5CUserR8MSG_BASER9ParamBase
// get_state(user)==5 → 0；否则 0x7fffffff（报告 DisPatcher_UseSkill/check_error.md 全文）。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
unsigned int DisPatcher_UseSkill::check_error(CUser* user, MSG_BASE& /*msg*/, ParamBase& /*param*/)

{
    // ORIG setne+test+je 布局：4.4.7 对 `if (x != 5) return INF; return 0;` 物化 bool。
    if (user->get_state() != 5)
        return 0x7fffffff;
    return 0;
}
