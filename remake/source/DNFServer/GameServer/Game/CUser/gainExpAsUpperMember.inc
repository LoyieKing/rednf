// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864fa08 | _ZN5CUser20gainExpAsUpperMemberEi
int CUser::gainExpAsUpperMember(int exp)
{
    // b28d-solo-CUser 复测：level/rate/result 全头部声明（P212/P309，深→浅序）实测
    // 槽位逐字不变（level -0x12 / rate -0x10 / result -0xc）⇒ 4.4.6 按首次使用序
    // （N98），ORIG 的 level@-0x1a 系 4.1 槽基差；维持 N82/P112 封顶。
    if (get_charac_memberkey() == 0)
        return 0;
    // ORIG 0x0864fa08：connect 块 fall-through（技巧59），未连接尾块 set(0)+ret0；
    // 率常量 = 0.01f（@0x08cf3958 = 0x3c23d70a，此前 0.1f 为语义错）；
    // result<=0 亦调 set_charac_member_bonus_exp(0)（ORIG 3 call 点）。
    if (is_connect_upper_member())
    {
        // b28d-lane5：ORIG 0x0864fa2x `movzbl %al,%eax; mov %ax,-0x1a; filds -0x1a`
        // ⇒ 原码局部为 **short**（非 uchar，P0 槽往返 + 宽度证据）；率计算
        // `fstps -0x10` 具名 float 局部（P252）后再 `fildl exp; fmuls -0x10`。
        short level = get_uppermember_exp_level();
        float rate = level * 0.01f;
        int result = (int)(exp * rate);
        if (result > 0)
            set_charac_member_bonus_exp(result);
        else
            set_charac_member_bonus_exp(0);
        return result;
    }
    set_charac_member_bonus_exp(0);
    return 0;
}