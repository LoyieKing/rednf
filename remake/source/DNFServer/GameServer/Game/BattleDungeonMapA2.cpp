// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BattleDungeonMapA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "BattleDungeonMapA2/drop_item.inc"
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
#include "BattleDungeonMapA2/get_field_item.inc"
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
#include "BattleDungeonMapA2/is_last_field_item.inc"
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
// [b28d-solo-BattleDungeonMapA2] IDENTICAL/AE（147=147，本波 DIFF→AE）：
// ① IsEnableHellDungeon 改裸 bool 判（P292；`> 0` 形 4.4 产 movzbl/test/setg）；
// ② min/max 三目臂序翻转 `(min>level)?min:level`（复现 ORIG jle 直落+then 岛）；
// ③ validCount 块 P222 反向包裹 `if(validCount>1){…}else{flag=1}`；
// ④ CParty::m_field1864/m_sealDoorFlag bool 化（P248 档②，ORIG
// `movzbl;xor $1;test;je`；CParty.h 已 R2 广播，消费点 CParty.cpp/
// CPartyDepsA2.cpp 回执零回归）。
#include "BattleDungeonMapA2/check_random_appear_hell_dungeon.inc"
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
    if (IsEnableHellDungeon())
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
            // ORIG @0x0e9/@0x0fc：三目物化岛（两臂各载一侧、统一 store）；
            // 目标值必须写在三目第一操作数位（P243 族：ORIG 先载目标槽）。
            minLevel = (minLevel > level) ? minLevel : level;
            maxLevel = (maxLevel < level) ? maxLevel : level;
            validCount += 1;
        }

        // ORIG @0x134：get_standard_level 返回值（eax）直接参与减法，不落槽
        // （P160 勿缓存）。
        if (minLevel - dungeon->get_standard_level() <= 5)
        {
            // ORIG @0x147-0x166：`jle <flag=1 块>` 使 validCount<=1 的
            // true 块落在远位、else-if 判断直落（4.1 块序）；4.4 对
            // if/else-if 恒反 ⇒ 反向包裹复现（P222/P47）。
            if (validCount > 1)
            {
                if (minLevel - maxLevel <= 8)
                    flag = true;
            }
            else
            {
                flag = true;
            }
        }

        if ((minLevel - maxLevel > 8 && validCount > 1) ||
            (!m_party->m_field1864 && type == 1) ||
            (!m_party->m_sealDoorFlag && type == 1))
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
// [b28d-solo-BattleDungeonMapA2] IDENTICAL/AE（313=313，本波 DIFF→AE）：
// ① logFormat 第 3 参改 __PRETTY_FUNCTION__（P118/P139；ORIG 具
// _ZZN13CBattle_Field13SelectDungeon…E19__PRETTY_FUNCTION__ 局部静态，
// 手写字符串只产匿名 .rodata）；② IsEnableHellDungeon 裸 bool 判（P292）；
// ③ m_hellPartyMode 经权威头匿名 union 的 enum 视角赋值复现 LHS-first
// （P211 档③；探针 f1/f2/f3 证 int 字段+(int)cast 恒 RHS-first）；
// ④ m_hellPartyRandom bool 化（同型 store）；⑤ CDungeon::m_89d bool 化 +
// `!dungeon->m_89d`（P248 档②）；⑥ startMap 声明=0 + 独立赋值（P198③）。
// CBattle_Field.h/CDungeon.h 两处 .h 改动均已 R2 广播。
#include "BattleDungeonMapA2/SelectDungeon.inc"
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
    // ORIG @0x41 为 LHS-first `mov this; mov type; store` ⇒ 字段与 RHS 同型
    // （P211 档③）；用权威头匿名 union 的 enum 视角（int 视角保留给其他消费点）。
    m_hellPartyModeE = type;                           // +0x1b0

    // ORIG 三处 logFormat 第 3 参为 `_ZZN13CBattle_Field13SelectDungeon…
    // E19__PRETTY_FUNCTION__` 局部静态（P118/P139）⇒ 必须传 __PRETTY_FUNCTION__，
    // 手写字符串只产匿名 .rodata 且留 missing。
    // 三条件各自 je 直达统一出口（ORIG @0x1e/@0x25/@0x36 →0x22a）：`&&` 链在
    // 4.4 末尾条件折 bool 聚合岛（mov$1/jmp/mov$0，+3），正向嵌套同形 ORIG；
    // ORIG @0x63 为 `call; test %al,%al; je`（bool callee 直判，P292）⇒ 裸调用。
    if (IsEnableHellDungeon())
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
                    1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x1fc,
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
                        1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x22b,
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

    if (dungeon != 0 && !dungeon->m_89d)
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

    // ORIG @0x34b：`movl $0x0,-0x10` 先置 0 再赋 GetStartMap 结果（P198③
    // 声明=0 + 独立赋值两句式）。
    int startMap = 0;
    startMap = dungeon->GetStartMap(grid->m_gridValue);
    if (startMap < 1)
    {
        LogManager::logFormat(1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x27c,
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
// [b28d-solo-BattleDungeonMapA2] §6.2 超大函数三件套验收（fresh .o）：
// ① gap +5（our 1058 / orig 1053）；② P167 调用点差集 CLEAR（182=182；
// 本波补回真语义缺口——ORIG @0xd8/0xea 是两条独立 put_byte 调用点，
// 旧实现写成三目合并致 MISSING put_byte×1）；③ put_byte 实参直方图 64=64
// 逐类全等（reg 44 / 0×14 / 0xff×2 / 0xb×2 / 0x1 / 0x3）、equal 885/1053、
// 帧 0x46c vs 0x45c。残余全为跨版本形态族：N82（帧/槽平移）+ N7/N18a
// （4.1 值经 %edx 中转 vs 4.4 %eax 直传）+ P194/setcc（bool 物化）⇒ 语义完成、
// byte 层 N 封顶。本波另修：0x182 的 setg 有符号比较、find_map 结果落具名槽
// （void* mapData）、`m_party==0` 两处 P222 反向 if/else。
// 六个包分支（switch 决策树 cmp 0xd3 根）：0x1d（进图全量：坐标/怪物/掉落）、
// 0x1c（房间基础+骑乘物+当日层）、0xd3/0x183（小地图信息，两份独立展开，
// find_map 失败走日志且不 finalize）、0xd4（无头的精简房间信息）、
// 0x182（锦标赛轮次表）。MapInfo 字段访问按 ORIG 逐点重取
// GetCurrentMapInfo()（P183 禁缓存；0x182 分支例外——ORIG 自身缓存 map ptr）。
#include "BattleDungeonMapA2/MakePacket.inc"
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
        // ORIG @0xd8-0xea：两处独立 put_byte 调用点（then 落 $1、else 落 $0），
        // 非三目合并（p167 MISSING put_byte×1 即此）。
        if (GetCurrentMapInfo()->m_layeredMap != 0)                          // +0xe8
            pkt->put_byte(1);
        else
            pkt->put_byte(0);
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
        pkt->put_byte(m_endX);                             // +0x0c
        pkt->put_byte(m_endY);                             // +0x10
        pkt->put_byte(m_hellPosX);                         // +0x1c0
        pkt->put_byte(m_hellPosY);                         // +0x1c4
        pkt->put_byte(m_bloodState);                       // +0x1b4
        pkt->put_byte(m_ridable.GetObjectCnt());
        std::vector<std::pair<int, int> > ridablePos;
        m_ridable.GetObjectPos(ridablePos);
        // ORIG @0x79d 尾测 `call size; cmp i,size; setg`（有符号比较）⇒
        // 索引 int 化且比较写 size > i。
        for (int i = 0; (int)ridablePos.size() > i; ++i)
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
        if (m_party != 0)
                    pkt->put_byte(m_party->get_random_buff_type());
                else
                    pkt->put_byte(0xb);
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
            // ORIG @0x9ff：结果先落具名槽 -0x44 再槽比较（`cmpl $0,-0x44; jne`），
            // 内联实参形 4.4 产 test/sete/test/je（+2）。
            void* mapData = dataMgr->find_map(GetCurrentMapInfo()->m_mapIndex);
            if (mapData == 0)
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
        pkt->put_byte(m_endX);
        pkt->put_byte(m_endY);
        pkt->put_byte(0);
        pkt->put_byte(0);
        pkt->put_byte(m_bloodState);
        if (m_party != 0)
                    pkt->put_byte(m_party->get_random_buff_type());
                else
                    pkt->put_byte(0xb);
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
            // ORIG @0xf8a：同 0xd3 分支（结果落具名槽 -0x1c 后槽比较）。
            void* mapData = dataMgr->find_map(GetCurrentMapInfo()->m_mapIndex);
            if (mapData == 0)
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
// [b28d-solo-BattleDungeonMapA2] NEAR 封顶（N82）：347=347 助记符序列全等、
// P167 调用点差集 CLEAR；本波把 12 组 pair 构造实参改为 PATH_DIRECTION 枚举
// （ORIG mangled `C1I14PATH_DIRECTIONRS2_EEOT_OT0_`，权威头 CDungeon.h 新增
// 全局 enum PATH_DIRECTION 定义）后调用点符号全等。残余 102 行全为 4.1
// 不复用槽（各分支独立 key@-0xbc/-0xac/…）vs 4.4 生命周期复用
// （-0x68/-0x60/…）+ call/jmp 目标显示差；槽策略跨版本不可翻。
#include "BattleDungeonMapA2/InsertMap.inc"
{
    if (map->get_map_type() == 0)
    {
        if ((direction & 1) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)1, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 2) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)2, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 4) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)4, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 8) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)8, map);
            m_map1a4[idx].insert(value);
        }
    }
    else
    {
        if (map->get_map_type() == 2)
        {
            if ((direction & 1) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)1, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 2) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)2, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 4) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)4, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 8) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)8, map);
                m_dummyMap4a4[idx].insert(value);
            }
            m_hasDummyMap = 1;                         // +0x624（byte 写）
        }
        else
        {
            if ((direction & 1) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)1, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 2) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)2, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 4) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)4, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 8) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)8, map);
                m_dummyMap324[idx].insert(value);
            }
        }
    }
}

