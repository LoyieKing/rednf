bool CBossTower::handleMoveMap(unsigned short* mapIndex)
{
    // ORIG 0x081439ae `call; xor $1,%eax; test %al,%al; je <body>` + 失败路径
    // `mov $0,%eax; jmp end` ⇒ 原码 `if (!f()) return 0;`（P168a①裸形）。
    if (!CBossStage::handleMoveMap(mapIndex))
        return 0;

    for (int i = 0; i < 4; ++i)
    {
        CUser* member = m_party->get_user(i);
        if (member != 0 && isMemberAlive(i))
        {
            m_dungeonLog.IncrementBossStageStart(
                *member,
                static_cast<unsigned char>(m_stageCur),
                static_cast<unsigned char>(m_difficulty));
        }
    }
    return 1;
