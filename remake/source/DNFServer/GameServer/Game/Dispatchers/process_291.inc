// ORIG 0x081e21d6（报告 Dispatcher_ImageCommunicationEquipmentUse/process.md）：
// param+4=check_error；==0 时 use_money(price,0x30,true)、param+8=price、
// param+0xc=getCurCharacMoney。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e21d6 orig/ours=51/46 ndiff-blocks=8
//   帧差 sub $24 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    0x10(%ebp),%eax
int Dispatcher_ImageCommunicationEquipmentUse::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [g6 2026-09-11] P198 两段式：ORIG 0x081e21d6 头部 `mov 0x10(%ebp),%eax;
    //   mov %eax,-0x10(%ebp)` = msg 形参的具名指针副本（全函数从不回读的 ORIG
    //   死存储，P171 照抄）；「声明 + 独立赋值」两句才保 store（探针 P198）。
    MSG_BASE* pmsg;
    pmsg = &msg;
    stImageCommunicationEquipmentUse& out =
        static_cast<stImageCommunicationEquipmentUse&>(param);
    // ORIG 槽序：param 引用(-0xc) 只有 out.m_4 具名（无 err 具名）——
    //   0x081e?: `mov -0xc(%ebp),%edx; mov %eax,0x4(%edx)` 直接写回，
    //   随后 `mov -0xc(%ebp),%eax; mov 0x4(%eax),%eax; test %eax,%eax; je <T>`
    //   读的是 out.m_4 槽。原 `const int err` 多占 -0x14 槽。
    out.m_4 = check_error(user, msg);
    // [g6 2026-09-11] ORIG 0x081e21d6 块序：`test %eax,%eax; je <body>;
    //   mov $0x0,%eax; jmp <epilogue>` ⇒ body 在跳转目标（尾部），早退走 fall-through。
    //   4.4 对 `if (cond) { body }` 恒 body fall-through ⇒ 改写为早返形
    //   （探针 /tmp/k1sbx/g6/probe/p2.cpp f4 族：then=return 才是 4.4 fall-through 侧）。
    if (out.m_4 != 0)
    {
        return 0;
    }
    {
        // ORIG：价格值经 %ebx 跨调用直传（`mov (%eax),%ebx`），无具名 price 槽
        //   （原 `const int price` 占 -0xc 槽，产生 `mov %eax,-0xc(%ebp)` 往返）。
        user->getCurCharacInvenW()->use_money(
            *(int*)G_CDataManager()->GetImageCommnicationData(),
            (eMoneySubReason)0x30, true);
        out.m_8 = *(int*)G_CDataManager()->GetImageCommnicationData();
        out.m_c = user->getCurCharacMoney();
    }
    return 0;
}