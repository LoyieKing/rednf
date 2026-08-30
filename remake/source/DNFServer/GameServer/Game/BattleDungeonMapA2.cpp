// ============================================================================
// df_game_r 还原 —— 地图/战场依赖 TU（BattleDungeonMapA2 批次）。
// 9 个 ORIG 符号，全部逐指令对照 docs/class_func_reports/ 完整报告转写
// （## 汇编 + ## 反编译 C 已全文核对；nm 仅用于符号核验）：
//
//   CBattle_Field::MakePacket                        0x083090a4
//   CBattle_Field::SelectDungeon                     0x082ff486
//   CBattle_Field::check_random_appear_hell_dungeon  0x0830a862
//   CBattle_Field::drop_item                         0x0830db84
//   CBattle_Field::get_field_item                    0x0830d9c0
//   CBattle_Field::is_last_field_item                0x0830da6c
//   CDungeon::InsertMap                              0x0834b988
//   CDungeon::insertStart                            0x0834be5a
//   CDungeon::IsOpenScheduleDungeon                  0x0834d2d4
//
// 字段化依据：CBattle_Field.h / CDungeon.h / CParty.h / WarField.h（MapInfo、
// map_item、map_monster）/ CMap.h 权威布局。
// ============================================================================

#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CWorldMap.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "PacketGuard.h"

// ============================ CBattle_Field ============================

// ---- drop_item(map_item) —— ORIG 0x0830db84（drop_item.md 全文） ----
// GetCurrentMapInfo()->m_dropItemCnt(+0x40) 作 uid 分配器：取旧值、+1 回写，
// 再 MapInfo::Add_Item(map_item)（ORIG 将按值参数复制到栈上传入）。
int CBattle_Field::drop_item(map_item item)
{
    MapInfo* mapInfo = GetCurrentMapInfo();
    int uid = mapInfo->m_dropItemCnt;
    mapInfo->m_dropItemCnt = uid + 1;
    // ORIG 0x0830dbad：将 uid 写回 item.m_itemIndex（Add_Item 以 m_itemIndex
    // 为 map key，覆盖可保证 key 唯一不冲突）。
    item.m_itemIndex = uid;
    mapInfo->Add_Item(item);
    return uid;
}

// ---- get_field_item(int, map_item&) —— ORIG 0x0830d9c0（get_field_item.md） ----
// 当前地图 m_itemMap(+0x24) 查找 uid；命中则 21 dword 复制到出参并返回 1。
bool CBattle_Field::get_field_item(int uid, map_item& out)
{
    std::map<int, map_item>::iterator it =
        GetCurrentMapInfo()->m_itemMap.find(uid);
    if (it == GetCurrentMapInfo()->m_itemMap.end())
        return false;
    out = it->second;
    return true;
}

// ---- is_last_field_item(int) —— ORIG 0x0830da6c（is_last_field_item.md） ----
// 与 get_field_item 同构，但作用于 GetLastMapInfo()；返回 uid 是否仍在场上。
bool CBattle_Field::is_last_field_item(int uid)
{
    std::map<int, map_item>::iterator it =
        GetLastMapInfo()->m_itemMap.find(uid);
    return it != GetLastMapInfo()->m_itemMap.end();
}

// ---- check_random_appear_hell_dungeon(CDungeon const*, ENUM_DUNGEON_MODE,
//      ENUM_DUNGEON_TYPE, bool&) —— ORIG 0x0830a862（报告全文） ----
// 外层守卫：m_party->m_hellWorldMap(+0x1868) 非零。地狱开关+所在地狱位面时，
// 统计 4 席有效成员的等级 min/max（无角色者按 1 级），依据与副本标准等级的
// 差值决定 *flag；等级离散(>8)且 ≥2 人、或两个旗标未置且类型 1 时清除缓存。
// mode==1（普通模式）强制清除并置 flag=false。
void CBattle_Field::check_random_appear_hell_dungeon(const CDungeon* dungeon,
                                                     ENUM_DUNGEON_MODE mode,
                                                     ENUM_DUNGEON_TYPE type,
                                                     bool& flag)
{
    if (m_party->m_hellWorldMap == 0)
        return;

    // ORIG 内层重复测试 m_hellWorldMap != 0（外层已保证非零），语义等价保留两条件。
    if (IsEnableHellDungeon() && m_party->m_hellWorldMap->IsInHellDungeon())
    {
        int minLevel = 1;
        int maxLevel = 200;
        int validCount = 0;
        for (int i = 0; i < 4; ++i)
        {
            if (!m_party->checkValidUser(i))
                continue;
            CUser* user = m_party->get_user(i);
            const _Charac_info* charac = user->getCurCharacR();
            int level;
            if (charac == 0)
                level = 1;
            else
                level = (int)charac->m_level;  // +0x27 short（movzwl+cwtl）
            validCount += 1;
            if (level > minLevel)
                minLevel = level;
            if (level < maxLevel)
                maxLevel = level;
        }

        int stdLevel = dungeon->get_standard_level();
        if (minLevel - stdLevel <= 5)
        {
            if (validCount <= 1)
                flag = true;
            else if (minLevel - maxLevel <= 8)
                flag = true;
        }

        if ((minLevel - maxLevel > 8 && validCount > 1) ||
            ((char)m_party->m_field1864 != 1 && type == 1) ||
            ((char)m_party->m_sealDoorFlag != 1 && type == 1))
        {
            m_party->m_hellWorldMap = 0;
        }
    }

    if (mode == 1)
    {
        m_party->m_hellWorldMap = 0;
        flag = false;
    }
}

