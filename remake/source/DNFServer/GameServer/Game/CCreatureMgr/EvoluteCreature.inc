bool CDBMsgSender::EvoluteCreature(int uid, int itemIdx)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x40;
    // ORIG 0x833fdc2：先 +0x30=uid，后 mov 0x10(%ebp),%edx; mov -0xc(%ebp),%eax;
    // mov %edx,0x8(%eax)（值先基后、单段 disp8）。4.4 store 恒基先+add 折叠
    // （N76 口径），probe register 钉 %edx/%eax 均反产多余 mov，维持最简形。
    buf->m_addInfo2 = uid;                            // +0x30
    // [b28d-solo-CCreatureMgr P235] cast 形改具名视角直访问（完整类型成员）：
    // ORIG `mov 0x10(%ebp),%edx; mov -0xc(%ebp),%eax; mov %edx,0x8(%eax)`。
    buf->m_itemView.m_itemIdx = itemIdx;              // buf+0x08 disp8
    return true;
