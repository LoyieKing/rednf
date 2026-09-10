// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDataManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// CAutoMarketConditionsControl：ORIG ctor 0x082f8108 —— m_vec3dc(+0x3dc) 默认
// 构造（非平凡成员先行，0x082f8119），随后函数体逐句置 0，顺序
// +0x380 → +0x00 → +0x3e8（0x082f8121/26/2f，每句重取 this = 体赋值形 P29）。
#include "CDataManager/CAutoMarketConditionsControl.inc"
}

// PVP_RELAY_BATTLE_TIME 无既有定义，由本 TU 提供（链接保证）。
int PVP_RELAY_BATTLE_TIME[2] = {0, 0};

// ===================== 本 TU 局部依赖类/全局定义（后续批次替换） =====================

// 全局数据（ORIG 属于其它 TU；当前先在此提供可链接定义）
CSystemTime g_systemTimeInst;  // TODO(后续批次)
int g_levelExpTable[256] = {0};  // TODO(后续批次)

// ORIG dtor 0x082f8144：仅 vector<int> @+0x3dc 析构（成员自动）。
#include "CDataManager/dtor_CAutoMarketConditionsControl.inc"
}

// ORIG LoadScript 0x082f815c（0x2e2 字节）逐分支转写：返回出错行，0=成功。
// 结构按 ORIG：各键失败即 return（ebx=err 直落尾，无 err 汇聚变量）；
// value(-0x20)/ok(-0x21) 为函数级共享（key2/key4 复用同槽）；键1/3/5 循环
// `cmpl $9; setle` = i<=9 头检 for；key2 上界检查 ORIG 不存在（仅 <=0 →
// 0x3c）；key4 push_back 后存在 ok 二次重载检查（0x082f8386，别名后重读）；
// 未知键直接 return 0x64（0x082f8401），非 continue。
// [b28d-solo-CDataManager AE 208=208] 真语义 bug ×2 修复：① ORIG 0x082f8287 有
// `cmp $0xc8; jle` 上界检查（value>200 → 0x3c），旧码缺；② ORIG 0x082f82ac 写
// 地址 = this+0xc+(value+0x14)*4 = m_goldPerLevel[value]，旧码 `[value+0x17]`
// 多写 0x5c 字节越界。本轮翻正（o 208 / n 209→208，DIFF→AE）：
//   ① 外层 while 旋转 1 条 ⇒ goto 头条件形（P223/P283）；
//   ② `if (v<=0) return 0x3c; if (v>200) return 0x3c;` 两处独立 err 块被 4.4
//      各自内联 ⇒ 合并短路 `if (v<=0 || v>200) return 0x3c;` 复现 ORIG 共享
//      失败块（P258 短路物化方向）。
// p167 19/19 CLEAR。
#include "CDataManager/LoadScript.inc"
}


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

// ===================== 查询 =====================
// ORIG W 0x0830e6d2 _ZNK12CDataManager11getDropRateEv：读 +0xb3f0 float。
#include "CDataManager/getDropRate.inc"
}

// ORIG T 0x088dcee2 _ZNK16channel_script_t23getBonusRateEventServerEhj
// 服务器频道加成版本：命中频道信息即固定 0.02f，否则 0.0f。
#include "CDataManager/getBonusRateEventServer.inc"
}


#include "CDataManager/find_item.inc"
}

#include "CDataManager/find_item_1.inc"
}

#include "CDataManager/FindGoods.inc"
}

#include "CDataManager/find_skill.inc"
}

#include "CDataManager/find_quest.inc"
}

#include "CDataManager/find_dungeon.inc"
}

#include "CDataManager/find_map.inc"
}

#include "CDataManager/find_monster.inc"
}

#include "CDataManager/find_mission.inc"
}

#include "CDataManager/find_npc.inc"
}

#include "CDataManager/find_lottery_use_cost.inc"
}

// ===================== 等级/经验 =====================

#include "CDataManager/set_level_exp.inc"
}

#include "CDataManager/get_level_exp.inc"
}

#include "CDataManager/get_level_section_exp.inc"
}

#include "CDataManager/get_level_exp_percent.inc"
}

#include "CDataManager/get_exp_level.inc"
}

#include "CDataManager/get_creature_level_exp.inc"
}

#include "CDataManager/get_creature_exp_level.inc"
}