// ---- SelectDungeon(...) —— ORIG 0x082ff486（SelectDungeon.md 全文） ----
// 记录副本/难度/类型/世界图参数 → 地狱派对判定（type==1 强制，或随机条件）
// → m_89d 非 1 时清 hell 标记 → 由 dungeon->m_6e0[diff] 取迷宫指针 →
// GetAppropriateMaze/SetGridPath/清除条件 → 起始格定位起始地图 →
// 构造 MapInfo{mapIndex,startMap} 插入 m_mapInfo 并 ConsistMap。
bool CBattle_Field::SelectDungeon(int dungeonIdx, const CDungeon* dungeon,
                                  int diff, ENUM_DUNGEON_TYPE type,
                                  int memberCount, CWorldMap* worldMap,
                                  bool isHellParty, int questStageMap)
{
    (void)memberCount;  // 第 5 参仅落存 m_field198（ORIG 0x082ff4c1）

    m_dungeonIndex = dungeonIdx;                       // +0x194
    set_dungeon_diff((char)diff);                      // ORIG set_dungeon_diff(char)
    m_field198 = memberCount;                          // +0x198
    m_questStageMap = questStageMap;                   // +0x19c
    m_hellPartyActive = 0;                             // +0x1b8
    m_hellPartyMode = type;                            // +0x1b0

    static const char s_pretty[] =
        "bool CBattle_Field::SelectDungeon(int, const CDungeon*, int, "
        "ENUM_DUNGEON_TYPE, int, CWorldMap*, bool, int)";

    if (IsEnableHellDungeon() && worldMap != 0 && worldMap->IsInHellDungeon())
    {
        if (type == 1)
        {
            m_party->sub_hellparty_value();
            m_hellPartyDifficulty = (char)GetHellPartyDifficulty(type);
            if (m_hellPartyDifficulty == 0)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp", s_pretty, 0x1fc,
                    "HELLPARTY_LOG : HELLPARTY DIFFICULTY ERROR!!\n");
                return false;
            }
            m_hellPartyEventType = (char)GetHellPartyEventType(m_hellPartyDifficulty);
            m_hellPartyActive = 1;
        }
        else if (isHellParty && m_party->get_member_count() != 1 &&
                 CheckHellPartyCondition())
        {
            m_party->sub_hellparty_value();
            m_hellPartyActive = 1;
            m_hellPartyRandom = isHellParty ? 1 : 0;   // +0x1ce（byte 写）
            m_hellPartyDifficulty = (char)GetHellPartyDifficulty(type);
            if (m_hellPartyDifficulty == 0)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp", s_pretty, 0x22b,
                    "HELLPARTY_LOG : HELLPARTY DIFFICULTY ERROR!!\n");
                return false;
            }
            m_hellPartyEventType = (char)GetHellPartyEventType(m_hellPartyDifficulty);
        }
    }

    if (dungeon != 0 && dungeon->m_89d != 1)
        m_hellPartyActive = 0;                         // +0x89d 非 1 清标记

    m_pDungeon = const_cast<CDungeon*>(dungeon);       // +0x188
    // ORIG 082ff6e9 读 *(dungeon + (0x1b8+diff)*4) = m_6e0[diff]（每图怪物基数 int，
    //   getAddChampionMobCnt 按难度缩放；非 MazeScript* 表）。
    m_mapMobCount = dungeon->m_6e0[diff];              // +0x70
    m_field1a4 = 0;                                    // +0x1a4

    MazeScript* maze = GetAppropriateMaze((ENUM_DUNGEON_DIFFICULTY)diff);
    m_mazeType = maze->m_64;                           // +0x1ac ← maze+0x64
    setQuestMazeInfo();
    m_clearCondition.SetDungeonClearConditionVector(maze->m_clearCondition);
    SetGridPath(maze, dungeon->GetDummyAppearCount());

    int posX = -1;
    int posY = -1;
    getCurPosXY(posX, posY);
    const GridScript* grid = &m_gridMaze->getGridR(posX, posY);
    if (grid == 0)
        return false;

    int startMap = dungeon->GetStartMap(grid->m_gridValue);
    if (startMap < 1)
    {
        LogManager::logFormat(1, "battle_field.cpp", s_pretty, 0x27c,
                              "[CBattle_Field::SelectDungeon] Error Map : (%d)(%d,%d)\n",
                              dungeonIdx, m_curX, m_curY);
        return false;
    }

    MapInfo newMapInfo;
    newMapInfo.m_mapIndex = startMap;                  // +0x04
    newMapInfo.m_gridValue = (char)grid->m_gridValue;  // +0x08（byte 写 %al）
    int key = GetMapInfoKeyValue(posX, posY);
    m_mapInfo.insert(std::make_pair(key, newMapInfo));
    ConsistMap();
    return true;
}

