void CGameManager::WarRoomCountManageTest(int hourIndex)
{
    // b28d-lane12: N82/N93 槽平移封顶（orig=248 our=253 gap=-5；ndiff 307）。
    // ORIG 0x082a2c08。
    // b28d-solo-CGameManager 判词：N78（ORIG `mov $0xa,%edx; rep stos` 内联 vs 4.4
    // `call memset`）+ N82 槽平移。
    // 同 WarRoomCountManage：零初始化声明复现 ORIG `rep stos` 段（P214③/P253，
    // 探针 /tmp/solo_probe4.cpp m4），清 10 dword（40B）。
    int current[10] = {0};
    int total = 0;
    for (int area = 0; area < 10; ++area)
    {
        int n = 0;
        // 同 WarRoomCountManage：peek 直连 cmp，无 need 槽（ORIG 0x082a2dXX 孪生段）。
        while (((CDataManager*)G_CDataManager())
                   ->m_warAreaCounter.GetWarRoomCountAtPeekTime(area) > n)
        {
            WarRoom* room = GetWarRoom(total++);
            if (room == 0)
            {
                LogManager::logFormat(1, "App.cpp",
                                      __PRETTY_FUNCTION__, 0x30e2,
                                      "WarRoomCountManage Error : WarRoom does not exist %d", total);
            }
            else
            {
                if (room->GetState() != -1)
                    current[area] = current[area] + 1;
            }
            n = n + 1;
        }
    }
    int hourTable = hourIndex;
    PacketGuard guard;
    guard.put_header(0, 0x58);
    int index = guard.get_index();
    guard.put_short(0);
    int totalDiff = 0;
    for (int area = 0; area < 10; ++area)
    {
        // ORIG 0x082a2d65..0x082a2d82：G_CDataManager 结果无具名槽（`mov %eax,%ecx`
        // 直用），成员数组二维索引 ⇒ 单条 scaled 读。
        int diff = ((CDataManager*)G_CDataManager())
                       ->m_warAreaCounter.m_warRoomCount[hourTable][area] -
                   current[area];
        if (diff < 0)
        {
            int need = diff < 0 ? -diff : diff;
            int lastIdx =
                ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetWarRoomCountLastIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(lastIdx--);
                // ORIG 0x082a2dee..0x082a2e12：`room!=0 && state!=-1`（两 && 项 je 同岛）。
                if (room != 0 && room->GetState() != -1)
                {
                    char shut = room->ShutDown();
                    if (shut != 0)
                        room->MakeStateInfo((char*)&guard);
                    need = need - 1;
                }
            }
        }
        else if (diff > 0)
        {
            unsigned int need = (unsigned int)diff;
            int firstIdx =
                ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetWarRoomCountFirstIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(firstIdx++);
                // ORIG 0x082a2e98..0x082a2eba：`room!=0 && state==-1`（je + jne 同岛）。
                if (room != 0 && room->GetState() == -1)
                {
                    room->Open();
                    room->MakeStateInfo((char*)&guard);
                    need = need - 1;
                }
            }
        }
        totalDiff = totalDiff + (diff < 0 ? -diff : diff);
    }
    if (totalDiff != 0)
    {
        guard.put_short(index, totalDiff);
        guard.finalize(true);
        G_GameWorld()->send_all(guard);
        PrintWarRoomList();
    }
