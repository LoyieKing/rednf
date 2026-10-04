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
        m_selected->m_bloodBestRecords.find(record);
    if (it == m_selected->m_bloodBestRecords.end())
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
        m_selected->m_bloodBestRecords.insert(
            std::make_pair<int&, stBloodBestRecord&>(record, local));
        m_selected->m_bloodBestRoundRecord = round;
        m_selected->m_bloodBestTimeRecord = time;
    }
    else
    {
        m_selected->m_bloodBestRoundRecord = it->second.m_round;
        m_selected->m_bloodBestTimeRecord = it->second.m_time;
        if (round < m_selected->m_bloodBestRoundRecord)
            goto L_SBR_END2;
        if (m_selected->m_bloodBestTimeRecord == time &&
            m_selected->m_bloodBestRoundRecord == round)
            goto L_SBR_END3;
        {
            if (m_selected->m_bloodBestRoundRecord == round)
            {
                setBloodBestRecordUpdate(true);
                m_selected->m_bloodBestTimeRecord = std::min(m_selected->m_bloodBestTimeRecord, static_cast<int>(time));
            }
            else
            {
                setBloodBestRecordUpdate(true);
                m_selected->m_bloodBestTimeRecord = time;
                m_selected->m_bloodBestRoundRecord = round;
            }
            it->second.m_round = m_selected->m_bloodBestRoundRecord;
            it->second.m_time = m_selected->m_bloodBestTimeRecord;
        }
    }
    }
    }
L_SBR_END3:;
L_SBR_END2:;
L_SBR_END:;
}