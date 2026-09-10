void CGameManager::WarRoomCountManage(int hourIndex)
{
    // b28d-solo-CGameManager 判词：DIFF·N82（本波 N78 部分翻正）。ORIG 0x082a271e
    // 帧 0xec / 本波帧 0xfc：清零段已由 `int current[10]={0}` 逐条复现（P214③/P253，
    // 见下），余差为 4.4 为 rep stos 多发 push %edi + 槽组整体平移 0x10（ORIG 的
    // 4.1 内联 memset 用 %edi 却不保存——ABI 层跨版本差，无合法 C++ 表达）。
    // 本波 dm/dm2 具名删（P0/P183 内联 G_CDataManager）后 gap 0（311=311）、
    // diff 行 432→332；p167 CLEAR（42=42）。语义等价，字节层 N82 封顶。
    // b28d-laneCGM（P227）：判词「gap 0 / diff 432→332」阶段的具名 tr0-tr3 逐点改
    //   全表达式临时链（同 CheckOutPvp/insert_game_world 实测有效形）。
    cMyTrace(__PRETTY_FUNCTION__, 0x306f, 0)("WarRoomCountManage Start");
    // ORIG 0x082a2768 实证：`mov $0x0,%eax; mov $0xa,%edx; mov %ebx,%edi;
    // mov %edx,%ecx; rep stos` —— **零初始化声明** `int current[10] = {0};` 在
    // 4.4.6 下逐条复现该段（探针 /tmp/solo_probe4.cpp m4 与 ORIG 全等），
    // 而 `memset(current,0,sizeof)` 恒发 `call memset`（N78）。P214③/P253。
    // 语义：10 个 dword（40 字节）全清（旧注"仅清 10 字节"误，曾为真 bug）。
    int current[10] = {0};
    int total = 0;
    for (int area = 0; area < 10; ++area)
    {
        int n = 0;
        // ORIG 0x082a2818 COND：peek 返回值**不入槽**，直接 `cmp -0x44(n),%eax;
        // setg;test;jne`（我们旧形 int need=… 多两条槽 store/load）。
        while (((CDataManager*)G_CDataManager())
                   ->m_warAreaCounter.GetWarRoomCountAtPeekTime(area) > n)
        {
            WarRoom* room = GetWarRoom(total++);
            if (room == 0)
            {
                LogManager::logFormat(1, "App.cpp",
                                      __PRETTY_FUNCTION__, 0x307d,
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
    int hourTable;
    if (hourIndex == -1)
    {
        // P0/P183：ORIG 0x082a2870 `call G_CDataManager; add $0x87b4,%eax; mov %eax,(%esp)`
        // ——无 dm 具名槽，内联表达式。
        hourTable = ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetCurrenTimeTable();
    }
    else
    {
        hourTable = hourIndex;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x3088, 0)(
        "WarRoomCountManage currentHourIndex %d", hourTable);
    PacketGuard guard;
    guard.put_header(0, 0x58);
    int index = guard.get_index();
    guard.put_short(0);
    int totalDiff = 0;
    int area = 0;
    // ORIG 0x082a291d `jmp 082a2b28`（入口跳条件）+ 0x082a2b28 `cmpl $0x9,-0x3c;
    // setle; test; jne 082a2922`（回边）⇒ while 形（P25：前置 jmp + 尾部 setle 物化）。
    // [jcc 目标判据修正] 原 do-while 形缺 ORIG 的入口 jmp（jcc 33 vs 32），已改回
    // while；Ghidra C 的 `do{...}while(true)`+`if(9<area)` 是条件块序误读。
    while (area <= 9)
    {
        // ORIG 0x082a292f..0x082a293f `add $0x223c; mov 0x8(%ecx,%eax,4),%edx`：
        // m_warRoomCount[hour][area] 成员数组二维索引（WarAreaCounter+0x144）；
        // G_CDataManager 结果无具名槽。
        int diff = ((CDataManager*)G_CDataManager())
                       ->m_warAreaCounter.m_warRoomCount[hourTable][area] -
                   current[area];
        cMyTrace(__PRETTY_FUNCTION__, 0x3097, 0)(
            "WarRoomCountManage diff(%d) currentWarRoomCount(lev%d)", diff, current[area]);
        if (diff < 0)
        {
            int need = diff < 0 ? -diff : diff;
            int lastIdx =
                ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetWarRoomCountLastIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(lastIdx--);
                if (room == 0)
                    break;
                // ORIG 0x082a2a00..0x082a2a24：`room!=0 && state!=-1` 两 && 项各 je
                // 同一 mov$0 岛（探针 boolp.cpp h 形）；德摩根 or 形在 4.4 反产 jne。
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
                if (room == 0)
                    break;
                // ORIG 0x082a2ab7..0x082a2ad9：`room!=0 && state==-1`（je + jne 同岛）。
                if (room != 0 && room->GetState() == -1)
                {
                    room->Open();
                    room->MakeStateInfo((char*)&guard);
                    need = need - 1;
                }
            }
        }
        totalDiff = totalDiff + (diff < 0 ? -diff : diff);
        area = area + 1;
    }
    if (totalDiff != 0)
    {
        guard.put_short(index, totalDiff);
        guard.finalize(true);
        G_GameWorld()->send_all(guard);
        PrintWarRoomList();
        cMyTrace(__PRETTY_FUNCTION__, 0x30cf, 0)("WarRoomCountManage end");
    }
