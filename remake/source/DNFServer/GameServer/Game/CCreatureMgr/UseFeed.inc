// ORIG UseFeed(CUser*, int&) T @ 0x083373d4：
// 仅 +0x4c==1 可喂；死亡则 MakeNotipacketRevivalCreature + SendAll；
// IncrementValue(30.0f) 截断写入 out；MakeNotipacketCreatureState(uid,out)
// 后 Send()，返回其值。
// [b28d-23 N·N82] 帧/槽差（79/81）⇒ N。ORIG 0x083373d4
// [b28d-solo-CCreatureMgr N] bool 规范化：`return pkt.Send()`（int→bool）4.1 直透 `mov %eax,%ebx` vs 4.4 test/setne；P183 去 value 槽与 GetUid 真调用已落；ORIG 0x83373d4
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
}
