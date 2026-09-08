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

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h         // CBattle_Field/MapInfo/map_item/map_monster
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

    // P0：ORIG 无 groupIdx 槽（logFormat 参数处 movzwl 0x336 直读重读），
    // 表达式直写两处内联。
    std::map<int, int>::iterator it =
        mapInfo.m_hellPartyGroup.find(
            static_cast<int>(mob.m_hellPartyGroupOrder));
    if (it == mapInfo.m_hellPartyGroup.end())
    {
        LogManager::logFormat(
            1, "battle_field.cpp", s_bfkc_pretty_kill_hell, 0x1dd9,
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

    // [b28d-20] ORIG 0x08304fd5 movzwl -0x22; cmp -0x24,%ax; jge —— 有符号 short 比较
    // （unsigned cast 会产双 movzwl + jae）。
    if (lastSmall < smallRound)
    {
        if (monsterCount == 0 && m_pMap->get_map_type() != 1)
        {
            m_bloodReward.onFinishBloodRound(false, roundOrder, m_party,
                                             m_pDungeon);
        }
        return;
    }

    // [b28d-20] ORIG 0x0830504b `movl $0x0,-0x20(%ebp)` 为写后未读的死局部（P171 照抄；
    // 补此 4 字节使全帧槽号与帧尺寸 0x3a0 对齐）。
    int deadSlot = 0;
    (void)deadSlot;
    map_monster probe;   // ORIG 局部构造（-0x37c），仅对齐原帧布局
    (void)probe;

    // [b28d-20] ORIG 0x08305060/0x0830535b：begin/end 各自内联 GetCurrentMapInfo()（无
    // monsterMap 引用缓存槽）；循环为标准 for（incr 在 cond 前），结算体在循环内，
    // changedRound==0 走 continue（je <++it>），结算完 break 落 +0x428 析构 probe。
    for (std::map<int, map_monster>::iterator it =
             GetCurrentMapInfo()->m_monsterMap.begin();
         it != GetCurrentMapInfo()->m_monsterMap.end(); )
    {
        // [b28d-20] 真语义 bug 修正：ORIG 0x08305090 读 pair+0x33e = map_monster+0x33a
        // = m_bloodSmallRoundOrder（原写 m_bloodRoundOrder(+0x33e) 会读成 pair+0x342）。
        if (it->second.m_bloodSmallRoundOrder < smallRound)
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

        // [b28d-20] ORIG 0x0830514b movb $0x0/-0x9 与 0x08305169 cmpb $0x0,-0x9 直比：
        // 局部为 char（bool 会产 movzbl+xor$1+test 三段，P248 档②）。
        char changedRound = 0;
        if (prevRound != curRound)
            changedRound = 1;
        else if (curDiff != prevDiff)
            changedRound = 1;
        // [b28d-20] ORIG 0x08305350 的 ++it 是循环体自然结尾（changedRound==0 走
        // `je +0x3c2` 直落 incr 块后落 cond）；写成 for 第三段会被 4.4 旋进体内。
        if (changedRound != 0)
        {
            // [b28d-20] ORIG 0x08305185 `mov (%ebx),%ebx; add $0xddc,%ebx`：第二处结算的
            // this 经 m_party->m_battleField(+0xb24).m_bloodReward(+0x2b8) 折叠为
            // m_party+0xddc（与首处提前返回分支的 this+0x2b8 写法不同，运行时同一对象）。
            m_party->m_battleField.m_bloodReward.onFinishBloodRound(
                false, roundOrder, m_party, m_pDungeon);

        // [b28d-20] ORIG 0x083051b5 `movzbl 0x89f(%eax),%eax; cmp $0x2,%al`：真实成员直访问
        // （reinterpret_cast 视图会多一条 add $0x89f，P235）。
        if (m_pDungeon->m_bloodDungeonType == 2)
        {
            // [b28d-20] ORIG 0x083051e4/0x08305213：实参内联（无 diff/advanced 具名局部）。
            m_bloodReward.incUltimateRoundClearPoint(getNextRoundDifficulty(
                static_cast<short>(static_cast<unsigned short>(roundOrder) - 1)));

            if (!forceValuesToChangeRound(roundOrder, smallRound, 1))
                return;

            setUltimateDifficulty(0);
            // [b28d-20] ORIG 0x08305245 setne + je<1s 块>：条件取反并把 10s 块放 then
            // （写 `==` 会产 sete 且分支互换）。
            if (static_cast<int>(roundOrder) != getBloodMaxRound() - 1)
            {
                G_TimerQueue()->InsertTimer(
                    static_cast<TimerEntry::OBJ_TYPE>(1), m_party->GetPartyIndex(),
                    static_cast<TIMER_MESSAGE>(0x26), 10,
                    m_party->gen_timer_key(static_cast<TIMER_MESSAGE>(0x26)), 0x26);
                sendUltimateRandomDifficulty(0, roundOrder, 10);
            }
            else
            {
                G_TimerQueue()->InsertTimer(
                    static_cast<TimerEntry::OBJ_TYPE>(1), m_party->GetPartyIndex(),
                    static_cast<TIMER_MESSAGE>(0x26), 1,
                    m_party->gen_timer_key(static_cast<TIMER_MESSAGE>(0x26)), 0x26);
            }
        }
        else
        {
            BloodSpawnTimerManager(true, false);
        }
            break;
        }
        ++it;
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

// 血轮或掉落域类型!=0 或绝望之塔或 ref+0x40 低字节!=0 → rate=0；
// premiumRate==0 → -1（禁用）；否则写 premiumRate。
// ORIG 0830b88a movzbl 0x40(%eax)（1 字节读），无 m_pDungeon 空检查；
// 2026-08-30 Fix21_C 修正（原用 4 字节 m_dungeon 指针比较且多加空检查）。
// ============================================================================
void CBattle_Field::setPremiumDropRate(WongWork::stGenerateRefData_t& ref,
                                       int premiumRate)
{
    // ORIG 0x830b848：短路链单表达式（无 bool 槽），链尾 mov$1/mov$0 双岛+
    // test；首条件 test+jg（有符号 char >0）。+0x40 低字节单条 disp 读
    // （movzbl 0x40(%eax)）。
    // ORIG 0x830b85f test%al+jg：源码按有符号 char>0（P22/P3 证据形态）；
    // 次条件 ORIG test%al：调用侧按单字节比较收窄（P3）。
    if (static_cast<signed char>(m_pDungeon->get_blood_dungeon_type()) > 0
        || static_cast<unsigned char>(getItemDropDungeonType()) != 0
        || m_pDungeon->isTowerOfDespairDungeon()
        || ref.m_field40_byte != 0)
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

    // [b28d-20] ORIG 0x0830b923/0x0830ba38 两处均为 `emblem.init()`（单参数成员调用，
    // stAvatarEmblemInfo_t sizeof=0x1e）；memset 三参数形与引用缓存槽（-0x24）均为偏差。
    stAvatarEmblemInfo_t emblem;
    emblem.init();

    for (std::list<map_item>::iterator it = itemList.begin();
         it != itemList.end(); it++)   // [b28d-20] ORIG 0x0830bb79 调 ppEi（后置，P28 增量侧）
    {
        map_item& mi = *it;
        packet.put_short(mi.m_itemIndex);
        packet.put_int(mi.m_item.m_addInfo);
        packet.put_byte(mi.m_item.GetItemAttr());
        // [b28d-20] ORIG 0x0830b9b0 test%al/je<else>（正极性：isAvatarItemType() 为真走
        // avatar 路径）—— `!isAvatarItemType()` 会多一条 xor $1 并反转分支布局。
        if (mi.m_item.isAvatarItemType())
        {
            CItem* found = G_CDataManager()->find_item(mi.m_item.m_addInfo);
            if (found == 0)
            {
                packet.put_int(mi.m_item.get_add_info());
            }
            else
            {
                CEquipItem* equip = static_cast<CEquipItem*>(found);
                // [b28d-20] ORIG 0x0830ba05 movzwl+movzbl（uchar 实参）/ 0x0830ba40
                // movzwl+movzwl（ushort→int 实参）：字段每处重读，无 slot/period 具名局部；
                // getAvatarPeriod 的 this 取自 -0x10（find_item 结果槽），getAvatarSocket 取 -0xc。
                packet.put_int(static_cast<CEquipItem*>(found)->getAvatarPeriod(
                    (unsigned char)mi.m_item.m_fieldb) * 0x15180u);
                emblem.init();
                equip->getAvatarSocket(
                    (unsigned short)mi.m_item.m_fieldb, emblem);
            }
        }
        else
        {
            packet.put_int(mi.m_item.get_add_info());
        }
        packet.put_short(
            static_cast<unsigned int>(mi.m_item.m_fieldb));
        packet.put_byte(mi.m_item.m_amp.getAbilityType());
        packet.put_short(mi.m_item.m_amp.getAbilityValue());
        packet.put_packet(mi.m_item);
        if (mi.m_item.isAvatarItemType())
        {
            packet.put_byte(1);
            packet.put_int(0x1e);
            packet.put_binary(reinterpret_cast<char*>(&emblem), 0x1e);
        }
        else
        {
            packet.put_byte(0);
        }
        packet.put_short(static_cast<unsigned int>(mapIndex));
    }

    packet.put_byte(0);
    // [b28d-20] ORIG 0x0830bbdc test%al/setne%al/test%al/je：条件直判 callee 返回值，
    // 无 roleCvt 具名槽（具名 char 局部会产 mov %al,-N; cmpb $0,-N 两段）。
    if (convertAPCRoleType2MonsterRoleType(
            static_cast<char>(mobSrc.m_roleType)) != 0)
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
                ? static_cast<char>(m_standardDimensionLevel)
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
                ? static_cast<char>(m_standardDimensionLevel)
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
