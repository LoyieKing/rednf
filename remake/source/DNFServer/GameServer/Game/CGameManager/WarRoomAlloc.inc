void CGameManager::WarRoomAlloc()
{
    // b28d-solo-CGameManager: ORIG 0x08298a30（77 insn）真形 = Ghidra C 双计数：
    //   roomSeq(-0x14, char, 初 0, 写 room+1 后 +1) 与 i(-0x10, 用于循环条件与
    //   SetState 比较) 是两个独立变量；原重建误用单 count 兼顾二者（语义/槽双错）。
    //   内循环为 4.1 底部条件布局（+0x36 jmp +0xc4）⇒ P283 goto 尾标签复现；
    //   dm/hourTable 去具名缓存（P0/P183：ORIG 无 dm 槽，hourTable 直接 -0x1c）。
    // b28d-solo-CGameManager 判词：DIFF·N82（gap 1，76 vs 77）。ORIG 帧 0x34+push %ebx
    //   vs 我方 0x38——%ebx 是 4.1 对表索引加法 `add %ebx,%eax` 的 area 副本
    //   （0x08298aa7 `mov -0x18(%ebp),%ebx`），4.4 恒用内存操作数（N18a/P0 族）；
    //   本波 dm3 具名删（内联 G_CDataManager）后 diff 行 51、p167 CLEAR。
    int hourTable = ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetCurrenTimeTable();
    for (int area = 0; area < 10; ++area)
    {
        // ORIG `addl $0x1,-0x14` + `mov %dl,0x1(%eax)` ⇒ roomSeq 为 int（Ghidra
        // 显示 char 仅因只取低字节）；声明 char 会产 movb/movzbl 三段（实测）。
        int roomSeq = 0;
        int i = 0;
        goto inner_check;
    inner_body:
        {
            WarRoom* room = GetWarRoom();
            // ORIG 0x8298a81/+0x05b：直写 +0x00=area、+0x01=roomSeq（区内序号）。
            room->m_levelBand = (char)area;
            room->m_roomSeq = (char)roomSeq;
            roomSeq = roomSeq + 1;      // ORIG +0x05e 在 Prepare 之前
            room->Prepare();
            // ORIG 0x08298a9d..0x08298aba：G_CDataManager 结果**无具名槽**（`mov %eax,%ecx`
            // 直用），表寻址 (hourTable*10+area+0x223c)*4+8 —— P0/P183 内联表达式。
            // ORIG 0x08298aa4..0x08298aba：成员数组二维索引 ⇒ 4.4 与 4.1 同发
            // `add $0x223c,%eax; mov 0x8(%ecx,%eax,4),%eax`（探针 f1；
            // reinterpret_cast<int*>(arr)[...] 指针索引恒折 add+shl+lea，f2/f3）。
            if (i < ((CDataManager*)G_CDataManager())
                         ->m_warAreaCounter.m_warRoomCount[hourTable][area])
                room->SetState((WARROOM_STATE)0);
            else
                room->SetState((WARROOM_STATE)-1);
            i = i + 1;
        }
    inner_check:
        if (((CDataManager*)G_CDataManager())
                ->m_warAreaCounter.GetWarRoomCountAtPeekTime(area) > i)
            goto inner_body;
    }
    PrintWarRoomList();