// [b28d-solo-CDataManager NEAR-N] 本轮复核：ORIG `mov level,%edx; mov this,
// %eax; add $0x36a2,%edx; mov 0x8(%eax,%edx,4),%eax`。探针 /tmp/p_sp_at{,2,3,4,5}.cpp
// 全形实测：显式 `m_sp8[level+0x36a2]`（视图 +8）4.4 恒产 `mov level,%eax;
// lea 0x36a2(%eax),%edx; mov this,%eax`（lea 合并 + this 后置，q1..q8/r1..r5
// 12 形全败）；改大偏移视图 `m[level]`（视图 +0xda90）可复现装载序
// `mov level,%edx; mov this,%eax; add …,%edx` 但常量被折成 add $0x36a4 + disp 0
// （ORIG 为 add $0x36a2 + disp 8）⇒ 4.4 对 scaled-index 大常量的「全折」约束
// （N94）无源码解。语义等价封顶。
#include "CDataManager/GetSpAtLevelUp.inc"
}

#include "CDataManager/GetMoneyLimitPerLevel.inc"
}

// 残余 DIFF 归因：ORIG else 臂把表值直读进 %edx 后 `mov %edx,(%eax)`，4.4.7
// 先落 %eax 再 spill %edx（N7/N18a 寄存器指派反向），界检查折叠同因。
// [b28d-solo-CDataManager AE] 修复：CDataManager.h 匿名 union 增同址 unsigned
// 视角 m_mobRewardU（P235/P246）复现 ORIG 单段 `mov 0x4(%eax,%edx,4),%edx`。
#include "CDataManager/get_mob_reward.inc"
}

#include "CDataManager/set_mob_reward.inc"
}

#include "CDataManager/BaseExpPenalty.inc"
}

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
}


#include "CDataManager/getNewRankIndex.inc"
}


#include "CDataManager/getRankBonusIndex.inc"
}


#include "CDataManager/calRankPoint.inc"
}

// ===================== 任务 =====================

#include "CDataManager/getDailyTrainingQuest.inc"
}

// [b28d-solo-CDataManager] 返回类型 int→bool（P292/P168a 档①）：ORIG
// 0x0836411e 返回点 `mov $0/1,%eax`，唯一调用点 Quest::check_possible
// 0x08352f3b `call; xor $0x1,%eax; test %al,%al; je` ⇒ callee bool。
#include "CDataManager/isThereDailyTrainingQuestList.inc"
}

#include "CDataManager/get_WithinMissionIndex.inc"
}

#include "CDataManager/get_kind_mission_list.inc"
}

#include "CDataManager/get_New_DailyMission.inc"
}

#include "CDataManager/get_BaseMissionExp_byRank.inc"
}

#include "CDataManager/get_daily_mission_kind.inc"
}

// ===================== 事件/道具 =====================

#include "CDataManager/getDecoEventItemIdx.inc"
}

#include "CDataManager/getVaildEventItem.inc"
}

#include "CDataManager/getMatchEventPoint.inc"
}

#include "CDataManager/getMaxEventItemCount.inc"
}

#include "CDataManager/getFatigue_contidion.inc"
}

#include "CDataManager/getRewardItem_MailExpireDate.inc"
}

#include "CDataManager/get_GiftGrowthWeaponBox.inc"
}

#include "CDataManager/get_GrowthWeaponMaterialItem.inc"
}

#include "CDataManager/get_GrowthWeaponMaterialMax.inc"
}

#include "CDataManager/get_GrowthWeaponEventBubbleMax.inc"
}

#include "CDataManager/get_GrowthWeaponEventSandMax.inc"
}

#include "CDataManager/get_GrowthWeaponEventReward.inc"
}

#include "CDataManager/getChangeAbleCreatureLev.inc"
}

#include "CDataManager/getClientChangeCreatureIndex.inc"
}

#include "CDataManager/getInfinityChangeCreatureIndex.inc"
}

#include "CDataManager/getGiveGrowCreatureEventJobCount.inc"
}

#include "CDataManager/isAvailableGiveGrowCreatureEventJob.inc"
}

#include "CDataManager/getSeriaRoomAniDecoItem.inc"
}

#include "CDataManager/getSeriaRoomAniDecoRewardItem.inc"
}

#include "CDataManager/maxAniDecoItem.inc"
}

#include "CDataManager/invalidAniDecoItem.inc"
}

#include "CDataManager/getChoiceItem.inc"
}

#include "CDataManager/setChoiceItem.inc"
}

#include "CDataManager/checkChoiceItem.inc"
}

#include "CDataManager/set_lottery_use_cost.inc"
}

// P223（b28）：ORIG `jg nop岛; body; jmp leave; nop; leave` 空块中继岛，
// 复现形=「if(越界) goto end; body; end:;」goto 尾空标签。
#include "CDataManager/set_original_dimensionInout.inc"
}

#include "CDataManager/reset_dimensionInout.inc"
}

// ===================== 情感/过滤 =====================

// ---- ChattingEmoticonBase 基类虚函数（ORIG 0x80e5f46/0x80e5dde）----
#include "CDataManager/isCommonEmoticon.inc"
}

