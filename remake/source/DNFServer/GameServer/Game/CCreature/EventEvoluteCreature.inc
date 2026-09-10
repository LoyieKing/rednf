bool user_creature::CDBMsgSender::EventEvoluteCreature(int uid, int itemIdx, int exp)
{
    // b28d-37 [N76 封顶复核，ORIG 0x833fddc]：P248 档③字段侧旋钮已试（m_itemIdx int→unsigned 后 store 仍 LHS-first，与 D23 回退实测一致），源码侧直赋/语句表达式亦不可翻；ORIG RHS-first 3 条形保留 N76。
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x248;
    buf->m_addInfo2 = uid;                            // +0x30（先写，ORIG 序）
    // ORIG 833fe39：`mov 0x10(%ebp),%edx; mov -0x10(%ebp),%eax; mov %edx,0x8(%eax)`
    // = RHS-first 3 条形。旧判词按 N76 封顶，根因其实是**视图 cast + 同型字段**：
    // P235 直取完整类型具名视角（`buf->m_itemView.m_itemIdx`）配合 P211 档③
    // **异型化**（m_itemIdx 为 unsigned int，RHS 为 int 形参）即翻回 ORIG 序。
    buf->m_itemView.m_itemIdx = itemIdx;              // +0x08
    buf->m_exp = exp;                                 // +0x4c（SigTypes 已具名 m_exp）
    CItem* item = G_CDataManager()->find_item(itemIdx);
    buf->m_days = item->getUsablePeriod();            // +0x54
    buf->m_expireTick = item->getExpirationDate();    // +0x58
    return true;
