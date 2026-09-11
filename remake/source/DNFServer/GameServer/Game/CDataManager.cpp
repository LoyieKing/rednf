// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDataManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// df_game_r CDataManager（全服数据仓库）还原（2026-08-16）。
// 函数形态/成员偏移以 ORIG 汇编为准；ST*Script 类实现属后续脚本批次，
// 本 TU 只声明其 C1/D1 并调用。

#include "CDataManager.h"
#include "CExpertJobList.h"  // CExpertJobList 完整类型（GetExpertJobScript → get(job)）
#include "CGameOption.h"  // CChattingEmoticonConfig 完整类型（ReCalc 需要）
#include "PvPTypes.h"    // BlueMarbleScriptManager 唯一权威声明（替代本 TU 局部镜像）
#include "CMap.h"          // CMap 完整类型（VerifyMap）
#include "CDungeon.h"      // CDungeon/MazeScript 完整类型（VerifyMap）
#include "Quest.h"         // Quest 完整类型（reselectDailyTrainingQuest）
#include "GameWorld.h"     // G_GameWorld/IsPVPChannel（GetPvPChannelGrade）
#include "CCharacList.h"   // CCharacList 权威定义（原 pad 镜像删除；ctor/dtor/init 见 CtorProducers_1.cpp）
#include "CItemLimitEdition.h"  // CItemLimitEditionMgr 唯一权威声明（m_npcRelation 删除）
#include "RefPvpGrade.h"     // stPvpRefTable_t/stPvPGradeExp_t/RefPvpGrade（权威定义）
#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat
#include "DisJoint.h"             // DisJoint/DisjointScript 权威类型（本地镜像删除，定义 CtorProducers_2.cpp）
#include "StrikerScript.h"        // StrikerScript 权威类型（本地镜像删除）
#include "CharacManageScript.h"   // CharacManageScript 权威类型（本地镜像删除）
#include "CItemUpgrade_Separate.h"  // WongWork::CItemUpgrade_Separate 权威类型（原全局命名空间镜像 mangled 错误，删除）
// __slang_filter__FlexLexer 基类：DNFLex.h 已把 yyFlexLexer 宏占为
// __dnf_script__FlexLexer，这里按 flex 手册手动切换 flavor 后再引权威 SlangFilter.h。
#undef yyFlexLexer
#define yyFlexLexer __slang_filter__FlexLexer
#include "FlexLexer.h"
#include "SlangFilter.h"  // SlangFilter 权威声明（destroy() 按ORIG经虚 D0 析构）
#include "GlobalData.h"      // GlobalData::s_ref_pvp_grade / s_db_mgr / s_p* 全局

// ---- init() 依赖（ORIG 0x08358332 链）----
#include "sync_script.h"             // CSyncScript/STEquipmentPartSetPiece（须先于 deps 头）
#include "DataManagerInitDepends.h"  // 脚本装载自由函数唯一声明点（须在 CDataManager.h 后）
#include "CEventScriptMng.h"         // EventClassify::CEventScriptMng 完整类型
#include "CCubeStatistic.h"          // getItemGroupNameTable 权威声明
#include "EventUpgradeJarInfo.h"     // Event_Upgrade_Jar 完整类型
#include "InstanceRentalSystem.h"    // InstanceRentalSystem::Ready
#include "CMCAPManager.h"            // WongWork::CMCAPManager::init
#include "CAssaultMgr.h"             // pvp_assault::GetInstanceAssaultMgr/SetAssaultChannel

// CGameManager 最小视图（避免 QuickParty.h/CDungeon.h 守卫冲突；
// 权威声明见 CGameManager.h，符号由 CGameManager.cpp 提供）。
#include "CQuickPartyRewardManager.h"  // CQuickPartyRewardManager 完整类型（init 调用）
class CGameManager
{
public:
    QuickParty::CQuickPartyRewardManager* GetQuickPartyRewardManager();
};
CGameManager* G_CGameManager();

