// ==== Dispatcher_SoloTeleport::process (ORIG 0x081e1d9c) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e1d9c orig/ours=81/83 ndiff-blocks=18
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x14(%ebp) | mov    0x10(%ebp),%eax | mov    -0x10(%ebp),%eax
int Dispatcher_SoloTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [G3 试] 声明序=槽序：ORIG 0x081e1da0 param→-0x14、0x081e1da7 msg→-0x10（param 先声明）。
    stSoloTeleportParam& res = static_cast<stSoloTeleportParam&>(param);
    MSG_SOLO_TELEPORT& req = reinterpret_cast<MSG_SOLO_TELEPORT&>(msg);

    // [G3 语义修正 2026-09-11] ORIG 0x081e1db3 `mov 0x941f818,%edx; mov %edx,0x44(%eax)`
    //   （==-1 支 0x081e1dc7 `mov 0x941f81c,%edx`）—— **无 `$` 前缀** ⇒ 存的是
    //   `g_pTeleport[i]` 的**值**（void* 元素内容），不是 `&g_pTeleport[i]`（地址）。
    //   旧码 `&…` + `(*reinterpret_cast<Teleport**>(m_44))->` 多一层解引用（+2 条）。
    //   GlobalData.h:278 `extern void* g_pTeleport[2];` @0x0941f818。
    // [G4-332 2026-09-11 · P46/P59] ORIG 0x081e1db4 `cmp $0xffffffff; je 081e1dc7`
    //   （==-1 跳 else）⇒ then=g_pTeleport[0] / else=g_pTeleport[1]，**两臂各自 store**
    //   （then 尾 0x081e1dc5 `jmp 0x081e1dd3` 越块）；三目在 4.4 折成两臂 load + 单 store
    //   （多 1 条 `mov %eax,%edx`）。
    if (req.m_34 != -1)
    {
        req.m_44 = (int)GlobalData::g_pTeleport[0];
    }
    else
    {
        req.m_44 = (int)GlobalData::g_pTeleport[1];
    }

    // ORIG 0x081e1dc7: STTeleportData data; data.m_field00=req.m_30;
    //   data.m_tp 前 10 字节 = req.m_tp(state..posy 按 4B/4B/2B 三段展开拷贝);
    //   (*Teleport**)(req.m_44)->set_teleport_data(data)（vtable 槽3 间接调用）
    STTeleportData data;
    data.m_field00 = req.m_30;
    data.m_tp = req.m_tp;
    reinterpret_cast<Teleport*>(req.m_44)->set_teleport_data(data);

    const int err = check_error(user, msg, param);
    if (err > 0)
    {
        res.m_4 = err;
    }
    else if (err < 0)
    {
        return -1;   // ORIG:err<0 → 返回 -1(dispatch_template result<0 → 不发包)
    }

    if (err == 0)
    {
        // [G4-332 2026-09-11] ORIG 0x081e1e64 `mov -0x10(%ebp),%eax; mov 0x44(%eax),%eax;
        //   mov %eax,(%esp)` ⇒ m_44 的**值即 this**（无二次解引用；原码 `->` 直呼）；
        //   旧形多一条 `mov (%eax),%eax`（把对象首字段当 this）。
        reinterpret_cast<Teleport*>(req.m_44)->set_end_cool_time(user);
        res.m_8 = req.m_30;   // ORIG:param+8 = msg+0x30(err==0 时)
    }

    res.m_c = req.m_44;       // ORIG:param+0xc = msg+0x44(无条件)
    return 0;
}