// [b28d-solo-CDataManager AE] 修复：原嵌套 if 形被 4.4 折成内存直比（cmpl $imm,0xc(%ebp)），
// ORIG 为形参物化 + 寄存器比较链。改 switch 连续 case 组（0x2898bf/c0→53、
// c1/c2→57）即复现 `mov 0xc(%ebp),%eax; cmp $0x2898bf,%eax; jl fail; …`
// 决策链（P251/P261 switch 判据 + 探针 /tmp/p_sp/h.cpp f6 逐条全等）。
#include "CDataManager/AddPremiumIndex.inc"
}

// ---- CChattingEmoticonList（ORIG find_emoticon 0x80e581c / ReCalc 0x80e5880）----
#include "CDataManager/find_emoticon.inc"
}

// ---- CChattingEmoticonList::ReCalc（报告 ReCalc.md，ORIG T 0x080e5880）：
//      ① 遍历 config 24 行：行 m_nCount 能 find_emoticon 命中且 isCommonEmoticon
//         （vtable+0x14）→ 行拷入 rows 并记 used；② config.clear()；
//      ③ rows 各行回填到 config 中 m_nCount==0 的空行（[GetStartArrayIndex,
//         GetEndArrayIndex] 区间内第一个空位，整行 0x16 字节拷贝）；
//      ④ 遍历 m_map：GetIndex 未在 used 且 isCommonEmoticon → 同样区间内
//         空行写入 m_nCount。 ----
// 残余 DIFF 归因（o=298 n=296，无符号界/setbe 已对）：主体为 N7/N18a/N22
// 族——同址参数装载的 eax↔edx 指派与 spill 序（如 `mov (%eax),%edx` vs
// `mov (%eax),%eax` 后接不同中转），4.4.7 分配器固定反向，穷举表达式形
// 不可翻（范式库 N7/N18a/N22 + setcc 装载组合）。
// [b28d-solo-CDataManager AE 298=298] 本轮翻正（o=298 n=296→298）：
//   ① 循环尾测 ORIG `mov -0x20,%eax; cmp $0x17,%eax; setbe` 两段形 ⇒ 局部改
//      int + 显式 `(unsigned int)i <= 0x17`（P310，unsigned 局部恒折内存直比）；
//   ② 尾块 `mov $0x1,%ebx … mov %ebx,%eax` ⇒ 返回类型 void→int（P159；头
//      CDataManager.h 同步，mangled 不含返回类型，唯一调用点忽略返回值）。
// p167 29/29 CLEAR。
#include "CDataManager/ReCalc.inc"
}

// ReCalcEmoticonConf —— ORIG T 0x0835fc76 _ZN12CDataManager18ReCalcEmoticonConfER23CChattingEmoticonConfigRK5CUser
//   （docs/class_func_reports/CDataManager/ReCalcEmoticonConf.md）：
//   直通 m_emoticonList(+0x28) 的 CChattingEmoticonList::ReCalc（call 前置：
//   user 压 0x8(%esp)、config 压 0x4(%esp)、m_emoticonList 压 (%esp)）。
// ---------------------------------------------------------------------------
#include "CDataManager/ReCalcEmoticonConf.inc"
}

// ---------------------------------------------------------------------------
// IsUsableEmoticons —— ORIG T 0x0835fc9a _ZN12CDataManager17IsUsableEmoticonsERK5CUserRKSt6vectorIiSaIiEE
// IsUsableEmoticons —— ORIG T 0x0835fc9a _ZN12CDataManager17IsUsableEmoticonsERK5CUserRKSt6vectorIiSaIiEE
//   （非 const；sret 返回 vector<int>；报告缺失，按 nm 定址 + objdump 转写）。
//   反汇编逐条转写：
//     result 默认构造；m_emoticonList(+0x28) 为空直接返回空表；
//     遍历 emoticons：find_emoticon(emoticons[i]) 未命中跳过；
//     命中且 !isUsable(user)（vtable+0x08 虚调用，bool 取反 P22）
//     → result.push_back(emoticons[i])。
//   N7 封顶：虚调用侧 4.4 vptr→%eax/obj→%edx（ORIG vptr→%edx/obj→%eax，
//   call *%edx↔*%eax 互换）；fn_diff_tu 归一残差仅此 5 行，跨版本寄存器分配
//   差无源码解（负例 N7 族），语义等价封顶。
// ---------------------------------------------------------------------------
#include "CDataManager/IsUsableEmoticons.inc"
}

#include "CDataManager/GetUpgradeRevisionPvPChannel.inc"
}

#include "CDataManager/getUpgradeItemRepairCostRate.inc"
}

#include "CDataManager/checkValidShopItem.inc"
}

#include "CDataManager/checkLimitNpcBuyItemList.inc"
}

#include "CDataManager/hasPreventString.inc"
}

#include "CDataManager/hasPreventStringName.inc"
}

#include "CDataManager/getCharacNameFilterPass.inc"
}

