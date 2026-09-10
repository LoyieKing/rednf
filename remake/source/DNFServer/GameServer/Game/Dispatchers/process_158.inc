int Dispatcher_Cancel_Quick_Party::process(CUser* user, MSG_BASE& msg,
                                           ParamBase& param)
{
    stQuickPartyParam& pref = static_cast<stQuickPartyParam&>(param);  // 裸偏移→具名字段（同偏移同宽度）
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    // [FL1 2026-09-10] P183 去缓存 + P290：ORIG 0x081de1b4 段 `mov %ebx,0x4(%esp)`
    //   （party 留 callee-saved %ebx 跨 receiver call，**无槽**），gm/mgr 亦无槽
    //   （链式内联）。旧形三个具名指针局部使 4.4 落 -0x18/-0x10 槽并逐点回读
    //   （帧 0x38 vs ORIG push %ebx + 0x24）⇒ 改链式内联。
    // X1/X2 配方（OPMC8 提供、probe px2 t8/t11 实证）：4.4 复现 ORIG
    // `xor $0x1,%eax; test %al,%al` 的唯一写法是「绑 %eax 的具名 char 局部 + ^= 1」；
    // 裸 `!call()` 只在 callee 返回 bool 时直落，`!= 1`/`== 1` 恒被折成 cmpb $1。
    unsigned char cancelBad =
        (unsigned char)G_CGameManager()->GetQuickPartySystemManager()
            ->cancel_quick_party((CParty*)user->GetParty(), true, user);
    cancelBad ^= 1;
    if (cancelBad)
    {
        pref.m_4 = 1;
    }
    return 0;
