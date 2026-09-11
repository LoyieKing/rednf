// ============================================================================
// KillHellPartyGroupMonsterCnt —— ORIG T 0x0830d704。
// 地狱派对组击杀计数：m_killedAllHellGroups(+0x1ca) 置位直接返回；
// 以 mob.m_hellPartyGroupOrder(+0x336，ORIG movzwl 实证)查 MapInfo::m_hellPartyGroup：
// 未登记 → HELLPARTY_LOG 0x1dd9；组剩余==0 → 移除组并 killedAll=true，
// 组表清空时置 m_killedAllHellGroups；否则剩余-1 且 killedAll=false。
// ============================================================================
void CBattle_Field::KillHellPartyGroupMonsterCnt(MapInfo& mapInfo,
                                                 map_monster& mob,
                                                 bool& killedAll)
{
    if (m_killedAllHellGroups != 0)
        return;

    // P0：ORIG 无 groupIdx 槽（logFormat 参数处 movzwl 0x336 直读重读），
    // 表达式直写两处内联。
    std::map<int, int>::iterator it =
        mapInfo.m_hellPartyGroup.find(
            static_cast<int>(mob.m_hellPartyGroupOrder));
    if (it == mapInfo.m_hellPartyGroup.end())
    {
        // [P6S5 2026-09-11 本波落地 · P139 prett 轴] ORIG 引用
        // `_ZZN13CBattle_Field28KillHellPartyGroupMonsterCntER7MapInfoR11map_monsterRbE19__PRETTY_FUNCTION__`
        // @0x8c233a0 size 0x50；旧译用 TU 级手写串 `s_bfkc_pretty_kill_hell`（逐字节相同）
        // ⇒ 我方缺 prett 符号。改 `__PRETTY_FUNCTION__` 后 nm 实测符号名/size 与 ORIG 全等，
        // 本件（原 identical）保持 identical；⚠ 遗留：TU 级定义 `s_bfkc_pretty_kill_hell[]`
        // （0x…… 行 45）现为死数据但仍发射 ⇒ 需父泳道按「TU 级实体」权限删除。
        LogManager::logFormat(
            1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x1dd9,
            "HELLPARTY_LOG : HELLPARTY GROUP INDEX (%d), MAP INDEX (%d) : "
            "KILL HELL GROUP ERROR!!\n",
            static_cast<int>(mob.m_hellPartyGroupOrder), mapInfo.m_mapIndex);
        return;
    }

    mob.m_roleType = 0;
    if (it->second == 0)
    {
        killedAll = true;
        mapInfo.m_hellPartyGroup.erase(it);
        if (mapInfo.m_hellPartyGroup.size() == 0)
            m_killedAllHellGroups = 1;
    }
    else
    {
        it->second -= 1;
        killedAll = false;
    }
}