#include "CDataManager/setCharacNameFilterPass.inc"
}

// ===================== 其它 =====================

#include "CDataManager/GetScriptPacks.inc"
}

#include "CDataManager/SetScriptPacks.inc"
}

#include "CDataManager/GetItemMakingSkill.inc"
}

#include "CDataManager/GetImageCommnicationData.inc"
}


#include "CDataManager/getUltimateDungeonList.inc"
}
// ORIG W 0x0822b64c：return this+0xb424（&m_vendingMachine）。
#include "CDataManager/getVendingMachineTable.inc"
}

// 注：ORIG 仅有非 const getBlueMarbleScript()（0x08365eea，nm 无 _ZNK 重载）；
// 原 TU 内发明的 const 重载已删除（调用点走非 const 版本）。


// [b28d-E03 2026-09-10] 返回类型改 stHellParty_t*（见头注释）：函数体仍为单条 mov，
// 仅头/定义签名同步；消费点 static_cast<stHellParty_t*>(…) 为恒等转换、codegen 不变。
#include "CDataManager/get_hellparty_script_values.inc"
}

// ORIG 0x08365fc0：sub $0x90120,%esp 后直接 leave/ret —— 保留等价栈帧。
#include "CDataManager/testActionType.inc"
}

// ORIG 0x08365ef8：sub $0x20,%esp 后直接 leave/ret —— 保留等价栈帧。
#include "CDataManager/testBingo.inc"
}

#include "CDataManager/IsDBLogItem.inc"
}

// ===================== 构造/析构 =====================

#include "CDataManager/CDataManager.inc"
}

#include "CDataManager/dtor_CDataManager.inc"
}

#include "CDataManager/destroy.inc"
}

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

// ---- CDataManager 成员类析构实现（G2/G5 批次细化）----
// CMapList/CMonsterManager/CDungeonList/CSkillList/QuestList 实现在
// CListTables.cpp（G2 第七批）；CChattingEmoticonList 由各自 TU 提供。
#include "CDataManager/dtor_CChattingEmoticonList.inc"

#include "CDataManager/get_limit_inout_count.inc"
}

// ============================================================================
// 迁移自 GameStubs.cpp 的 CDataManager 桩方法（ORIG 符号/偏移已核对）
// ============================================================================

#include "CDataManager/set_AuctionRegFreeCeraItem.inc"
}

#include "CDataManager/GetExpertJobScript.inc"
}

#include "CDataManager/GetExpertJobEtcScript.inc"
}

#include "CDataManager/GetChannelScript.inc"
}

// [b28d-solo-CDataManager AE] 修复：trace 改 `cMyTrace(__FUNCTION__,0xd44,5)(...)`
// 全表达式临时链（P221）翻正 it/end/trace 槽向，并把 ctor 串改回 ORIG 的
// __FUNCTION__ 符号、fmt 改回 ORIG 原文 "not finnd tower : %u"（0x8b6f4c0）。
#include "CDataManager/GetTowerScript.inc"
}

#include "CDataManager/getBlueMarbleScript.inc"
}

#include "CDataManager/get_event_script_mng.inc"
}

#include "CDataManager/getItemLimitEditionMgr.inc"
}

#include "CDataManager/get_dimensionInout.inc"
}

#include "CDataManager/GetPvPChannelGrade.inc"
}

#include "CDataManager/GetMaxGradePvPChannel.inc"

}

#include "CDataManager/GetPvPChannelType.inc"
}

#include "CDataManager/GetCeraShopGoodsName.inc"
}

#include "CDataManager/SuddenShopFileName1.inc"
}

#include "CDataManager/SuddenShopFileName2.inc"
}

#include "CDataManager/SuddenShopFileName3.inc"
}

