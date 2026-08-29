// ============================================================================
// CBattle_Field_KillChain.cpp —— CBattle_Field 域击杀/奖励链第三阶依赖
// （LinkKillChain 批次；全部经 docs/class_func_reports/CBattle_Field/ 对应报告
//   ## 汇编 + ## 反编译 C 完整读取后忠实转写，无桩）：
//
//   KillHellPartyGroupMonsterCnt   T 0x0830d704（KillHellPartyGroupMonsterCnt.md）
//   checkKilledSpawnBloodMonsters  T 0x08304f8e（checkKilledSpawnBloodMonsters.md）
//   send_elevator_time_packet      T 0x0830d848（send_elevator_time_packet.md）
//   getItemDropDungeonType         T 0x0830aa46（getItemDropDungeonType.md；
//                                    _MakeDropItems 链上依赖，同批补齐）
//   setPremiumDropRate             T 0x0830b848（setPremiumDropRate.md；依赖）
//   _MakeDropItems                 T 0x0830adf6（_MakeDropItems.md，0xa51 字节）
//   _MakeNotiPacketDieMonster      T 0x0830b8cc（_MakeNotiPacketDieMonster.md）
//
// 本 TU 不编辑权威头（CBattle_Field.h/WarField.h 已具全部声明）。
// ============================================================================

#include <string.h>

#include <list>
#include <map>
#include <vector>

#include "CBattle_Field.h"         // CBattle_Field/MapInfo/map_item/map_monster
#include "CDungeon.h"              // get_dimension_possible/get_standard_level/…
#include "CDataManager.h"          // G_CDataManager/find_item/find_monster
#include "CGuildServerProxy.h"    // CEventBase / CEventManager::GetRepeatEvent
                                   //   CItemGenerator/CItemGeneratorMgr/CLuckPoint
#include "CItemGeneratorMgr.h"
#include "CSystemTime.h"           // CSystemTime::getCurSec
#include "GlobalData.h"            // s_systemTime_/s_event_manager
#include "LogManager.h"            // LogManager::logFormat
#include "InterfacePacketBuf.h"    // PacketGuard 打包接口
#include "Inven_Item.h"
#include "CEquipItem.h"            // getAvatarPeriod/getAvatarSocket
#include "CItemAmplifier.h"        // stAmplifyOption_t::getAbilityType/Value
#include "GamePODTypes.h"          // stAvatarEmblemInfo_t
#include "BlueMarble.h"            // BlueMarble::getBuff
#include "PvPTypes.h"              // BlueMarbleBuffType
#include "CUserCharacInfo.h"       // get_charac_job/get_charac_level
#include "TimerQueue.h"            // G_TimerQueue/InsertTimer/TIMER_MESSAGE

// CEventBase 虚槽 +0x34 = IsEventing(CUser*)（ORIG _MakeDropItems vcall 实证）。