#include "CCreatureScriptMgr.h"      // user_creature::GetInstanceCreatureScriptMgr
#include "CreatureScript.h"          // CreatureScript::ExpTable_E（get_creature_level_exp）

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <set>

#include "CSystemTime.h"
#include "CSyncSlangFilter.h"
#include "DBMgr.h"
#include "MySQL.h"

// ===================== 外部依赖声明（其它 TU 提供） =====================

template <class T>
class GlobalInstance_CDM
{
public:
    static T* inst_ptr();
};

extern CSystemTime g_systemTimeInst;
extern int g_levelExpTable[256];

class CMapListImpl;
class CMonsterManagerImpl;

class CNPCDynamicInfo;  // 前置：仅以指针出现在 m_map 值类型

// 权威实现/布局见 CtorProducers_1.cpp（ctor 0x083760f2W / dtor 0x08376106W /
// getNPCInfo / add）。本 TU 只镜像布局（ORIG sizeof 0x18，+0x00 std::map），
// 保证 new/delete 尺寸正确；ctor/dtor 定义唯一归属 CtorProducers_1.cpp。
class CNPCDynamicInfoManager
{
public:
    CNPCDynamicInfoManager();  // ORIG _ZN22CNPCDynamicInfoManagerC2Ev W
    ~CNPCDynamicInfoManager();

    std::map<unsigned int, CNPCDynamicInfo*> m_map;   // +0x00
};

class CChattingEmoticon
{
public:
    virtual ~CChattingEmoticon();
    virtual void f4();
    virtual bool isUsable(const CUser* user);
};

extern int* Get_instance_stHellParty_t();
extern int* Get_instance_groupOrder_map_pool();

extern int importSpPerLevelReferenceTable(const char* path, void* table);
// importPvPReferenceTable 在本 TU 下方实现（见 set_pvp_grade 区），无需 extern。
// PVP_NORMAL_BATTLE_TIME / PVP_DM_BATTLE_TIME 定义于 InitScriptProducers_2.cpp
// （ORIG 同源首个具名对象，决定 _GLOBAL__I_ 键名）；PvP_Room.cpp 仅 extern 引用。
extern int PVP_NORMAL_BATTLE_TIME[2];
// PVP_NORMAL_BATTLE_TIME / PVP_DM_BATTLE_TIME 定义于 PvP_Room.cpp。
extern int PVP_NORMAL_BATTLE_TIME[2];
extern int PVP_DM_BATTLE_TIME[2];

#include "CDataManager/CAutoMarketConditionsControl.inc"


// PVP_RELAY_BATTLE_TIME 无既有定义，由本 TU 提供（链接保证）。
int PVP_RELAY_BATTLE_TIME[2] = {0, 0};

// ===================== 本 TU 局部依赖类/全局定义（后续批次替换） =====================

// 全局数据（ORIG 属于其它 TU；当前先在此提供可链接定义）
CSystemTime g_systemTimeInst;  // TODO(后续批次)
int g_levelExpTable[256] = {0};  // TODO(后续批次)

#include "CDataManager/dtor_CAutoMarketConditionsControl.inc"


#include "CDataManager/LoadScript.inc"



int* Get_instance_stHellParty_t()
{
    // ORIG 0x089097c4：函数局部 static stHellParty_t hellPartyScript_，
    // __cxa_guard 构造 + __cxa_atexit(D1)（P20 单例形态）。
    // 完整类型见 DataManagerInitDepends.h（stHellParty_t），ctor/dtor
    // 由 InitScriptProducers_7.cpp 提供；本函数保持 int* 返回与既有
    // 各 TU 的 extern int* 声明兼容（ORIG 实为 stHellParty_t*）。
    static stHellParty_t hellPartyScript_;
    return reinterpret_cast<int*>(&hellPartyScript_);
}

#include "CDataManager/getDropRate.inc"


#include "CDataManager/getBonusRateEventServer.inc"



#include "CDataManager/find_item.inc"


#include "CDataManager/find_item_1.inc"


#include "CDataManager/FindGoods.inc"


