bool ProcStage::ProcSummonObjectMs::dieMob(unsigned short mobSeq, unsigned short param,
                                          const MSG_MONSTER_DIE& msg)
{
    (void)param;  // ORIG 未使用
    std::map<short, _SummonObjectMs*>::iterator it = m_summonMap.find((short)mobSeq);
    // b28d-solo-ProcStage：ORIG 0x0812db53 起正向包裹（`test %al; je <尾 mov$0>` 单出口），
    //   且 `it->second` 每次重发 operator->（P48/P167：0x812db73/0x812db84/0x812dbb1/
    //   0x812dbe9 四处 `call operator->`），`so` 仅作首次结果缓存；
    //   m_deadFlag 判读为 `movzbl; xor $1; test; je`（P248 档② bool 字段 + 裸 `!`）。
    if (it != m_summonMap.end())
    {
        _SummonObjectMs* so = it->second;
        // P171 死写入照抄：ORIG 0x812db7e~0x812db8f 第二次 operator-> 取
        //   it->second->m_sentFlag 存 -0x10 后从不读取。
        const int sentFlag = it->second->m_sentFlag;
        (void)sentFlag;
        if (so->m_sentFlag == 1 && !so->m_deadFlag)
        {
            it->second->m_deadFlag = true;
            ++m_deadCount;
            CUser* user = m_characData->getUser();
            if (user != 0)
            {
                HistoryLog::killMonster(*user, it->second->m_summon.m_id, msg);
            }
            return 1;
        }
    }
    return 0;
