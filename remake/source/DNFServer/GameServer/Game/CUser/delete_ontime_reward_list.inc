bool CUser::delete_ontime_reward_list()   // b28c-L5 SIGFIX（P174）
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x33);
    // b28d-PoolCUser 重写（2026-09-09）：ORIG 0x0868c3bc 物理块序 = [main: xor;test;je
    //   DEL] [mov$0;jmp end] [TRACE 块 +0x4c] [DEL 块 +0x89 fall-through 尾]。
    //   IsEventing 真 → je 直达尾 DEL 块（return 1 fall 到 leave）；假 → 落 mov$0 岛。
    //   ⇒ 用 goto DEL 把 delete 块物理后置（P223 尾标签系）。
    //   注：改顺序 if 结构（TRACE 内联 if 体）实测反劣 5→27 行，已回退。
    if (ev == 0)
        goto TRACE;
    if (!ev->IsEventing(0))
        return 0;
    goto DEL;
TRACE:
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa828, 5)
            ("[ON_TIME_EVENT]delete_ontime_reward_list() error");
        return 0;
    }
DEL:
    ((COnTimeEvent*)ev)->delete_reward_user(this);
    return 1;