#include "CDataManager/find_skill.inc"


#include "CDataManager/find_quest.inc"


#include "CDataManager/find_dungeon.inc"


#include "CDataManager/find_map.inc"


#include "CDataManager/find_monster.inc"


#include "CDataManager/find_mission.inc"


#include "CDataManager/find_npc.inc"


#include "CDataManager/find_lottery_use_cost.inc"


// ===================== 等级/经验 =====================

#include "CDataManager/set_level_exp.inc"


#include "CDataManager/get_level_exp.inc"


#include "CDataManager/get_level_section_exp.inc"


#include "CDataManager/get_level_exp_percent.inc"


#include "CDataManager/get_exp_level.inc"


#include "CDataManager/get_creature_level_exp.inc"


#include "CDataManager/get_creature_exp_level.inc"


#include "CDataManager/GetSpAtLevelUp.inc"


#include "CDataManager/GetMoneyLimitPerLevel.inc"


#include "CDataManager/get_mob_reward.inc"


#include "CDataManager/set_mob_reward.inc"


#include "CDataManager/BaseExpPenalty.inc"


// ORIG 0x08360716：float 比较用 fucompp+fnstsw+sahf（i486 形态）、循环体
// fildll/fmuls/fistpll（64 位零扩展中间量）——以 target attribute 与强转链复刻。
// 尾部 `jmp; nop` 空块中继岛（0x083607d4）按 P223 用「if(相等) goto end; …
// end:;」形复现。
__attribute__((target("arch=i486"))) void CDataManager::SetExpRate(float rate)
{
    if (m_expRate == rate)
        goto end;
    memcpy(m_mobReward, m_mobRewardBackup, 0x324);
    m_expRate = rate;
    for (int i = 0; i <= 200; ++i)
    {
        m_mobReward[i] =
            (int)(long long)((float)(unsigned long long)(unsigned int)m_mobReward[i] *
                             rate);
    }
end:
    ;
}

// ORIG 0x083607d8：ratio 先行物化到局部（fmuls 直乘内存槽），两段循环分别
// 遍历 m_genRefs1(+0x468)/m_genRefs0(+0x428)，元素 +0x10=m_dropRate 按
// ROUND(v*ratio) 截断缩放；float 比较为 i486 形态。尾部空块中继岛按 P223
// goto 尾空标签形复现；x87 分步 flds+fdivp vs 4.4 fdivs 折叠属 N85附②。
// [b28d-solo-CDataManager DIFF-N] 本轮复核（o 96 / n 95）：唯一实质残差 =
// ratio 除法 ORIG `flds 0xb3f0; flds rate; fdivp` vs 4.4 `flds 0xb3f0;
// fdivs rate`（N85附② 跨版本中端差）。六形探针 /tmp/p_x87div.cpp s1..s6
// 全败：s1 直除/s2 具名 cur/s4 两语句/s5 double 提升/s6 +0.0f 恒 `fdivs`；
// s3 volatile 分母产 `flds+fdivrp`（助记符与语义方向均异且多槽）。
// 4.4 对 `/` 恒折内存操作数除法，无源码形可产 ORIG 的 `fdivp`；后续地址列
// 因 2 字节长度差整体错位致 ndiff 虚高（difflib 伪差）。语义等价封顶。
__attribute__((target("arch=i486"))) void CDataManager::SetItemDropRate(float rate)
{
    if (m_dropRate == rate)
        return;
    float ratio = m_dropRate / rate;
    m_dropRate = rate;
    m_itemGeneratorMgr->setDropRatio(rate);
    for (int i = 0; i < (int)m_serverParameter.m_genRefs1.size(); ++i)
    {
        sItemGenRef& ref = m_serverParameter.m_genRefs1[i];
        ref.m_dropRate = (int)((float)ref.m_dropRate * ratio);
    }
    for (int i = 0; i < (int)m_serverParameter.m_genRefs0.size(); ++i)
    {
        sItemGenRef& ref = m_serverParameter.m_genRefs0[i];
        ref.m_dropRate = (int)((float)ref.m_dropRate * ratio);
    }
}