// 结构终态（o=704 n=689）：七段语义/控制流全对齐——monster 短路空体、ai
// empty()==false 岛、dungeonList 条件序、item×dungeon 三层 at() 汇聚岛、
// hash 扫描 for+break（found 检查体首 0x08362481）、无条件 dungeonList 复扫
// (0x0836283c)、missing 判定后置、entrance 读取弃。残余 ndiff 归因：
//  跨版本 this-cache 物化——4.1.2 对局部容器成员调用先 `lea obj; mov %eax,-slot`
//  （-0x58=&monsterList、-0x54=aiList 等，0x08362125 实证），4.4.7 直发 lea 不
//  物化，全帧槽号系统性平移（P112 族）；条目基址/负值检查行随之整块错位。
// [b28d-solo-CDataManager DIFF-N] 历轮翻正：① monsterList 具名指针
// `pMonsterList`（ORIG 0x08362125 `lea -0xd0; mov %eax,-0x58`，P0 槽往返）；
// ② 空体短路链（ORIG 0x08362153 两处条件跳转）。
// [SweepCDataManager 2026-09-09 本轮] gap 12→5（o 704 / n 699，.o 口径），
// 帧尺寸 0x130 与 ORIG 一致，p167 94/94 CLEAR。翻正项：
//  ① **真 bug**：hash 扫描条件极性反——ORIG 0x0836251c `call empty;
//     test %al,%al; je 0836292a`（非空跳第二段、**空**落扫描体，Ghidra
//     `if (cVar1=='\0') { LAB_0836292a: }` 同证）；旧 `empty()==false` 语义反。
//  ② ai 循环删具名 apcType、实参内联（ORIG 0x08362227 `mov (%eax),%eax;
//     mov %eax,%ebx` 跨 G_CDataManager 保活，P52）+ get() 结果具名判空
//     （ORIG -0x40 槽 + `cmpl $0x0,槽; jne`，P197④）。
//  ③ usable 判定改正极性短路 `m_objectIdx != -1 && !empty()`（ORIG
//     0x0836240b/0x0836241d 两 je 同落 FALSE 块 + `mov $1/$0` 物化，P214①）。
//  ④ maze 两条 find 改 `||` 短路（ORIG 0x08362787 `jne 真块`/0x0836281a
//     `je 假块`/0x0836281c `mov $1/jmp`+`mov $0`，P214①）。
//  ⑤ mazeList 尾测 unsigned（ORIG 0x083628a1 `cmp -0x24,%eax; jbe`，P196/P218）。
//  ⑥ itemList 具名指针 `pItemList`（ORIG 0x0836231f `mov %eax,-0x50`）。
//  ⑦ aiList/内层 dungeonList/第二段 dungeonList 三处 end() 提到循环初始化处
//     缓存（ORIG 0x08362206 / 0x08362384 / 0x0836297f，P13）。
// 残余 5 条为跨版本 -O0 DCE/布局差，不可翻：
//  a) 0x08362401 `cmp %edx,%eax`（size()==m_objectIdx 死比较）与
//     0x0836248c `cmp $0xffffffff,%eax`（at().m_assignItemIdx==-1 死比较）：
//     4.1 -O0 保留纯表达式语句的比较，4.4 DCE（探针 /tmp/probe_deadcmp.cpp
//     h1/h3 加 __builtin_expect 反多 sete/movzbl/test 三条）。
//  b) 0x0836282c 处 `jmp` 短程 + 1 条 nop 填充（跳转距离布局）。
//  c) 0x08362a61 `cmp $0x64,%eax` 死比较 vs 我方 `mov %eax,-0xc`（同 N）。
//  d) [SweepCDataManager-2 复核补] 0x083629d9 `call empty; test %al,%al`
//     （m_rivalData2.empty() 返回值归一化）我方缺 test 1 条；探针
//     /tmp/probe_dce2.o f1/f2/f4 复证 4.4 -O0 恒删无消费者比较/test
//     （f7 具名 bool / f8 volatile 反多 store 与 movzbl）。
// ⇒ 语义完成、byte 层 N 封顶。
#include "CDataManager/VerifyMap.inc"
}

#include "CDataManager/get_dungeon.inc"
}

// 残余 DIFF 归因（o=306 n=311；CStringMaker for_each 段/const 迭代器形/
// cur-prev 单游标语义已按 ORIG 恢复）：① 70 元 multiset 数组 ORIG 4.1 发
// 显式构造循环 `mov $0x45; 倒计数 call multiset C1`（0x08363d03..0x08363d2a），
// 4.4.7 对 `multiset applyLevels[0x46]` 的构造循环寄存器/步序不可逐项复刻
// （P151 同族形已用，槽 -0x6e4 对齐）；② for_each 段 4.4 引用别名物化槽
// 使 maker 区平移（P112）；③ ORIG EH landing pad（0x08363d2c..0x08363d89
// cleanup 展开）与 4.4 异常表结构差异属跨版本负例。
// [b28d-solo-CDataManager AE 306=306] 本轮翻正（o=306 n=294→306）：
//   ① quest 提升为函数头声明（ORIG 0x08363cec 入口 `movl $0,-0x28`，P212/P309）；
//   ② cur/questIdx 提到 level 循环头（ORIG 0x08363dbe/0x08363dc5 每 level 一次
//      具名初始化）；
//   ③ 去掉 bucket 引用槽：内联 `static_cast<const multiset<int>&>(applyLevels
//      [level-1]).begin()/end()`，复现 ORIG 每轮重算地址 + 重调 end（P183）；
//   ④ m_type 分支改否定式（ORIG 0x08363e6f `je` 跳 push_back 块，P272）；
//   ⑤ 两处空块 nop 中继岛用否定 goto + 尾标签串（ORIG 0x08363e42/0x08363e64，
//      P223）。p167 31/31 CLEAR。
#include "CDataManager/reselectDailyTrainingQuest.inc"
}

