void GameWorld::send_all_dungeon_inout_message(int dungeonIdx, bool open)
{
    PacketGuard packet;
    packet.put_header(0, 0x127);
    packet.put_int(dungeonIdx);
    packet.put_byte(open);
    packet.finalize(true);

    int minLevel = getDungeonMinimumRequiredLevel(dungeonIdx);
    if (minLevel != -1)
    {
        for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
             it != m_UserInWorld.end(); it++)
        {
            CUser* user = it->second;
            // ORIG 0x86c8…：两子条件各自 `setXX;test;je` 物化 ⇒ 单层 && 被 4.4
            // 折叠为直跳，唯嵌套 if 复现（子条件独立 if 形）
            if (user->get_state() > 2)
            {
                if (minLevel <= user->get_charac_level())
                {
                    user->Send(packet);
                }
            }
        }
    }
}