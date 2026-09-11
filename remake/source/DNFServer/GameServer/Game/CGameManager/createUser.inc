CUser* CGameManager::createUser()
{
    // ORIG 0x08294296：Guard<Mutex> 仅护住 Acquire（析构紧随其后），随后才判
    // user==0 → 空则 cMyTrace(0x8e3,level5)"createUser Failed! need to restart!"
    // 并返 0；非空走 else 正常路径（原实现误删 null 分支且把 guard 放整函数作用域）。
    // b28d-solo-CGameManager 判词：N82附——ORIG `push edi/esi/ebx; sub $0x6c`（Guard
    // 对象 -0x54）vs 4.4 `push esi/ebx; sub $0x70`（Guard -0x2c）：ORIG 多 edi 且
    // Guard 槽深 0x28，系 4.4 具名标量/类临时分区分配（P112/N82）不可控。
    CUser* user;
    {
        Guard<Mutex> guard(&m_mutex);
        user = m_userPool.Acquire();
    }
    if (user == 0)
    {
        // b28d-laneCGM（P227 全表达式临时链）：ORIG 该处 op() 实参在 ctor 前求值/直落
        //   寄存器（具名 tr 形多一次对象槽 + ctor 后读）。
        cMyTrace(__PRETTY_FUNCTION__, 0x8e3, 5)("createUser Failed! need to restart!");
        return 0;
    }
    unsigned int uid = user->GetUID();
    unsigned int accId = user->get_acc_id();
    check_user_var(user);
    int idx = GetIdx(user);
    user->SetSlotIDX((short)idx);
    // b28d-laneCGM S1 实测（2026-09-10）：`register int err` 反劣（ndiff 36→48、gap +2→+1）
    //   ⇒ 回滚裸形；ORIG 的 `mov -0x20(%ebp),%edi` 属 4.1/4.4 寄存器分配差（N7），
    //   不满足 P248 裸 register 四条件，封顶。
    // [K9GameRestA.CGameMgr12Run 2026-09-11 复核] 串已按 ORIG 0x082943ff 修正
    //   （`MEMORY_POOL` 下划线形）；DIFF 126/124 残差 = ORIG `push %edi` + err 经 %edi
    //   跨 cMyTrace ctor 保活（0x082943a8 段）vs 4.4 恒槽重读（N7），register 形实测反劣。
    int err = m_userPool.GetLastErrorCode();
    if (err != 0)
    {
        // ORIG 0x082943a8/0x082943bd：GetUID/NumberToString 结果入 %esi/%ebx 跨 ctor
        // 保活（无具名槽）⇒ 实参内联（P329/P227）。
        cMyTrace(__PRETTY_FUNCTION__, 0x8f6, 0)(
            // §6.5 串盲区修正：ORIG 0x082943ff 直证为 "MEMORY_POOL"（下划线，非空格）。
            "STATIC MEMORY_POOL createUser error(%d), user(%x), last user m_acc_id(%s), idx(%d), new user idx(%d)",
            err, user, NumberToString(accId, 0), uid, user->GetUID());
    }
    short nxt = getNextUID();
    user->SetIncreID(nxt);
    // ORIG 0x08294441：movzwl %ax,%eax 直传（无具名 short 槽）。
    user->set_unique_id(CUserGlobalInfoHandleInstance()->get_uniqueid());
    return user;
}