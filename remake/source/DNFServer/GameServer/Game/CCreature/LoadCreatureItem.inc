bool CCreature::LoadCreatureItem(CCreatureMgr* mgr, Inven_Item* item,
                                 SIG_CREATURE_ITEM_DETAIL_INFO* info,
                                 CUser* user)
{
    if (info->m_stat != 0)
    {
        item->reset();
        return false;
    }

    m_exp = info->m_exp;                           // → +0x30
    SetItemId(item->m_addInfo);                    // *(int*)(item+2) 驱动脚本解析
    m_uid = info->m_uid;                           // → +0x0c
    m_slotNo = info->m_slotNo;                     // → +0x10
    // round1 R19：ORIG 833862f 段 = `movzbl 0x24(%eax),%eax; movsbl %al,%edx;
    // mov 0x8(%ebp),%eax; mov %edx,0x8(%eax)`（RHS 值先 + disp store）。
    // int& 视图（同型）在 4.4 产 LHS 地址先物化（`add $0x8,%eax; mov %edx,(%eax)`）；
    // 按 P211 档③ 用**异型 RHS**（signed char → enum 字段）触发转换中转翻装载序。
    m_creatureItemType =
        static_cast<CREATURE_ITME_TYPE>(static_cast<signed char>(info->m_creatureItemType));
    m_stomach.SetStomachValue((float)info->m_stomachValue);
    strncpy(m_name, info->m_name, 0xc);            // → +0x1c，12B（ORIG call strncpy PLT）
    // ORIG 8338657：`movzbl 0x4c(%eax),%eax; xor $0x1,%eax; test %al,%al; je`
    // ⇒ P248 档②/P292 字段 bool 直证：m_isGrow 本身已是 bool（CGameManager.h:367）
    // ⇒ 删具名 uchar 局部（4.4 恒落栈槽 `mov %al,-0xd; xorb $1,-0xd; cmpb $0,-0xd`
    // 并顶掉后续全部槽 4B）、写裸 `if (!m_isGrow)`。
    // 高 16 位提取走 union 拆半（P197①：整字落槽 -0x10 → movzwl -0xe + cwtl 直传）。
    if (!m_isGrow)
    {
        union { int all; unsigned short w[2]; } u;
        u.all = m_exp;
        m_level = GetExpLevel((int)(short)u.w[1]);
        info->m_growExpFlag = 0;
    }
    else
    {
        m_level = GetExpLevel(m_exp);
        info->m_growExpFlag = 1;
    }
    info->m_isMaxLevel = isMaxLevel();
    m_noCharge = (info->m_noCharge != 0) ? 1 : 0;
    m_stat = info->m_stat;
    m_expireTime = info->m_expireTime;
    if (m_isGrow != 0)
        m_stomach.Resume();
    SetState(user, CREATURE_STATE_2);
    m_saveFlag = 1;
    // ORIG：两独立守卫共享失败出口（cmp;jne L + call;xor;test;je L，P133 独立守卫形；
    // 合并 && 会致 4.4 布尔汇聚 mov$1/jmp/mov$0 岛）。
    if (info->m_slotNo == 0xee)                        // 0xee=装备槽编码（0xd8+0x16）
    {
        if (!mgr->Equip(this))
            return false;
    }
    if (!mgr->RegisterCreatureItem(reinterpret_cast<CCreatureItem*>(this),
                                   info->m_uid))
        return false;
    // P197③双槽置 0：ORIG 块入口 `movl $0,-0xc` 预清 + 后续独立赋值两段式
    //（`T* p=0;` + `p=f()` 合并形缺 init 行）。
    Inven_Item* slotItem = 0;
    slotItem =
        user->getCurCharacInvenW()->GetInvenRef(
            3, reinterpret_cast<CCreatureItem*>(this)->GetSlotNo());
    slotItem->set_add_info(m_uid);
    // 残差 N76：ORIG `mov %al,0x31(%edx)` RHS 值先载；4.4 恒 LHS 地址先载
    //（mov %eax,%edx 中转 + mov 0x14(%ebp),%eax），probe 全试不可翻。
    m_saveFlag = m_stomach.IsSaveStomach((CREATURE_STATE)m_state) ? 1 : 0;
    return true;
