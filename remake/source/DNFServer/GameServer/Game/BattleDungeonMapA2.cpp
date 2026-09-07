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
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CWorldMap.h"
#include "GlobalData.h"
#include "DataManagerInitDepends.h"  // TOD_UserState/TOD_Layer（MakePacket 0x1c 当日层链）
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
    int uid = mapInfo->m_dropItemCnt++;
    // ORIG 0x0830dbad：将 uid 写回 item.m_itemIndex（Add_Item 以 m_itemIndex
    // 为 map key，覆盖可保证 key 唯一不冲突）。
    item.m_itemIndex = uid;
    mapInfo->Add_Item(item);
    return uid;
}

// ---- get_field_item(int, map_item&) —— ORIG 0x0830d9c0（get_field_item.md） ----
// 当前地图 m_itemMap(+0x24) 查找 uid；命中则 21 dword 复制到出参并返回 1。
// ORIG 形态同 is_last_field_item：迭代器先默认构造（C1Ev @+0x0f）、find 经
// sret 临时槽（-0x2c）赋值（P28），op-> 一次 + rep movsl 21 dword 整体复制
// （P83/N78 4.1 内联形态）。
bool CBattle_Field::get_field_item(int uid, map_item& out)
{
    std::map<int, map_item>::iterator it;
    it = GetCurrentMapInfo()->m_itemMap.find(uid);
    if (it == GetCurrentMapInfo()->m_itemMap.end())
        return false;
    out = it->second;
    return true;
}

// ---- is_last_field_item(int) —— ORIG 0x0830da6c（is_last_field_item.md） ----
// 与 get_field_item 同构，但作用于 GetLastMapInfo()；返回 uid 是否仍在场上。
// ORIG 形态（报告 0x830da72-0x830da6b）：迭代器先默认构造（C1Ev @+0x09）、
// find 经 sret 临时槽（-0x1c）再 4 字节赋值到 -0x10（P28「先声明后赋值」）、
// == end 后走 test/je→mov$1 分支岛（P56，非 sete 直折）。
bool CBattle_Field::is_last_field_item(int uid)
{
    std::map<int, map_item>::iterator it;
    it = GetLastMapInfo()->m_itemMap.find(uid);
    if (it == GetLastMapInfo()->m_itemMap.end())
        return false;
    return true;
}

