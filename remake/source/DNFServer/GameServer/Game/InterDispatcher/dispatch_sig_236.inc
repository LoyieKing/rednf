// [b28d-lane2] verdict=DIFF ndiff=85（ORIG 0x084e4774）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+9。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=87（ORIG 0x84e4774）：P226 具名 bool/char 落槽+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_Antibot_PunishClient::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Antibot_PunishClientPktView* pPkt = reinterpret_cast<Inter_Antibot_PunishClientPktView*>(data);
    // §P0：ORIG 084e4782/084e479c 两次独立重读 msg+0x0（uid 无槽），
    //   084e4792/084e47f8 两次独立重读 msg+0x4（punishMode 留 %esi / 重载）⇒ 不设具名缓存。
    CUser* target = getUser(pPkt->m_uid);

    // ORIG 0x84e4792..0x84e47b1：punish_mode→%esi、NumberToString→%ebx（均无槽往返）
    // ⇒ 全表达式临时链，无 uidStr 具名局部（P0/P328⑩/P227）。
    cMyTrace(__PRETTY_FUNCTION__, 0x5eb3, 0)(
        "requested uid(%s), punish_mode(%d), user(%x)",
        NumberToString(pPkt->m_uid, 0), (int)pPkt->m_type, target);

    // ORIG 0x84e4802..0x84e480f 决策链 cmp $2/cmp $3/cmp $1 ⇒ 原码 switch（P17/P251）。
    // [K3 负结论 2026-09-11] 按 ORIG 判定序把 case 1 移到末尾（2→3→1）：我方 94→92 条，
    //   hunks 5→6，且 ORIG 的 `cmp $0x1,%eax` 被我方 4.4 与 default 合并 ⇒ 劣化，已回滚。
    switch (pPkt->m_type)
    {
    case 1:
        break;
    case 2:
        // [N 判词·2026-09-11] ORIG 084e4818 `je→mov $1` 岛 / 084e4827 `jne→mov $0` 岛（负极性
        //   物化 + `jne <skip>`）vs 我方 `je <body>`（正极性）。试形：P226 语句表达式包负条件
        //   `!({ target==0 || get_state(target)==0; })` 实测 **劣化**（94→99 条、ndiff 5→6）⇒ 回退。
        //   残差 = 4.4 对 `&&` 短路恒折条件跳转（P225 推广域）+ N21 jcc 极性 ⇒ 封 N。
        if (target != 0 && target->get_state() != 0)
            target->DisConnSig((DISCONN_SIG)0x23, true, 0);
        break;
    case 3:
        if (target != 0 && target->get_state() != 0 &&
            0 < pPkt->m_c)
        {
            target->DisConnSig((DISCONN_SIG)0x23, true, 0);
        }
        break;
    }
    return 0;
}