// ---- MakePacket(int, void*) —— ORIG 0x083090a4（MakePacket.md 全文） ----
// 六个包分支：0x1c（房间基础+骑乘物+当日层）、0x1d（进图全量：坐标/怪物/掉落）、
// 0xd3 / 0x183（小地图信息，find_map 失败走日志且不 finalize）、
// 0xd4（无头的精简房间信息）、0x182（锦标赛轮次表）。
void CBattle_Field::MakePacket(int packetId, void* packet)
{
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)packet;

    // ORIG 怪物序列化首个 put_short 读取未初始化栈槽 [ebp-0x114]
    // （报告 asm 0x8309368 为全函数唯一引用、无任何写入点）。以未初始化局部
    // 变量复现同一行为。
    short staleStackValue;

    if (packetId == 0x1d)
    {
        pkt->put_header(0, 0x1d);
        int posX = -1;
        int posY = -1;
        getCurPosXY(posX, posY);
        pkt->put_byte(posX);
        pkt->put_byte(posY);
        MapInfo* mapInfo = GetCurrentMapInfo();
        pkt->put_byte(mapInfo->m_layeredMap != 0 ? 1 : 0);   // +0xe8
        pkt->put_int(mapInfo->m_mapBasisLevel);              // +0x48
        pkt->put_byte((char)m_hellPartyDifficulty);          // +0x1cc
        pkt->put_byte((char)m_hellPartyEventType);           // +0x1cd
        if (mapInfo->m_visited == '\0')                      // +0x01
        {
            pkt->put_byte((char)mapInfo->m_mapType);         // +0x44
            pkt->put_short(mapInfo->m_mapIndex);             // +0x04
            pkt->put_byte((int)mapInfo->m_monsterMap.size());  // +0x0c

            map_monster mob;
            for (std::map<int, map_monster>::iterator it =
                     mapInfo->m_monsterMap.begin();
                 it != mapInfo->m_monsterMap.end(); ++it)
            {
                mob = it->second;
                pkt->put_short(staleStackValue);             // 未初始化槽复现
                pkt->put_int(mob.m_mobId);                   // +0x00
                pkt->put_short((short)mob.m_instId);         // +0x04 低 16 位
                pkt->put_int(mob.m_mobIndex);                // +0x0c
                pkt->put_byte((unsigned char)mob.m_level);   // +0x10（movzbl）
                if (mob.m_flag34 != 0)                       // +0x34
                    pkt->put_byte(3);
                else
                    pkt->put_byte((char)mob.m_roleType);     // +0x08
                pkt->put_byte((unsigned char)mob.m_field13); // +0x13
                pkt->put_byte((unsigned char)mob.m_field14); // +0x14
            }

            pkt->put_byte((int)mapInfo->m_itemMap.size());   // +0x24
            for (std::map<int, map_item>::iterator it =
                     mapInfo->m_itemMap.begin();
                 it != mapInfo->m_itemMap.end(); ++it)
            {
                map_item item = it->second;              // 21 dword 复制
                pkt->put_byte((char)item.m_standardLevel);       // +0x01
                pkt->put_short(item.m_itemIndex);                // +0x04
                pkt->put_int(item.m_item.m_addInfo);             // +0x12
                pkt->put_int(item.m_item.get_add_info());        // +0x10 对象
                pkt->put_short(item.m_item.m_fieldb);            // +0x1b
                pkt->put_byte(item.m_item.m_amp.getAbilityType());   // +0x21
                pkt->put_short(item.m_item.m_amp.getAbilityValue());
            }
            pkt->put_byte((unsigned char)m_hellPartyActive);     // +0x1b8
            CheckAppearRidableObject(reinterpret_cast<PacketGuard*>(pkt));
            if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
                pkt->put_byte(m_curSeatNo);                      // +0x114
            else
                pkt->put_byte(0xff);
            pkt->finalize(true);
        }
        else
        {
            pkt->put_byte(0);
            CheckAppearRidableObject(reinterpret_cast<PacketGuard*>(pkt));
            if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
                pkt->put_byte(m_curSeatNo);
            else
                pkt->put_byte(0xff);
            pkt->finalize(true);
        }
    }
    else if (packetId == 0x1c)
    {
        pkt->put_byte(m_mazeType);                         // +0x1ac
        pkt->put_byte(m_pos[0].first);                     // +0x0c
        pkt->put_byte(m_pos[0].second);                    // +0x10
        pkt->put_byte(m_hellPosX);                         // +0x1c0
        pkt->put_byte(m_hellPosY);                         // +0x1c4
        pkt->put_byte(m_bloodState);                       // +0x1b4
        pkt->put_byte(m_ridable.GetObjectCnt());
        std::vector<std::pair<int, int> > ridablePos;
        m_ridable.GetObjectPos(ridablePos);
        for (size_t i = 0; i < ridablePos.size(); ++i)
        {
            pkt->put_byte(ridablePos[i].first);
            pkt->put_byte(ridablePos[i].second);
        }

        unsigned short todayLayer = 0;                     // local_4a
        if (m_party != 0)
        {
            CUser* manager = m_party->getManager();
            if (manager != 0)
            {
                // （同 CDungeon.cpp tod_detail 注）。ORIG 语义（0x86439c8/0x85fe7b4）：
                // layer = (ushort)(state+0x14) + 1，>0x64 时 clamp 到 1。
                char* todState = (char*)manager->GetCharacExpandData(
                    (ENUM_CHARAC_EXPAND_TYPE)0xd);
                unsigned short todBase = *(unsigned short*)(todState + 0x14);
                todayLayer = (unsigned short)(todBase + 1);
                if (todayLayer > 0x64)
                    todayLayer = 1;
            }
        }
        pkt->put_short(todayLayer);
        if (m_party == 0)
            pkt->put_byte(0xb);
        else
            pkt->put_byte(m_party->get_random_buff_type());
        pkt->finalize(true);
    }
    else if (packetId == 0xd4)
    {
        pkt->put_byte(m_mazeType);
        pkt->put_byte(m_pos[0].first);
        pkt->put_byte(m_pos[0].second);
        pkt->put_byte(0);
        pkt->put_byte(0);
        pkt->put_byte(m_bloodState);
        if (m_party == 0)
            pkt->put_byte(0xb);
        else
            pkt->put_byte(m_party->get_random_buff_type());
        pkt->finalize(true);
    }
    else if (packetId == 0x182)
    {
        char memberCount = (char)m_party->get_member_count();
        pkt->put_byte(memberCount);                        // movsbl
        MapInfo* mapInfo = GetCurrentMapInfo();
        std::map<int, map_monster>::iterator mobIt =
            mapInfo->m_monsterMap.begin();
        for (char round = 1; round <= 4; ++round)
        {
            pkt->put_byte(round);
            std::map<short, std::vector<TournamentInfo> >::iterator roundIt =
                m_tournamentRoundMap.find((short)round);   // +0x13c
            std::vector<TournamentInfo>& infos = roundIt->second;
            pkt->put_byte((int)infos.size());
            for (size_t j = 0; j < infos.size(); ++j)
            {
                TournamentInfo& info = infos[j];
                pkt->put_byte(info.m_roundOrder);
                for (int k = 0; k < memberCount; ++k)
                {
                    pkt->put_int(info.m_monster[k].m_mobId);
                    pkt->put_short(info.m_monster[k].m_rate);
                }
            }
            int written = 0;
            while (written < memberCount &&
                   mobIt != mapInfo->m_monsterMap.end())
            {
                const map_monster& mob = mobIt->second;
                pkt->put_short(mob.m_instId);
                pkt->put_int(mob.m_mobIndex);
                pkt->put_byte((unsigned char)mob.m_level);
                pkt->put_byte((char)mob.m_roleType);
                ++written;
                ++mobIt;
            }
        }
        pkt->finalize(true);
    }
    else if (packetId == 0xd3 || packetId == 0x183)
    {
        pkt->put_header(0, packetId);
        int posX = -1;
        int posY = -1;
        getCurPosXY(posX, posY);
        pkt->put_byte(posX);
        pkt->put_byte(posY);
        MapInfo* mapInfo = GetCurrentMapInfo();
        pkt->put_int(mapInfo->m_mapBasisLevel);
        pkt->put_byte(0);
        if (mapInfo->m_visited == '\0')
        {
            pkt->put_byte((char)mapInfo->m_mapType);
            pkt->put_short(mapInfo->m_mapIndex);
            const CDataManager* dataMgr = G_CDataManager();  // const → find_map const
            if (dataMgr->find_map(mapInfo->m_mapIndex) == 0)
            {
                // ORIG 两分支日志行不同：0x1746(0xd3) / 0x17d2(0x183)
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    __PRETTY_FUNCTION__,
                    packetId == 0xd3 ? 0x1746 : 0x17d2,
                    "Find Map[%d] Error(In Map Maker)(%d/%d)",
                    mapInfo->m_mapIndex, m_curX, m_curY);
                // 错误路径不 finalize，直接返回
            }
            else
            {
                pkt->put_byte(0);
                pkt->put_byte(0);
                pkt->put_byte(0);
                pkt->finalize(true);
            }
        }
        else
        {
            pkt->put_byte(0);
            pkt->finalize(true);
        }
    }
}