// ---- check_random_appear_hell_dungeon(CDungeon const*, ENUM_DUNGEON_MODE,
//      ENUM_DUNGEON_TYPE, bool&) —— ORIG 0x0830a862（报告全文） ----
// 外层守卫：m_party->m_hellWorldMap(+0x1868) 非零。地狱开关+所在地狱位面时，
// 统计 4 席有效成员的等级 min/max（无角色者按 1 级），依据与副本标准等级的
// 差值决定 *flag；等级离散(>8)且 ≥2 人、或两个旗标未置且类型 1 时清除缓存。
// mode==1（普通模式）强制清除并置 flag=false。
// 【验收 batch-11】orig=149 / our=149（gap 0）：双臂 P48 重发 get_user/
// getCurCharacR 链、正向嵌套防聚合岛、uchar `> 0` 真值形、level 循环前
// 0 初始化槽。残差为 N82 槽号平移与 call 目标显示。
void CBattle_Field::check_random_appear_hell_dungeon(const CDungeon* dungeon,
                                                     ENUM_DUNGEON_MODE mode,
                                                     ENUM_DUNGEON_TYPE type,
                                                     bool& flag)
{
    if (m_party->m_hellWorldMap == 0)
        return;

    // ORIG 三条件各自 je 直达统一尾部（mode 检查 @0x1c4）：正向嵌套 if 的
    // 失败分支 4.4 亦归一到 body 后统一出口，同形。&& 链会折 bool 聚合岛、
    // `!call` 会多 sete/xor 物化，均不用。内层重复测试 m_hellWorldMap != 0
    // （外层已保证非零）系 ORIG 原样（@0x026-0x033）。
    // uchar 返回真值测试：`!= 0`/裸调用在 4.4 折 setne 物化（P73），
    // `> 0` 无符号下限形可回 test/je 直跳（对齐 ORIG @0x1e）。
    if (IsEnableHellDungeon() > 0)
    {
    if (m_party->m_hellWorldMap != 0)
    {
    if (m_party->m_hellWorldMap->IsInHellDungeon())
    {
        int minLevel = 1;
        int maxLevel = 200;
        int level = 0;                    // ORIG 循环前初始化槽 -0x14
        int validCount = 0;
        for (int i = 0; i <= 3; ++i)
        {
            if (!m_party->checkValidUser(i))
                continue;
            // ORIG @0x097/@0x0bc 双臂各自重发 get_user+getCurCharacR 完整链
            // （P48 无 CSE）；m_level +0x27 short（movzwl+cwtl 为 4.1 符号
            // 扩展两段形态，跨版本 N 豁免）。
            if (m_party->get_user(i)->getCurCharacR() != 0)
                level = (int)m_party->get_user(i)->getCurCharacR()->m_level;
            else
                level = 1;
            // ORIG @0x0e9/@0x0fc：三目物化岛（两臂各载一侧、统一 store）。
            minLevel = (level <= minLevel) ? level : minLevel;
            maxLevel = (level >= maxLevel) ? level : maxLevel;
            validCount += 1;
        }

        // ORIG @0x134：get_standard_level 返回值（eax）直接参与减法，不落槽
        // （P160 勿缓存）。
        if (minLevel - dungeon->get_standard_level() <= 5)
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

    // 三条件各自 je 直达统一出口（ORIG @0x1e/@0x25/@0x36 →0x22a）：`&&` 链在
    // 4.4 末尾条件折 bool 聚合岛（mov$1/jmp/mov$0，+3），正向嵌套同形 ORIG；
    // uchar 真值测试用 `> 0`（P73 反向，同 check_random 实证）。
    // X1 形在此**不适用**（实测 ndiff 10→13 已回退）：ORIG 是
    // `xor $1,%eax; test %al,%al; je BODY`（真值跳进 body），而 4.4 对
    // `if (!v) {body}` 恒产 `test; jne SKIP`（假值跳过 body），je/jne 块序相反；
    // 要翻成 je 需把三层嵌套体改成 `goto` 尾置块，会连带影响本函数
    // 统一出口与其它 2 个条件的既有同形性 —— 属"块序+装载序"复合差，非纯 xor 族。
    if (IsEnableHellDungeon() > 0)
    {
    if (worldMap != 0)
    {
    if (worldMap->IsInHellDungeon())
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
        else
        {
        // ORIG else 链同直跳形：isHellParty（-0x2fc 参数叶局部槽 cmpb/je）、
        // member_count != 1（setne 物化为 4.1 && 行为，嵌套后 N 封顶）、
        // CheckHellPartyCondition 结果具名 bool 落槽回读（@0x155 mov%al,-0x9）。
        if (isHellParty)
        {
        if (m_party->get_member_count() != 1)
        {
            bool hellCond = CheckHellPartyCondition();
            if (hellCond)
            {
                m_party->sub_hellparty_value();
                m_hellPartyActive = 1;
                m_hellPartyRandom = isHellParty;       // +0x1ce（byte 直copy）
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
        }
        }
    }
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
// 【§6.2 超大函数验收（batch-11）】orig_insns=1053 / our=1058（gap −5）；
// P167 调用点差集已清零（48 符号×次数全命中，含 find_map/logFormat/TOD 链/
// ppEi 后置自增）；switch 决策树与叶块序逐条同 ORIG（cmp 0xd3 根、叶序
// 0x1d,0x1c,0xd3,0xd4,0x182,0x183）。残余为 N82（帧 0x46c vs 0x45c、pkt 槽
// -0x5c vs -0x58 全帧平移）与 N76/N18a（store 装载两步/单步）封顶，勿再追。
// 六个包分支（switch 决策树 cmp 0xd3 根）：0x1d（进图全量：坐标/怪物/掉落）、
// 0x1c（房间基础+骑乘物+当日层）、0xd3/0x183（小地图信息，两份独立展开，
// find_map 失败走日志且不 finalize）、0xd4（无头的精简房间信息）、
// 0x182（锦标赛轮次表）。MapInfo 字段访问按 ORIG 逐点重取
// GetCurrentMapInfo()（P183 禁缓存；0x182 分支例外——ORIG 自身缓存 map ptr）。
void CBattle_Field::MakePacket(int packetId, void* packet)
{
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)packet;

    // ORIG 怪物序列化首个 put_short 读取未初始化栈槽 [ebp-0x114]
    // （报告 asm 0x8309368 为全函数唯一引用、无任何写入点）。以未初始化局部
    // 变量复现同一行为。
    short staleStackValue;

    switch (packetId)
    {
    case 0x1d:
    {
        pkt->put_header(0, 0x1d);
        int posX = -1;
        int posY = -1;
        getCurPosXY(posX, posY);
        pkt->put_byte(posX);
        pkt->put_byte(posY);
        pkt->put_byte(GetCurrentMapInfo()->m_layeredMap != 0 ? 1 : 0);   // +0xe8
        pkt->put_int(GetCurrentMapInfo()->m_mapBasisLevel);              // +0x48
        pkt->put_byte((char)m_hellPartyDifficulty);          // +0x1cc
        pkt->put_byte((char)m_hellPartyEventType);           // +0x1cd
        if (GetCurrentMapInfo()->m_visited == '\0')          // +0x01
        {
            pkt->put_byte((char)GetCurrentMapInfo()->m_mapType);         // +0x44
            pkt->put_short(GetCurrentMapInfo()->m_mapIndex);             // +0x04
            pkt->put_byte((int)GetCurrentMapInfo()->m_monsterMap.size());  // +0x0c

            map_monster mob;
            for (std::map<int, map_monster>::iterator it =
                     GetCurrentMapInfo()->m_monsterMap.begin();
                 it != GetCurrentMapInfo()->m_monsterMap.end(); it++)
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

            pkt->put_byte((int)GetCurrentMapInfo()->m_itemMap.size());   // +0x24
            map_item item;
            for (std::map<int, map_item>::iterator it =
                     GetCurrentMapInfo()->m_itemMap.begin();
                 it != GetCurrentMapInfo()->m_itemMap.end(); it++)
            {
                item = it->second;                           // 21 dword 复制
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
        break;
    }
    case 0x1c:
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

        CUser* manager = 0;
        TOD_UserState* todState = 0;
        unsigned short todayLayer = 0;                     // -0x46（ushort 写）
        if (m_party != 0 && (manager = m_party->getManager()) != 0)
        {
            todState = reinterpret_cast<TOD_UserState*>(
                manager->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd));
            TOD_Layer layer = todState->getTodayEnterLayer();
            todayLayer = layer.GetLayer();
        }
        pkt->put_short(todayLayer);
        if (m_party == 0)
            pkt->put_byte(0xb);
        else
            pkt->put_byte(m_party->get_random_buff_type());
        pkt->finalize(true);
        break;
    }
    case 0xd3:
    {
        pkt->put_header(0, 0xd3);
        int posX = -1;
        int posY = -1;
        getCurPosXY(posX, posY);
        pkt->put_byte(posX);
        pkt->put_byte(posY);
        pkt->put_int(GetCurrentMapInfo()->m_mapBasisLevel);  // +0x48
        pkt->put_byte(0);
        if (GetCurrentMapInfo()->m_visited == '\0')          // +0x01
        {
            pkt->put_byte((char)GetCurrentMapInfo()->m_mapType);       // +0x44
            pkt->put_short(GetCurrentMapInfo()->m_mapIndex);            // +0x04
            const CDataManager* dataMgr = G_CDataManager();
            if (dataMgr->find_map(GetCurrentMapInfo()->m_mapIndex) == 0)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x1746,
                    "Find Map[%d] Error(In Map Maker)(%d/%d)",
                    GetCurrentMapInfo()->m_mapIndex, m_curX, m_curY);
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
        break;
    }
    case 0xd4:
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
        break;
    }
    case 0x182:
    {
        char memberCount = (char)m_party->get_member_count();
        pkt->put_byte(memberCount);                        // movsbl
        MapInfo* mapInfo = GetCurrentMapInfo();            // ORIG 缓存 map ptr
        std::map<int, map_monster>::iterator mobIt =
            mapInfo->m_monsterMap.begin();
        for (char round = 1; round <= 4; ++round)
        {
            pkt->put_byte(round);
            short roundKey = round;                        // -0x62 short 槽
            std::map<short, std::vector<TournamentInfo> >::iterator roundIt =
                m_tournamentRoundMap.find(roundKey);       // +0x13c
            std::vector<TournamentInfo>& infos = roundIt->second;
            pkt->put_byte((int)infos.size());
            for (unsigned int j = 0; j < infos.size(); ++j)
            {
                pkt->put_byte(infos[j].m_roundOrder);
                for (int k = 0; k < memberCount; ++k)
                {
                    pkt->put_int(infos[j].m_monster[k].m_mobId);
                    pkt->put_short(infos[j].m_monster[k].m_rate);
                }
            }
        }
        for (char round2 = 1; round2 <= 4; ++round2)
        {
            pkt->put_byte(round2);
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
                mobIt++;
            }
        }
        pkt->finalize(true);
        break;
    }
    case 0x183:
    {
        pkt->put_header(0, 0x183);
        int posX = -1;
        int posY = -1;
        getCurPosXY(posX, posY);
        pkt->put_byte(posX);
        pkt->put_byte(posY);
        pkt->put_int(GetCurrentMapInfo()->m_mapBasisLevel);  // +0x48
        pkt->put_byte(0);
        if (GetCurrentMapInfo()->m_visited == '\0')          // +0x01
        {
            pkt->put_byte((char)GetCurrentMapInfo()->m_mapType);       // +0x44
            pkt->put_short(GetCurrentMapInfo()->m_mapIndex);            // +0x04
            const CDataManager* dataMgr = G_CDataManager();
            if (dataMgr->find_map(GetCurrentMapInfo()->m_mapIndex) == 0)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x17d2,
                    "Find Map[%d] Error(In Map Maker)(%d/%d)",
                    GetCurrentMapInfo()->m_mapIndex, m_curX, m_curY);
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
        break;
    }
    default:
        break;
    }
}
// ============================== CDungeon ==============================

// ---- InsertMap(int, int, CMap*) —— ORIG 0x0834b988（InsertMap.md 全文） ----
// 按 CMap::get_map_type() 选择容器：0 → m_map1a4[idx]；2 → m_dummyMap4a4[idx]
// 并置 m_624=1；其余 → m_dummyMap324[idx]。direction 位 1/2/4/8 各插入一条
// pair(方向键, CMap*)。三段结构按 ORIG 原样展开。
// N82 封顶（b26 Lane27）：347 条助记符序列全等、语义/调用点差集归零；残余
// 102 行全为 4.1 不复用槽（各分支独立 key@-0xbc/-0xac/-0x9c…）vs 4.4 生命周期
// 复用（-0x68/-0x60/-0x58…）+ call/jmp 目标显示差。ND82 槽策略跨版本不可翻。
void CDungeon::InsertMap(int idx, int direction, CMap* map)
{
    if (map->get_map_type() == 0)
    {
        if ((direction & 1) != 0)
        {
            std::multimap<int, CMap*>::value_type value(1, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 2) != 0)
        {
            std::multimap<int, CMap*>::value_type value(2, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 4) != 0)
        {
            std::multimap<int, CMap*>::value_type value(4, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 8) != 0)
        {
            std::multimap<int, CMap*>::value_type value(8, map);
            m_map1a4[idx].insert(value);
        }
    }
    else
    {
        if (map->get_map_type() == 2)
        {
            if ((direction & 1) != 0)
            {
                std::multimap<int, CMap*>::value_type value(1, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 2) != 0)
            {
                std::multimap<int, CMap*>::value_type value(2, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 4) != 0)
            {
                std::multimap<int, CMap*>::value_type value(4, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 8) != 0)
            {
                std::multimap<int, CMap*>::value_type value(8, map);
                m_dummyMap4a4[idx].insert(value);
            }
            m_hasDummyMap = 1;                         // +0x624（byte 写）
        }
        else
        {
            if ((direction & 1) != 0)
            {
                std::multimap<int, CMap*>::value_type value(1, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 2) != 0)
            {
                std::multimap<int, CMap*>::value_type value(2, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 4) != 0)
            {
                std::multimap<int, CMap*>::value_type value(4, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 8) != 0)
            {
                std::multimap<int, CMap*>::value_type value(8, map);
                m_dummyMap324[idx].insert(value);
            }
        }
    }
}

// ---- insertStart(int, CMap*) —— ORIG 0x0834be5a（insertStart.md 全文） ----
// 以 CMap::get_index() 为键把起始地图插入 m_maplist[idx]（hash_map @+0x64）。
// P112/N82 封顶：助记符全等，仅 key/value/insert-sret-dest 三槽分配位互换
//（4.1 key@-0xc/value@-0x14/dest@-0x20 vs 4.4 -0x18/-0x20/-0x14）；嵌套实参形、
// 具名接收 r 形均劣化（+5 条 N86），穷举后封顶。
void CDungeon::insertStart(int idx, CMap* map)
{
    std::multimap<int, CMap*>::value_type value(map->get_index(), map);
    m_maplist[idx].insert(value);
}

// ---- IsOpenScheduleDungeon() const —— ORIG 0x0834d2d4（报告全文） ----
// GlobalData::s_DailyScheduleManager.GetScheduleState(type=0, m_index) ∈ {1,2}。
bool CDungeon::IsOpenScheduleDungeon() const
{
    // ORIG @0x834d2da 函数头 `movl $0,-0x10` 死初始化槽（全函数无后续读写，
    // P171 照抄）。
    int deadSlot = 0;
    int state = GlobalData::s_DailyScheduleManager->GetScheduleState(
        (_DailyScheduleType)0, (unsigned int)m_index);
    if (state == 1 || state == 2)
        return true;
    return false;
}
