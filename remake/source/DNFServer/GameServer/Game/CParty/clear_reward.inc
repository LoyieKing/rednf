void CParty::clear_reward(int param)
{
    // b28d-49 判词：N 封顶（N82 帧/槽 + N89 STL 内联）。P167 CLEAR（327/327）。@0x85ae0d4
    // [轴 B 自校·实测] axisB_points.tsv 行（0x85b090a `cmp -0x30(%ebp),%eax; setg`，
    //   getClearRewardBonusExp 尾循环）按地址自校：我方同点为 `call vector::size;
    //   cmp …,%eax; setg`（setg×3）⇒ **两侧同为有符号**，表内序号错配假阳性，
    //   未改源码（另 ORIG `cmp $0x2; ja` 与我方 `setbe` 仅极性差异，符号性同为无符号）。
    CDataManager* dm = G_CDataManager();
    int dungIdx =
        reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index();
    CDungeon* dungeon = dm->find_dungeon(dungIdx);
    int memberCount = get_member_count();

    if (dungeon == 0)
    {
        LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                              0x2a07, "Unknown Dungeon %d",
                              reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index());
        return;
    }
    if (memberCount < 1)
    {
        LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                              0x2a0c, "No Member[%d] in party", memberCount);
        return;
    }
    if (dungeon->isTournamentDungeon())
    {
        LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                              0x2a13, "TournamentDungeon(%d) can not call function",
                              reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index());
        return;
    }

// §P183：ORIG 无 bd/bfView 别名槽，每处访问都从 this 重物化
// （add $0x390×9、$0xb24×12、$0x79d×6、$0x10×16）；引用缓存会省掉这些
// 地址计算 ⇒ 宏化为「每次直取 this+off」，语义等价。