// ============================================================================
// getCharacterJobToEnumValue（ORIG 0x89bbc0f，_Z26getCharacterJobToEnumValueRSs）
// ----------------------------------------------------------------------------
// 被 importPvPReferenceTable（"[pvp record order]" 键）与
// EquipmentParameterInfo.cpp 引用。ORIG 反汇编（objdump -d df_game_r）为 12 次
// std::string==const char* 比较链（_ZSteqIcSt11char_traitsIcESaIcEEbRKSbIT_T0_T1_EPKS3_
// @0x80809a1）；字符串表 .rodata 0x8e198be..0x8e19923 逐字节核对：
//   "swordman"→0 "fighter"→1 "gunner"→2 "mage"→3 "priest"→4 "at gunner"→5
//   "thief"→6 "at fighter"→7 "at mage"→8 "demonic swordman"→9
//   "creator mage"→10 "common"→11，全不中→-1（0xffffffff）
// ============================================================================
#include "CDataManager/getCharacterJobToEnumValue.inc"
}

// ============================================================================
// importPvPReferenceTable —— ORIG 0x0890857b（_Z23importPvPReferenceTablePKcR15stPvpRefTable_t）
// 解析 Etc/pvp_ref.etc 填充 stPvpRefTable_t。返回 0 成功（token 流耗尽 EOF）；
// 非 0 为各键解析失败码，均与 ORIG 反汇编逐操作数一致。
// 反汇编证据（objdump -d df_game_r，0x890857b）：
//   loadRDARScriptFile("", path) 失败 → return 0x1816
//   Quote 循环：ScanType(line,1) 返回 0（EOF）→ return 0
//   "[pvp experience max grade]"：m_count=ScanInt+1；!ok→0x181f；reserve+resize
//   "[pvp experience]"：循环 {grade=ScanInt(!ok则break), exp=ScanInt(!ok→0x182d);
//                            m_grades[grade]={grade,exp}}
//   "[pvp experience loss]"：m_pvpExpLoss1=ScanInt(!ok→0x1835)；
//                             m_pvpExpLoss2=ScanInt(!ok→0x1838)
//   "[pvp rank rate grade]"：m_rankRateGrade=ScanInt(!ok→0x183e)
//   "[pvp rank rate]"：循环 push_back(ScanInt)，!ok break
//   6 个战斗时间键 → PVP_*_BATTLE_TIME[i] 对应地址（0x93f4ee0..0x93f4ef4）
//   "[pvp record order]"：clear；while(ScanStr) m_recordOrder.push_back(getCharacterJobToEnumValue)
//   "[pvp team hp correction]"：ScanFloat 读取但丢弃（fstp st0）
// 未知键忽略（继续 ScanType 循环）。
// ============================================================================
// 残余 DIFF 归因（o=318 n=309；ok 共享旗/落字段序/逗号 while/临时链已按
// ORIG 恢复）：① resize 值参签名 `vector<GradeExp>::resizeEjS1_`(0x0891f268)
// 为 4.1 libstdc++ 值传递形，4.4.7 头仅有 `resizeEjRKS1_` const& 形——跨
// libstdc++ 版本结构性不可翻（范式库「libstdc++ 内部结构差」行）；② 内层
// 逗号 while ORIG 头检测形 vs 4.4 循环旋转（P75/EvtC9 技巧①对该形未试全）；
// ③ 外 guard `xor$1;test;je` 与 `cnt add$1;mov %eax,%edx` 两段式 = N18a/N1；
// ④ 入口三组 `jmp +0x42; nop` 中转岛 = 跨版本空 else nop;jmp 负例。
// [b28d-solo-CDataManager DIFF-N] 本轮复核（o 318 / n 305→317，差 1 条）：
// p167 46/46 CLEAR。翻正项：① 外层 while 与两个内层逗号 while 改 goto 头条件
// 形（P223/P283；ORIG 0x089085bd 条件在循环头、失败路径 `jmp +0x42`），消
// 入口 jmp 旋转并复现 ORIG 的 ScanInt→查 ok→continue 直落序；② m_count 字段
// int→unsigned（P211 异型化，RefPvpGrade.h 已 R2 广播）复现 ORIG
// `add $0x1,%eax; mov %eax,%edx` 两段 store；③ grade 去入口清零、`1+ScanInt`
// 书写序。残差 1 条 = 4.4 为 goto 回边生成 4 个 `nop;jmp` 中继岛 vs ORIG 2 个
// （jump-threading 跨版本差），另 resize 值参 `EjS1_` vs const& `EjRKS1_` 属
// 跨 libstdc++ 结构性差异。语义完成、byte 层 N 封顶。
#include "CDataManager/importPvPReferenceTable.inc"
}

