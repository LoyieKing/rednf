unsigned int DisPatcher_UseSkill::check_error(CUser* user, MSG_BASE& /*msg*/, ParamBase& /*param*/)

{
    // ORIG setne+test+je 布局：4.4.7 对 `if (x != 5) return INF; return 0;` 物化 bool。
    if (user->get_state() != 5)
        return 0x7fffffff;
    return 0;
}
