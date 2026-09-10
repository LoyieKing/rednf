bool CDBMsgSender::InsertCreatureItem(CUser* user, Inven_Item* item, int slot,
                                      int reason, int mapSize, int days,
                                      int expireTick)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    // ORIG：`lea 0x4(%eax),%edx` 临时 this（无 compat 槽）。【N96/P0 违例清零】
    // 历史 register asm("eax") 钉装填序已删，回与 RecvMail/SendMail 同构裸形；
    // 残差 lea-vs-add + store 序 = N81/N76。
    buf->m_itemView.setCopy(*item);   // [b28d-solo P235] 具名视角出 lea（N107 边界）
    buf->m_slot = slot;                               // +0x2c
    // [b28d-solo-CCreatureMgr N·N7/N18a 封顶] ORIG `call; mov %eax,%edx;
    // mov -0x14(%ebp),%eax; mov %edx,0x48(%eax)`（call 返回值经 edx 中转 + 基址后载）
    // vs 4.4 `mov -0x14(%ebp),%edx; mov %eax,0x48(%edx)`。已试形：裸 int（同型
    // LHS-first）、(unsigned int) cast、enum cast、语句表达式（反产 lea+mov 各 1 条）
    // 均不闭合；字段 unsigned 化可翻正本点但使 RecvMail/SendMail 同字段反向
    // （P248 两端冲突反例，实测回退）⇒ 跨版本 call 返回值寄存器分配差异封顶。
    buf->m_characNo = user->getCurCharacNo();  // +0x48
    buf->m_stomachValue = 100;                        // +0x44（固定 100）
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_mapSize = mapSize;                         // +0x68
    buf->m_days = days;                               // +0x54
    buf->m_expireTick = expireTick;                   // +0x58
    // [b28d-solo-CCreatureMgr P211] 去冗余 (int) cast：字段已 unsigned 同型化
    // （ORIG 0x833fb65 LHS-first 直存，无 edx 中转）。
    buf->m_ceraOrderNo = user->get_cera_item_order_no();       // +0x60

    // 【N96/P0 违例清零】register asm("ebx") 已删；P197② 裸语句表达式复现
    // ORIG `mov 0x2(%eax),%eax; mov %eax,%ebx`…`mov %ebx,0x4(%esp)` callee-saved 直传。
    CItem* found = G_CDataManager()->find_item(({ item->m_addInfo; }));  // +0x02
    if (found == 0)
        return false;
    if (found->is_stackable())
        return false;

    CEquipItem* equip = static_cast<CEquipItem*>(found);
    if (equip->get_sub_type() == 0)
    {
        buf->m_flag50 = 1;                            // +0x50
        buf->m_field64 = 6;                           // +0x64
    }
    else
    {
        buf->m_flag50 = 0;
    }
    return true;