// ===================== PvP/排行 =====================

#include "CDataManager/getRankIndex.inc"



#include "CDataManager/getNewRankIndex.inc"



#include "CDataManager/getRankBonusIndex.inc"



#include "CDataManager/calRankPoint.inc"


// ===================== 任务 =====================

#include "CDataManager/getDailyTrainingQuest.inc"


#include "CDataManager/isThereDailyTrainingQuestList.inc"


#include "CDataManager/get_WithinMissionIndex.inc"


#include "CDataManager/get_kind_mission_list.inc"


#include "CDataManager/get_New_DailyMission.inc"


#include "CDataManager/get_BaseMissionExp_byRank.inc"


#include "CDataManager/get_daily_mission_kind.inc"


// ===================== 事件/道具 =====================

#include "CDataManager/getDecoEventItemIdx.inc"


#include "CDataManager/getVaildEventItem.inc"


#include "CDataManager/getMatchEventPoint.inc"


#include "CDataManager/getMaxEventItemCount.inc"


#include "CDataManager/getFatigue_contidion.inc"


#include "CDataManager/getRewardItem_MailExpireDate.inc"


#include "CDataManager/get_GiftGrowthWeaponBox.inc"


#include "CDataManager/get_GrowthWeaponMaterialItem.inc"


#include "CDataManager/get_GrowthWeaponMaterialMax.inc"


#include "CDataManager/get_GrowthWeaponEventBubbleMax.inc"


#include "CDataManager/get_GrowthWeaponEventSandMax.inc"


#include "CDataManager/get_GrowthWeaponEventReward.inc"


#include "CDataManager/getChangeAbleCreatureLev.inc"


#include "CDataManager/getClientChangeCreatureIndex.inc"


#include "CDataManager/getInfinityChangeCreatureIndex.inc"


#include "CDataManager/getGiveGrowCreatureEventJobCount.inc"


#include "CDataManager/isAvailableGiveGrowCreatureEventJob.inc"


#include "CDataManager/getSeriaRoomAniDecoItem.inc"


#include "CDataManager/getSeriaRoomAniDecoRewardItem.inc"


#include "CDataManager/maxAniDecoItem.inc"


#include "CDataManager/invalidAniDecoItem.inc"


#include "CDataManager/getChoiceItem.inc"


#include "CDataManager/setChoiceItem.inc"


#include "CDataManager/checkChoiceItem.inc"


#include "CDataManager/set_lottery_use_cost.inc"


#include "CDataManager/set_original_dimensionInout.inc"


#include "CDataManager/reset_dimensionInout.inc"


// ===================== 情感/过滤 =====================

#include "CDataManager/isCommonEmoticon.inc"


#include "CDataManager/AddPremiumIndex.inc"


#include "CDataManager/find_emoticon.inc"


#include "CDataManager/ReCalc.inc"


#include "CDataManager/ReCalcEmoticonConf.inc"


#include "CDataManager/IsUsableEmoticons.inc"


#include "CDataManager/GetUpgradeRevisionPvPChannel.inc"


#include "CDataManager/getUpgradeItemRepairCostRate.inc"


#include "CDataManager/checkValidShopItem.inc"


#include "CDataManager/checkLimitNpcBuyItemList.inc"


#include "CDataManager/hasPreventString.inc"


#include "CDataManager/hasPreventStringName.inc"


#include "CDataManager/getCharacNameFilterPass.inc"


#include "CDataManager/setCharacNameFilterPass.inc"


// ===================== 其它 =====================

#include "CDataManager/GetScriptPacks.inc"


#include "CDataManager/SetScriptPacks.inc"


#include "CDataManager/GetItemMakingSkill.inc"


#include "CDataManager/GetImageCommnicationData.inc"



#include "CDataManager/getUltimateDungeonList.inc"

#include "CDataManager/getVendingMachineTable.inc"


