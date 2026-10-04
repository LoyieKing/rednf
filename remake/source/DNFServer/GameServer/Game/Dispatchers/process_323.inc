// ==== Dispatcher_Teleport::process (ORIG 0x081d07bc) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d07bc orig/ours=60/62 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,%edx | mov    -0x10(%ebp),%eax | mov    %edx,0x4(%eax)
int Dispatcher_Teleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stTeleportParam& out = static_cast<stTeleportParam&>(param);


    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2bcc,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    MSG_TELEPORT& req = reinterpret_cast<MSG_TELEPORT&>(msg);
    out.m_10 = req.m_field14;
    // [L7g5] ORIG 0x081d084d/0x081d0853 `mov -0x10(%ebp),%eax; mov %edx,0xc(%eax)`
    //   —— +0xc 是**单级 disp 写**；裸 `&out.m_gapA[2]` 恒产 `add $0xc,%eax` +
    //   `mov %edx,(%eax)` 两步（负 gap 的 1 条来源）⇒ 改 packed 具名视图。
    out.m_fieldC = req.m_field10;
    out.m_8 = req.m_field0e;
    out.m_11 = req.m_field15;

    return 0;
}