// ============================== CDungeon ==============================

// ---- InsertMap(int, int, CMap*) —— ORIG 0x0834b988（InsertMap.md 全文） ----
// 按 CMap::get_map_type() 选择容器：0 → m_map1a4[idx]；2 → m_dummyMap4a4[idx]
// 并置 m_624=1；其余 → m_dummyMap324[idx]。direction 位 1/2/4/8 各插入一条
// pair(方向键, CMap*)。三段结构按 ORIG 原样展开。
void CDungeon::InsertMap(int idx, int direction, CMap* map)
{
    if (map->get_map_type() == 0)
    {
        if ((direction & 1) != 0)
        {
            std::pair<int, CMap*> value(1, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 2) != 0)
        {
            std::pair<int, CMap*> value(2, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 4) != 0)
        {
            std::pair<int, CMap*> value(4, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 8) != 0)
        {
            std::pair<int, CMap*> value(8, map);
            m_map1a4[idx].insert(value);
        }
    }
    else
    {
        if (map->get_map_type() == 2)
        {
            if ((direction & 1) != 0)
            {
                std::pair<int, CMap*> value(1, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 2) != 0)
            {
                std::pair<int, CMap*> value(2, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 4) != 0)
            {
                std::pair<int, CMap*> value(4, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 8) != 0)
            {
                std::pair<int, CMap*> value(8, map);
                m_dummyMap4a4[idx].insert(value);
            }
            m_hasDummyMap = 1;                         // +0x624（byte 写）
        }
        else
        {
            if ((direction & 1) != 0)
            {
                std::pair<int, CMap*> value(1, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 2) != 0)
            {
                std::pair<int, CMap*> value(2, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 4) != 0)
            {
                std::pair<int, CMap*> value(4, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 8) != 0)
            {
                std::pair<int, CMap*> value(8, map);
                m_dummyMap324[idx].insert(value);
            }
        }
    }
}

// ---- insertStart(int, CMap*) —— ORIG 0x0834be5a（insertStart.md 全文） ----
// 以 CMap::get_index() 为键把起始地图插入 m_maplist[idx]（hash_map @+0x64）。
void CDungeon::insertStart(int idx, CMap* map)
{
    std::pair<int, CMap*> value(map->get_index(), map);
    m_maplist[idx].insert(value);
}

// ---- IsOpenScheduleDungeon() const —— ORIG 0x0834d2d4（报告全文） ----
// GlobalData::s_DailyScheduleManager.GetScheduleState(type=0, m_index) ∈ {1,2}。
bool CDungeon::IsOpenScheduleDungeon() const
{
    int state = GlobalData::s_DailyScheduleManager->GetScheduleState(
        (_DailyScheduleType)0, (unsigned int)m_index);
    if (state == 1 || state == 2)
        return true;
    return false;
}
