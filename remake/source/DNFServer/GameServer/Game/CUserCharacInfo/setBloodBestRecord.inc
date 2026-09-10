void CUserCharacInfo::setBloodBestRecord(unsigned int time, int round, int record, bool flag)
{
    // [b28 BM8 P223] ORIG 三空块中继岛（+0x24c/0x24f/0x252 `nop;jmp`）：
    // m_selected==0 空 else、`!(best<=round)`、`best==cur 全等` 短路空出口。
    // 4.4 jump-threading 折叠空块 ⇒ 改 goto+尾空标签形逐岛复现。
    if (m_selected == 0)
        goto L_SBR_END;
    {
    if (round > 0xf)
        round = 0xf;
    {
    std::map<int, stBloodBestRecord>::iterator it =
        CUR->m_bloodBestMap.m_map.find(record);
    if (it == CUR->m_bloodBestMap.m_map.end())
    {
        if (!flag)
            setBloodBestRecordUpdate(true);
        // ORIG（0x086880a3-0x086880c4）实参槽序：0x4(%esp)=lea 0x14(%ebp)
        // （&record），0x8(%esp)=lea -0x38(%ebp)（&local{-0x38=round,-0x34=time}）
        // ⇒ make_pair(record, local)。旧形把两实参对调且把 record+flag 裸字节
        // 重读成 Blood 值——真语义 bug（P196 族），按 ORIG 恢复：key=record、
        // value=stBloodBestRecord{m_round=round,m_time=time} 具名局部。
        stBloodBestRecord local;
        local.m_round = round;
        local.m_time = time;
        CUR->m_bloodBestMap.m_map.insert(
            std::make_pair<int&, stBloodBestRecord&>(record, local));
        CUR->m_bloodBestRound = round;
        CUR->m_bloodBestTime = time;
    }
    else
    {
        CUR->m_bloodBestRound = it->second.m_round;
        CUR->m_bloodBestTime = it->second.m_time;
        if (round < CUR->m_bloodBestRound)
            goto L_SBR_END2;
        if (CUR->m_bloodBestTime == time &&
            CUR->m_bloodBestRound == round)
            goto L_SBR_END3;
        {
            if (CUR->m_bloodBestRound == round)
            {
                setBloodBestRecordUpdate(true);
                CUR->m_bloodBestTime = std::min(CUR->m_bloodBestTime, time);
            }
            else
            {
                setBloodBestRecordUpdate(true);
                CUR->m_bloodBestTime = time;
                CUR->m_bloodBestRound = round;
            }
            it->second.m_round = CUR->m_bloodBestRound;
            it->second.m_time = CUR->m_bloodBestTime;
        }
    }
    }
    }
L_SBR_END3:;
L_SBR_END2:;
L_SBR_END:;
