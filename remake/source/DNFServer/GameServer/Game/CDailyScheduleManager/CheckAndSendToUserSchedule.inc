void CDailyScheduleManager::CheckAndSendToUserSchedule(CUser* user, bool flag)
{
    std::tm tm;                    // -0x98（localtime 用）
    std::time_t cur;               // -0x24
    unsigned int out;              // -0x28
    _DailySchedule* prev;          // -0x2c
    _DailySchedule* next;          // -0x30
    unsigned int count;            // -0x20
    int wday;                      // -0x1c
    unsigned int i;                // -0x18
    _DailySchedule* entry;         // -0x14
    bool changed;                  // -0xd
    int state;                     // -0xc

    count = m_schedules.size();
    cur = GlobalData::s_systemTime_.getCurSec();
    localtime_r(&cur, &tm);
    wday = tm.tm_wday;
    i = 0;
    while (i < count)
    {
        entry = &m_schedules[i];
        out = 0;
        changed = false;
        if (user == 0)
        {
            state = CheckScheduleState(*entry, out, flag);
            if (state == 1)
            {
                if (entry->m_state == 0)
                {
                    entry->m_state = 1;
                    changed = true;
                }
            }
            else if (state == 0)
            {
                if (entry->m_state == 1)
                {
                    entry->m_state = 0;
                    changed = true;
                }
            }
        }

        if (changed)
        {
            prev = 0;
            next = 0;
            if (CheckContinueSchedule(*entry, &prev, &next))
            {
                if (entry->m_state == 0)
                {
                    if (next)
                        next->m_state = 1;   // ORIG 0x8125e15
                    changed = false;
                }
                else if (entry->m_state == 1)
                {
                    if (prev)
                        prev->m_state = 0;   // ORIG 0x8125e37
                    changed = false;
                }
            }
        }

        if (changed)
        {
            // 广播 0x1ed：{type(+4), state(+0x20), idx(+8), out} 各 4 字节。
            PacketGuard packet;
            int body[4];               // -0x4c..-0x40
            packet.put_header(0, 0x1ed);
            body[0] = entry->m_type;
            body[1] = entry->m_state;
            body[2] = entry->m_idx;
            body[3] = (int)out;
            packet.put_binary((char*)body, 0x10);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }
        else if (!changed && entry->m_state == 1 && user != 0)
        {
            // 单播：state 字段固定 2（ORIG 0x8125f58）。
            PacketGuard packet;
            int bodyU[4];              // -0x68..-0x5c
            packet.put_header(0, 0x1ed);
            bodyU[0] = entry->m_type;
            bodyU[1] = 2;
            bodyU[2] = entry->m_idx;
            bodyU[3] = (int)out;
            packet.put_binary((char*)bodyU, 0x10);
            packet.finalize(true);
            user->Send(packet);
        }
        ++i;
    }
