// ORIG 0x081de334（报告 Dispatcher_Direct_Entrance_Quick_Party/process.md）：
// direct_entrance_quick_party(user) 返回值写 param+0x4；两侧分支均 return 0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081de334 orig/ours=43/39 ndiff-blocks=9
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | cmpl   $0x0,-0x10(%ebp) | mov    -0x10(%ebp),%eax
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
    // [L7g5 2026-09-11 试验·负结论] ORIG 0x081de393 `call direct_entrance_quick_party`
    //   后为 `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,0x4(%eax)`（接收者地址装
    //   %eax、call 结果中转 %edx）；试过指针形（`stQuickPartyParam* pref = &…` +
    //   `pref->m_4 = …`）—— 编译后该函数指令数/形状**零变化**（引用与指针在 4.4
    //   同形），仍为装载序差（N7）⇒ 已回滚，维持引用形。
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
}