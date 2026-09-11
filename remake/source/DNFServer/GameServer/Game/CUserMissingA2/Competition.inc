// ----------------------------------------------------------------------------
// 24. ORIG T 0x081b3814 sz 0x353。竞技报名：Reward 预结算+状态/期次/地区校验；
// area m_worldMapIdx==0xffff 时随机选可报名地区（probArea + 有效地区表）；
// GetCompetitionPeriodSecond 非 0 → alter + 记录时间/期次/地区 + DB 日志。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82/N86：ORIG 0x081b3814；p167 CLEAR（30/30），残差为容器迭代器
// 临时槽 + 帧分配，语义闭合。
// [b28d-solo-CUserMissingA2] 修复：`if (!CheckCompetitionAreaIndex(...)) areaIdx=0;
// else areaIdx=idx;` 否定形复现 ORIG 0x081b3a4a `xor $1,%eax` 物化 + pArea 表达式
// 内联（消具名槽）⇒ 全段 diff 行 254→200。p167 CLEAR 30/30；残差帧 0x8c vs 0x7c +
// 槽序（N82/N93）封顶。
bool CHARAC_LOAD_MERCENARY::Competition(CUser* user, char periodIdx,
                                        char areaIdx)
{
    if (user == 0)
        return false;

    competition_reward_result result;
    Reward(user, result);
    if (GetCompetitionState() != 0)
        return false;
    if (!CheckCompetitionPeriodIndex(periodIdx))
        return false;
    if (!CheckCompetitionAreaIndex(areaIdx, m_level))
        return false;

    // ORIG 0x081b38d1：`call operator[]; movzwl 0x14(%eax),%eax` 直接内联
    // （无 pArea 具名局部槽）⇒ 表达式直读（P157/P183）。
    if (G_CDataManager()->m_mercenarySystem
            .m_vec40[static_cast<int>(areaIdx)].m_worldMapIdx == 0xffff)
    {
        static CMTRand probArea;
        std::vector<int> validIdx;
        for (int i = 0; i < (int)G_CDataManager()
                 ->m_mercenarySystem.m_vec40.size(); ++i)
        {
            stMercenaryCompetitionArea* pA = &G_CDataManager()
                ->m_mercenarySystem.m_vec40[i];
            // [b28d-solo-CUserMissingA2②] P287 同族：ORIG 0x081b3990 `movzx
            // eax,[eax+0x14]; test ax,ax` + 0x081b399c `cmp dx,ax; jg`（16 位
            // 有符号比较），4.4 默认提升 32 位（cwde）⇒ 源侧加 (short) 收窄。
            if ((short)pA->m_areaLevel != 0 &&
                (short)pA->m_areaLevel <= m_level)
                validIdx.push_back(i);
        }
        unsigned long range = validIdx.size() - 1;
        int pick = probArea.randInt(range);
        int idx = validIdx[pick];
        // ORIG 0x081b3a4b：`call CheckCompetitionAreaIndex; xor $1,%eax; test %al,%al;
        // je <idx 分支>`（bool 取反物化，P22/P292）⇒ 源码是否定形 if(!cond)。
        if (!CheckCompetitionAreaIndex(static_cast<char>(idx), m_level))
            areaIdx = 0;
        else
            areaIdx = static_cast<char>(idx);
    }

    // [b28d-solo-CUserMissingA2②] ORIG 0x081b3b?? `mov eax,[eax+0x28]; mov
    // edx,eax; add edx,[ebp-0x28]`（add mem 直加）；m_field28 已 unsigned，
    // 4.4 对 unsigned+int 生 lea ⇒ 试 seconds 同型化。
    unsigned int seconds = GetCompetitionPeriodSecond(periodIdx);
    if (seconds == 0)
        return false;

    alter();
    m_field28 = GlobalData::s_systemTime_.getCurSec();
    m_field2c = m_field28 + seconds;
    m_field30 = areaIdx;
    m_field31 = periodIdx;
    DB_MercenaryLogInfo::makeRequest(user->get_acc_id(), m_characNo, m_field28,
                                     m_field2c, 0, periodIdx, areaIdx,
                                     -1, -1, -1);
    return true;
}