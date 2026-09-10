int CUser::GetItemSerialChecker(unsigned short serialIdx, unsigned short serialVal,
                                int monsterIdx)
{
    // b28d-solo-W8_CU_A：AE 复原（原 b28d-35「N82 帧/槽 + bool 物化」判词作废）。
    //   ORIG @0x0823066a 帧 0x34（与本重建同）；三处修法：
    //   ① 删具名 bool inWindow（4.4 恒物化 `movb %al,-0x12; movzbl; xor $1; test`
    //   多 4 条）⇒ 四条件直写 if 头，失败路径即 ORIG 的 `jl/jg <reset 尾块>`；
    //   ② 删具名 bool hit 与 `CBattle_Field& bf` 引用（引用强制 `add $0xb24`
    //   具名槽）⇒ 经 p->m_battleField 直接访问，复现 ORIG 0x0820729 段
    //   `movzbl 0xcec(%eax)`/`mov 0xce0(%eax),%ebx` disp 折叠；
    //   ③ if/else 结构（reset 为 else 后置）= ORIG 0x082077e 尾块布局。
    if (((int)(short)serialIdx - (int)m_mcap.m_serialLastIdx) >= -23 &&
        ((int)(short)serialIdx - (int)m_mcap.m_serialLastIdx) <= 23 &&
        ((int)(short)serialVal - (int)m_mcap.m_serialLastVal) >= -17 &&
        ((int)(short)serialVal - (int)m_mcap.m_serialLastVal) <= 17) {
        CParty* p = static_cast<CParty*>(GetParty());
        if (p != NULL && p->isMonsterDrop_item(monsterIdx) &&
            (!p->m_battleField.m_visitedGotoFlag ||
             p->m_battleField.m_hellMapIdx !=
                 p->m_battleField.get_cur_map())) {
            ++m_mcap.m_serialHitCount;
        }
    } else {
        // 重置：命中计数清零、记录本次基准对
        m_mcap.m_serialHitCount = 0;
        m_mcap.m_serialLastIdx_u = serialIdx;
        m_mcap.m_serialLastVal_u = serialVal;
    }
    return m_mcap.m_serialHitCount;
