// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d01ae orig/ours=64/61 ndiff-blocks=10
//   帧差 sub $24 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x24,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0x18(%ebp)
int Dispatcher_EnterDisjointStore::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d01ae：check_error(三参) 结果写 param+0x04；
    // err>0 → 0；err<0 → 直接 return LineFunc(0x2adf)；err==0 → 分解机进入商店。
    stEnterDisjointStoreParam& out = static_cast<stEnterDisjointStoreParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        // ORIG 0x081d022e: call LineFunc 后 `jmp +0xd2`（eax=LineFunc 返回值），
        // 无 `mov $0x0,%eax`；即源码是 `return LineFunc(...)` 而非先调用再 return 0。
        return LineFunc(0x2adf,
                        __PRETTY_FUNCTION__,
                        out.m_4, (unsigned int)user->get_acc_id());
    }

    // ORIG 0x081d0230: `mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)` —— 具名 msg 引用占槽，
    // 随后 `mov -0x14(%ebp),%eax; movzwl 0xe(%eax),%eax` 常量偏移直读（无 `add $0xe` 折叠差）。
    MSG_ENTER_DISJOINT_STORE& req = reinterpret_cast<MSG_ENTER_DISJOINT_STORE&>(msg);
    CUser* target = G_GameWorld()->find_from_world(req.m_field0e);

    // ORIG 0x081d0254: `movl $0x0,-0xc(%ebp)` —— job 具名量带 0 预清后再赋值。
    // ORIG 0x081d0276: cast 在赋值点（P277 g2 形）⇒ 实参 user 先于 this 装载。
    expert_job::CDisjointer* job = 0;
    job = reinterpret_cast<expert_job::CDisjointer*>(target->GetCurCharacExpertJob());
    job->OnEnterDisjointStore(user);
    return 0;
}