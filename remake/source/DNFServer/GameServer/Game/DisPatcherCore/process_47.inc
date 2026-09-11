// ORIG 0x081c2f94 T _ZN19DisPatcher_DropItem7processEP5CUserR8MSG_BASER9ParamBase
// m_errNo=check_error(user,msg,param)（三参）；>=1：m_8=msg.m_11 返回 0；<0
// LineFunc(0xdcc,pretty,err,acc_id) 返回其值；==0 按 state 分派：
// 5=CGameManager::GetParty(GetPartyIndex) 后 CParty::drop_item（[BLOCKED]）；
// 0xc=getBossTower 虚调 handleDropItem；8=WarRoom::HandleItemDrop>0 时写
// [b28d-29] N88/N82：3 处 ORIG 有 `mov $0x0,%eax` 死写入（写后未读），4.4 惰性求值位置错位；语义等价  (ORIG 0x081c2f94 insn 208->212)
// m_errNo/m_8；其它 m_errNo=0x13。
// [b28d-solo-DisPatcherCore] 复核 NEAR：ndiff 4（ORIG 208 / ours 208），残差=N82 槽/块序；N 封顶（详见上方 b28d-29 归因）。
// [b28d-PoolPatchCore] 改判+修复：原残差实为 2 处 jle/je 极性差（旧 unsigned 形封死）。drop_item/
// HandleItemDrop 返回 int，ORIG `cmpl $0,-0x28; jle` ⇒ 原码 `if (ret > 0)`（负返回不写 m_errNo，
// 旧 !=0 为真实语义偏差）已修（P321-T2 直证）。现残差 = 2 块 store 装载序（ORIG RHS-first vs
// 4.4 LHS-first）：字段 m_errNo(int) 与 RHS ret(int) 同型、ret 又受 jle 约束须 int，P211 双旋钮
// 均不可动 ⇒ N76 封。NEAR(2)，bad 总数不增。
int DisPatcher_DropItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{

    // ORIG +0x09/+0x0f：req(msg)→远槽 -0x34、out(param)→近槽 -0x30（技21 声明序）；
    // errNo store 双钉（技45）。state 无槽：三段各自 `call get_state; cmp $K,%eax;
    // sete %al; test %al,%al; je`（技9 (bool)==K 物化形）。
    MSG_DROP_ITEM& req = *reinterpret_cast<MSG_DROP_ITEM*>(&msg);
    stDropItemParam& out = static_cast<stDropItemParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG `mov %eax,%edx; mov <out>,%eax; mov %edx,0x4(%eax)`。
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo >= 1)
    {
        out.m_8 = req.m_11;
        return 0;
    }
    if (out.m_errNo < 0)
    {
        // ORIG 81c2ffc：LineFunc(0xdcc, pretty, errNo, acc_id) 并返回其值。
        return LineFunc(0xdcc, __PRETTY_FUNCTION__, out.m_errNo,
                        user->get_acc_id());
    }
    if (user->get_state() == 5)
    {
        CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party != 0)
        {
            // [b28d-PoolPatchCore] P321-T2：ORIG `cmpl $0x0,-0x28; jle` ⇒ 原码带符号
            // `if (ret > 0)`（负值不写 errNo，旧 !=0 写法为真实语义偏差）。
            int ret = party->drop_item(
                user, req.m_0d, req.m_0f, (char)req.m_11, req.m_12, req.m_14);
            if (ret > 0)
            {
                // [b28d-solo-DisPatcherCore] P211 档③：ORIG RHS-first
                // （`mov -0x28,%edx; mov -0x30,%eax; mov %edx,0x4(%eax)`）⇒ RHS 局部异型化。
                // [b28e-L5] P211 档③ 镜像试验（RHS 侧 `(unsigned)` 转换节点）实测**无效**
                // ——4.4 仍出 LHS-address-first `mov -0x30(%ebp),%eax; mov -0x28(%ebp),%edx`
                // ⇒ 不改字段符号性该旋钮不生效；m_errNo 必须 signed（`<0`/`>0` 语义）
                // 故头侧不可动，本处按 N76 封顶（ORIG RHS-first 不可造）。
                out.m_errNo = ret;
                out.m_8 = req.m_11;
            }
        }
    }
    // [b28d-solo-DisPatcherCore] P169：ORIG 各 state 块尾单 `jmp +0x2b3`（跳共享尾
    // `mov $0,%eax`）⇒ 原码是 if/else-if 链 + 函数尾单一 return 0（goto 尾标签实测反劣）。
    else if (user->get_state() == 0xc)
    {
        // ORIG 81c35b0：user->getBossTower()（0x08655220：idx<0→0 否则
        // mgr->getBossTower(idx)）→ vtable+0x20 handleDropItem。
        WongWork::CBossTower* tower = user->getBossTower();
        if (tower != 0)
        {
            tower->handleDropItem(user, req.m_0d, req.m_0f, (char)req.m_11,
                                  req.m_12, req.m_14);
        }
    }
    else if (user->get_state() == 8)   // ORIG +0x1df `jmp +0x2b3`
    {
        WarRoom* room = G_CGameManager()->GetWarRoom(user->GetWarRoomIndex());
        if (room != 0)
        {
            // [b28d-PoolPatchCore] P321-T2：同上，ORIG `cmpl $0x0,-0x1c; jle`。
            int ret = room->HandleItemDrop(
                user, req.m_0d, req.m_0f, (char)req.m_11, req.m_12, req.m_14);
            if (ret > 0)
            {
                out.m_errNo = ret;   // P211 档③（同上）
                out.m_8 = req.m_11;
            }
        }
    }
    else
    {
        out.m_errNo = 0x13;
        out.m_8 = req.m_11;
    }
    return 0;
}