// 注：ORIG 仅有非 const getBlueMarbleScript()（0x08365eea，nm 无 _ZNK 重载）；
// 原 TU 内发明的 const 重载已删除（调用点走非 const 版本）。


#include "CDataManager/get_hellparty_script_values.inc"


#include "CDataManager/testActionType.inc"


#include "CDataManager/testBingo.inc"


#include "CDataManager/IsDBLogItem.inc"


// ===================== 构造/析构 =====================

#include "CDataManager/CDataManager.inc"


#include "CDataManager/dtor_CDataManager.inc"


#include "CDataManager/destroy.inc"


// ===================== 单例 =====================

template <class T>
T* GlobalInstance_CDM<T>::inst_ptr()
{
    static T inst;  // TODO(后续批次)
    return &inst;
}

CDataManager* G_CDataManager()
{
    // ORIG _Z14G_CDataManagerv：GlobalInstance<CDataManager>::inst_ptr()，
    // 非本地 GlobalInstance_CDM（callee 名不同）。
    return GlobalInstance<CDataManager>::inst_ptr();
}

#include "CDataManager/dtor_CChattingEmoticonList.inc"

#include "CDataManager/get_limit_inout_count.inc"


// ============================================================================
// 迁移自 GameStubs.cpp 的 CDataManager 桩方法（ORIG 符号/偏移已核对）
// ============================================================================

#include "CDataManager/set_AuctionRegFreeCeraItem.inc"


#include "CDataManager/GetExpertJobScript.inc"


#include "CDataManager/GetExpertJobEtcScript.inc"


#include "CDataManager/GetChannelScript.inc"


#include "CDataManager/GetTowerScript.inc"


#include "CDataManager/getBlueMarbleScript.inc"


#include "CDataManager/get_event_script_mng.inc"


#include "CDataManager/getItemLimitEditionMgr.inc"


#include "CDataManager/get_dimensionInout.inc"


#include "CDataManager/GetPvPChannelGrade.inc"


#include "CDataManager/GetMaxGradePvPChannel.inc"


#include "CDataManager/GetPvPChannelType.inc"


#include "CDataManager/GetCeraShopGoodsName.inc"


#include "CDataManager/SuddenShopFileName1.inc"


#include "CDataManager/SuddenShopFileName2.inc"


#include "CDataManager/SuddenShopFileName3.inc"


#include "CDataManager/VerifyMap.inc"


#include "CDataManager/get_dungeon.inc"


#include "CDataManager/reselectDailyTrainingQuest.inc"


#include "CDataManager/getCharacterJobToEnumValue.inc"


#include "CDataManager/importPvPReferenceTable.inc"


#include "CDataManager/set_pvp_grade.inc"


#include "CDataManager/GetResetItemInfo.inc"


#include "CDataManager/GetRefillItemInfo.inc"


#include "CDataManager/InitLoginState.inc"


#include "CDataManager/init.inc"


// ===================== 副本次数排行 =====================

