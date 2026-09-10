void CUser::saveTaxMoneyForUpperMember(int money)
{
    // b28d-solo-CUser AE: ORIG 0x864f8fb-0x864f902 `sete %al; test %al,%al; jne`
    //   （P292 非 bool 返回的 ==0 物化）；具名 mk0 落槽（mov %al,-0x9; cmpb $0,-0x9）
    //   ⇒ 删局部内联条件（probe_bool a3）。2/3 守卫直折 test %al/je + cmpl/jle。
    if (get_charac_memberkey() == 0)
        return;
    // ORIG 0x864f90f-0x864f917 正向嵌套（技巧 19）：`test %al,%al; je` /
    // `cmpl $0x0,0xc; jle` 均直落 leave，无 xor/中转 jmp。
    if (is_connect_upper_member())
    {
        if (money > 0)
            add_member_pay_tex_money_to_upper(money);
    }
