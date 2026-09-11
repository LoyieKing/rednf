// ORIG T 0x085baa06（_Send_TOD_ClearReward.md）：0x15b 包——管理者的 layer
// 扩展数据最后进入层 + 最多 9 组奖励清单（不足补 -1/0）+ 特殊奖励索引/数量。
// b28d-solo-CPartyOnlyOrig 判词：DIFF（N 封顶）@0x085baa06。修法已落：具名
// userState 指针落槽（P0 槽往返）+ 匿名 sret 全表达式链（P278）+
// `m_battleData.m_field0` 具名成员访问 ⇒ packet 槽 -0x24 与 ORIG 对齐。
// 残余 N 封顶：4.1 `mov 0x330(%eax),%edx` 冗余装载（N18a）与首循环前
// `call size; cmp $9` 死比较（4.1 tail-duplicator 产物）。语义等价。
int CParty::_Send_TOD_ClearReward()
{
    PacketGuard packet;
    packet.put_header(0, 0x15b);
    // b28d-solo：ORIG +0x2e/+0x31 为 `mov 0x330(%eax),%edx; mov 0x32c(%eax),%eax`
    // ——BattleData.h P180 u64 别名（m_clearPlayedTime）的**64 位整体装载后取低
    // 32 位**（与 SetPlayResult 0x085b2457 同形），非单 int 直读；单读少 1 条。
    packet.put_int(static_cast<int>(m_battleData.m_clearPlayedTime));
    // b28d-solo-CPartyOnlyOrig：ORIG +0x67 `mov %eax,-0x14` 指针落槽（P0 槽
    // 往返 ⇒ 原码有具名指针局部，此前留 edx 无槽）⇒ 补具名 userState；
    // +0x77 getLastEnterLayer 用 sret 匿名临时 -0x16 后直接 GetLayer
    //（P278 匿名 sret 全表达式链）。ORIG 无条件（无 null 检查）。
    TOD_UserState* userState = reinterpret_cast<TOD_UserState*>(
        getManager()->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)));
    packet.put_short(static_cast<unsigned short>(
        userState->getLastEnterLayer().GetLayer()));
    packet.put_byte(10);   // ORIG +0x09c 固定 byte 10
    // [P171/P184 死块照抄] ORIG +0x0af..+0x0bf：`add $0x17a8,%eax` + 一次 size()
    // 调用 + `cmp $0x9,%eax`（无分支消费者）。探针 /tmp/probe_hnq/t.cpp h8 实证：
    // 空 if 体被 4.4 前端删除但比较与调用保留，逐条同形（`> 9` 无符号比较）。
    // ⚠ b28d-solo 语义修正：基址必须是 m_battleField（CParty+0xb24，+0xc84
    // → 0x17a8 = ORIG disp）。旧轮写 `&m_battleData`（+0x32c）产
    // `add $0x32c; add $0xc84` 两条且**读到错地址 0xfb0**（越出 BattleData
    // 0x7f8 尾），既多 1 条指令又与 ORIG 语义不符。
    if (reinterpret_cast<CBattle_Field*>(&m_battleField)
            ->m_todReward.m_todMap.size() > 9)
    {
    }
    // P48/P157：size()/operator[] 每使用点独立求值（ORIG 循环条件每轮
    // reload this + call size，无缓存）。
    for (unsigned int i = 0;
         i < reinterpret_cast<CBattle_Field*>(&m_battleField)
                 ->m_todReward.m_todMap.size();
         ++i)
    {
        packet.put_int(
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->m_todReward.m_todMap[i].first);
        packet.put_int(
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->m_todReward.m_todMap[i].second);
    }
    for (unsigned int i = 0;
         i < 9U -
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->m_todReward.m_todMap.size();
         ++i)
    {
        packet.put_int(-1);
        packet.put_int(0);
    }
    packet.put_int(reinterpret_cast<CBattle_Field*>(&m_battleField)
                       ->m_todReward.m_todPos.first);
    packet.put_int(reinterpret_cast<CBattle_Field*>(&m_battleField)
                       ->m_todReward.m_todPos.second);
    packet.finalize(true);
    send_to_party(packet);
    return 1;
}