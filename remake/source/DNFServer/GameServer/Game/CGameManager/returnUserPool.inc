void CGameManager::returnUserPool(CUser* user)
{
    // b28d-lane12: N82 槽平移封顶（orig=147 our=147 gap=0；ndiff 102 全为槽号——
    // Guard/cMyTrace/map 迭代器槽组 4.4 分区 vs 4.1 生命周期穿插）。ORIG 0x08294b2c。
    // b28d-solo-CGameManager 判词：N82 维持（P167 CLEAR，gap 0；残差全为槽号：
    // find sret -0x28 vs -0x34、accId -0x24 vs -0x30）。
    // [SweepCGameManager2 本轮] lastId 具名删 → P227 全表达式临时链，复现 ORIG
    //   0x08294cd8 NumberToString 结果入 %ebx 跨 cMyTrace ctor 保活；残余 =
    //   err 跨 ctor 的 %esi 缓存（ORIG `mov -0xc,%esi`）4.4 恒槽重读（裸 register
    //   实测反偏离 ORIG，已回滚）+ 尾部 `jmp epilogue` 岛（P285）⇒ N82/P52。
    // [K9GameRestA.CGameMgr12Run 2026-09-11 复核] 读 fresh .o：ORIG 0x08294cd8 段
    //   `mov -0xc(%ebp),%esi; mov %esi,0x8(%esp)` vs 我方 `mov -0xc(%ebp),%eax;
    //   mov %eax,0x8(%esp)`（纯寄存器角色 N7，指令数 148=148 持平）⇒ 封顶。
    // ORIG 0x08294b38：`cmpl $0x0,0xc(%ebp); je 08294d30`（08294d30 是 nop 空岛，
    // 08294d31 才是 epilogue）⇒ 裸早返形复现尾 nop 岛（P223 变体）。
    if (user == 0)
        return;
    {
        unsigned int accId = user->get_acc_id();
        std::map<unsigned int, CUser*>::iterator it = m_userByAccId.find(accId);
        if (it != m_userByAccId.end())
        {
            m_userByAccId.erase(it);
            GlobalData::s_pLogGameChannel->IncOutUser();
        }
        unsigned int accId2 = user->get_acc_id();
        std::map<unsigned int, CUser*>::iterator it2 = m_userByAccId2.find(accId2);
        if (it2 != m_userByAccId2.end())
        {
            m_userByAccId2.erase(it2);
        }
        unsigned int uid = user->GetUID();
        unsigned int accId3 = user->get_acc_id();
        user->log_out();
        // ORIG 0x08294c71..0x08294ca8：Guard 在独立作用域，Free 后立即 ~Guard
        // （0x08294ca8），随后才 GetLastErrorCode ⇒ 不能与 err 同作用域。
        {
            Guard<Mutex> guard(&m_mutex);
            m_userPool.Free(user);
        }
        // ORIG 0x08294cc1..0x08294cd2：err 入槽后 `mov -0xc(%ebp),%esi` 跨
        // cMyTrace ctor 保活；4.4 恒从槽重读（裸 register 实测 146→145 反偏离
        // ORIG，已回滚）⇒ N82/P52 封顶。
        int err = m_userPool.GetLastErrorCode();
        if (err != 0)
        {
            // ORIG 0x08294cd8..0x08294d00：NumberToString 结果入 %ebx 跨 cMyTrace
            // ctor 保活（无具名槽）⇒ P227 全表达式临时链。
            // [PoolA 2026-09-11 试形判词] err 跨 ctor 的 %esi 缓存：P226 裸语句表达式
            // `({ err; })` 实测无效（仍 `mov -0xc(%ebp),%eax` 槽重读，148/149 ndiff2
            // 前后逐值不变）⇒ 维持 N7/N82 封顶（ORIG `mov -0xc(%ebp),%esi`）。
            cMyTrace(__PRETTY_FUNCTION__, 0xa0e, 0)(
                "STATIC MEMORY_POOL returnUserPool error(%d), user(%x), last user m_acc_id(%s), idx(%d)",
                err, user, NumberToString(accId3, 0), uid);
        }
    }
}