// ============================================================================
// CDataManager::set_pvp_grade —— ORIG 0x08360aa2
// 加载 PvP 等级参考表，填充 GlobalData::s_ref_pvp_grade（RefPvpGrade）。
// 调用 importPvPReferenceTable（本 TU 实现，ORIG 0x890857b）。
// ============================================================================
// 残余 DIFF 归因（o=93 n=90）：ORIG `mov &s_ref_pvp_grade,%eax` 为链接后
// 绝对地址（.o 显示 $0x0 reloc）；循环体 SetPvpGrade 的 grade 指针 ORIG 经
// 单次装载复用（0x08360b0x 区），4.4.7 每次重装载全局（P48 反向），序列
// 等价但行序差；table ctor 槽 -0x50 vs -0x54（4.4 对 4B 对齐选择差异，P112）。
// [b28d-solo-CDataManager AE] 修复：prevExp/lastGrade 声明序按 ORIG 入口清零序
// [b28d-orig-bool] 返回类型 int→bool：init 调用点 ORIG `call set_pvp_grade;
// xor $0x1,%eax; test %al`（bool 值域 0/1 的取反形），我方 int 声明发
// `test %eax,%eax; sete %al`。mangled 不含返回类型，符号不变。
// （-0x14 先），循环体序改为 ++lastGrade → data.curr=prevExp → data.next →
// prevExp → SetPvpGrade（三次独立 op[]，P48）。
// [SweepCDataManager-2] 本函数局部 `stPvpRefTable_t table;` 发射 W 符号
// _ZNSt12_Vector_baseIN15stPvpRefTable_t15stPvPGradeExp_tESaIS1_EED1Ev
// （ORIG 0x083b1e6a，40 条 vs 我方 25 条，gap 15）：ORIG 4.1 布 EH landing
// pad（`_Unwind_Resume` + `_Vector_implD1Ev` 双份，0x083b1ea4..0x083b1ebf），
// 我方 4.4.7 无——根因是 4.4 对类内可见定义的 `_M_deallocate`
// （→`new_allocator::deallocate`→`::operator delete` throw()）推断 nothrow
// ⇒ 不布 cleanup，4.1 不推断。`bits/stl_vector.h` 禁改、allocator 换型会改
// mangled（`SaIS1_E` 编码）⇒ 无源码解，N 封顶；正文 19 条逐条一致。
#include "CDataManager/set_pvp_grade.inc"
}

// ORIG _ZNK12CDataManager16GetResetItemInfoEv @ 0x08693f24 (W)：return this+0x4d04
// = &m_limitItemUsage.m_resetC.m_list（stLimitItemUsageInfoEx@0x4cf8 + stResetItemInfo@+0x0c）。
#include "CDataManager/GetResetItemInfo.inc"
}

// ORIG _ZNK12CDataManager17GetRefillItemInfoEv @ 0x08693f32 (W)：return this+0x4d10
// = &m_limitItemUsage.m_refill18.m_list（stLimitItemUsageInfoEx@0x4cf8 + stRefillItemInfo@+0x18）。
#include "CDataManager/GetRefillItemInfo.inc"
}

// ---------------------------------------------------------------------------
// CDataManager::InitLoginState —— ORIG 0x08360306（0xf9），报告
// docs/class_func_reports/CDataManager/InitLoginState.md 逐条转写：
// 取 DB_HANDLE_6 句柄；operation_type 0/2 → 按 channel 清 login_status；
// operation_type==1 且 env+0x1b0==1 → 不带 channel 清；否则直接成功。
// env+0x1b0 字段未建模，保留裸偏移（推断）。
// ---------------------------------------------------------------------------
// 残余 DIFF 归因（o=67 n=67，指令序列已同构）：hdb(-0xc)/result(-0xd) 与
// 4.4 (hdb -0x10/result -0x9) 的 1B/4B 槽排布方向差（P112：4.1 后声明更深、
// 4.4 后声明更浅），及 DBMgr 地址/字符串 reloc 显示差。
#include "CDataManager/InitLoginState.inc"
}

