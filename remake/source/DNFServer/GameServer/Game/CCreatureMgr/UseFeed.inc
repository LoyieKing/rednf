bool CCreature::UseFeed(CUser* user, int& stomachValue)
{
    if (m_isGrow != 1)
        return false;
    CPacketRespondent pkt(user, 0, 0);
    if (IsDieCreature())
    {
        pkt.MakeNotipacketRevivalCreature();
        pkt.SendAll();
    }
    stomachValue = (int)m_stomach.IncrementValue(30.0f);
    // [b28d-solo-CCreatureMgr P0/P183] ORIG 0x8337470 `mov 0x10(%ebp),%eax;
    // mov (%eax),%ebx`（直读 out 形参入 callee-saved，无具名 value 槽）+ 
    // 0x833747b 外联 `call CCreatureItem::GetUid`（非 m_uid 直读）⇒ 删缓存
    // 局部、uid 改真调用（P167 调用点差集口径）。
    pkt.MakeNotipacketCreatureState(
        reinterpret_cast<CCreatureItem*>(this)->GetUid(), stomachValue);
    // [b28d-solo-CCreatureMgr P32] ORIG 0x8337499 `call Send; mov %eax,%ebx`
    // （bool 直透，无 test/setne）⇒ 去掉 `!= 0` 物化。
    return pkt.Send();
