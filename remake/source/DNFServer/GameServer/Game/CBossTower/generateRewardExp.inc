// ----------------------------------------------------------------------------
// ORIG T 0x08144d54 _ZN8WongWork10CBossTower17generateRewardExpEv：
// 逐有效成员：get_mob_reward(charac_level) 得基础奖励经验后直调基类
// CBossPlay::generateRewardExp(i, m_stageCur, mobReward, m_dungeon)（ORIG
// call 08149e20 非虚）。恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08144d54：CBossPlay::generateRewardExp 保持 int ⇒ 缺 ORIG
//   `call; xor $1; test %al; jmp` 死物化（P197④ + P225 dead 变体）；callee
//   bool 化跨 TU（CBossPlay.h 虚槽）不可行。invalid 局部已去、调用序列全等。
// [b28d-solo-CBossTower] AE（66:66，本波翻正）：Main 代落 CBossPlay.h:57
//   generateRewardExp int→bool 后，循环体尾改 P225 dead 变体
//   `while (!f()) break;` 复现 ORIG 0x08144e10 `call; xor $1; test %al,%al; jmp`。
bool CBossTower::generateRewardExp()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // b28d-54 翻正：ORIG 0x08144d7e 段 `test %al,%al; jne` 直测（无 bool 槽）
        // ⇒ 去具名 invalid 局部，条件内联（P0：无槽往返 ⇒ 无具名局部）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        unsigned int mobReward = 0;              // ebp-0x14
        G_CDataManager()->get_mob_reward(member->get_charac_level(), mobReward);
        // [b28d-solo-CBossTower] P225 dead 变体：ORIG 0x08144e10 `call;
        // xor $1,%eax; test %al,%al; jmp`（bool callee 取反物化后无条件跳，
        // test 无消费者）⇒ 源形 `while (!f()) break;`（callee 已由 Main
        // 代落为 bool）。
        while (!CBossPlay::generateRewardExp(
                   i, static_cast<unsigned int>(m_stageCur), mobReward, m_dungeon))
            break;
    }
    return 1;
}