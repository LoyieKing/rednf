// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081ce1fc orig/ours=59/69 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
int Dispatcher_ChangeLetterStat::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stChangeLetterStatParam& out = static_cast<stChangeLetterStatParam&>(param);

    out.m_result = check_error(user, msg);
    if (out.m_result > 0)
        return 0;
    if (out.m_result < 0)
    {
        // [DispA2 语义修复·返回值直通] ORIG 0x081ce26c `call LineFunc(0x2744)`
        //   后紧接 0x081ce271 `jmp 081ce2b8 <+0xbc>`（tail=leave），**无**
        //   `mov $0x0,%eax` ⇒ 原码是 `return LineFunc(...)`。
        return LineFunc(0x2744,
                        __PRETTY_FUNCTION__,
                        out.m_result, user->get_acc_id());
    }
    // ORIG 0x081ce2ba 后段 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`（msg 具名引用，
    // check_error 之后声明）⇒ 两次字段访问经同一引用；实参右→左（0x8=letterStat 先算）。
    MSG_CHANGE_LETTER_STAT& mref = reinterpret_cast<MSG_CHANGE_LETTER_STAT&>(msg);
    out.m_result = WongWork::CMailBoxHelper::ReqChangeLetterStat(
        user, mref.m_mailIdx, static_cast<int>(mref.m_letterStat));
    // ORIG 尾 `test %eax,%eax; je <T>; mov $0,%eax; jmp; <T>: mov $0,%eax`
    // ⇒ 互试形（P222 两形互试）：`!=0` 为 then 时产 je。
    if (out.m_result != 0)
        return 0;
    return 0;
}