namespace
{

#define BFKC_ROUND(x) ((int)((x) + 0.5f))

char const s_bfkc_pretty_kill_hell[] =
    "void CBattle_Field::KillHellPartyGroupMonsterCnt(MapInfo&, map_monster&, bool&)";

char const s_bfkc_pretty_check_blood[] =
    "void CBattle_Field::checkKilledSpawnBloodMonsters(int)";

// CEventBase 虚槽 +0x34 = IsEventing(CUser*)（CGuildServerProxy.h 注释实证）。
typedef bool (*BfkcIsEventingFn)(void*, CUser*);

inline bool bfkcIsEventing(CEventBase* ev)
{
    if (ev == 0)
        return false;
    BfkcIsEventingFn fn = reinterpret_cast<BfkcIsEventingFn>(
        (*reinterpret_cast<void***>(ev))[0x34 / 4]);
    return fn(ev, reinterpret_cast<CUser*>(0));
}

}  // namespace

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

    const int groupIdx = static_cast<unsigned short>(mob.m_hellPartyGroupOrder);
    std::map<int, int>::iterator it = mapInfo.m_hellPartyGroup.find(groupIdx);
    if (it == mapInfo.m_hellPartyGroup.end())
    {
        LogManager::logFormat(
            1, "battle_field.cpp", s_bfkc_pretty_kill_hell, 0x1dd9,
            "HELLPARTY_LOG : HELLPARTY GROUP INDEX (%d), MAP INDEX (%d) : "
            "KILL HELL GROUP ERROR!!\n",
            groupIdx, mapInfo.m_mapIndex);
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

// ============================================================================
// checkKilledSpawnBloodMonsters —— ORIG T 0x08304f8e。
// 血轮小轮收口判定：lastSmall<smallRound 时（未到最后一小轮）仅当
// monsterCount==0 且当前地图非 boss 房图才提前 onFinishBloodRound(false)；
// 否则遍历当前图怪物中属于本 smallRound 者（m_bloodRoundOrder>=smallRound），
// 该 smallRound 相对上一 smallRound 发生轮次或难度变化才推进结算：
// ultimate（dm+0x89f==2）路径走 incUltimateRoundClearPoint/
// forceValuesToChangeRound/setUltimateDifficulty + 0x26 计时器（末轮 1s，
// 否则 10s + sendUltimateRandomDifficulty）；普通路径 BloodSpawnTimerManager(true,false)。
// ============================================================================
void CBattle_Field::checkKilledSpawnBloodMonsters(int monsterCount)
{
    const short roundOrder =
        static_cast<short>(m_bloodRound.getBloodRoundOrder());        // -0x26
    const short smallRound =
        static_cast<short>(m_bloodRound.getBloodSmallRoundOrder());   // -0x24
    const short lastSmall =
        static_cast<short>(m_bloodRound.getBloodLastSmallRoundOrder());  // -0x22

    if (static_cast<unsigned short>(lastSmall)
        < static_cast<unsigned short>(smallRound))
    {
        if (monsterCount == 0 && m_pMap->get_map_type() != 1)
        {
            m_bloodReward.onFinishBloodRound(false, roundOrder, m_party,
                                             m_pDungeon);
        }
        return;
    }

    map_monster probe;   // ORIG 局部构造（-0x37c），仅对齐原帧布局
    (void)probe;

    GetCurrentMapInfo();
    std::map<int, map_monster>& monsterMap =
        GetCurrentMapInfo()->m_monsterMap;
    std::map<int, map_monster>::iterator it = monsterMap.begin();     // -0x30
    bool changedRound = false;
    for (;;)
    {
        // ORIG 求值序：每轮 begin 后独立调 end（GetCurrentMapInfo 各一次）。
        GetCurrentMapInfo();
        if (it == monsterMap.end())
            break;
        // mob.m_bloodRoundOrder(+0x33e) < smallRound 时跳出（不结算）。
        if (it->second.m_bloodRoundOrder < smallRound)
            break;

        const int curRound =
            get_blood_round_of_small_round(smallRound);               // -0x1c
        const int prevRound =
            get_blood_round_of_small_round(
                static_cast<short>(static_cast<unsigned short>(smallRound)
                                   - 1));                             // -0x18
        const int curDiff =
            get_blood_round_difficylty_of_small_round(smallRound);    // -0x14
        const int prevDiff =
            get_blood_round_difficylty_of_small_round(
                static_cast<short>(static_cast<unsigned short>(smallRound)
                                   - 1));                             // -0x10
        if (curRound < 0 || prevRound < 0)
        {
            LogManager::logFormat(
                1, "battle_field.cpp", s_bfkc_pretty_check_blood, 0xd59,
                "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ROUND!!\n",
                static_cast<int>(smallRound));
            return;
        }

        changedRound = false;
        if (prevRound != curRound)
            changedRound = true;
        else if (curDiff != prevDiff)
            changedRound = true;
        if (changedRound)
            break;
        ++it;
    }

    if (!changedRound)
        return;   // 循环自然耗尽：ORIG 直接析构返回，无结算

    // ORIG 经 m_party+0xddc 触达（= BF 子对象视图 +0x2b8，同一 m_bloodReward）。
    m_bloodReward.onFinishBloodRound(false, roundOrder, m_party, m_pDungeon);

    // +0x89f（m_bloodDungeonType）按 signed char 读取（ORIG movsbl 口径）
    const signed char ultimateMode = *reinterpret_cast<signed char*>(
        &m_pDungeon->m_bloodDungeonType);
    if (ultimateMode == 2)
    {
        const int diff = getNextRoundDifficulty(
            static_cast<short>(static_cast<unsigned short>(roundOrder) - 1));
        m_bloodReward.incUltimateRoundClearPoint(diff);

        const bool advanced =
            forceValuesToChangeRound(roundOrder, smallRound, 1);
        if (!advanced)
            return;

        setUltimateDifficulty(0);
        if (static_cast<int>(roundOrder) == getBloodMaxRound() - 1)
        {
            G_TimerQueue()->InsertTimer(
                static_cast<TimerEntry::OBJ_TYPE>(1), m_party->GetPartyIndex(),
                static_cast<TIMER_MESSAGE>(0x26), 1,
                m_party->gen_timer_key(static_cast<TIMER_MESSAGE>(0x26)), 0x26);
        }
        else
        {
            G_TimerQueue()->InsertTimer(
                static_cast<TimerEntry::OBJ_TYPE>(1), m_party->GetPartyIndex(),
                static_cast<TIMER_MESSAGE>(0x26), 10,
                m_party->gen_timer_key(static_cast<TIMER_MESSAGE>(0x26)), 0x26);
            sendUltimateRandomDifficulty(0, roundOrder, 10);
        }
    }
    else
    {
        BloodSpawnTimerManager(true, false);
    }
}

// ============================================================================
// send_elevator_time_packet —— ORIG T 0x0830d848。
// 组 0xfd 包：byte(m_elevatorTimer +0x139)、byte(state)；finalize 后 send_to_party。
// ============================================================================
void CBattle_Field::send_elevator_time_packet(char state)
{
    PacketGuard packet;
    packet.put_header(0, 0xfd);
    packet.put_byte(static_cast<int>(static_cast<char>(m_elevatorTimer)));
    packet.put_byte(static_cast<int>(state));
    packet.finalize(true);
    m_party->send_to_party(packet);
}

// ============================================================================
// getItemDropDungeonType —— ORIG T 0x0830aa46。
// 无副本=3；维度可入=2；脚本 boss 图(dungeon+0x89c)=1；普通=0。
// ============================================================================
int CBattle_Field::getItemDropDungeonType()
{
    if (m_pDungeon == 0)
        return 3;
    if (static_cast<signed char>(m_pDungeon->get_dimension_possible()) > 0)
        return 2;
    if (m_pDungeon->m_89c != 0)   // +0x89c 脚本 boss 图标记
        return 1;
    return 0;
}

// ============================================================================
// setPremiumDropRate —— ORIG T 0x0830b848。
// 血轮或掉落域类型!=0 或绝望之塔或 ref+0x40!=0 → rate=0；
// premiumRate==0 → -1（禁用）；否则写 premiumRate。
// ============================================================================
void CBattle_Field::setPremiumDropRate(WongWork::stGenerateRefData_t& ref,
                                       int premiumRate)
{
    bool disable = true;
    if (m_pDungeon != 0 && m_pDungeon->get_blood_dungeon_type() < 1)
    {
        if (getItemDropDungeonType() == 0)
        {
            if (!m_pDungeon->isTowerOfDespairDungeon()
                && ref.m_dungeon == 0)   // +0x40
            {
                disable = false;
            }
        }
    }
    if (disable)
        ref.m_premiumDropRate = 0;
    else if (premiumRate == 0)
        ref.m_premiumDropRate = -1;
    else
        ref.m_premiumDropRate = premiumRate;
}

// ============================================================================
// _MakeNotiPacketDieMonster —— ORIG T 0x0830b8cc。
// 怪物死亡通知包（0x26）：uid、掉落列表逐项（idx/数量/属性/avatar 周期·徽章/
// 强化选项/裸 Inven_Item/头像标志）、mapIndex、APC 击杀数、终点坐标。
// ============================================================================
void CBattle_Field::_MakeNotiPacketDieMonster(
    PacketGuard& packet, std::list<map_item>& itemList, int uid,
    unsigned short mapIndex, const map_monster& mobSrc, map_monster& mobOut)
{
    packet.put_header(0, 0x26);
    packet.put_short(uid);
    packet.put_byte(static_cast<int>(itemList.size()));

    // ORIG 栈上徽章缓冲 0x1e 字节（stAvatarEmblemInfo_t 0x16 + 尾随 8 字节栈
    // 残留一并 put_binary）；此处显式清零替代不可复现的残留。
    unsigned char emblemBuf[0x1e];
    memset(emblemBuf, 0, sizeof(emblemBuf));
    stAvatarEmblemInfo_t& emblem =
        *reinterpret_cast<stAvatarEmblemInfo_t*>(emblemBuf);

    for (std::list<map_item>::iterator it = itemList.begin();
         it != itemList.end(); ++it)
    {
        map_item& mi = *it;
        packet.put_short(mi.m_itemIndex);
        packet.put_int(mi.m_item.m_addInfo);
        packet.put_byte(mi.m_item.GetItemAttr());
        if (!mi.m_item.isAvatarItemType())
        {
            packet.put_int(mi.m_item.get_add_info());
        }
        else
        {
            CItem* found = G_CDataManager()->find_item(mi.m_item.m_addInfo);
            if (found == 0)
            {
                packet.put_int(mi.m_item.get_add_info());
            }
            else
            {
                CEquipItem* equip = static_cast<CEquipItem*>(found);
                const unsigned char slot =
                    *reinterpret_cast<const unsigned char*>(
                        reinterpret_cast<const char*>(&mi) + 0x1b);
                const int period = equip->getAvatarPeriod(slot);
                packet.put_int(period * 0x15180);
                emblem.init();
                equip->getAvatarSocket(slot, emblem);
            }
        }
        packet.put_short(
            static_cast<unsigned int>(*reinterpret_cast<const unsigned short*>(
                reinterpret_cast<const char*>(&mi) + 0x1b)));
        packet.put_byte(mi.m_item.m_amp.getAbilityType());
        packet.put_short(mi.m_item.m_amp.getAbilityValue());
        packet.put_packet(mi.m_item);
        if (!mi.m_item.isAvatarItemType())
        {
            packet.put_byte(0);
        }
        else
        {
            packet.put_byte(1);
            packet.put_int(0x1e);
            packet.put_binary(reinterpret_cast<char*>(emblemBuf), 0x1e);
        }
        packet.put_short(static_cast<unsigned int>(mapIndex));
    }

    packet.put_byte(0);
    const char roleCvt = convertAPCRoleType2MonsterRoleType(
        static_cast<char>(mobSrc.m_roleType));
    if (roleCvt != 0)
        packet.put_byte(m_apcKillCount);
    else
        packet.put_byte(-1);

    packet.put_byte(static_cast<unsigned char>(mobOut.m_flag34));
    if (mobOut.m_flag34 != 0)
    {
        // 终点坐标（m_pos[0]，check_end_point 同源）。
        packet.put_byte(m_pos[0].first);
        packet.put_byte(m_pos[0].second);
    }
    packet.finalize(true);
}

// ============================================================================
// _MakeDropItems —— ORIG T 0x0830adf6（0xa51 字节）。
// 生成器驱动掉落：填 WongWork::stGenerateRefData_t（两遍 blob 写入，中间穿插
// 条件生成调用），经 CItemGeneratorMgr 各 eGenerateType 槽生成 Inven_Item 集，
// 再逐项转 map_item（分配 dropIdx/createTick、蓝矿金币缩放、幸运归属）推入
// itemList 与 MapInfo::Add_Item。
// ref blob 槽位按 ORIG 写入宽度逐条对齐（+0x34..+0x42 为调用方上下文区）。
// ============================================================================
void CBattle_Field::_MakeDropItems(std::list<map_item>& itemList,
                                   MapInfo& mapInfo, const map_monster& mob,
                                   unsigned short uid, int dieReason, int a,
                                   int b, bool hellPartyKilled,
                                   std::vector<bool>& flags, float rate,
                                   int premiumRate)
{
    (void)hellPartyKilled;   // ORIG 栈参数（ebp+0x28），函数体内未读取
    const int curSec = GlobalData::s_systemTime_.getCurSec();       // -0x40
    const char roleCvt = convertAPCRoleType2MonsterRoleType(
        static_cast<char>(mob.m_roleType));                          // -0x39
    const char* mobBlob = reinterpret_cast<const char*>(&mob);

    WongWork::stGenerateRefData_t ref;                               // -0xc0
    char* const refBlob = reinterpret_cast<char*>(&ref);
    ref.m_dropRate = b;                                              // +0x00

    // ---- 第一遍 blob（ORIG +0x067..+0x114） ----
    refBlob[0x37] = static_cast<char>(get_dungeon_diff());           // +0x37
    {
        const char dimPossible = m_pDungeon->get_dimension_possible();
        refBlob[0x36] =
            dimPossible > 0
                ? static_cast<char>(m_hellPartyLevel)
                : static_cast<char>(m_pDungeon->get_standard_level());  // +0x36
    }
    refBlob[0x35] = roleCvt;                                         // +0x35
    refBlob[0x34] = mobBlob[0x10];                                   // +0x34 ← mob.level
    refBlob[0x38] = static_cast<char>(dieReason);                    // +0x38
    refBlob[0x39] = mobBlob[0x12];                                   // +0x39 ← mob.field11 hi
    *reinterpret_cast<int*>(refBlob + 0x3c) = a;                     // +0x3c dword
    *reinterpret_cast<int*>(refBlob + 0x40) = 0;                     // +0x40 dword
    refBlob[0x41] = static_cast<char>(getItemDropDungeonType());     // +0x41
    ref.m_forceTag = flags;                                          // +0x08 vector<bool> op=
    ref.m_field1c = rate;                                            // +0x1c float

    // ---- 幸运点收集 + 蓝矿 buff 死存储（ORIG +0x136..+0x268） ----
    WongWork::stGenerateResult_t result;                             // -0x78
    // +0x0c 子对象（stGenerateResult_t::m_luckPoint 为 WongWork::CLuckPoint，
    // 本函数按全局 CLuckPoint 语义使用，显式转换）
    CLuckPoint* resultLuck = reinterpret_cast<CLuckPoint*>(&result.m_luckPoint);
    int blueBuff1 = 0;                                               // -0x38
    int blueBuff2 = 0;                                               // -0x34
    if (m_party != 0)
    {
        m_party->GetLuckPoint(resultLuck, true);
        if (m_party->IsSinglePlay() && m_party->getManager() != 0
            && m_party->getManager()->getMoveSpace() == 1)
        {
            BlueMarble* marble = static_cast<BlueMarble*>(
                m_party->getManager()->getBlueMarble());
            if (marble != reinterpret_cast<BlueMarble*>(0))
            {
                blueBuff1 = marble->getBuff(
                    m_party->getManager(),
                    static_cast<BlueMarbleBuffType::T>(1));
                blueBuff2 = marble->getBuff(
                    m_party->getManager(),
                    static_cast<BlueMarbleBuffType::T>(2));
                // ORIG -0x9b/-0x98/-0x94/-0x90（blueActive/job/level/buff2）
                // 写后无读取：死存储，保留语义注释即可。
            }
        }
    }
    (void)blueBuff2;

    setPremiumDropRate(ref, premiumRate);

    // ---- 维度/绝望塔标志（ORIG +0x28d..+0x2cb） ----
    // +0x89f/+0x89e（m_bloodDungeonType/m_89e）按 signed char 读取
    const bool ultFlag =
        *reinterpret_cast<signed char*>(&m_pDungeon->m_bloodDungeonType) > 0;
    const bool dimFlag =
        *reinterpret_cast<signed char*>(&m_pDungeon->m_89e) != 0;
    const bool notBoss = mob.m_isBoss != 1;

    WongWork::CItemGeneratorMgr* genMgr = G_CDataManager()->getItemGenerator();

    // ---- 条件生成 1：普通图随机生成（ORIG +0x2e4..+0x32d，gen0.random） ----
    if (!dimFlag && !ultFlag && notBoss)
    {
        genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_0)
            ->generateRandomItem(ref, result);
    }

    // ---- 第二遍 blob（ORIG +0x32f..+0x425；覆盖第一遍大部分槽位） ----
    {
        const char dimPossible = m_pDungeon->get_dimension_possible();
        refBlob[0x35] =
            dimPossible > 0
                ? static_cast<char>(m_hellPartyLevel)
                : static_cast<char>(m_pDungeon->get_standard_level());  // +0x35
    }
    refBlob[0x36] = static_cast<char>(get_dungeon_diff());           // +0x36
    *reinterpret_cast<int*>(refBlob + 0x38) = mob.m_mobIndex;        // +0x38 dword
    refBlob[0x3c] = mobBlob[0x8];                                    // +0x3c ← roleType
    refBlob[0x34] = mobBlob[0x10];                                   // +0x34 ← level
    refBlob[0x3d] = mobBlob[0x11];                                   // +0x3d ← field11 lo
    refBlob[0x3e] = static_cast<char>(dieReason);                    // +0x3e
    refBlob[0x3f] = static_cast<char>(m_party->getMemberMaxLevel()); // +0x3f
    refBlob[0x40] = static_cast<char>(uid);                          // +0x40 byte
    refBlob[0x41] = mob.m_hellPartyGroupIdx != 0 ? 1 : 0;            // +0x41
    refBlob[0x42] = mob.m_hellPartyScriptIdx;                        // +0x42
    const bool bloodFlag = mob.m_bloodRoundFlag != 0;                // -0x7b
    (void)m_party->GetPartyMemberForceDropFlag();                    // -0x9c（死存储）

    // ---- 条件生成 2：普通图指定生成（ORIG +0x42b..+0x47c，gen0.specific） --
    if (notBoss || bloodFlag)
    {
        genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_0)
            ->generateSpecificItem(ref, result);
    }

    // ---- 无条件：getDropItems 槽写入 + gen4.specific（ORIG +0x47e..+0x4cd）-
    *reinterpret_cast<void**>(refBlob + 0x34) = m_pDungeon->getDropItems();
    genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4)
        ->generateSpecificItem(ref, result);

    refBlob[0x35] = mobBlob[0x10];                                   // +0x35 ← level

    // ---- 条件生成 3：难度档随机（ORIG +0x4dc..+0x5c2，gen3.random） -------
    if (!dimFlag && !ultFlag && notBoss)
    {
        if (dieReason >= 1 && dieReason <= 4)
        {
            // dm+0x524 = m_serverParameter.m_weekendBouns4b4.m_field8（+0x68+0x4b4+8）
            const signed char cnt = *reinterpret_cast<signed char*>(
                reinterpret_cast<char*>(
                    &G_CDataManager()->m_serverParameter.m_weekendBouns4b4.m_field8)
                + dieReason - 1);
            for (int i = 0; i < cnt; ++i)
            {
                genMgr
                    ->getGenerator(
                        WongWork::CItemGeneratorMgr::eGenerateType_3)
                    ->generateRandomItem(ref, result);
            }
        }
        else
        {
            genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_3)
                ->generateRandomItem(ref, result);
        }
    }

    // ---- 条件生成 4/5：PC 房事件加成（ORIG +0x5c4..+0x71f，gen5/gen6） -----
    if (!dimFlag && !ultFlag)
    {
        if (m_party != 0
            && bfkcIsEventing(GlobalData::s_event_manager->GetRepeatEvent(0x16))
            && m_party->hasPCRoomPlayer() && notBoss)
        {
            genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_5)
                ->generateRandomItem(ref, result);
        }
        if (m_party != 0
            && bfkcIsEventing(GlobalData::s_event_manager->GetRepeatEvent(0x30))
            && m_party->hasPCRoomPlayer() && notBoss)
        {
            genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_6)
                ->generateRandomItem(ref, result);
        }
    }

    // ---- 条件生成 6：PC 房玩家追加（ORIG +0x726..+0x76f，gen7.random） -----
    if (m_party != 0 && m_party->hasPCRoomPlayer())
    {
        genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_7)
            ->generateRandomItem(ref, result);
    }

    // ---- 结果落地（ORIG +0x771..+0x9ee） ----------------------------------
    int seq = 0;                                                     // -0x30
    for (std::vector<Inven_Item>::iterator gi = result.m_dropItems.begin();
         gi != result.m_dropItems.end(); ++gi)
    {
        map_item mi;                                                 // -0x114
        mi.m_count = 1;
        mi.m_itemIndex = mapInfo.m_dropItemCnt;
        mapInfo.m_dropItemCnt = mi.m_itemIndex + 1;
        mi.m_dropIndex = uid;
        mi.m_createTick = curSec;
        memcpy(&mi.m_item, &*gi, 0x3d);

        // 蓝矿金币缩放：单人且 manager 在移动空间且 itemIdx==0 时按脚本系数
        // ROUND((buff*f2+f1)*gold) 缩放 item.m_addInfo2(+0x07)。
        if (m_party != 0 && m_party->IsSinglePlay()
            && m_party->getManager() != 0
            && m_party->getManager()->getMoveSpace() == 1
            && mi.m_item.m_addInfo == 0)
        {
            const float f1 = *reinterpret_cast<float*>(
                reinterpret_cast<char*>(G_CDataManager()->getBlueMarbleScript())
                + 0x68);
            const float f2 = *reinterpret_cast<float*>(
                reinterpret_cast<char*>(G_CDataManager()->getBlueMarbleScript())
                + 0x6c);
            int buff = blueBuff1;
            if (buff < 1)
                buff = 1;
            mi.m_item.m_addInfo2 =
                BFKC_ROUND((buff * f2 + f1)
                           * static_cast<float>(mi.m_item.m_addInfo2));
        }

        mi.m_ownerId = resultLuck->GetUserGainItem(seq);
        itemList.push_back(mi);
        mapInfo.Add_Item(mi);
        seq += 1;
    }
}
