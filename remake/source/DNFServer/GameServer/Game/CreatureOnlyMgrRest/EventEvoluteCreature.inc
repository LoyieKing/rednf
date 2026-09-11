// ORIG _ZN13user_creature12CCreatureMgr20EventEvoluteCreatureEiii T 0x0833da1e
// （EventEvoluteCreature.md）：等级门槛不足 trace 0x10c2；进化成功广播列表包
// MakeNotipacketCreatureItemList(map*,0xee) 后返 1。
bool CCreatureMgr::EventEvoluteCreature(int creatureId, int minLevel, int slot)
{
    if (m_equippedCreature != 0)
    {
        // b28d-61 [P222 正向包裹翻极性，ORIG 0x0833da3x `cmp 0x10(%ebp),%eax;
        // setge %al; test %al,%al; je +0xcc`]：旧「GetLevel()<minLevel 早判」
        // 产 setl 且 trace 块落 fall-through；改正向 if 后 trace 归 else。
        if (m_equippedCreature->GetLevel() >= minLevel)
        {
            if (m_equippedCreature->EventEvolute(m_user, creatureId, slot))
            {
                CPacketRespondent pkt(m_user, 0, 0);
                pkt.MakeNotipacketCreatureItemList(&m_creatureItems, 0xee);
                return true;
            }
        }
        else
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x10c2, 0);
            trace("event evolute creature error : low creature level.");
        }
    }
    return false;
}