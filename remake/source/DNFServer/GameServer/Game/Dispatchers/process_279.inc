int Dispatcher_Direct_Entrance_Quick_Party::process(CUser* user, MSG_BASE& msg,
                                                    ParamBase& param)
{
    // ORIG 0x081de35a: err 具名占 -0x10 槽（先声明）
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    // ORIG 0x081de375: pref 引用槽 -0xc，声明在 err 判定之后
    stQuickPartyParam& pref = static_cast<stQuickPartyParam&>(param);  // 裸偏移→具名字段（同偏移同宽度）
    // ORIG 0x081de37b/0x383: `call G_CGameManager`→`mov %eax,(%esp)`→`call GetQuickPartySystemManager`
    //   两级工厂均无具名槽 ⇒ 链式调用（原 gm/mgr 具名量各占一槽，为多余局部）。
    pref.m_4 = G_CGameManager()->GetQuickPartySystemManager()->direct_entrance_quick_party(user);
    // ORIG 尾岛：`mov -0xc(%ebp),%eax; mov 0x4(%eax),%eax; test %eax,%eax; je <T>;
    //   mov $0x0,%eax; jmp <T>` ⇒ 源码为 `if (pref.m_4 != 0) { return 0; } return 0;`
    //   （我方原先只有单个 return 0，比 ORIG 少 3 条指令）。
    if (pref.m_4 != 0)
    {
        return 0;
    }
    return 0;