// ---------------------------------------------------------------------------
// CDataManager::init —— ORIG 0x08358332..0x0835f1d5（0x6ea3 字节，~5430 指令）。
// 报告 docs/class_func_reports/CDataManager/init.md 全量转写（汇编 + 反编译 C
// 逐段核对，含 +0x1124/+0x18f6/+0x2565/+0x2640 等歧义区回查原始 asm）。
//
// 结构：约 160 个脚本装载步骤组成的「成功续链」——每步失败按 ORIG 各自的
// 行为处理：多数 TRACE 后 return 0；个别仅告警继续（ItemInfoMaster、
// GoldLimit、bindsphere、TowerOfDespair、CheckLightServer/Statistic 等，
// 均在对应分支注释标注报告行号）。
//
// 失败路径返回 0；全链成功返回 1。栈局部对象（vector<PVPMAP_DATA>、
// vector<_DailySchedule>、SDCActiveStatusSyncServer、AvatarRoulette）由 C++
// 作用域析构，与 ORIG 异常清理块（+0x6df2..+0x6e96）等价。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManager DIFF-N] 真语义缺口修复 ×3：① importDailyScheduleEtcScript
// 失败 printf("importDDayEventEtcScript Fail(%d)\n")（ORIG 0x08359eaa）；②
// initWorldMapScript 失败 puts("Load WorldMap.lst fail")（ORIG 0x0835b652）；
// ③ 非空判定改 size()!=0（ORIG 0x08359ec9 调 size，旧 empty() 为 EXTRA）。另消除
// ptEv ×5（EquipmentPartSet 段 data/pieces 具名引用，ORIG 0x0835e4f0/0x0835e4fe）。
// Main 已代落 CItemGeneratorMgr::C1 throw()（CItemGeneratorMgr.h + CtorProducers_2C.cpp）
// ⇒ new-expression landing pad 消除，p167 843/843 CLEAR（0 missing/0 extra）。
// [b28d-BigDataMgrInit 大件收敛] 读数 **orig 5430 / ours 5429**（fn_diff_tu
// norm 5429=5429，gap 0；TU 四数 600/1193/3/10/436 与基线全等）。本轮修：
// ① P248 档① bool 化 ×3 调用点（importChannelScript / CItemUpgrade::LoadUpgradeScript
//    / CItemUpgrade_Separate::LoadUpgradeScript）：ORIG `call; xor $1,%eax;
//    test %al,%al; je` = bool 取反形，旧 `&1` 等价改写每处多 2 条（探针
//    /tmp/probe_xor.cpp 六形实证 int callee 无解）；声明+定义同步 bool，callee
//    机器码零变化（返回语句全常量；W6/Sep/CtorProducers_2C 三件 10/10、11/11）。
// ② P310 循环尾测：`for (int i=0;(unsigned int)i<=6;++i)` 复现两段寄存器形。
// ③ 帧对齐：char _pad[16] 撑帧后 sub $0x192c 与 ORIG 全等、ver 槽
//    -0xf1c..-0xf0c 对齐（P232）。
// 残差（封顶）：a) cMyTrace 临时 arena 槽号差 0x10（90 行，N95/P182：4.1 基址
// -0xf08 vs 4.4 -0xef8，帧已等仍不随帧移）；b) 0x0835cecb/0x0835cfcb 出厂单字节
// 改写 `eb 56`(jmp) 应为 `74 56`(je)，失败块不可达——合法 C++ 不可造（N111 类）；
// c) EquipmentPartSet 段 data/pieces 引用槽序互换 1 对（N93，交换声明序实测
// 反劣 5431/5432 + 658 行非槽差，已回滚）。
#include "CDataManager/init.inc"
}

// ===================== 副本次数排行 =====================

// ORIG T 0x083639d2 _ZN12CDataManager26getDungeonClearAverageTimeEii23ENUM_DUNGEON_DIFFICULTY：
//   以 (dungeonIdx, partyCount) 为键查 m_rankSystem.m_dungeonRank.m_map0；
//   未命中返回 0；命中按 difficulty(0..3) 取 stClearTimeAverage::m_v[难度]，
//   其它难度返回 0。
#include "CDataManager/getDungeonClearAverageTime.inc"
}

// ORIG T 0x08363ac8 _ZN12CDataManager22getClearTimeBonusPointEii：
//   avgTime/clearTime 任一为 0 → 0；diff = avgTime - clearTime；
//   遍历 m_dungeonRank.m_vec18（vector<stBonusPointValues>）：
//     若 diff < 当前 lo：列表首项 → 0，否则返回上一项 value；
//     否则记录 (lo, value) 继续；循环结束返回最后一个 value。
// [b28d-solo-CDataManager AE] 修复：cur/prev 声明序（cur 先，ORIG 0x08363af0
// 先构造 -0x20）+ diff 赋值后置到两次 ctor 之后 + end 缓存到 e（P13，ORIG
// 0x08363b30 循环外一次 end）复现 ORIG 槽位与循环形。
#include "CDataManager/getClearTimeBonusPoint.inc"
}

// ---- ORIG W force-use 发射（结构生命周期专项）----
// LevelUpRewardItemTableKey C1 0x0891bac0 / clear 0x0891bad4：
//   默认构造在既有 TU 无使用点（均为 C2Eiii），此处合法 force-use 触发弱符号。
// LevelUpRewardItemData C1 0x081b88bc / clear 0x081b88d0 同段发射（重复弱符号无害）。
namespace {
#include "CDataManager/emitLevelUpRewardWeakSymbols.inc"
}
}  // namespace