// ---- insertStart(int, CMap*) —— ORIG 0x0834be5a（insertStart.md 全文） ----
// 以 CMap::get_index() 为键把起始地图插入 m_maplist[idx]（hash_map @+0x64）。
// [b28d-solo-BattleDungeonMapA2] NEAR 封顶（N86/N82）：31=31 助记符序列全等、
// P167 CLEAR，pair 构造符号与 ORIG 全等（`C1IiRS2_EEOT_OT0_`）；仅三临时槽
// 分配位互换（ORIG int 临时@-0xc/pair@-0x14/insert-sret@-0x20 vs 4.4
// sret@-0x14/临时@-0x18/pair@-0x20）。本波实测：具名 key 形改符号为
// `C1IRiRS2_E`（左值）不可用；static_cast<int&&> 形劣化为 DIFF；4.4 的
// 临时槽分配序由前端固定，源形不可择。
#include "BattleDungeonMapA2/insertStart.inc"
{
    std::multimap<int, CMap*>::value_type value(map->get_index(), map);
    m_maplist[idx].insert(value);
}

// ---- IsOpenScheduleDungeon() const —— ORIG 0x0834d2d4（报告全文） ----
// GlobalData::s_DailyScheduleManager.GetScheduleState(type=0, m_index) ∈ {1,2}。
#include "BattleDungeonMapA2/IsOpenScheduleDungeon.inc"
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