#include "CDataManager/getDungeonClearAverageTime.inc"
                                                      ENUM_DUNGEON_DIFFICULTY difficulty)
{
    // ORIG 0x083639d2：result(-0xc) 入口初始化在 key ctor 前；case 0..3 走
    // -0xc 赋值汇聚（0x08363ac2 mov -0xc），end/default 两路直接 mov$0 返回
    // （0x08363a77/0x08363abb）。
    unsigned int result = 0;
    stClearTimeAverageMapKey key;                                // -0x18
    key.m_a = dungeonIdx;
    key.m_b = partyCount;

    // ORIG 仅发射非 const find/end（_ZNSt3map...4findERS5_ / 3endEv，无
    // _ZNK 重载）且 it 无 const_iterator 转换调用 ⇒ 原码经非 const 引用
    // 内联 cast 访问（ORIG 每处 `lea 0x5108(%eax)` 直发，无引用槽物化，
    // 0x083639f9/0x08363a1b），iterator 直接比较。
    typedef std::map<stClearTimeAverageMapKey, stClearTimeAverage> RankMap;
    RankMap::iterator it =
        const_cast<RankMap&>(m_rankSystem.m_dungeonRank.m_map0).find(key);
    // ORIG 0x08363a45 == 调用的 this = end 迭代器（-0x10）、实参 = it
    // （-0x1c）⇒ 原码操作数序为 `end() == it`。
    if (const_cast<RankMap&>(m_rankSystem.m_dungeonRank.m_map0).end() == it)
    {
        return 0;
    }

    // [b28d-lane16] 本地 ENUM_DUNGEON_DIFFICULTY 只声明了 _0=0（值域 0..0），
    // 直接 switch(difficulty) 会被前端裁掉 case 2/3（编译警告「case 标号值比该
    // 类型的最大值还要大」，ORIG 4 次 ptEv 只剩 2 次）；cast 到 int 后值域完整，
    // 4 个 case 逐条发射（mangled 不含 switch 类型，零 ABI 影响）。
    switch ((int)difficulty)
    {
    case 0: result = (unsigned int)it->second.m_v[0]; break;
    case 1: result = (unsigned int)it->second.m_v[1]; break;
    case 2: result = (unsigned int)it->second.m_v[2]; break;
    case 3: result = (unsigned int)it->second.m_v[3]; break;
    default: return 0;
    }
    return result;
}

#include "CDataManager/getClearTimeBonusPoint.inc"
{
    // ORIG 0x08363ac8：result(-0x10)/diff(-0xc) 双双入口零初始化；早退
    // `return result`（0x08363ae8 mov -0x10 而非立即 0）；cur(-0x20)/
    // prev(-0x28) 两个带 ctor 的结构本地（0x08363af0/af6）；vector 走非
    // const begin/end/==/operator->/postfix ++（无 const 实例⇒const_cast
    // 内联形，0x08363b06 二段装载 N18a）；命中 `cur.m_a > diff` 时
    // it==begin() → result=0 走 -0x10 物化（0x08363b9f..0xa6）。
    int result = 0;
    int diff = 0;
    if (avgTime == 0 || clearTime == 0)
        return result;

    typedef std::vector<stBonusPointValues> Vec;
    // [b28d-solo-CDataManager] ORIG 0x08363af0：先构造 cur(-0x20) 再 prev(-0x28)
    // ⇒ 声明序 cur → prev；diff 赋值在两次 ctor 之后（ORIG 0x08363b06）。
    stBonusPointValues cur;
    stBonusPointValues prev;
    diff = avgTime - clearTime;
    // 指针形 const_cast：4.4 对引用 cast 结果会物化临时槽（把 cur 起整体
    // 平移 4B），指针形直传 this 与 ORIG `lea 0x5120(%eax)` 一致。
    // end 缓存到 e（ORIG 0x08363b30 循环外一次 end → -0x30，循环条件只 ne(it,e)；
    // 旧形每次迭代重调 end() 使 it/end 槽位与 ORIG 相反，P13）。
    for (Vec::iterator it =
             const_cast<Vec*>(&m_rankSystem.m_dungeonRank.m_vec18)->begin(),
         e = const_cast<Vec*>(&m_rankSystem.m_dungeonRank.m_vec18)->end();
         it != e;
         it++)
    {
        cur = *it;
        if (cur.m_a > diff)
        {
            if (it ==
                const_cast<Vec*>(&m_rankSystem.m_dungeonRank.m_vec18)->begin())
            {
                result = 0;
                return result;
            }
            result = prev.m_b;
            return result;
        }
        prev = cur;
    }
    result = prev.m_b;
    return result;
}

// ---- ORIG W force-use 发射（结构生命周期专项）----
// LevelUpRewardItemTableKey C1 0x0891bac0 / clear 0x0891bad4：
//   默认构造在既有 TU 无使用点（均为 C2Eiii），此处合法 force-use 触发弱符号。
// LevelUpRewardItemData C1 0x081b88bc / clear 0x081b88d0 同段发射（重复弱符号无害）。
namespace {
#include "CDataManager/emitLevelUpRewardWeakSymbols.inc"

}  // namespace
