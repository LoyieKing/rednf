// ============================================================================
// CDungeonSuccessNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x084851de
// m_partyMap.find(stat->m_charac[0x20] 处 int)：命中 → PlayInfo::operator+= 聚合
// （ORIG 走 Character::operator+= 语义）+ m_f3c 自增；
// m_jobMap.find(*reinterpret_cast<JobKey*>(stat+0x10))：命中 → CSuccessJobNode::UpdateCharacter。
// ============================================================================
int CDungeonSuccessNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    // [b28d-solo-CDungeonNode · N107 封顶] ORIG 0x084851de：两段均 find sret 具名
    // （-0x1c/-0x20）、end 临时（-0x18/-0x14，具名 e/eJob 时 4.4 反置，已删）；第二段补具名
    // `CSuccessJobNode* node = &itJob->second;`。残差 = N107（`&stat->m_charac[0x20]`/
    // `stat->m_charac`/`&stat->m_job` 三处取址 4.4 恒 add+mov vs ORIG `lea K(%eax),%reg`；
    // 本轮 20 形探针复证不可造：/tmp/b28d_s218_probe{,2,3}.cpp 覆盖 reinterpret_cast/
    // char* 算术/整型加法回 cast/union 视图/真实具名成员/pack(1)/数组引用/指针形参等，全产 add）。
    // 78 vs 81（+3 = 三处 N107 各多 1 条）；p167 10/10 CLEAR。
    std::map<int, Party>::iterator it =
        m_partyMap.find(*reinterpret_cast<const int*>(&stat->m_charac[0x20]));
    if (it != m_partyMap.end())
    {
        Party& p = it->second;
        p += *reinterpret_cast<const PlayInfo*>(stat->m_charac);
        p.m_f3c = p.m_f3c + 1;
    }

    std::map<JobKey, CSuccessJobNode>::iterator itJob =
        m_jobMap.find(*reinterpret_cast<const JobKey*>(&stat->m_job));
    if (itJob != m_jobMap.end())
    {
        CSuccessJobNode* node = &itJob->second;
        node->UpdateCharacter(stat);
    }
    return 1;
}