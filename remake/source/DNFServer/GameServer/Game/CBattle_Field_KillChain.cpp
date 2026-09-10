// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBattle_Field_KillChain/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

char const s_bfkc_pretty_kill_hell[] =
    "void CBattle_Field::KillHellPartyGroupMonsterCnt(MapInfo&, map_monster&, bool&)";

char const s_bfkc_pretty_check_blood[] =
    "void CBattle_Field::checkKilledSpawnBloodMonsters(int)";

// G_CDataManager()->getBlueMarbleScript() 返回 void*；ORIG 0x0830b6fa/0x0830b70d
// 为单条 `mov 0x68(%eax)`/`mov 0x6c(%eax)` disp 读（float 系数）。取址算术
// （char*+0x68）在 4.4 恒多一条 add ⇒ 类型化视图（P58，trivial、零符号发射）。
struct BfkcBlueMarbleScriptView
{
    char m_pad00[0x68];
    float m_f1;      // +0x68
    float m_f2;      // +0x6c
};

}  // namespace

// ============================================================================
// KillHellPartyGroupMonsterCnt —— ORIG T 0x0830d704。
// 地狱派对组击杀计数：m_killedAllHellGroups(+0x1ca) 置位直接返回；
// 以 mob.m_hellPartyGroupOrder(+0x336，ORIG movzwl 实证)查 MapInfo::m_hellPartyGroup：
// 未登记 → HELLPARTY_LOG 0x1dd9；组剩余==0 → 移除组并 killedAll=true，
// 组表清空时置 m_killedAllHellGroups；否则剩余-1 且 killedAll=false。
// ============================================================================
#include "CBattle_Field_KillChain/KillHellPartyGroupMonsterCnt.inc"
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
#include "CBattle_Field_KillChain/checkKilledSpawnBloodMonsters.inc"
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
                1, "battle_field.cpp", __PRETTY_FUNCTION__, 0xd59,
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
#include "CBattle_Field_KillChain/send_elevator_time_packet.inc"
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
#include "CBattle_Field_KillChain/getItemDropDungeonType.inc"
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
#include "CBattle_Field_KillChain/setPremiumDropRate.inc"
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
#include "CBattle_Field_KillChain/_MakeNotiPacketDieMonster.inc"
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
            // [b28d-20] ORIG 调 put_binary(char const*, int)（p167 实证
            // _ZN18InterfacePacketBuf10put_binaryEPKci）；传 char* 会选到
            // put_binary(char*, int) 重载（EXTRA Pci）。
            packet.put_binary(reinterpret_cast<const char*>(&emblem), 0x1e);
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
        // 终点坐标（m_endX/m_endY，check_end_point 同源）。
        packet.put_byte(m_endX);
        packet.put_byte(m_endY);
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
#include "CBattle_Field_KillChain/_MakeDropItems.inc"
                                   MapInfo& mapInfo, const map_monster& mob,
                                   unsigned short uid, int dieReason, int a,
                                   int b, bool hellPartyKilled,
                                   std::vector<bool>& flags, float rate,
                                   int premiumRate)
{
    const int curSec = GlobalData::s_systemTime_.getCurSec();       // -0x40
    const char roleCvt = convertAPCRoleType2MonsterRoleType(
        static_cast<char>(mob.m_roleType));                          // -0x39

    WongWork::stGenerateRefData_t ref;                               // -0xc0
    ref.m_dropRate = b;                                              // +0x00

    // ---- 第一遍 blob（ORIG +0x067..+0x114） ----
    // [b28d-20] P183 地址重物化：ORIG 每处均为 `mov %al,-0x??(%ebp)` 单条 ref
    // 成员直写；refBlob/mobBlob 指针缓存形每处多 mov+lea/add 两条。
    ref.m_pad37 = static_cast<char>(get_dungeon_diff());             // +0x37
    // [b28d-20] ORIG `test %al,%al; setg %al; test %al,%al; je`：无 dimPossible
    // 具名槽（P0 表达式直用）；具名 char 局部会多一槽并改分支形。
    if (static_cast<signed char>(m_pDungeon->get_dimension_possible()) > 0)
        ref.m_rateSlot = static_cast<char>(m_standardDimensionLevel);
    else
        ref.m_rateSlot = static_cast<char>(m_pDungeon->get_standard_level());
    ref.m_field35 = roleCvt;                                         // +0x35
    ref.m_field34 = static_cast<char>(mob.m_level);                  // +0x34 ← mob.level
    ref.m_field38 = static_cast<unsigned char>(dieReason);           // +0x38
    ref.m_field39 = mob.m_field11b.m_field11b1;                      // +0x39 ← mob.field11 hi
    ref.m_field3c = a;                                               // +0x3c dword
    // [b28d-20] ORIG 0830aee0 `movb $0x0,-0x80(%ebp)` 为 1 字节写（P10 宽度判据）。
    ref.m_field40_byte = 0;                                          // +0x40 byte
    ref.m_field41 = static_cast<unsigned char>(getItemDropDungeonType()); // +0x41
    ref.m_forceTag = flags;                                          // +0x08 vector<bool> op=
    ref.m_field1c = rate;                                            // +0x1c float

    // ---- 幸运点收集 + 蓝矿 buff 死存储（ORIG +0x136..+0x268） ----
    // [b28d-20] P183：ORIG 每次 `lea -0x78(%ebp),%eax; add $0xc,%eax` 重算
    // m_luckPoint 地址，无 resultLuck 指针缓存槽。
    int blueBuff1 = 0;                                               // -0x38
    int blueBuff2 = 0;                                               // -0x34
    // [b28d-20] ORIG 发射序 blueBuff1/2 清零 → result C1（P212 声明序=发射序）。
    WongWork::stGenerateResult_t result;                             // -0x78
    if (m_party != 0)
    {
        m_party->GetLuckPoint(&result.m_luckPoint, true);
        if (m_party->IsSinglePlay() && m_party->getManager() != 0)
        {
            if (m_party->getManager()->getMoveSpace() == 1)
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
                    // [b28d-20] ORIG -0x9b/-0x98/-0x94/-0x90 落在 ref 对象内部
                    // （ref=-0xc0，sizeof 0x48）⇒ 不是独立局部，而是 ref 字段写入：
                    // +0x25=1 / +0x28=get_charac_job() / +0x2c=get_charac_level() /
                    // +0x30=blueBuff2（原 (void) 丢弃 = 真语义缺口）。
                    // ORIG 逐处重调 getManager（P183 禁缓存 ⇒ 原样内联重取）。
                    ref.m_field25 = 1;
                    // [SweepMonsterDrop 2026-09-09] m_field28 int→ENUM_CHARACTERJOB
                    // （P254）：get_charac_job() 返回 int ⇒ 必须显式 cast；探针实测
                    // cast 形仍为单条直存（与 ORIG 0x0830b03d `mov %eax,-0x98` 同形）。
                    ref.m_field28 = static_cast<ENUM_CHARACTERJOB>(
                        m_party->getManager()->get_charac_job());
                    ref.m_field2c = m_party->getManager()->get_charac_level();
                    ref.m_field30 = blueBuff2;
                }
            }
        }
    }
    setPremiumDropRate(ref, premiumRate);

    // ---- 维度/绝望塔标志（ORIG +0x28d..+0x2cb） ----
    // [b28d-20] ORIG 写 -0x7c/-0x7d 落在 ref 对象内部 ⇒ 是 ref 字段
    // （m_field44_byte=ultFlag、m_field43=dimFlag），非独立局部。
    // 成员直读单条 movzbl（P235：取址视图多一条 add）；比较按 signed char（jle/je）。
    // ORIG 物化形：ultFlag 为 if/else 直赋（movb $1/$0）；dimFlag 为三目经 eax。
    if (static_cast<signed char>(m_pDungeon->m_bloodDungeonType) > 0)
        ref.m_field44_byte = 1;
    else
        ref.m_field44_byte = 0;
    // [N5A] m_field43 为 bool（消费者形 `movzbl;xor $1;test;je` 双处实证；h_probe H2 对
    // uchar 目标反产 `test;jne`）。ORIG 的 store 形（0x0830b0b5..0xc1）只能由「向**字节
    // 左值**写 int 三目」产生（4.1.2/4.4.7 对 bool 目标恒 setne：e_probe E1..E5 /
    // x_probe X1..X7 实测）⇒ 经 CItemGenerator.h 的 union 字节视角写（u_probe.cpp 实证）。
    ref.m_field43Byte = static_cast<signed char>(m_pDungeon->m_89e) != 0 ? 1 : 0;
    // [b28d-20] ORIG 无 notBoss 局部槽（使用点内联 `movzbl 0x349; xor $1`）
    // ⇒ P0 判读：使用点直写 !mob.m_isBoss（bool 字段裸测形）。

    // ---- 条件生成 1：普通图随机生成（ORIG +0x2e4..+0x32d，gen0.random） ----
    if (!ref.m_field43 && !ref.m_field44_byte && !mob.m_isBoss)
    {
        G_CDataManager()->getItemGenerator()->getGenerator(
                WongWork::CItemGeneratorMgr::eGenerateType_0)
            ->generateRandomItem(ref, result);
    }

    // ---- 第二遍 blob（ORIG +0x32f..+0x425；覆盖第一遍大部分槽位） ----
    if (static_cast<signed char>(m_pDungeon->get_dimension_possible()) > 0)
        ref.m_field35 = static_cast<char>(m_standardDimensionLevel);
    else
        ref.m_field35 = static_cast<char>(m_pDungeon->get_standard_level());
    ref.m_rateSlot = static_cast<char>(get_dungeon_diff());          // +0x36
    ref.m_rankBonusIndex = mob.m_mobIndex;                           // +0x38 dword
    ref.m_field3c_byte = static_cast<unsigned char>(mob.m_roleType); // +0x3c ← roleType
    ref.m_field34 = static_cast<char>(mob.m_level);                  // +0x34 ← level
    ref.m_field3d = mob.m_field11b.m_field11b0;                      // +0x3d ← field11 lo
    ref.m_field3e_byte = static_cast<unsigned char>(dieReason);      // +0x3e
    ref.m_field3f =
        static_cast<unsigned char>(m_party->getMemberMaxLevel());    // +0x3f
    // [b28d-20] ORIG 0x0830b1c7 `movzbl -0x120(%ebp)` 读的是 hellPartyKilled 槽
    // （序言 0x28(%ebp)→-0x120），不是 uid（-0x11c）⇒ 原 uid 赋值是错参数来源。
    ref.m_field40_byte = static_cast<unsigned char>(hellPartyKilled); // +0x40 byte
    // [b28d-20] ORIG 0x0830b1d4 `movzwl; test %ax,%ax; setne %al` ⇒ 直接赋 bool 表达式。
    ref.m_field41 = static_cast<unsigned char>(
        mob.m_hellPartyGroupIdx != 0);                               // +0x41
    ref.m_field42_byte = static_cast<unsigned char>(mob.m_hellPartyScriptIdx); // +0x42
    // [b28d-20] ORIG -0x7b = ref+0x45（m_field45）⇒ bloodFlag 是 ref 字段写入。
    ref.m_field45 = mob.m_bloodRoundFlag != 0 ? 1 : 0;               // +0x45
    // [b28d-20] ORIG `mov %al,-0x9c(%ebp)` = ref+0x24（m_field24）⇒ 返回值写入
    // ref 字段（原 (void) 丢弃 = 真语义缺口）。
    ref.m_field24 = static_cast<char>(m_party->GetPartyMemberForceDropFlag());

    // ---- 条件生成 2：普通图指定生成（ORIG +0x42b..+0x47c，gen0.specific） --
    // [b28d-20] ORIG 0x0830b232 `movzbl -0x7f(%ebp)` = ref+0x41（m_field41，
    // hellPartyGroupIdx!=0），非 m_field45（后者全函数写后未读 = 死存储）。
    if (!mob.m_isBoss || ref.m_field41)
    {
        G_CDataManager()->getItemGenerator()->getGenerator(
                WongWork::CItemGeneratorMgr::eGenerateType_0)
            ->generateSpecificItem(ref, result);
    }

    // ---- 无条件：getDropItems 槽写入 + gen4.specific（ORIG +0x47e..+0x4cd）-
    ref.m_field34p = m_pDungeon->getDropItems();                     // +0x34 指针
    G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_4)
        ->generateSpecificItem(ref, result);

    ref.m_field35 = static_cast<char>(mob.m_level);                  // +0x35 ← level

    // ---- 条件生成 3：难度档随机（ORIG +0x4dc..+0x5c2，gen3.random） -------
    if (!ref.m_field43 && !ref.m_field44_byte && !mob.m_isBoss)
    {
        if (dieReason >= 1 && dieReason <= 4)
        {
            // dm+0x524 = m_serverParameter.m_weekendBouns4b4.m_dropRoutineByParty
            // （+0x68+0x4b4+8；union 数组视角 ⇒ 单条 `movzbl 0x524(%eax,%edx,1)`，
            // 取址形会多 add，P235/P311）
            // [b28d-20] ORIG movzbl+movsbl 后存 **int** 槽（-0x28）⇒ 局部为 int。
            const int cnt = static_cast<signed char>(
                G_CDataManager()->m_serverParameter.m_weekendBouns4b4
                    .m_dropRoutineByParty[dieReason - 1]);
            for (int i = 0; i < cnt; ++i)
            {
                G_CDataManager()->getItemGenerator()
                    ->getGenerator(
                        WongWork::CItemGeneratorMgr::eGenerateType_3)
                    ->generateRandomItem(ref, result);
            }
        }
        else
        {
            G_CDataManager()->getItemGenerator()->getGenerator(
                    WongWork::CItemGeneratorMgr::eGenerateType_3)
                ->generateRandomItem(ref, result);
        }
    }

    // ---- 条件生成 4/5：PC 房事件加成（ORIG +0x5c4..+0x71f，gen5/gen6） -----
    if (!ref.m_field43 && !ref.m_field44_byte)
    {
        if (m_party != 0
            && GlobalData::s_event_manager->GetRepeatEvent(0x16)->IsEventing(0)
            && m_party->hasPCRoomPlayer() && !mob.m_isBoss)
        {
            G_CDataManager()->getItemGenerator()->getGenerator(
                    WongWork::CItemGeneratorMgr::eGenerateType_5)
                ->generateRandomItem(ref, result);
        }
        if (m_party != 0
            && GlobalData::s_event_manager->GetRepeatEvent(0x30)->IsEventing(0)
            && m_party->hasPCRoomPlayer() && !mob.m_isBoss)
        {
            G_CDataManager()->getItemGenerator()->getGenerator(
                    WongWork::CItemGeneratorMgr::eGenerateType_6)
                ->generateRandomItem(ref, result);
        }
    }

    // ---- 条件生成 6：PC 房玩家追加（ORIG +0x726..+0x76f，gen7.random） -----
    // [b28d-20] ORIG 0x0830b51c 直接 `mov (%eax),%eax; mov %eax,(%esp); call
    // hasPCRoomPlayer`，无 m_party 判空（P140：ORIG 无防御时不补）。
    if (m_party->hasPCRoomPlayer())
    {
        G_CDataManager()->getItemGenerator()->getGenerator(
                WongWork::CItemGeneratorMgr::eGenerateType_7)
            ->generateRandomItem(ref, result);
    }

    // ---- 结果落地（ORIG +0x771..+0x9ee） ----------------------------------
    int seq = 0;                                                     // -0x30
    // [b28d-20] P13/P283：ORIG 循环前 begin 一次、end 一次（各自独立槽），cond 用
    // 两缓存迭代器比较；把 end() 写进 for 条件会每轮重调 end。
    std::vector<Inven_Item>::iterator gi = result.m_dropItems.begin();
    std::vector<Inven_Item>::iterator giEnd = result.m_dropItems.end();
    for (; gi != giEnd; ++gi)
    {
        map_item mi;                                                 // -0x114
        mi.m_count = 1;
        mi.m_itemIndex = mapInfo.m_dropItemCnt++;  // N5A: ORIG 复用 eax（无 reload）
        mi.m_dropIndex = uid;
        mi.m_createTick = curSec;
        // P83/N84：ORIG 为 Inven_Item(0x3d) 结构整体拷贝（15×mov dword+1×movb，
        // 非 rep movsl）；memcpy 在 4.4 恒退化为 call。
        mi.m_item = *gi;

        // 蓝矿金币缩放：单人且 manager 在移动空间且 itemIdx==0 时按脚本系数
        // ROUND((buff*f2+f1)*gold) 缩放 item.m_addInfo2(+0x07)。
        if (m_party != 0)
        {
            if (m_party->IsSinglePlay() && m_party->getManager() != 0)
            {
                if (m_party->getManager()->getMoveSpace() == 1)
                {
                    if (mi.m_item.m_addInfo == 0)
                    {
                        const float f1 = reinterpret_cast<BfkcBlueMarbleScriptView*>(
                            G_CDataManager()->getBlueMarbleScript())->m_f1;
                        const float f2 = reinterpret_cast<BfkcBlueMarbleScriptView*>(
                            G_CDataManager()->getBlueMarbleScript())->m_f2;
                        // [b28d-20] ORIG 直接改写 blueBuff1 槽（无独立 buff 局部），且公式为
                        // 纯 (int) 截断（fistpl+RC=11，无 fadds 0.5）⇒ 原 BFKC_ROUND 的
                        // +0.5f 多出 flds 0.5/faddp 且语义不同（四舍五入 vs 截断）=真 bug。
                        // P243：ORIG 首条 fildl 即 addInfo2（目标在左）⇒ 目标写在乘法左侧。
                        if (blueBuff1 < 1)
                            blueBuff1 = 1;
                        mi.m_item.m_addInfo2 = static_cast<int>(
                            static_cast<float>(mi.m_item.m_addInfo2)
                            * (blueBuff1 * f2 + f1));
                    }
                }
            }
        }

        mi.m_ownerId = result.m_luckPoint.GetUserGainItem(seq);
        itemList.push_back(mi);
        mapInfo.Add_Item(mi);
        seq += 1;
    }
}
