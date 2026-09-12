void CDungeonEntranceLog::IncrementDungeonEntrance(int dungeonIdx, bool isParty)
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    // [b28d-59] AE（ORIG 0x860e1ee）：指令序列逐条一致，.o 层仅 call 重定位标签
    // 未解析的显示差（AE 口径归零）；95/95。
    // ORIG 0x860e1ee：**两条路径**的 isParty 分支均只 ++m_partyEnter（不加 total/curInside）。
    //   铁证（ORIG 0x860e2bb 区，插入路径）：`cmpb $0x0,-0x5c(%ebp); je 0x860e2bd`
    //   （isParty==0 → ++m_enter@-0x50）→ isParty 分支 ++m_partyEnter@-0x4c 后
    //   `jmp 0x860e2d8`（ORIG `eb 1b`）**直接跳过** ++m_total@-0x48 / ++m_curInside@-0x44；
    //   旧形把两条自增放到 if 外，生成 `jmp 0x860e2c6`（我方 `eb 09`）多累加 2 项
    //   ⇒ 真语义差异（2026-09-12 修正：两条自增移入 else 块）。
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.find(dungeonIdx);
    if (it != m_inoutMap.end())
    {
        if (isParty)
        {
            it->second.m_partyEnter += 1;
        }
        else
        {
            it->second.m_enter += 1;
            it->second.m_total += 1;
            it->second.m_curInside += 1;
        }
    }
    else
    {
        STDungeonInoutCnt cnt;
        if (isParty)
        {
            cnt.m_partyEnter += 1;
        }
        else
        {
            cnt.m_enter += 1;
            cnt.m_total += 1;
            cnt.m_curInside += 1;
        }
        m_inoutMap.insert(std::make_pair(dungeonIdx, cnt));
    }
}