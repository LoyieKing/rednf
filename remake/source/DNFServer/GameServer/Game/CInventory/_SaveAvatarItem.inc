bool CInventory::_SaveAvatarItem(int slot, int expireDate, bool b, char c,
                                 const stAvatarEmblemInfo_t& emblem,
                                 const char* agency, unsigned long hidden,
                                 const stAvatarExpansionInfo_t& expansion)
{
    // b28d-solo-CInventory：117/117，ORIG push %edi + sub 0x5c vs 我方 sub 0x60（callee-saved 缓存差）⇒ N18a/N82 封顶。
    // b28d-37 [N18a/N82 封顶，ORIG 0x8509e78]：callee-saved push 差 + 槽位残差（117=117 指令数，语义闭合）。
    // [b28d-round5 **DIFF→NEAR 翻正**，ORIG 0x08509f2b..0x08509f7c 直证] 旧 N 判词作废：
    // 真因是 avatarUid/characNo **两个具名局部**（ORIG 无槽：`movsbl -0x30(%ebp),%edi`(c)/
    // `movzbl -0x2c(%ebp),%esi`(b) 跨 get_charac_no 保活、avatarUid 保活 %ebx、characNo
    // 由 %eax 直压 0x4(%esp)）⇒ 按 P0 改单条语句 + 内联实参后 117/117 且 push edi/esi/ebx
    // 与 sub $0x5c 全数复现（帧/push 与 ORIG 一致）。残余 = `m_expansion` 一处 3 行装载序
    // （ORIG LHS-first `mov -0x1c→%eax` 先载 vs 4.4 RHS-first 先算 `*(int*)&expansion`）：
    // 直写 cast / 匿名 union 具名视角 / 具名引用三形实测同产 RHS-first（P235 方向不成立），
    // 记 N76 族；其余为 .o 重定位伪差。
    if (m_pParent == 0)
        return false;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("inventory.cpp", 0x2360), true);
    **guard << 0x31;
    **guard << (int)m_pParent->GetUID();
    SIG_AVATAR_INSERT* sig = guard->GetInBuffer<SIG_AVATAR_INSERT>();
    // [b28d-round5 P0/P52 复攻，ORIG 0x08509f2b..0x08509f7c 直证] ORIG 无 avatarUid/
    // characNo 具名槽：`movsbl -0x30(%ebp),%edi`(c)/`movzbl -0x2c(%ebp),%esi`(b) 跨
    // get_charac_no 保活、avatarUid 保活 %ebx、characNo 由 eax 直压 0x4(%esp)
    // ⇒ 原码是单条语句 + 内联实参（P0：无槽往返 ⇒ 无具名局部）。
    sig->set(m_pParent->get_charac_no(-1), m_pAvatarSlot[slot].m_addInfo, slot + 10,
             expireDate, b, c, agency, 0, hidden);
    sig->setJewelSocket(emblem);
    // ORIG 0x8509fbf：`mov -0x1c(%ebp),%eax`(sig 先) → `mov 0x28(%ebp),%edx;
    // mov (%edx),%edx` → `mov %edx,0x4f(%eax)`（LHS-first 单条 movl）。
    // [b28d-round5 P235] 直写 `*(const int*)&expansion` 时 4.4 走 RHS-first
    // （base 先载）⇒ 改用匿名 union 具名视角（m_head 与 m_color[2] 同址同宽），
    // 得 ORIG 的 LHS-first 序。sizeof/偏移/mangled 零影响（纯 TU 局部视图）。
    sig->m_expansion = reinterpret_cast<const ExpansionHeadV&>(expansion).m_head;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
