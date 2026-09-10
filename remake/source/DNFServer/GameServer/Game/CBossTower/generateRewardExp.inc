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
