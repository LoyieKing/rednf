void ProcStage::ProcSummonObjectMs::summonObjectAndSendPacket()
{
    // b28d-solo-ProcStage：ORIG 0x0812d660 先把 size() 缓存到 -0x10（P13），
    //   `so` 具名局部在 -0xc；判空用一次独立 operator[]（P48 无 CSE），
    //   m_sentFlag 条件再发第三次 operator[]。
    unsigned int count = static_cast<unsigned int>(m_summons.size());
    _SummonObjectMs* so;
    while (static_cast<unsigned int>(m_currentMs) < count)
    {
        // b28d-solo-ProcStage：P223——ORIG 0x0812d691 `sete %al; test %al,%al;
        //   jne 0812d7d9`（nop 占位岛）+ body 尾 `jmp 0812d7da` ⇒ 原码是
        //   `if (p == 0) goto L_next; body; L_next: ++m_currentMs;`（空 then 岛）。
        //   旧正向 `if (p != 0) {body}` 4.4 恒产 setne/je（差集观察）。
        if (m_summons[m_currentMs] == 0)
        {
            goto L_next;
        }
        so = m_summons[m_currentMs];
        if (so->m_ms > *m_pCurrentMs)
        {
            return;
        }
        // b28d-solo-ProcStage：P48/P157——ORIG 0x812d6cd/0x812d6e2 两处均读
        //   `-0xc(%ebp)`（so 缓存）而非重发 operator[]（差集旧形多 1 次
        //   `vector::operator[](unsigned)`）。
        if (so->m_ms <= *m_pCurrentMs && so->m_sentFlag == 0)
        {
            ++m_aliveDelta;
            so->m_sentFlag = 1;

            NOTIPacketStruct::_NotiAdvanceAltarSpawnUnit body;
            body.m_spawnIndex = static_cast<unsigned short>(so->m_seq);
            std::memcpy(&body.m_summon, &so->m_summon, 0x24);

            PacketGuard guard;
            guard.put_header(0, 0x20b);
            guard.put_binary((char*)&body, 0x28);
            guard.finalize(true);
            // [d03-T1b② 槽复用试形] ORIG 无 user 具名槽（我方-only -0x14 疑为该 4 字节
            //   局部致全帧平移 4）⇒ 内联表达式直调；一次旋钮，无改善即回滚。
            m_characData->getUser()->Send(guard);
        }
    L_next:
        ++m_currentMs;
    }
