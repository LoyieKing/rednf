// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cfcac orig/ours=70/68 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x14(%ebp) | mov    %eax,%edx
int Dispatcher_RequestDisjointItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cfcac：check_error(三参) 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x2a1f)；err==0 → 按 msg 字段分解物品。
    stRequestDisjointItemParam& p = static_cast<stRequestDisjointItemParam&>(param);
    p.m_4 = check_error(user, msg, param);
    if (p.m_4 > 0)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        // ORIG 0x081cfd2c: call LineFunc 后 `jmp +0xe7`（eax=LineFunc 返回值），无 mov $0x0,%eax
        return LineFunc(0x2a1f,
                        __PRETTY_FUNCTION__,
                        p.m_4, (unsigned int)user->get_acc_id());
    }

    // msg+0xe：ushort 对象 uid → find_from_world；msg+0x10：short 槽；
    // msg+0x12：char（有符号）背包类型。命令字 0xef，区域 0xffff。
    // ORIG 0x081cfd2e: `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` 具名 mref 引用占槽；
    // 0x081cfd37: `movzwl 0xe(%eax),%eax` 常量偏移直读 —— 故删去 uid 具名量、
    // 也不得对成员取址（`*reinterpret_cast<ushort*>(&mref.m_field0e)` 会产 add 形）。
    MSG_REQUEST_DISJOINT_ITEM& mref =
        reinterpret_cast<MSG_REQUEST_DISJOINT_ITEM&>(msg);
    CUser* target = G_GameWorld()->find_from_world(mref.m_field0e);

    // ORIG 0x081cfd52: `movzbl 0x12(%eax),%eax; movsbl %al,%edx`（+0x12 字节）
    // 0x081cfd5f: `movzwl 0x10(%eax),%eax; cwtl`（+0x10 有符号短槽）
    DisPatcher_DisJointItem::_disjoint(
        user,
        (int)mref.m_field10,
        (int)(signed char)mref.m_field12,
        (ENUM_CMDPACKET)0xef,
        target,
        0xffff);
    return 0;
}