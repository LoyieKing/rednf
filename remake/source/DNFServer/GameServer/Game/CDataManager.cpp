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
CAutoMarketConditionsControl::CAutoMarketConditionsControl()
{
    m_field380 = 0;
    m_field00 = 0;
    m_field3e8 = 0;
}

// PVP_RELAY_BATTLE_TIME 无既有定义，由本 TU 提供（链接保证）。
int PVP_RELAY_BATTLE_TIME[2] = {0, 0};

// ===================== 本 TU 局部依赖类/全局定义（后续批次替换） =====================

// 全局数据（ORIG 属于其它 TU；当前先在此提供可链接定义）
CSystemTime g_systemTimeInst;  // TODO(后续批次)
int g_levelExpTable[256] = {0};  // TODO(后续批次)

// ORIG dtor 0x082f8144：仅 vector<int> @+0x3dc 析构（成员自动）。
CAutoMarketConditionsControl::~CAutoMarketConditionsControl()
{
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
int CAutoMarketConditionsControl::LoadScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x1e;
    }
    std::string token;
    int value = 0;
    bool ok = false;
    // [b28d-solo-CDataManager] ORIG 条件块在循环头（+0x42 直落）、尾部回跳；
    // 4.4 对 while(cond) 恒旋转成入口 jmp 尾条件（+1 条；while(true)+break
    // 形实测 our 210 更差）。改 goto 头条件形（P223/P283 族）复现。
load_cond:
    if (!ScanType(token, true))
        goto load_done;
    {
        if (token == "[control of gold drop amount rate]")
        {
            for (int i = 0; i <= 9; ++i)
            {
                m_goldRate[i] = ScanFloat(3, &ok);
                if (ok != true)
                {
                    return 0x2f;
                }
            }
        }
        else if (token == "[control of gold drop amount rate per level]")
        {
            memset(m_goldPerLevel, 0, 0x324);
            while (true)
            {
                value = ScanInt(&ok);
                if (ok != true)
                {
                    break;
                }
                // [b28d-solo-CDataManager 真语义 bug 修复] ORIG 0x082f8280-8c 有
                // 上界检查（`cmp $0xc8; jle` ⇒ value>200 也返回 0x3c），旧码缺。
                // 短路合并复现 ORIG `jle err; cmp; jle body; err:` 共享失败块
                // （P258 德摩根/短路物化方向；两个独立 if-return 在 4.4 会各自
                // 内联 err 块，our +3）。
                if (value <= 0 || value > 200)
                {
                    return 0x3c;
                }
                // [b28d-solo-CDataManager 真语义 bug 修复] ORIG 0x082f82ac 写地址
                // = this+0xc+(value+0x14)*4 = this+0x5c+value*4 = m_goldPerLevel
                // [value]（成员 +0x5c 实证）；旧码 `[value+0x17]` 多写 0x5c 字节
                // （23 个 int），越界破坏后续成员。
                m_goldPerLevel[value] = ScanInt(&ok);
                if (ok != true)
                {
                    return 0x40;
                }
            }
        }
        else if (token == "[control of equipment drop prob]")
        {
            for (int i = 0; i <= 9; ++i)
            {
                m_equipProb[i] = ScanFloat(3, &ok);
                if (ok != true)
                {
                    return 0x49;
                }
            }
        }
        else if (token ==
                 "[control of equipment drop prob per level range]")
        {
            // ORIG 0x082f81e9 `movl $0,-0x10`：key4 块首存在一个仅初始化
            // 的本地（后续未使用，P31 槽对齐实证保留）。
            int cnt = 0;
            (void)cnt;
            while (true)
            {
                value = ScanInt(&ok);
                if (ok != true)
                {
                    break;
                }
                m_vec3dc.push_back(value);
                // ORIG 0x082f8386：push_back（不透明调用）后 ok 重读判失败
                // → 0x57（别名性二次检查，忠实保留）。
                if (ok != true)
                {
                    return 0x57;
                }
            }
        }
        else if (token == "[control of durability decrease rate]")
        {
            for (int i = 0; i <= 9; ++i)
            {
                m_durRate[i] = ScanFloat(3, &ok);
                if (ok != true)
                {
                    return 0x60;
                }
            }
        }
        else
        {
            return 0x64;
        }
    }
    goto load_cond;
load_done:
    return 0;
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
float CDataManager::getDropRate() const
{
    return m_dropRate;
}

// ORIG T 0x088dcee2 _ZNK16channel_script_t23getBonusRateEventServerEhj
// 服务器频道加成版本：命中频道信息即固定 0.02f，否则 0.0f。
float channel_script_t::getBonusRateEventServer(unsigned char channel, unsigned int idx) const
{
    // ORIG 0x088dcee2：结果指针落槽(-0xc)后 `cmpl $0; jne 真值支`，双支
    // bits 写回统一槽 flds 返回 = 具名指针三元形（!=0 直判产 setne 岛异形）。
    const void* info = getChannelInfo(channel, idx);
    return info == 0 ? 0.0f : 0.02f;
}


CItem* CDataManager::find_item(int idx) const
{
    if (idx == 0 || idx == 1 || idx == 2)
    {
        return 0;
    }
    return m_itemList->find_item(idx);
}

CItem* CDataManager::find_item(const char* name) const
{
    return m_itemList->find_item(name);
}

CCeraShopGoods* CDataManager::FindGoods(int goodsIdx) const
{
    return m_itemList->FindGoods(goodsIdx);
}

CSkill* CDataManager::find_skill(int skillId, int level) const
{
    return m_skillList->find_skill(skillId, level);
}

Quest* CDataManager::find_quest(int idx) const
{
    return m_questList->find_quest(idx);
}

CDungeon* CDataManager::find_dungeon(int idx) const
{
    return m_dungeonList->find_dungeon(idx);
}

CMap* CDataManager::find_map(int idx) const   // [b28d-Main 代落·S374 P211档③] void*→CMap*（声明端 CDataManager.h:3499 已改）
{
    return m_mapList->find_map(idx);
}

void* CDataManager::find_monster(int idx) const
{
    return m_monsterManager->find_monster(idx);
}

void* CDataManager::find_mission(int idx) const
{
    return (void*)m_pvpMissionSystem->find_mission(idx);
}

int CDataManager::find_npc(unsigned int idx) const
{
    return (int)(unsigned long)m_npcScriptList->find(idx);
}

int CDataManager::find_lottery_use_cost(unsigned int money) const   // ORIG 0x835fb5a：bool→int（%eax 透传）
{
    return m_lotteryNeedMoney.isNeedMoney(money);
}

// ===================== 等级/经验 =====================

bool CDataManager::set_level_exp()
{
    for (int i = 0; i <= 199; ++i)
    {
        m_expTable[i] = g_levelExpTable[i];
    }
    return 1;
}

int CDataManager::get_level_exp(int level) const
{
    if (level < 2 || level > 200)
    {
        return 0;
    }
    return m_expTable[level - 2];
}

int CDataManager::get_level_section_exp(int level) const
{
    if (level < 2 || level > 200)
    {
        return 0;
    }
    return m_expTable[level - 1] - m_expTable[level - 2];
}

int CDataManager::get_level_exp_percent(int level, int percent) const
{
    int section = get_level_section_exp(level);
    return section * percent / 100;
}

int CDataManager::get_exp_level(int exp) const
{
    int level = 1;
    for (; level <= 99; level += 1)
    {
        if (get_level_exp(level) > exp)
        {
            break;
        }
    }
    return level - 1;
}

int CDataManager::get_creature_level_exp(int level) const
{
    if (level < 2 || level > 99)
    {
        return 0;
    }
    // ORIG 消费 CreatureScript::ExpTable_E（0x94fefa0，initCreatureScript 填充）。
    return CreatureScript::ExpTable_[level - 2];
}

int CDataManager::get_creature_exp_level(int exp) const
{
    int level = 1;
    for (; level <= 98; level += 1)
    {
        if (get_creature_level_exp(level) > exp)
        {
            break;
        }
    }
    return level - 1;
}

// [b28d-solo-CDataManager NEAR-N] 本轮复核：ORIG `mov level,%edx; mov this,
// %eax; add $0x36a2,%edx; mov 0x8(%eax,%edx,4),%eax`。探针 /tmp/p_sp_at{,2,3,4,5}.cpp
// 全形实测：显式 `m_sp8[level+0x36a2]`（视图 +8）4.4 恒产 `mov level,%eax;
// lea 0x36a2(%eax),%edx; mov this,%eax`（lea 合并 + this 后置，q1..q8/r1..r5
// 12 形全败）；改大偏移视图 `m[level]`（视图 +0xda90）可复现装载序
// `mov level,%edx; mov this,%eax; add …,%edx` 但常量被折成 add $0x36a4 + disp 0
// （ORIG 为 add $0x36a2 + disp 8）⇒ 4.4 对 scaled-index 大常量的「全折」约束
// （N94）无源码解。语义等价封顶。
unsigned int CDataManager::GetSpAtLevelUp(int level) const
{
    if (level < 1 || level > 95)
    {
        return 0;
    }
    // ORIG 0x8360cb8：SP-at-level-up 表基址 +0xda90；与 set_reward_sp 共用
    // 同一具名 ABI 视图。ORIG 0x8360cda `mov 0x8(%eax,%edx,4)`，edx=level+0x36a2
    // ⇒ 名义基址 this+8、两步下标 0x36a2+level（P205 拆形族）。
    // 残差 1 条：4.4 恒先载 this 地址再求下标（N76 load 侧地址先物化，
    // register 钉 edx/交换加数均无效），ORIG 为 edx=level+0x36a2 先行。
    return ((const CDataManagerRewardSpView*)this)->m_sp8[level + 0x36a2];
}

unsigned int CDataManager::GetMoneyLimitPerLevel(int level, const char* serverGroup) const
{
    if (level <= 0 || level > 200)
    {
        LogManager::logFormat(1, "data_manager.cpp",
                              "CDataManager::GetMoneyLimitPerLevel", 0x2705,
                              "Out of range level (%d)", level);
        level = std::max(1, std::min(200, level));
    }
    if (serverGroup && strcmp(serverGroup, "0") == 0)
    {
        return 0x7fffffff;
    }
    return m_moneyLimit[level];
}

// 残余 DIFF 归因：ORIG else 臂把表值直读进 %edx 后 `mov %edx,(%eax)`，4.4.7
// 先落 %eax 再 spill %edx（N7/N18a 寄存器指派反向），界检查折叠同因。
// [b28d-solo-CDataManager AE] 修复：CDataManager.h 匿名 union 增同址 unsigned
// 视角 m_mobRewardU（P235/P246）复现 ORIG 单段 `mov 0x4(%eax,%edx,4),%edx`。
void CDataManager::get_mob_reward(int level, unsigned int& out) const
{
    if (level <= 0 || level > 199)
    {
        out = 0;
        return;
    }
    // [b28d-solo-CDataManager] ORIG `mov 0x4(%eax,%edx,4),%edx` 单段直落 edx
    // （unsigned 目标与源同型）；源 int 数组直读会多 `mov %eax,%edx` 中转。
    // 修法 = CDataManager.h 的匿名 union 同址 unsigned 视角 m_mobRewardU
    // （P235/P246，sizeof/偏移/mangled 零影响；探针 /tmp/p_sp/f.cpp 逐条全等）。
    out = m_mobRewardU[level];
}

bool CDataManager::set_mob_reward()
{
    short level;
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect level, exp from monster_reward_ref");
    ok = db->exec(true);
    if (ok != 1)
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows < 1)
    {
        return 0;
    }
    int i = 0;
    while (i < rows)
    {
        ok = db->fetch();
        if (ok != 1)
        {
            break;
        }
        ok = db->get_short(0, level);
        if (ok != 1)
        {
            return 0;
        }
        ok = db->get_short(1, m_mobReward[level]);
        if (ok != 1)
        {
            return 0;
        }
        i = i + 1;
    }
    m_mobReward[0] = 0;
    memcpy(m_mobRewardBackup, m_mobReward, 0x324);
    return 1;
}

float CDataManager::BaseExpPenalty(int levelA, int levelB)
{
    int diff = levelB - levelA;
    float result;
    if (diff < -6)
    {
        result = 0.05f;
    }
    else if (diff < -5)
    {
        result = 0.2f;
    }
    else if (diff == -5)
    {
        result = 0.5f;
    }
    else if (diff == -4)
    {
        result = 0.75f;
    }
    else if (diff == -3)
    {
        result = 1.0f;
    }
    else if (diff == -2)
    {
        result = 1.0f;
    }
    else if (diff == -1)
    {
        result = 1.0f;
    }
    else if (diff == 0)
    {
        result = 1.0f;
    }
    else if (diff == 1)
    {
        result = 1.12f;
    }
    else if (diff == 2)
    {
        result = 1.12f;
    }
    else if (diff == 3)
    {
        result = 1.12f;
    }
    else if (diff == 4)
    {
        result = 1.0f;
    }
    else if (diff == 5)
    {
        result = 1.0f;
    }
    else if (diff == 6)
    {
        result = 0.75f;
    }
    else if (diff == 7)
    {
        result = 0.7f;
    }
    else if (diff == 8)
    {
        result = 0.6f;
    }
    else if (diff == 9)
    {
        result = 0.5f;
    }
    else if (9 < diff)
    {
        result = 0.05f;
    }
    return result;
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

unsigned short CDataManager::getRankIndex(int idx)
{
    if (m_rankSystem.m_dungeonRank.m_rankLevel[0] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[0];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[1] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[1];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[2] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[2];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[3] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[3];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[4] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[4];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[5] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[5];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[6] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[6];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[7] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[7];
    return 0;
}


unsigned int CDataManager::getNewRankIndex(int idx) const
{
    if (m_rankSystem.m_dungeonRank.m_rankLevel[0] <= idx) return 0;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[1] <= idx) return 1;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[2] <= idx) return 2;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[3] <= idx) return 3;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[4] <= idx) return 4;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[5] <= idx) return 5;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[6] <= idx) return 6;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[7] <= idx) return 7;
    return 8;
}


int CDataManager::getRankBonusIndex(int idx) const
{
    if (m_rankSystem.m_dungeonRank.m_rankLevel[0] <= idx) return 4;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[1] <= idx) return 3;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[2] <= idx) return 2;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[3] <= idx) return 1;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[4] <= idx) return 0;
    return -1;
}


int CDataManager::calRankPoint(int a, int b)
{
    return a + b;
}

// ===================== 任务 =====================

std::vector<int>* CDataManager::getDailyTrainingQuest(int idx)
{
    return &m_dailyTrainingQuest[idx - 1];
}

// [b28d-solo-CDataManager] 返回类型 int→bool（P292/P168a 档①）：ORIG
// 0x0836411e 返回点 `mov $0/1,%eax`，唯一调用点 Quest::check_possible
// 0x08352f3b `call; xor $0x1,%eax; test %al,%al; je` ⇒ callee bool。
bool CDataManager::isThereDailyTrainingQuestList(int idx, int questIdx)
{
    if (idx <= 0 || idx > 0x46)
    {
        return 0;
    }
    std::vector<int>* list = getDailyTrainingQuest(idx);
    for (std::vector<int>::iterator it = list->begin(); it != list->end(); ++it)
    {
        if (*it == questIdx)
        {
            return 1;
        }
    }
    return 0;
}

int CDataManager::get_WithinMissionIndex(int idx) const
{
    return m_pvpMissionSystem->get_WithinMissionIndex(idx);
}

std::list<CMission*>* CDataManager::get_kind_mission_list(int idx) const
{
    return &m_pvpMissionSystem->get_kind_mission_list(idx);
}

void CDataManager::get_New_DailyMission(
    std::vector<std::pair<unsigned int, unsigned int> >& out)
{
    m_pvpMissionSystem->get_New_DailyMission(out);
}

int CDataManager::get_BaseMissionExp_byRank(unsigned short rank) const
{
    return m_pvpMissionSystem->get_BaseMissionExp_byRank(rank);
}

std::bitset<32> CDataManager::get_daily_mission_kind() const
{
    return m_pvpMissionSystem->get_daily_mission_kind();
}

// ===================== 事件/道具 =====================

int CDataManager::getDecoEventItemIdx()
{
    std::vector<int>::iterator it = m_seriaRoomDeco.m_eventItemIdx.begin();
    if (it != m_seriaRoomDeco.m_eventItemIdx.end())
    {
        return *it;
    }
    return 0xffffffff;
}

bool CDataManager::getVaildEventItem(int idx)
{
    (void)idx;
    std::vector<int>::iterator it = m_seriaRoomDeco.m_eventItemIdx.begin();
    if (it != m_seriaRoomDeco.m_eventItemIdx.end())
    {
        return true;
    }
    return false;
}

int CDataManager::getMatchEventPoint(int idx)
{
    std::map<int, int>::iterator it = m_seriaRoomDeco.m_matchEventMap.find(idx);
    if (it != m_seriaRoomDeco.m_matchEventMap.end())
    {
        return it->second;
    }
    return 0;
}

int CDataManager::getMaxEventItemCount()
{
    return m_seriaRoomDeco.m_maxEventItemCount;
}

int CDataManager::getFatigue_contidion()
{
    return m_seriaRoomDeco.m_fatigueCondition;
}

int CDataManager::getRewardItem_MailExpireDate()
{
    return m_seriaRoomDeco.m_rewardMailExpireDate;
}

int CDataManager::get_GiftGrowthWeaponBox()
{
    return m_growthWeapon.m_giftBox;
}

int CDataManager::get_GrowthWeaponMaterialItem()
{
    return m_growthWeapon.m_materialItem;
}

unsigned short CDataManager::get_GrowthWeaponMaterialMax()
{
    return m_growthWeapon.m_materialMax;
}

unsigned short CDataManager::get_GrowthWeaponEventBubbleMax()
{
    return m_growthWeapon.m_eventBubbleMax;
}

unsigned short CDataManager::get_GrowthWeaponEventSandMax()
{
    return m_growthWeapon.m_eventSandMax;
}

int CDataManager::get_GrowthWeaponEventReward(int idx) const
{
    std::map<int, int>::const_iterator it = m_growthWeapon.m_rewardMap.find(idx);
    if (it != m_growthWeapon.m_rewardMap.end())
    {
        return it->second;
    }
    return 0;
}

int CDataManager::getChangeAbleCreatureLev()
{
    return m_createCreatureGift.m_changeAbleCreatureLev;
}

int CDataManager::getClientChangeCreatureIndex(int idx)
{
    std::map<int, int>::iterator it = m_createCreatureGift.m_infinityChangeMap.find(idx);
    if (it != m_createCreatureGift.m_infinityChangeMap.end())
    {
        return it->second;
    }
    return 0xffffffff;
}

int CDataManager::getInfinityChangeCreatureIndex(int idx)
{
    std::map<int, int>::iterator it = m_createCreatureGift.m_clientChangeMap.find(idx);
    if (it != m_createCreatureGift.m_clientChangeMap.end())
    {
        return it->second;
    }
    return 0xffffffff;
}

int CDataManager::getGiveGrowCreatureEventJobCount(int level)
{
    if (m_createCreatureGift.m_eventJobMap.empty())
    {
        return 0;
    }
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator it =
        m_createCreatureGift.m_eventJobMap.begin();
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator e =
        m_createCreatureGift.m_eventJobMap.end();
    int count = 0;
    while (it != e)
    {
        if (it->second.m_minLevel < level && level < it->second.m_maxLevel)
        {
            count += 1;
        }
        it++;
    }
    return count;
}

bool CDataManager::isAvailableGiveGrowCreatureEventJob(int job, int level)
{
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator
        it = m_createCreatureGift.m_eventJobMap.find(job);
    if (it != m_createCreatureGift.m_eventJobMap.end())
    {
        if (it->second.m_minLevel < level && level < it->second.m_maxLevel)
        {
            return true;
        }
    }
    return false;
}

int CDataManager::getSeriaRoomAniDecoItem(int idx)
{
    if (m_seriaAniDeco.m_itemMap.empty())
    {
        return 0;
    }
    ++idx;
    std::map<int, int>::iterator it = m_seriaAniDeco.m_itemMap.find(idx);
    if (it != m_seriaAniDeco.m_itemMap.end())
    {
        return it->second;
    }
    return 0;
}

int CDataManager::getSeriaRoomAniDecoRewardItem(int idx)
{
    if (m_seriaAniDeco.m_rewardList.empty())
    {
        return 0;
    }
    std::vector<seriaRoomDecoRewardInfo>::iterator it =
        m_seriaAniDeco.m_rewardList.begin();
    std::vector<seriaRoomDecoRewardInfo>::iterator e =
        m_seriaAniDeco.m_rewardList.end();
    while (it != e)
    {
        if (it->m_itemIdx == idx)
        {
            return it->m_reward;
        }
        it++;
    }
    return 0;
}

unsigned int CDataManager::maxAniDecoItem()
{
    return (unsigned int)m_seriaAniDeco.m_itemMap.size();
}

bool CDataManager::invalidAniDecoItem(int idx, int value)
{
    ++idx;
    std::map<int, int>::iterator it = m_seriaAniDeco.m_itemMap.find(idx);
    if (it != m_seriaAniDeco.m_itemMap.end())
    {
        if (it->second == value)
        {
            return true;
        }
    }
    return false;
}

unsigned int CDataManager::getChoiceItem()
{
    return m_choiceItem.m_choice;
}

void CDataManager::setChoiceItem(bool flag, unsigned int idx)
{
    m_choiceItem.m_checked = flag;
    m_choiceItem.m_choice = idx;
}

bool CDataManager::checkChoiceItem()
{
    return m_choiceItem.m_checked;
}

void CDataManager::set_lottery_use_cost(unsigned int money, unsigned int cost)
{
    m_lotteryNeedMoney.add_lottery_needMoney(money, cost);
}

// P223（b28）：ORIG `jg nop岛; body; jmp leave; nop; leave` 空块中继岛，
// 复现形=「if(越界) goto end; body; end:;」goto 尾空标签。
void CDataManager::set_original_dimensionInout(int idx, char value)
{
    if (idx > 5)
        goto end;
    m_originalDimensionInout[idx] = value;
end:
    ;
}

void CDataManager::reset_dimensionInout()
{
    memcpy(m_dimensionInout, m_originalDimensionInout, 6);
}

// ===================== 情感/过滤 =====================

// ---- ChattingEmoticonBase 基类虚函数（ORIG 0x80e5f46/0x80e5dde）----
bool ChattingEmoticonBase::isCommonEmoticon(const CUser& user) const
{
    // ORIG 0x80e5f46：vtable+0x08（isUsable）转发。
    // [b28c-L12 代落·C28L19 判词·N7] 唯一残差 = 虚调用目标寄存器：ORIG
    // `mov (%eax),%edx; …; call *%edx` vs 4.4 恒 `call *%eax`（14/14 助记符
    // 全等，范式库 N7）；源形已最简，不可翻。
    return isUsable(user);
}

// [b28d-solo-CDataManager AE] 修复：原嵌套 if 形被 4.4 折成内存直比（cmpl $imm,0xc(%ebp)），
// ORIG 为形参物化 + 寄存器比较链。改 switch 连续 case 组（0x2898bf/c0→53、
// c1/c2→57）即复现 `mov 0xc(%ebp),%eax; cmp $0x2898bf,%eax; jl fail; …`
// 决策链（P251/P261 switch 判据 + 探针 /tmp/p_sp/h.cpp f6 逐条全等）。
void ChattingEmoticonBase::AddPremiumIndex(int index)
{
    // ORIG 0x080e5dde：特定 index 映射到 premium 槽位集合。
    switch (index)
    {
    case 0x2898bf:
    case 0x2898c0:
        m_set.insert(0x53);
        break;
    case 0x2898c1:
    case 0x2898c2:
        m_set.insert(0x57);
        break;
    }
}

// ---- CChattingEmoticonList（ORIG find_emoticon 0x80e581c / ReCalc 0x80e5880）----
ChattingEmoticonBase* CChattingEmoticonList::find_emoticon(int idx)
{
    std::map<int, ChattingEmoticonBase*>::iterator it = m_map.find(idx);
    if (m_map.end() == it)   // ORIG @0x80e5850：eq 调用对象=end 槽(arg0=-0xc)
        return 0;
    return it->second;
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
int CChattingEmoticonList::ReCalc(CChattingEmoticonConfig& config,
                                  const CUser& user)
{
    std::vector<CChattingEmoticonConfigRow> rows;
    std::set<int> used;

    // ORIG 0x080e58?? `mov -0x20; cmp $0x17; setbe` = 无符号比较（范式库
    // P2 反例：signed setle 异形）。P310：局部 int + 显式 (unsigned) 比较
    // 复现 ORIG 的「装载 + 寄存器比较」两段尾测（unsigned 局部恒折内存直比）。
    for (int i = 0; (unsigned int)i <= 0x17; ++i)
    {
        ChattingEmoticonBase* p = find_emoticon(config.m_Row[i].m_nCount);
        if (p == 0)
            continue;
        if (!p->isCommonEmoticon(user))
            continue;
        rows.push_back(config.m_Row[i]);
        used.insert(config.m_Row[i].m_nCount);
    }
    config.clear();

    for (unsigned int r = 0; r < rows.size(); ++r)
    {
        ChattingEmoticonBase* p = find_emoticon(rows[r].m_nCount);
        if (p == 0)
            continue;
        for (int i = p->GetStartArrayIndex(); i <= p->GetEndArrayIndex(); ++i)
        {
            if (config.m_Row[i].m_nCount == 0)
            {
                config.m_Row[i] = rows[r];
                break;
            }
        }
    }

    for (std::map<int, ChattingEmoticonBase*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        if (used.find(it->second->GetIndex()) != used.end())
            continue;
        if (!it->second->isCommonEmoticon(user))
            continue;
        for (int i = it->second->GetStartArrayIndex();
             i <= it->second->GetEndArrayIndex(); ++i)
        {
            if (config.m_Row[i].m_nCount == 0)
            {
                config.m_Row[i].m_nCount = (short)it->second->GetIndex();
                break;
            }
        }
    }
    return 1;
}

// ReCalcEmoticonConf —— ORIG T 0x0835fc76 _ZN12CDataManager18ReCalcEmoticonConfER23CChattingEmoticonConfigRK5CUser
//   （docs/class_func_reports/CDataManager/ReCalcEmoticonConf.md）：
//   直通 m_emoticonList(+0x28) 的 CChattingEmoticonList::ReCalc（call 前置：
//   user 压 0x8(%esp)、config 压 0x4(%esp)、m_emoticonList 压 (%esp)）。
// ---------------------------------------------------------------------------
void CDataManager::ReCalcEmoticonConf(CChattingEmoticonConfig& config, const CUser& user)
{
    m_emoticonList->ReCalc(config, user);
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
std::vector<int> CDataManager::IsUsableEmoticons(const CUser& user,
                                                 const std::vector<int>& emoticons)
{
    std::vector<int> result;
    if (m_emoticonList != 0)
    {
        ChattingEmoticonBase* p;
        for (unsigned int i = 0; i < emoticons.size(); ++i)
        {
            p = m_emoticonList->find_emoticon(emoticons[i]);
            if (p == 0)
                continue;
            if (!p->isUsable(user))
                continue;
            result.push_back(emoticons[i]);
        }
    }
    return result;
}

int CDataManager::GetUpgradeRevisionPvPChannel()
{
    return m_pvpChannel.m_upgradeRevision;
}

float CDataManager::getUpgradeItemRepairCostRate(int idx, bool flag)
{
    return m_itemUpgrade->getUpgradeItemRepairCostRate(idx, flag);
}

bool CDataManager::checkValidShopItem(unsigned int itemIdx, unsigned long count)
{
    return m_itemShop->checkValidShopItem(itemIdx, count);
}

bool CDataManager::checkLimitNpcBuyItemList(unsigned int itemIdx, unsigned long count)
{
    return m_itemShop->checkLimitNpcBuyItemList(itemIdx, count);
}

bool CDataManager::hasPreventString(const char* name)
{
    if (getCharacNameFilterPass())
    {
        setCharacNameFilterPass(false);
        return false;
    }
    std::string str(name);
    return CSyncSlangFilter::GetInstance()->HasSlang(str);
}

bool CDataManager::hasPreventStringName(const char* name)
{
    std::string str(name);
    return CSyncSlangFilter::GetInstance()->HasSlangName(str);
}

bool CDataManager::getCharacNameFilterPass() const
{
    return m_characNameFilterPass;
}

void CDataManager::setCharacNameFilterPass(bool flag)
{
    m_characNameFilterPass = flag;
}

// ===================== 其它 =====================

const char* CDataManager::GetScriptPacks() const
{
    return m_scriptPacks.c_str();
}

void CDataManager::SetScriptPacks(const char* path)
{
    if (path == 0)
    {
        m_scriptPacks = "Script.pvf";
    }
    else
    {
        m_scriptPacks = path;
    }
}

void* CDataManager::GetItemMakingSkill() const
{
    return (void*)&m_itemMakingSkill;
}

void* CDataManager::GetImageCommnicationData()
{
    return (void*)&m_imageCommData;
}


std::set<int>* CDataManager::getUltimateDungeonList() const
{
    return (std::set<int>*)&m_ultimateDungeonList;
}
// ORIG W 0x0822b64c：return this+0xb424（&m_vendingMachine）。
std::vector<stVendingMachine_t>* CDataManager::getVendingMachineTable() const
{
    return (std::vector<stVendingMachine_t>*)&m_vendingMachine;
}

// 注：ORIG 仅有非 const getBlueMarbleScript()（0x08365eea，nm 无 _ZNK 重载）；
// 原 TU 内发明的 const 重载已删除（调用点走非 const 版本）。


// [b28d-E03 2026-09-10] 返回类型改 stHellParty_t*（见头注释）：函数体仍为单条 mov，
// 仅头/定义签名同步；消费点 static_cast<stHellParty_t*>(…) 为恒等转换、codegen 不变。
stHellParty_t* CDataManager::get_hellparty_script_values()
{
    return reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
}

// ORIG 0x08365fc0：sub $0x90120,%esp 后直接 leave/ret —— 保留等价栈帧。
void CDataManager::testActionType()
{
    char buf[0x90118];
    (void)buf;
}

// ORIG 0x08365ef8：sub $0x20,%esp 后直接 leave/ret —— 保留等价栈帧。
void CDataManager::testBingo()
{
    char buf[0x18];
    (void)buf;
}

int CDataManager::IsDBLogItem(unsigned int itemIdx)
{
    int cur = g_systemTimeInst.getCurSec();
    for (std::vector<stDBLogItem>::const_iterator it = m_dbLogItems.begin();
         it != m_dbLogItems.end(); it++)
    {
        if (it->m_itemIdx == itemIdx && cur <= it->m_time)
        {
            return it->m_value;
        }
    }
    return 0xffffffff;
}

// ===================== 构造/析构 =====================

CDataManager::CDataManager()
{
    m_disJoint = 0;
    m_characList = 0;
    m_questList = 0;
    m_itemList = 0;
    m_skillList = 0;
    m_monsterManager = 0;
    m_mapList = 0;
    m_dungeonList = 0;
    m_itemUpgrade = 0;
    m_itemUpgradeSeparate = 0;
    m_emoticonList = 0;
    m_itemGeneratorMgr = 0;
    m_invalidChar.m_slangFilter = 0;
    m_aiCharacterList = 0;
    m_autoMarket = 0;
    m_npcScriptList = 0;
    m_npcDynamicInfo = 0;
    m_npcRelation.m_itemLimitEditionMgr = 0;
    m_expRate = 1.0f;
    m_dropRate = 1.0f;
    m_altarConfig.m_strikerScript = 0;
    m_altarConfig.m_characManageScript = 0;
    m_choiceItem.m_blueMarbleScriptMgr = 0;
}

CDataManager::~CDataManager()
{
    destroy();
}

void CDataManager::destroy()
{
    // [b28d-solo-CDataManager AE] 本波 TU 口径已闭合（不在 DIFF/NEAR 列表）。
    // N7 封顶：delete 链中唯一虚析构调用（+0x86f4 vptr+4 间接 call）两侧
    // vptr/obj 寄存器互换（ORIG edx/eax vs 4.4 eax/edx，N7 族）；其余 307 条
    // 归一后逐条相等（fn_diff_tu 实证）。语义等价封顶。
    if (m_disJoint)
    {
        delete m_disJoint;
        m_disJoint = 0;
    }
    if (m_characList)
    {
        delete m_characList;
        m_characList = 0;
    }
    if (m_questList)
    {
        delete m_questList;
        m_questList = 0;
    }
    if (m_itemList)
    {
        delete m_itemList;
        m_itemList = 0;
    }
    if (m_skillList)
    {
        delete m_skillList;
        m_skillList = 0;
    }
    if (m_monsterManager)
    {
        delete m_monsterManager;
        m_monsterManager = 0;
    }
    if (m_mapList)
    {
        delete m_mapList;
        m_mapList = 0;
    }
    if (m_dungeonList)
    {
        delete m_dungeonList;
        m_dungeonList = 0;
    }
    if (m_itemUpgrade)
    {
        delete m_itemUpgrade;
        m_itemUpgrade = 0;
    }
    if (m_itemUpgradeSeparate)
    {
        delete m_itemUpgradeSeparate;
        m_itemUpgradeSeparate = 0;
    }
    if (m_emoticonList)
    {
        delete m_emoticonList;
        m_emoticonList = 0;
    }
    if (m_itemGeneratorMgr)
    {
        delete m_itemGeneratorMgr;
        m_itemGeneratorMgr = 0;
    }
    if (m_invalidChar.m_slangFilter)
    {
        // ORIG destroy：(**(code**)(vptr+4))(p) —— 经 vtable 第 2 槽调 SlangFilter
        // 虚删除析构 D0（权威类 vtable 槽1 即 deleting dtor），等价 delete。
        delete m_invalidChar.m_slangFilter;
        m_invalidChar.m_slangFilter = 0;
    }
    if (m_aiCharacterList)
    {
        delete m_aiCharacterList;
        m_aiCharacterList = 0;
    }
    if (m_autoMarket)
    {
        delete m_autoMarket;
        m_autoMarket = 0;
    }
    if (m_npcScriptList)
    {
        delete m_npcScriptList;
        m_npcScriptList = 0;
    }
    if (m_npcDynamicInfo)
    {
        delete m_npcDynamicInfo;
        m_npcDynamicInfo = 0;
    }
    if (m_npcRelation.m_itemLimitEditionMgr)
    {
        delete m_npcRelation.m_itemLimitEditionMgr;
        m_npcRelation.m_itemLimitEditionMgr = 0;
    }
    if (m_altarConfig.m_strikerScript)
    {
        delete m_altarConfig.m_strikerScript;
        m_altarConfig.m_strikerScript = 0;
    }
    if (m_altarConfig.m_characManageScript)
    {
        delete m_altarConfig.m_characManageScript;
        m_altarConfig.m_characManageScript = 0;
    }
    if (m_choiceItem.m_blueMarbleScriptMgr)
    {
        // ORIG destroy：BlueMarbleScriptManager::~BlueMarbleScriptManager(p) +
        // operator delete(p)（destroy.md 0x8357a66 尾段）→ 等价 delete。
        // 权威类（PvPTypes.h）无用户析构，隐式析构 + operator delete 由编译器生成。
        delete m_choiceItem.m_blueMarbleScriptMgr;
        m_choiceItem.m_blueMarbleScriptMgr = 0;
    }
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
CChattingEmoticonList::~CChattingEmoticonList() {}

char CDataManager::get_limit_inout_count(ENUM_BLOOD_TYPE type)
{
    return m_limitInoutCount[type];
}

// ============================================================================
// 迁移自 GameStubs.cpp 的 CDataManager 桩方法（ORIG 符号/偏移已核对）
// ============================================================================

void CDataManager::set_AuctionRegFreeCeraItem(unsigned int idx)
{
    // ORIG 0x08513fbc W：m_choiceSet(+0xb4ac).insert(idx)
    m_choiceSet.insert(idx);
}

void* CDataManager::GetExpertJobScript(int job)
{
    // ORIG 0x0822b5f2 W：m_regeneration.m_expertJobList（CDM+0x5090）直取后
    // 调 CExpertJobList::get(job)，无中间暂存。
    return m_regeneration.m_expertJobList->get(job);
}

void* CDataManager::GetExpertJobEtcScript()
{
    // ORIG 0x0849f516 W：调 CExpertJobList::get_etc()（W 0x0849f50a，返回 this+0x18
    // = &m_etc）。CUser 侧将其作 std::map<uchar,short>（等级→所需角色等级）读取。
    return m_regeneration.m_expertJobList->get_etc();
}

void* CDataManager::GetChannelScript() const
{
    // ORIG 0x0814a6f8 W：return &m_channelScript（+0xb3f4）
    return const_cast<channel_script_t*>(&m_channelScript);
}

// [b28d-solo-CDataManager AE] 修复：trace 改 `cMyTrace(__FUNCTION__,0xd44,5)(...)`
// 全表达式临时链（P221）翻正 it/end/trace 槽向，并把 ctor 串改回 ORIG 的
// __FUNCTION__ 符号、fmt 改回 ORIG 原文 "not finnd tower : %u"（0x8b6f4c0）。
stDeathTower_t* CDataManager::GetTowerScript(unsigned int dungeonIdx)
{
    // ORIG W 0x081455d8：m_deathTowerMap.find(dungeonIdx)；未找到打 cMyTrace；
    // 尾部单条 add $0x4,%eax 是 pair.second 的偏移 —— 返回值即 &it->second
    // （结构体基址）。调用方（CBossPlay/CBossStage）按字段偏移取用：
    // flds 0x4(%eax,%edx,4) = m_rateA[idx]、+0xc88/+0xc8c/+0xc94 奖励概率。
    std::map<unsigned int, stDeathTower_t>::iterator it =
        m_deathTowerMap.find(dungeonIdx);
    if (it == m_deathTowerMap.end())
    {
        // ORIG：dungeonIdx 先物化到 %ebx 跨 C1+cl 两调用保活（P87 register 形）。
        // b28 BM4 判词（N93/N82附 在册负例）：fresh 实残 7 行 = 块内条件构造
        // trace 与函数头 iterator/end 临时槽深方向跨版本相反（4.1 构造序
        // it=-0x20/end=-0x1c/trace=-0x18；4.4 trace=-0x20/it=-0x10/end=-0xc）。
        // trace ctor 受 if 路守护不可头部化（P212 不适用）；声明序交换=构造序
        // 变化反劣（N93 实证）。dungeonIdx→%ebx 直传段已同形。语义等价封顶。
        register unsigned int idx = dungeonIdx;
        cMyTrace(__FUNCTION__, 0xd44, 5)("not finnd tower : %u", idx);
    }
    return &it->second;
}

void* CDataManager::getBlueMarbleScript()
{
    // ORIG 0x08365eea T：return *(void**)(this+0xb4a8)
    //   = m_choiceItem.m_blueMarbleScriptMgr（ChoiceItemInfo +0x08，已类型化）。
    return (void*)m_choiceItem.m_blueMarbleScriptMgr;
}

void* CDataManager::get_event_script_mng()
{
    // ORIG 0x08110b62 W：return m_liveServerInfo.m_eventScriptMng（已类型化）
    return (void*)m_liveServerInfo.m_eventScriptMng;
}

CItemLimitEditionMgr* CDataManager::getItemLimitEditionMgr()
{
    // ORIG 0x08179fa6 W：return m_npcRelation.m_itemLimitEditionMgr（+0x18）
    return m_npcRelation.m_itemLimitEditionMgr;
}

char CDataManager::get_dimensionInout(int idx)
{
    // ORIG 0x0822b612：返回类型 char（movzbl 单条读取，无 32 位扩展）；
    // 越界检查顺序与 ORIG 一致（cmp $5 在前、cmp $0 在后）。
    if (idx > 5 || idx < 0)
        return 0;
    return m_dimensionInout[idx];
}

bool CDataManager::GetPvPChannelGrade()
{
    // [b28d-solo-CDataManager AE] 本波 TU 口径已闭合（不在 DIFF/NEAR 列表）。
    // ORIG 0x08357e9e T：-0x1c(dst)/-0x38(serverGroup) 在 guard 前物化；
    // guard `xor$1;test;je` 为 if(IsPVPChannel()) 正向包裹（0x08357ecd）。
    // 返回 0/1（ORIG `mov$0/mov$1,%eax` 双路，P159）。
    pvp_channel_info_t* dst = &m_pvpChannel;                    // 0x08357eaa
    int serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x378
    if (!G_GameWorld()->IsPVPChannel())
    {
        return 0;
    }
    {
        std::map<int, std::vector<pvp_channel_info_t> >::iterator it =
            m_liveServerInfo.m_pvpChannelMap.find(serverGroup);
        if (it != m_liveServerInfo.m_pvpChannelMap.end())
        {
            // ORIG 0x08357f57：channelNo 每轮重取 env+0x1b0（G_CEnvironment
            // 调用不被缓存，P48）；命中整结构赋值（0x08357f83..0x119 逐 dword
            // 展开拷贝 9 项）后跳出。end 每轮重取 it->second.end()（0x08357fcd）。
            for (std::vector<pvp_channel_info_t>::iterator vit =
                     it->second.begin();
                 vit != it->second.end(); ++vit)
            {
                if (G_CEnvironment()->m_serverEnvir.m_channelNo == vit->m_field0)
                {
                    // ORIG 0x08357f83 `mov -0x1c(%ebp),%edx` + 逐 dword (0..0x20)
                    // = 经命名指针槽的聚合赋值（非 this+disp 直写）。
                    *dst = *vit;
                    break;
                }
            }
        }
        // ORIG 0x08358001..：P37 临时链式——R2L 求值使 maxLevel..+0xc 落
        // -0x58..-0x4c 槽、+8/+4 跨 ctor 存 edi/esi、group 存 ebx，随后才
        // ctor（0x08358052）→ operator()（0x08358095）。pvpChannelNum 常数 0。
        cMyTrace(__PRETTY_FUNCTION__, 0x18f3, 0)(
            "PVP Channel Information  serverGroup(%d), pvpChannelNum(%d), "
            "minGrade(%d), maxGrade(%d), upgradeRevision(%d), type(%d), "
            "minLevel(%d), maxLevel(%d)",
            serverGroup, 0, dst->m_minGrade, dst->m_maxGrade,
            dst->m_upgradeRevision, dst->m_channelType, dst->m_minLevel,
            dst->m_maxLevel);
        return 1;
    }
    return 0;
}

int CDataManager::GetMaxGradePvPChannel()
{
    // ORIG 0x0822b65a W：返回 m_pvpChannel(+0xb430) 内 +0x8 字段。
    return m_pvpChannel.m_maxGrade;

}

int CDataManager::GetPvPChannelType()
{
    // ORIG 0x085e68a8 W：mov 0xb440(%eax) → m_pvpChannel+0x10。
    return m_pvpChannel.m_channelType;
}

const char* CDataManager::GetCeraShopGoodsName()
{
    // ORIG 0x08513fe6 W：return m_scriptPackPath.c_str()（+0x8cb8）
    return m_scriptPackPath.c_str();
}

const char* CDataManager::SuddenShopFileName1()
{
    // ORIG 0x08513ffe W：return m_packName2.c_str()（+0xa8c4）
    return m_packName2.c_str();
}

const char* CDataManager::SuddenShopFileName2()
{
    // ORIG 0x08514016 W：return m_packName3.c_str()（+0xa8c8）
    return m_packName3.c_str();
}

const char* CDataManager::SuddenShopFileName3()
{
    // ORIG 0x0851402e W：return m_packName4.c_str()（+0xa8cc）
    return m_packName4.c_str();
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
bool CDataManager::VerifyMap(const CMap& map, int idx) const
{
    // ORIG 0x083620ee：map type 与 monster list 只被读取，结果不参与返回。
    (void)map.get_map_type();
    const std::list<_mapMonster> monsterList = map.get_monster_list();
    // ORIG 0x08362125：`lea -0xd0; mov %eax,-0x58` —— monsterList 经具名指针
    // 局部访问（-0x58 槽，P0 槽往返判据），begin/end 实参从该槽重载。
    const std::list<_mapMonster>* pMonsterList = &monsterList;
    for (std::list<_mapMonster>::const_iterator it = pMonsterList->begin();
         it != pMonsterList->end(); it++)
    {
        // ORIG 0x08362153..：`test %al; jne ++; mov 0x8; test %eax` 尾 test 无
        // 消费者 = `flag==0 && field` 空体短路链（结果丢弃，不物化槽）。
        const _mapMonster& mon = *it;
        // ORIG 0x08362153..：`movzbl 0x4; test; jne INCR; mov 0x8; test`
        // ——内层空体 if 的 je 被 4.1 jump-to-next 删除、外层保留。4.4 对裸空
        // if 会连条件求值一起 DCE；`__builtin_expect(cond,0)` 使其保留求值并
        // 同样删除内层 jcc（探针 /tmp/probe_mon5.cpp g8 逐条同构）。
        if (mon.m_flag04 == 0)
        {
            if (__builtin_expect(mon.m_field08 == 0, 0))
            {
            }
        }
    }

    const std::vector<STAICharacterMapArrangeData>* aiList =
        map.getAICharacters();
    if (aiList->empty() == false)
    {
        // ORIG 0x083621f6..0x0836220b：end() 在循环**初始化**处求值一次并缓存
        // 到具名 itEnd(-0xe4)，条件块 0x0836229d 只调 ne（P13）——旧形
        // `it != aiList->end()` 把 end() 留在条件块（每次迭代重调）。
        for (std::vector<STAICharacterMapArrangeData>::const_iterator it =
                 aiList->begin(),
                 itEnd = aiList->end(); it != itEnd; ++it)
        {
            const STAICharacterMapArrangeData& ai = *it;
            // ORIG 0x08362..：经 G_CDataManager() 全局取 m_aiCharacterList（+0x869c）。
            // 实参 ai.m_type 在 G_CDataManager() 之前求值并跨调用保活
            // （ORIG 0x08362227 `mov (%eax),%eax; mov %eax,%ebx`，P52/P0：
            // 无具名 apcType 槽）；结果具名指针后判空（ORIG -0x40 槽 +
            // `cmpl $0x0,槽; jne`，P197④ 槽比零直跳）。
            CAICharacter* pAICharacter =
                G_CDataManager()->m_aiCharacterList->get(ai.m_type);
            if (pAICharacter == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", __PRETTY_FUNCTION__, 0x2847,
                    "Unknown APC Type(%d). Map(%d)", ai.m_type,
                    map.get_index());
            }
        }
    }

    std::list<int> dungeonList;
    // ORIG 0x083622cb..：idx != -1 → push_back(&idx)（形参槽按引用）；
    // 否则拷贝赋值 getDungeonLists() 解引用。
    if (idx != -1)
        dungeonList.push_back(idx);
    else
        dungeonList = *map.getDungeonLists();

    const std::list<_mapItem> itemList = map.get_item_list();
    // ORIG 0x0836231f..：`lea -0xc0; mov %eax,-0x50` —— itemList 经具名指针
    // 局部访问（-0x50 槽，P0 槽往返判据），begin/end 实参从该槽重载。
    const std::list<_mapItem>* pItemList = &itemList;
    for (std::list<_mapItem>::const_iterator itemIt = pItemList->begin();
         itemIt != pItemList->end(); itemIt++)
    {
        const _mapItem& item = *itemIt;
        // ORIG 0x08362369/0x08362384：内层 dungeonList 的 begin/end 同在
        // 循环初始化处求值（end 缓存 -0xf0），条件块只调 ne（P13）。
        for (std::list<int>::const_iterator dungeonIt = dungeonList.begin(),
                 dungeonEnd = dungeonList.end();
             dungeonIt != dungeonEnd; ++dungeonIt)
        {
            CDungeon* dungeon = find_dungeon(*dungeonIt);
            const std::vector<stDungeonAssignItem_t>* passive =
                dungeon->getSpecialPassiveObjectItems();
            // ORIG 0x08362401 `call size; mov (%edx),%edx; cmp %edx,%eax`：
            // size() 结果与 m_objectIdx 的死比较（无消费者；4.4 把 cmp DCE、
            // 仅保留 call —— 跨版本 -O0 DCE 差，N 封顶）。
            passive->size() == (unsigned int)item.m_objectIdx;
            // ORIG 0x0836240b/0x0836241d：`m_objectIdx != -1` 与 `!empty()` 两
            // 守卫共落 FALSE 块、`mov $1/jmp/mov $0` 寄存器物化 ⇒ 正极性短路
            // 形（P214①；旧 usable 具名 bool 产生槽物化 movb/cmpb 三连）。
            if (item.m_objectIdx != -1 && !passive->empty())
            {
                // ORIG 0x0836243e/0x0836245d：两分支各自重取 at()（P48），
                // 命中再第三次 at() 读弃（0x08362476）。
            if (passive->at(item.m_objectIdx).m_assignItemIdx < 1 ||
                passive->at(item.m_objectIdx).m_assignItemIdx > 200)
                {
                passive->at(item.m_objectIdx);
                }
            }
        }
    }

    // ORIG 0x0836242a 起：missingDungeon(-0x49) 先置 0；hash 全表扫描仅在
    // dungeonList **为空**侧执行——0x0836251c `call empty; test %al,%al;
    // je 0836292a`：empty()!=0（空）落扫描体，非空直接跳到第二段
    // （Ghidra `if (cVar1=='\0') { LAB_0836292a: 第二段 }` 同证）；found(-0x2d)
    // 检查位于循环体首（for+break 形，0x08362481）；扫描后无条件再过一遍
    // dungeonList（0x0836283c，空表 0 轮），missing 判定在两段扫描之后。
        bool missingDungeon = false;
    if (dungeonList.empty())
    {
    bool found = false;
    for (__gnu_cxx::hash_map<int, CDungeon*>::const_iterator it =
             m_dungeonList->m_dungeonMap.begin();
             it != m_dungeonList->m_dungeonMap.end(); ++it)
        {
            if (found)
                break;
        PvPMapInfo pvpInfo;
        (void)pvpInfo;
        // ORIG 0x08362592/0x083625a4：范围判定两次独立 get_index（P44 实参即时求值）。
        if (map.get_index() > 0xc350 && map.get_index() <= 0xc738)
            {
                found = true;
                break;
            }

        CDungeon* dungeon = it->second;
        if (dungeon->getDungeonKind() == 1)
            {
                // ORIG 0x083625e3..：直读 m_deathTowerMaxStage(+0x680) 字段 +
                // m_deathTowerMapIndexes(+0x674) operator[]，非 getter。
            for (int stage = 1;
                 stage <= dungeon->m_deathTowerMaxStage && !found;
                 ++stage)
                {
                if (dungeon->m_deathTowerMapIndexes[stage] == map.get_index())
{
                        found = true;
            break;
    }
                }
            }

            // ORIG 0x08362657..：m_mazeList(+0x62c).size() + operator[] 索引。
            // ORIG 0x083628a1 `cmp -0x24(%ebp),%eax; jbe` 为**无符号**尾测
            // （P196/P218）⇒ mi 声明 unsigned（旧 `(int)size()` 强转产 jle）。
        for (unsigned int mi = 0; mi < dungeon->m_mazeList.size() && !found; ++mi)
            {
            const MazeScript& maze = dungeon->m_mazeList[mi];
            if (maze.m_2c == map.get_index())
                {
                    found = true;
                break;
                }
            for (int y = 0; y < maze.m_height && !found; ++y)
                {
                for (int x = 0; x < maze.m_width && !found; ++x)
    {
                    const GridScript& grid = maze.getGridR(x, y);
                    if (grid.m_gridValue == 0)
                continue;
                        // ORIG 0x0836276a/0x08362803：两条 find 走 `||` 短路
                        // 物化（0x08362787 `jne 真块`、0x0836281a `je 假块`、
                        // 0x0836281c `mov $1/jmp` + `mov $0`），命中即 found；
                        // 旧 if/else-if 两独立块多一组 movb/jmp（P214①）。
                    if (std::find(grid.m_mapCandidates.begin(),
                                  grid.m_mapCandidates.end(),
                                  map.get_index()) != grid.m_mapCandidates.end() ||
                        std::find(grid.m_secondCandidates.begin(),
                                  grid.m_secondCandidates.end(),
                                  map.get_index()) != grid.m_secondCandidates.end())
                        found = true;
            }
                }
            }
        }
    }

        // ORIG 0x08362952/0x0836297f：第二段 dungeonList 的 begin/end 同在
        // 循环初始化处求值（end 缓存 -0x108），条件块只调 ne（P13）。
        for (std::list<int>::const_iterator dungeonIt = dungeonList.begin(),
                 dungeonEnd = dungeonList.end();
             dungeonIt != dungeonEnd; ++dungeonIt)
    {
            CDungeon* dungeon = find_dungeon(*dungeonIt);
            if (dungeon == 0)
                missingDungeon = true;
            // ORIG 0x083629c2：直读 m_eventMonsters(+0x690) 成员（内联 getEventMonsters）。
        if (dungeon->m_eventMonsters.empty() == false)
                map.m_rivalData2.empty();
    }

        if (missingDungeon)
            return 0;

    // ORIG 0x08362a43..：读 pair first/second 负值检查 + first 与 0x64 比较，
    // 结果全部丢弃；保留读取不发明校验规则。
        for (std::vector<std::pair<int, int> >::const_iterator it =
                 map.m_entranceVec.begin(); it != map.m_entranceVec.end(); it++)
    {
            const std::pair<int, int>& e = *it;
        if (e.first >= 0 && e.second >= 0)
        {
            int cmp = e.first;
            (void)cmp;
        }
    }
    return 1;
}

void* CDataManager::get_dungeon(int idx)
{
    // ORIG 0x086d18c8 W：return find_dungeon(idx)
    return find_dungeon(idx);
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
bool CDataManager::reselectDailyTrainingQuest()
{
    // ORIG 0x08363ce0 T。主分支：
    //  1) suffleTrainingQuests() 重排每日训练任务。
    //  2) getApplyLevel 填充 multiset<int>（各档可选等级）。
    //  3) 对每个等级 level(1..0x46)：清空 m_dailyTrainingQuest[level-1]，
    //     遍历 multiset，用 getTrainingQuest(level, prevApply, prevMin) 取任务 idx；
    //     find_quest 命中且 Quest::m_type==1 → push_back 到当日列表；否则 error 日志。
    //  4) reset_same_apply_level_cnt()；非空列表记 TRAINING_QUEST trace。
    // ORIG 0x08363cec 函数入口 `movl $0x0,-0x28` ⇒ quest 为函数头声明
    // （P212/P309 全头部声明），循环内仅赋值。
    Quest* quest = 0;
    m_trainingQuest.suffleTrainingQuests();

    // ORIG 0x08363d03：getApplyLevel 按 level 分桶写入 multiset 数组
    // （out[level-1]，level 1..0x46），主循环对每个 level 遍历对应桶。
    std::multiset<int> applyLevels[0x46];
    m_trainingQuest.getApplyLevel(applyLevels);

    for (int level = 1; level < 0x47; ++level)
    {
        m_dailyTrainingQuest[level - 1].clear();
        int prevApply = 0x7fffffff;
        // ORIG 0x08363dbe/0x08363dc5：cur(-0x1c)/questIdx(-0x50) 亦为每
        // level 一次的具名初始化（cur 初值 0x7fffffff、questIdx 0）。
        int cur = 0x7fffffff;
        int questIdx = 0;
        // ORIG 0x08363dee 调 _ZNKSt8multiset...beginEv（const 版）+
        // _Rb_tree_const_iterator deref/!=，且无 iterator→const_iterator
        // 转换调用 ⇒ 经 const 视图走 const 重载。ORIG 0x08363ee7/0x08363f0e
        // 每次迭代重算 applyLevels[level-1] 地址并重调 end()（无 bucket 引用
        // 槽、无 end 缓存，P183 地址重物化）⇒ 内联 static_cast 视图表达式。
        for (std::multiset<int>::const_iterator it =
                 static_cast<const std::multiset<int>&>(applyLevels[level - 1]).begin();
             it != static_cast<const std::multiset<int>&>(applyLevels[level - 1]).end();
             ++it)
        {
            cur = *it;
            questIdx = m_trainingQuest.getTrainingQuest(level, cur, prevApply);
            prevApply = cur;
            // ORIG 0x08363e42 `je +0x1f8` / 0x08363e64 `je +0x1fb` 两处空块
            // nop 中继岛（P223）：否定 goto + 尾标签串（标签序=守卫序）。
            if (questIdx == 0x7fffffff)
                goto train_skip_quest;
            quest = m_questList->find_quest(questIdx);
            if (quest == 0)
                goto train_skip_branch;
            {
                if (quest->m_type != 1)
                {
                    // ORIG 0x08363e6f `je +0x1cd`（m_type==1 跳 push_back 块，
                    // 日志块直落）⇒ 源形为否定式（P272 德摩根块序交换）。
                    LogManager::logFormat(
                        1, "data_manager.cpp",
                        __PRETTY_FUNCTION__,
                        0x2ac8,
                        "reselectDailyTrainingQuest() ERROR : quest(%d), "
                        "level(%d)",
                        questIdx, level);
                }
                else
                {
                    m_dailyTrainingQuest[level - 1].push_back(questIdx);
                }
            }
        train_skip_branch:;
        train_skip_quest:;
        }
        m_trainingQuest.reset_same_apply_level_cnt();
        if (!m_dailyTrainingQuest[level - 1].empty())
        {
            // ORIG 0x08363f58..0x08364075：仅一次 `CStringMakerC1Ev`
            // （0x08363f6e）+ 4×D1、**零** copy ctor call ⇒ 原码传匿名临时
            // （copy elision 直构形参）；旧具名 maker 形多 1 copy C1ERKS_ + 2 D1
            // （p167 EXTRA）。全表达式临时链复现（P37/P221）。
            cMyTrace(__PRETTY_FUNCTION__, 0x2ad2, 0)(
                "TRAINING_QUEST level(%d), quest(%s)", level,
                std::for_each(m_dailyTrainingQuest[level - 1].begin(),
                              m_dailyTrainingQuest[level - 1].end(),
                              sync_script::CStringMaker()).c_str());
        }
    }
    return 1;
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
int getCharacterJobToEnumValue(std::string& job)
{
    if (job == "swordman")
        return 0;
    if (job == "fighter")
        return 1;
    if (job == "gunner")
        return 2;
    if (job == "mage")
        return 3;
    if (job == "priest")
        return 4;
    if (job == "at gunner")
        return 5;
    if (job == "thief")
        return 6;
    if (job == "at fighter")
        return 7;
    if (job == "at mage")
        return 8;
    if (job == "demonic swordman")
        return 9;
    if (job == "creator mage")
        return 10;
    if (job == "common")
        return 11;
    return -1;
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
int importPvPReferenceTable(const char* path, stPvpRefTable_t& table)
{
    // ORIG 0x0890857b：ok(-0x25) 为函数级共享旗（入口 `movb $0,-0x25` 一次，
    // 各 ScanInt(&ok) 复用不重置）；line(-0x2c) 先于 load 构造；各块
    // 「先落 table 字段、后查 ok」（0x08908720/0x08908746/0x08908787）。
    bool ok = false;
    std::string line;
    if (!loadRDARScriptFile("", path))
    {
        return 0x1816;
    }
    // [b28d-solo-CDataManager] ORIG 0x089085bd 条件块在循环头（+0x42 直落）、
    // 各块失败路径 `jmp +0x42` 回边；4.4 对 while 恒旋转成入口 jmp（P164）。
    // 改 goto 头条件形复现（LoadScript 同法 AE 实证）。
pvp_cond:
    if (!ScanType(line, true))
        goto pvp_done;
    {
        if (line == "[pvp experience max grade]")
        {
            // [b28d-solo-CDataManager] ORIG 0x0890861c 无 cnt 槽往返
            // （`call ScanInt; add $0x1,%eax` 表达式直用，P0）——旧具名 cnt
            // 多 store+load 两条。
            table.m_count = 1 + ScanInt(&ok);
            if (!ok)
                return 0x181f;
            table.m_grades.reserve(table.m_count);
            table.m_grades.resize(table.m_count, stPvPGradeExp_t());
        }
        else if (line == "[pvp experience]")
        {
            // ORIG 0x0890867f：while(逗号) 形——grade 先落 -0x10、再查 ok；
            // 内层失败 ebx=0x182d；exp 落 -0xc；operator[] LHS 地址先行
            // （0x089086c1），stPvPGradeExp_t(grade,exp) 临时(-0x1c) 后构，
            // 末两 dword 写入。
            int grade;
        pvp_exp_scan:
            grade = ScanInt(&ok);
            if (!ok)
                goto pvp_cond;
            {
                int exp = ScanInt(&ok);
                if (!ok)
                    return 0x182d;
                table.m_grades[grade] = stPvPGradeExp_t(grade, exp);
            }
            goto pvp_exp_scan;
        }
        else if (line == "[pvp experience loss]")
        {
            table.m_pvpExpLoss1 = ScanInt(&ok);
            if (!ok)
            {
                return 0x1835;
            }
            table.m_pvpExpLoss2 = ScanInt(&ok);
            if (!ok)
            {
                return 0x1838;
            }
        }
        else if (line == "[pvp rank rate grade]")
        {
            table.m_rankRateGrade = ScanInt(&ok);
            if (!ok)
                return 0x183e;
        }
        else if (line == "[pvp rank rate]")
        {
            // [b28d-solo-CDataManager] ORIG 0x089087bd：v 无入口清零、
            // push_back 传左值槽（`push_backERKi`，ORIG call 8111126）；
            // 旧形 `int v = 0` + `static_cast<int&&>(v)` 多入口 store 与
            // 右值临时槽（-0x18）两条并选错 EOi 重载。
            int v;
        pvp_rate_scan:
            v = ScanInt(&ok);
            if (!ok)
                goto pvp_cond;
            table.m_rankRate.push_back(v);
            goto pvp_rate_scan;
        }
        else if (line == "[pvp normal battle time]")
        {
            PVP_NORMAL_BATTLE_TIME[0] = ScanInt((bool*)0);
        }
        else if (line == "[pvp deathmatch battle time]")
        {
            PVP_DM_BATTLE_TIME[0] = ScanInt((bool*)0);
        }
        else if (line == "[pvp relay battle time]")
        {
            PVP_RELAY_BATTLE_TIME[0] = ScanInt((bool*)0);
        }
        else if (line == "[pvp normal battle time for tournament]")
        {
            PVP_NORMAL_BATTLE_TIME[1] = ScanInt((bool*)0);
        }
        else if (line == "[pvp deathmatch battle time for tournament]")
        {
            PVP_DM_BATTLE_TIME[1] = ScanInt((bool*)0);
        }
        else if (line == "[pvp relay battle time for tournament]")
        {
            PVP_RELAY_BATTLE_TIME[1] = ScanInt((bool*)0);
        }
        else if (line == "[pvp record order]")
        {
            table.m_recordOrder.clear();
            std::string job;
            // [b28d-SweepDataMgrInit] ORIG 0x08908931..0x08908974 实测：
            //   call ScanStr; xor $1,%eax; test %al,%al; jne 退出; body;
            //   jmp 回 ScanStr —— 循环头即 ScanStr 调用、失败取反物化退出。
            // 4.4 对 `while(ScanStr(&job))` 恒旋转成入口 jmp + 尾 test/jne
            // （缺 xor $1，入口多 jmp），与 ORIG 逐条不符；改 goto 头条件形
            // 复现（P164/P223；本函数外层与两个内层逗号 while 已同法）。
        pvp_record_scan:
            if (!ScanStr(&job))
                goto pvp_record_done;
            // [b28d-solo-CDataManager] ORIG 0x08908961 选
            // `push_backEOi`（右值）⇒ 直接传函数返回值，无具名 idx
            // （具名左值会选 ERKi，p167 MISSING EOi/EXTRA ERKi）。
            table.m_recordOrder.push_back(getCharacterJobToEnumValue(job));
            goto pvp_record_scan;
        pvp_record_done:
            ;
        }
        else if (line == "[pvp team hp correction]")
        {
            ScanFloat((bool*)0);  // 读取但丢弃
        }
    }
    goto pvp_cond;
pvp_done:
    return 0;  // EOF：解析成功
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
bool CDataManager::set_pvp_grade()
{
    // ORIG 0x08360aa2：Clear 走 `mov &s_ref_pvp_grade,%eax; call` 两段直发
    // （无指针命名槽）；lastGrade(-0x14)/prevExp(-0x10) 在 table ctor 前
    // 初始化；table 由 C1 0x0837f45e 自带清零（ORIG 无 m_count 手写 store）。
    // [b28d-lane16 真语义缺口] ORIG 0x08360aaa-0x08360ab2 先调 RefPvpGrade::Clear()
    // （p167 差集 MISSING -1 _ZN11RefPvpGrade5ClearEv）；原重建缺失该清表调用。
    GlobalData::s_ref_pvp_grade->Clear();
    // [b28d-solo-CDataManager] ORIG 入口清零序 -0x14(prevExp) → -0x10(lastGrade)
    // ⇒ 声明序 prevExp 先（4.1/4.4 同向：先声明占深槽）。循环体序按 ORIG
    // 0x08360aff：++lastGrade → data.curr=prevExp → data.next → prevExp →
    // SetPvpGrade（三次独立 op[]，P48）。
    int prevExp = 0;
    int lastGrade = 0;
    stPvpRefTable_t table;
    if (importPvPReferenceTable("Etc/pvp_ref.etc", table) != 0)
        return false;
    for (unsigned int i = 1; i < (unsigned int)table.m_count; ++i)
    {
        RefPvpGrade::RefPvpGradeData data;
        ++lastGrade;
        data.curr = prevExp;
        data.next = table.m_grades[i].b;
        prevExp = table.m_grades[i].b;
        GlobalData::s_ref_pvp_grade->SetPvpGrade(table.m_grades[i].a, data);
    }
    GlobalData::s_ref_pvp_grade->set(table);
    GlobalData::s_ref_pvp_grade->SetLastGrade(lastGrade);
    return true;
}

// ORIG _ZNK12CDataManager16GetResetItemInfoEv @ 0x08693f24 (W)：return this+0x4d04
// = &m_limitItemUsage.m_resetC.m_list（stLimitItemUsageInfoEx@0x4cf8 + stResetItemInfo@+0x0c）。
std::vector<stResetItemInfo::ResetItem>* CDataManager::GetResetItemInfo() const
{
    return const_cast<std::vector<stResetItemInfo::ResetItem>*>(
        &m_limitItemUsage.m_resetC.m_list);
}

// ORIG _ZNK12CDataManager17GetRefillItemInfoEv @ 0x08693f32 (W)：return this+0x4d10
// = &m_limitItemUsage.m_refill18.m_list（stLimitItemUsageInfoEx@0x4cf8 + stRefillItemInfo@+0x18）。
std::vector<stRefillItemInfo::RefillItem>* CDataManager::GetRefillItemInfo() const
{
    return const_cast<std::vector<stRefillItemInfo::RefillItem>*>(
        &m_limitItemUsage.m_refill18.m_list);
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
bool CDataManager::InitLoginState(ENUM_SERVER_GROUP serverGroup, int channelNo)
{
    // ORIG 0x08360306：get_operation_type 无缓存、多次重调（P48 双臂双调 =
    // 源码多处独立调用）；op!=1 时 result 无赋值（ORIG +0xa7 je 直落 +0xf4
    // 返未初始化槽，忠实保留 UB 形）；内层 channelNo!=1 → result=true。
    bool result;
    // result 声明前置（4.4 槽序随声明序）：hdb 落 -0xc、result 落 -0xd，
    // 与 ORIG 槽布局一致（原序 hdb 落 -0x10/result -0x9，N82 槽平移）。
    MySQL* hdb = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                                   (ENUM_SERVER_GROUP)0);
    if (G_CEnvironment()->get_operation_type() == 0 ||
        G_CEnvironment()->get_operation_type() == 2)
    {
        hdb->set_query(
            "upDate login_account_%d set login_status=0 where m_channel_no=%d",
            serverGroup, channelNo);
        result = hdb->exec(true);
    }
    else if (G_CEnvironment()->get_operation_type() == 1)
    {
        if (G_CEnvironment()->m_serverEnvir.m_channelNo == 1)
        {
            hdb->set_query("upDate login_account_%d set login_status=0",
                           serverGroup);
            result = hdb->exec(true);
        }
        else
        {
            result = true;
        }
    }
    return result;
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
bool CDataManager::init()
{
    // ORIG 版本五元组落在具名区最浅端（槽 -0xf1c/-0xf18/-0xf14/-0xf10/
    // -0xf0c，紧邻临时 arena 基址 -0xf08）；标量局部在本 GCC 下会进顶部
    // 表达式临时槽（实测 -0x5c..-0x6c），只有聚合体（数组）才拿到具名帧槽
    // ⇒ 原函数以 int[5] 形式声明，常数下标折叠后访存形态与标量完全一致。
    // [frame-align] 本侧帧比 ORIG 浅 0x10（0x191c vs 0x192c）；未引用的
    // int[4]（16B）撑帧后帧 0x192c 与 ORIG 全等、ver 五元组槽 -0xf1c..-0xf0c
    // 对齐（P232：-O0 未读写局部照样占 sub；char[16] 实测同形）。
    int _pad[4];
    int ver[5];
    // ORIG 的 5 个函数体作用域 int（按声明序取深→浅槽 -0x3c/-0x38/-0x34/-0x30/
    // -0x2c；两个循环计数器在子块里，故槽号更浅 -0x28/-0x24）。err 为多站点复用
    // 的工作变量（ORIG 在 17 处 `movl $0x0,-0x38` 后接独立赋值+判定两段形）。
    int errDT, err, errPrem, errRO, errMerc;

    // ORIG:每站点独立调用 G_CEnvironment()(无 env 缓存)。

    // [ORIG +0x000c..+0x005d] 遍历 m_serverEnvir.m_priorityScriptDir(+0x2fc)
    // 逐个 SetPriorityBaseDirectory。
    for (unsigned int i = 0; i < G_CEnvironment()->m_serverEnvir.m_priorityScriptDir.size(); ++i)
    {
        SetPriorityBaseDirectory(
            G_CEnvironment()->m_serverEnvir.m_priorityScriptDir[i].c_str());
    }

    // [ORIG +0x005f] m_characNameFilterPass(+0xa8d8) = 0。
    m_characNameFilterPass = false;

    // [+0x0069..+0x00b1] InitRDARScript(基目录 c_str(), m_scriptpacks 字节,
    // GetScriptPacks(), NULL)。
    InitRDARScript(G_CEnvironment()->m_serverEnvir.m_baseScriptDirectory.c_str(),
                   G_CEnvironment()->m_serverEnvir.m_scriptpacks,
                   G_CDataManager()->GetScriptPacks(), (int*)0);

    // [+0x00b3..+0x0176] InitRDARScriptStringManager("n_String.lst", NULL, NULL)
    // —— 失败是唯一的前导致命错误（TRACE 0x1972 / 0x1975）。
    // 两处 TRACE 均为匿名临时对象（ORIG 槽 -0xf08/-0xef8 随站点递增 ⇒ 临时
    // arena 形态，具名对象则反向递增，探针 probe3 实证），故用链式临时写法。
    if (!InitRDARScriptStringManager("n_String.lst", (const char*)0,
                                     (std::vector<ErrorLog>*)0))
    {
        cMyTrace("bool CDataManager::init()", 0x1972, 9, true, true)(
            "\t- InitRDARScriptStringManager Fail");
        return 0;
    }
    cMyTrace("bool CDataManager::init()", 0x1975, 9, true, true)(
        "\t- InitRDARScriptStringManager Success");

    // [+0x0176] InitMapEquipmentAniScript()。
    InitMapEquipmentAniScript();

    // [+0x017b..+0x02a8] ItemInfoMaster：失败/DB 步骤失败均只 TRACE 继续
    // （0x1982/0x1986/0x1989）。
    // ORIG:每站点独立 GetInstanceSyncScript()；分支布局为「失败 TRACE 在前
    // （fall-through）、成功链在后（+0x01e8 起）」——故源码用 if(!ok){TRACE}
    // else{…} 形式，站点顺序才是 0x1982 → 0x1986 → 0x1989（报告 +0x0195 je）。
    if (!sync_script::GetInstanceSyncScript()->LoadItemInfoMasterScript("Etc/ItemTypeMaster.etc"))
    {
        cMyTrace("bool CDataManager::init()", 0x1982, 9, true, true)(
            "\t- Failed to load < ITEM INFO MASTER SCRIPT >");
    }
    else
    {
        if (!sync_script::GetInstanceSyncScript()->truncate_item_info_master_table())
        {
            cMyTrace("bool CDataManager::init()", 0x1986, 9, true, true)(
                "\t- Failed to < TRUNCATE ITEM INFO MASTER TABLE >");
        }
        if (!sync_script::GetInstanceSyncScript()->insert_item_info_master_to_db())
        {
            cMyTrace("bool CDataManager::init()", 0x1989, 9, true, true)(
                "\t- Failed to < INSERT ITEM INFO MASTER TO DB >");
        }
    }

    // [+0x02a8..+0x03xx] OldEquipmentStatInfo：同上 log-only（0x198f/93/96）。
    // 同上：失败 TRACE fall-through 在前（报告 +0x02b9 je）。
    if (!importOldEquipmentStatInfo(""))
    {
        cMyTrace("bool CDataManager::init()", 0x198f, 9, true, true)(
            "\t- Failed to import < OldEquipmentStatInfo.dat >");
    }
    else
    {
        if (!sync_script::GetInstanceSyncScript()->truncate_old_equip_info_table())
        {
            cMyTrace("bool CDataManager::init()", 0x1993, 9, true, true)(
                "\t- Failed to < TRUNCATE OLD EQUIP INFO TABLE >");
        }
        if (!sync_script::GetInstanceSyncScript()->insert_old_equip_info_to_db())
        {
            cMyTrace("bool CDataManager::init()", 0x1996, 9, true, true)(
                "\t- Failed to < INSERT OLD EQUIP INFO TO DB >");
        }
    }

    // ---- 主成功续链开始：任一 FAIL 分支 TRACE 后 return 0 ----

    // ServerParameter.etc（fail TRACE 0x1972 区尾段 0x1a35）。
    if (!importServerParameterScript(&m_serverParameter,
                                     "Etc/ServerParameter.etc"))
    {
        cMyTrace("bool CDataManager::init()", 0x1a35, 9, true, true)(
            "\t- importServerParameterScript Fail");
        return 0;
    }

    // serverType==2 时把 expGroup378 拷入 expGroup368（+0x368/+0x378 各 0x10）。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        memcpy(m_serverParameter.m_expGroup368,
               m_serverParameter.m_expGroup378, 0x10);
    }

    // IngameAdvertisementParameter.etc（fail 0x1a4a）。
    if (!importInGameAdvertisementEtcScript(&m_inGameAd,
                                            "Etc/IngameAdvertisementParameter.etc"))
    {
        cMyTrace("bool CDataManager::init()", 0x1a4a, 9, true, true)(
            "\t- importInGameAdvertisementEtcScript Fail");
        return 0;
    }

    // SeriaRoomDecoEvent.evt（fail 0x1a53）。
    if (!importSeriaRoomDecoEvent(&m_seriaRoomDeco,
                                  "Event/SeriaRoomDecoEvent.evt"))
    {
        cMyTrace("bool CDataManager::init()", 0x1a53, 9, true, true)(
            "\t- SeriaRoomDecoEventScript Fail!");
        return 0;
    }

    // GrowthWeapon.evt（fail "GrownEquipmentScript Fail!"）。
    if (!importGrowthEquipmentScript(&m_growthWeapon, "Event/GrowthWeapon.evt"))
    {
        cMyTrace("bool CDataManager::init()", 0x1a5b, 9, true, true)(
            "\t- GrownEquipmentScript Fail!");
        return 0;
    }

    // CreateCharacterGift.evt。
    if (!importCharacterCreateGiftScript(&m_createChrac,
                                         "Event/CreateCharacterGift.evt"))
    {
        cMyTrace("bool CDataManager::init()", 0x1a63, 9, true, true)(
            "\t- CharacterCreateGiftScript Fail!");
        return 0;
    }

    // JobFatigue.evt。
    if (!importUseJobFatigueGiftScript(&m_useJobFatigue, "Event/JobFatigue.evt"))
    {
        cMyTrace("bool CDataManager::init()", 0x1a6b, 9, true, true)(
            "\t- UseJobFatigueGiftScript Fail!");
        return 0;
    }

    // CreateCharacterGiftCreature.evt。
    if (!importCharacterCreateCreatureGiftScript(
            &m_createCreatureGift, "Event/CreateCharacterGiftCreature.evt"))
    {
        cMyTrace("bool CDataManager::init()", 0x1a73, 9, true, true)(
            "\t- CharacterCreateCreatureGiftScript Fail!");
        return 0;
    }

    // SeriaRoomDecoAnimation.evt。
    if (!importSeriaRoomAnimationDecoEvent(&m_seriaAniDeco,
                                           "Event/SeriaRoomDecoAnimation.evt"))
    {
        cMyTrace("bool CDataManager::init()", 0x1a7b, 9, true, true)(
            "\t- SeriaRoomAnimationDecoEventScript Fail!");
        return 0;
    }

    // EventReward/reward.evt（返回 0=成功）。
    if (m_eventReward.importRewardTable("Event/EventReward/", "reward.evt") != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1a83, 9, true, true)(
            "\t- JumpingCharacterEventScript Fail!");
        return 0;
    }

    // antibot 版本号：读 m_versionBytes 四字节计算后写 m_antibotVersion。
    // ORIG +0x082b..+0x08f2（报告 init.md）逐条转写：5 个 int 局部
    //（ORIG 槽 -0xf1c/-0xf18/-0xf14/-0xf10/-0xf0c，按声明序自深向浅），
    // 每字节读取各自独立调 G_CDataManager()（6 次 call 实证）；
    // hi = byte/100（QI 模式 magic 41：mov $0x29/mul/shr 8/shr 4/movzbl）、
    // lo = byte - hi*100（ORIG `imul $0xffffff9c` + `lea` 实证，非 %100）。
    // 求值序：表达式第 2 组按 (hi*1e4, lo*100) 书写 ⇒ 汇编先 lo→ecx 后
    // hi→edx（右操作数先求值，P23/P24 口径）。
    ver[0] = G_CDataManager()->m_serverParameter.m_versionBytes[0];
    ver[1] = G_CDataManager()->m_serverParameter.m_versionBytes[1];
    ver[2] = G_CDataManager()->m_serverParameter.m_versionBytes[2] / 100;
    ver[3] = G_CDataManager()->m_serverParameter.m_versionBytes[2] - ver[2] * 100;
    ver[4] = G_CDataManager()->m_serverParameter.m_versionBytes[3];
    G_CDataManager()->m_serverParameter.m_antibotVersion =
        ver[0] * 10000000 + ver[1] * 100000 + ver[2] * 10000 + ver[3] * 100 +
        ver[4];
    cMyTrace("bool CDataManager::init()", 0x1aa7, 9, true, true)(
        "\t- Server version for antibot : %d",
        G_CDataManager()->m_serverParameter.m_antibotVersion);

    // ImageCommunication.etc。
    if (!importImageCommunicationInfo("Etc/ImageCommunication.etc",
                                      &m_imageCommData))
    {
        cMyTrace("bool CDataManager::init()", 0x1aad, 9, true, true)(
            "\t- Import ImageCommunicationInfo Failed\n");
        return 0;
    }

    // CheckTraningFirstGive 为假 → TRACE 0x1ab7 后 return 0。
    if (!G_CDataManager()->m_serverParameter.CheckTraningFirstGive())
    {
        cMyTrace("bool CDataManager::init()", 0x1ab7, 9, true, true)(
            "\t- [training first give] error");
        return 0;
    }

    // expertjob.lst（失败 TRACE 0x1abf）。
    if (!initExpertJobScript((char*)"Character/", (char*)"expertjob.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1abf, 9, true, true)(
            "\t- Load expertjob.lst fail");
        return 0;
    }

    // new CExpertJobList(0x48) → m_regeneration.m_expertJobList；init() 失败 TRACE 0x1ac5。
    m_regeneration.m_expertJobList = new CExpertJobList;
    if (!m_regeneration.m_expertJobList->init())
    {
        cMyTrace("bool CDataManager::init()", 0x1ac5, 9, true, true)(
            "\t- Load expertjob fail");
        return 0;
    }

    // TowerList.lst（返回 0=成功）。
    if (importTowerInfo("Etc/", "TowerList.lst", m_deathTowerMap) != 0)
    {
        // ORIG +0x0b7a：puts("importTowerInfo Faile")（PLT 0x0807e570 → puts，
        // GOT 0x0936cf34 实证；原文案拼写即少一个 d），非 cMyTrace。
        puts("importTowerInfo Faile");
        return 0;
    }

    // DeathTower.etc（返回 0=成功）。
    // ORIG +0x0b90：errDT 置 0 → `if ((errDT = importDeathTowerScript(..)) != 0)`
    // （store+cmpl+setne），TRACE 带 errDT。
    errDT = 0;
    if ((errDT = importDeathTowerScript("Etc/DeathTower.etc", m_deathTower)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1ad4, 9, true, true)(
            "\t- importDeathTowerScript Fail(%d)", errDT);
        return 0;
    }

    // RankSystemInfo.etc。
    if (!importRankSystemScript("Etc/RankSystemInfo.etc", &m_rankSystem))
    {
        cMyTrace("bool CDataManager::init()", 0x1ada, 9, true, true)(
            "\t- importRankSystemScript Fail");
        return 0;
    }

    // BloodClearReward.etc（返回 0=成功）。
    err = 0;
    if ((err = importBloodDungeonScript("Etc/BloodClearReward.etc", m_bloodDungeon)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1ae1, 9, true, true)(
            "\t- importBloodDungeonScript Fail(%d)", err);
        return 0;
    }

    // HellParty.etc（全局单例，返回 0=成功）。
    err = 0;
    if ((err = importHellPartyScript( "Etc/HellParty.etc", *reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t()))) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1ae8, 9, true, true)(
            "\t- importHellPartyScript Fail(%d)", err);
        return 0;
    }

    // CharacLinkSystem.etc（返回 0=成功）。
    err = 0;
    if ((err = importCharacLinkSystemScript("Etc/LinkSystem/CharacLinkSystem.etc", m_characLink)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1af3, 9, true, true)(
            "\t- importCharacLinkSystemScript Fail(%d)", err);
        return 0;
    }

    // striker.etc（getInsance 单例存入 m_altarConfig.m_strikerScript）。
    m_altarConfig.m_strikerScript = StrikerScript::getInsance();
    if (!m_altarConfig.m_strikerScript->importScript("Etc/LinkSystem/striker.etc"))
    {
        cMyTrace("bool CDataManager::init()", 0x1afb, 9, true, true)(
            "\t- importStrikerScript Fail");
        return 0;
    }

    // CharacterManage.etc。
    m_altarConfig.m_characManageScript = CharacManageScript::getInsance();
    if (!m_altarConfig.m_characManageScript->importScript(
            "Etc/LinkSystem/CharacterManage.etc"))
    {
        cMyTrace("bool CDataManager::init()", 0x1b02, 9, true, true)(
            "\t- importCharacterManageScript Fail");
        return 0;
    }

    // BlueMarble 三连（Tile→Map→Info），单例存入 m_choiceItem.m_blueMarbleScriptMgr。
    m_choiceItem.m_blueMarbleScriptMgr = BlueMarbleScriptManager::getInsance();
    if (!m_choiceItem.m_blueMarbleScriptMgr->initTileList(
            "Event/BlueMarble/", "BlueMarbleTile.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1b14, 9, true, true)(
            "\t- BlueMarble initTileList Fail\n");
        return 0;
    }
    if (!m_choiceItem.m_blueMarbleScriptMgr->initMapList(
            "Event/BlueMarble/", "BlueMarbleMap.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1b1a, 9, true, true)(
            "\t- BlueMarble initMapList Fail\n");
        return 0;
    }
    if (!m_choiceItem.m_blueMarbleScriptMgr->initBlueMarbleInfo(
            "Event/BlueMarble/", "BlueMarble.blu"))
    {
        cMyTrace("bool CDataManager::init()", 0x1b20, 9, true, true)(
            "\t- BlueMarble initBlueMarbleInfo Fail\n");
        return 0;
    }

    // questshop.etc（ORIG +0x10ba：`movl $0x0,-0x38` 后独立赋值+判定两段形，
    // TRACE 带 err 实参）。err 为函数体作用域变量，此处仅复位。
    err = 0;
    if ((err = importQuestShopScript("Etc/questshop.etc", m_questShop)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1b4f, 9, true, true)(
            "\t- importQuestShopScript Fail(%d)", err);
        return 0;
    }

    // IndependentDrop 三连（lst 加载 bool / 列表加载 / 数据导入返回 0=成功；
    // 后两步 fail 均 TRACE "Load IndependentDrop.lst fail" 0x1b56/0x1b5c）。
    if (!initIndependentDropScript((char*)"Etc/", (char*)"IndependentDrop.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1b56, 9, true, true)(
            "\t- Load IndependentDrop.lst fail");
        return 0;
    }
    if (!m_independentDrop.load_independent_drop_list())
    {
        cMyTrace("bool CDataManager::init()", 0x1b5c, 9, true, true)(
            "\t- Load IndependentDrop.lst fail");
        return 0;
    }
    err = 0;
    if ((err = m_independentDrop.ImportIndependentDrop("Etc/Independent_Drop.etc")) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1b63, 9, true, true)(
            "\t- importIndependent_DropScript Fail(%d)", err);
        return 0;
    }

    // Event 脚本簇（SeriaBlessing/LevelUpReward/GrowthCapsule/NewAccountLevelUp/
    // ReturnUserReward/Bingo/BroadCastItem，全部返回 0=成功）。
    err = 0;
    if ((err = ImportSeriaBlessingScript("Event/SeriaBlessing.evt", m_seriaBlessing)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1b83, 9, true, true)(
            "\t- ImportSeriaBlessingScript Fail(%d)", err);
        return 0;
    }
    err = 0;
    if ((err = ImportLevelUpRewardItemScript("Event/LevelUpRewardItem.evt", m_levelUpReward)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1b8a, 9, true, true)(
            "\t- ImportLevelUpRewardItemScript Fail(%d)", err);
        return 0;
    }
    // ORIG +0x13c0..+0x1405：int err = 0；err 由条件内赋值（+0x13df `mov
    // %eax,-0x38` + `setne` 形态，probe7 v7_c 实证），失败走
    // printf("ImportGrowthCapsuleScript Fail(%d)\n", err)（PLT 0x0807db60）。
    err = 0;
    if ((err = ImportGrowthCapsuleScript("Event/GrowthCapsule.evt", m_growthCapsule)) != 0)
    {
        printf("ImportGrowthCapsuleScript Fail(%d)\n", err);
        return 0;
    }
    err = 0;
    if ((err = ImportNewAccountLeventUpToJobScript("Event/NewAccountLevelUpToJob.evt", m_newAccountLevelUp)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1ba7, 9, true, true)(
            "\t- NewAccountLevelUpToJob StackableReward Fail(%d)", err);
        return 0;
    }
    err = 0;
    if ((err = ImportReturnUserRewardScript("Event/ReturnUserReward.evt", m_returnUserReward)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1bb0, 9, true, true)(
            "\t- ReturnUserReward Fail(%d)", err);
        return 0;
    }
    err = 0;
    if ((err = ImportBingoScript("Event/BingoEvent.evt", m_bingo)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1bbb, 9, true, true)(
            "\t- ImportBingoScript Fail(%d)", err);
        return 0;
    }
    err = 0;
    if ((err = ImportBroadCastItemScript("Etc/BroadCastItem.etc", m_broadCastItem)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1bcf, 9, true, true)(
            "\t- ImportBroadCastItemScript Fail(%d)", err);
        return 0;
    }

    // SecurityReward（bool 语义）。
    if (!ImportSecurityReward(&m_securityReward))
    {
        cMyTrace("bool CDataManager::init()", 0x1bd7, 9, true, true)(
            "\t- ImportSecurityReward Fail");
        return 0;
    }

    // ItemMakingSkill.etc（返回 0=成功）。
    err = 0;
    if ((err = ImportItemMakingSkill("Etc/ItemMakingSkill.etc", m_itemMakingSkill)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1be0, 9, true, true)(
            "\t- importItemMakingSkillScript Fail(%d)", err);
        return 0;
    }

    // UltimateDungeonList.etc。
    if (!importUltimateDungeonList("Etc/UltimateDungeonList.etc",
                                   m_ultimateDungeonList))
    {
        cMyTrace("bool CDataManager::init()", 0x1be8, 9, true, true)(
            "\t- import UltimateDungeonList Script Fail(%d)");
        return 0;
    }

    // mapping equip info：truncate 成功才继续（失败无 TRACE ret0）；
    // 两张表 false/true 导入（fail TRACE "Equipment Script mapping table
    // load fail" 0x1c0c/0x1c12）；insert 用第二张表（ORIG +0x18f6 取 +0xa940）。
    // 帧差备忘（P31 类）：ORIG 站点 0x1be8→0x1c06 之间槽号跨 0x20（-0xc38 在
    // 全函数无任何引用 ⇒ 原码此处有一个未引用 16 字节局部）。实测：无论以
    // {} 子作用域还是函数体作用域补一个 POD dummy，都只使帧 +0x10 而不产生
    // arena 内 gap ⇒ 该槽来自带非平凡析构的对象（本 TU 无法以合法 C++ 复现），
    // 归入结构性差异，不强凑。
    if (!sync_script::GetInstanceSyncScript()->truncate_mapping_equip_info_table())
    {
        cMyTrace("bool CDataManager::init()", 0x1c06, 9, true, true)(
            "\t- Fail truncate_mapping_equip_info_table");
        return 0;
    }
    if (!importExperienceItemMapInfo(m_mapA928, false))
    {
        cMyTrace("bool CDataManager::init()", 0x1c0c, 9, true, true)(
            "\t- Equipment Script mapping table load fail");
        return 0;
    }
    if (!importExperienceItemMapInfo(m_mapA940, true))
    {
        cMyTrace("bool CDataManager::init()", 0x1c12, 9, true, true)(
            "\t- Equipment Script mapping table load fail");
        return 0;
    }
    if (!sync_script::GetInstanceSyncScript()->insert_mapping_equip_info_to_db(m_mapA940))
    {
        cMyTrace("bool CDataManager::init()", 0x1c18, 9, true, true)(
            "\t- Fail insert_mapping_equip_info_table");
        return 0;
    }

    // VillageAttacked.etc（返回 0=成功）。
    err = 0;
    if ((err = importVillageAttackedScript("Etc/VillageAttacked.etc", m_villageAttacked)) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1c20, 9, true, true)(
            "\t- importVillageAttackedScript Fail(%d)", err);
        return 0;
    }

    // ActionPoint 两份（普通/GM；GetActionPointEtcParameter(bool) W 0x08122ace）。
    // ORIG +0x19f4..+0x1a2a：每份各起 err 复位（复用同一临时槽），判定为
    // store+cmpl+setne 两段形态；访问器结果不经局部变量、直接由 %eax 落到
    // 实参槽（不留具名指针）。
    err = 0;
    if ((err = APSystem::CActionPointEtcParameter::importActionPointSystemEtcScript( "Etc/Chn_ActionPointSystem.etc", *GetActionPointEtcParameter(false))) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1c3a, 9, true, true)(
            "\t- importActionPointSystemEtcScript Chn_ActionPointSystem.etc Fail(%d)",
            err);
        return 0;
    }
    err = 0;
    if ((err = APSystem::CActionPointEtcParameter::importActionPointSystemEtcScript( "Etc/Chn_ActionPointSystem_GM.etc", *GetActionPointEtcParameter(true))) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1c41, 9, true, true)(
            "\t- importActionPointSystemEtcScript Chn_ActionPointSystemGM.etc Fail(%d)",
            err);
        return 0;
    }

    // action point desc DB 同步（无条件）。
    sync_script::GetInstanceSyncScript()->truncate_charac_action_point_desc();
    sync_script::GetInstanceSyncScript()->insert_charac_action_point_desc();

    // Chn_ServerCheckSchedule.etc → vector<_DailySchedule> → AddDailSchedule。
    // ORIG 清理梯 +0x6e88：schedules 为函数体作用域对象，析构在函数末尾统一
    // 执行（此后所有 return 0 都经其 dtor），故此处不加嵌套块。
    // err 复位（ORIG +0x1b38 `movl $0x0,-0x38(%ebp)` 先于 vector 构造，
    // AdvanceAltar 四段复用同一变量）。
    err = 0;
    std::vector<_DailySchedule> schedules;
    // [b28d-solo-CDataManager 真语义缺口修复] ORIG 0x08359e95-0x08359ebb：
    // err(-0x38) = importDailyScheduleEtcScript(...)；err!=0 时 **printf**
    // ("importDDayEventEtcScript Fail(%d)\n", err) 再 ret0（旧码无 printf，
    // p167 MISSING printf ×1）；且非空判定为 `size() != 0`（ORIG
    // 0x08359ec9 调 vector<_DailySchedule>::size，旧码 empty() 为 EXTRA）。
    if ((err = importDailyScheduleEtcScript(
             "Etc/Chn_ServerCheckSchedule.etc", schedules)) != 0)
    {
        printf("importDDayEventEtcScript Fail(%d)\n", err);
        return 0;   // ORIG 清理链经 vector dtor 后 ret0（+0x6e88 区）
    }
    if (schedules.size() != 0)
    {
        GlobalData::s_DailyScheduleManager->AddDailSchedule(schedules);
    }

    // AdvanceAltar 三件套 + TimeLine：ORIG +0x1bc7..+0x1ca7 四处共用一个
    // int err（-0x38 槽，`cmpl $0,-0x38` 直接分支、无 setne ⇒ 形如
    // `err = f(); if (err != 0)`），失败均 printf("<fn> Fail(%d)\n", err) 后
    // ret0（PLT 0x0807db60 = printf）。
    // [b28d-orig-int] ORIG 四站点为 `store; cmpl $0; je` 两段形（无 setne）：
    // `err = f(); if (err != 0)`，与 DailySchedule 站点（单表达式 setne 形）相反。
    err = loadAdvanceAltarTimeLineParameter(m_stageTimeLine);
    if (err != 0)
    {
        printf("loadAdvanceAltarTimeLineParameter Fail(%d)\n", err);
        return 0;
    }
    err = m_altarShop.importAdvanceAltarShopParameter( "Etc/AdvanceAltar/AdvanceAltarShop.etc");
    if (err != 0)
    {
        printf("importAdvanceAltarShopParameter Fail(%d)\n", err);
        return 0;
    }
    err = m_altarReward.importAchievementRewardParameter( "Etc/AdvanceAltar/AdvanceAltarAchievementReward.etc");
    if (err != 0)
    {
        printf("importAchievementRewardParameter Fail(%d)\n", err);
        return 0;
    }
    err = m_altarConfig.importConfigParameter( "Etc/AdvanceAltar/AdvanceAltarConfig.etc");
    if (err != 0)
    {
        printf("importConfigParameter Fail(%d)\n", err);
        return 0;
    }

    sync_script::GetInstanceSyncScript()->truncate_charac_advance_altar_item_desc();
    sync_script::GetInstanceSyncScript()->insert_charac_advance_altar_item_desc();

    // pvpChannelList.etc（live_server_info 宿主）。
    if (!importPVPChannelList((char*)"Etc/pvpChannelList.etc",
                              &m_liveServerInfo, 0))
    {
        cMyTrace("bool CDataManager::init()", 0x1cd5, 9, true, true)(
            "\t- importPVPChannelList Fail()");
        return 0;
    }

    // Guild/Power/SecretShop/AccountCargo/TitleBook 参数链。
    if (!importGuildParameterScript(&m_guildParameter, "Etc/Guild.etc"))
    {
        cMyTrace("bool CDataManager::init()", 0x1ce2, 9, true, true)(
            "\t- importGuildParameterScript Fail");
        return 0;
    }
    if (!importPowerParameterScript(&m_powerParameter, "Etc/Power.etc"))
    {
        cMyTrace("bool CDataManager::init()", 0x1ce9, 9, true, true)(
            "\t- importPowerParameterScript Fail");
        return 0;
    }
    if (!importSecretShopScript(&m_secretShop, "Etc/SecretShop.etc"))
    {
        cMyTrace("bool CDataManager::init()", 0x1d0f, 9, true, true)(
            "\t- importSecretShopScript Fail");
        return 0;
    }
    if (!importAccountCargoScript(&m_accountCargo, "Etc/AccountCargo.etc"))
    {
        cMyTrace("bool CDataManager::init()", 0x1d16, 9, true, true)(
            "\t- importAccountCargoScript Fail");
        return 0;
    }
    if (!importTitleBookEtcScript("Etc/TitleBook.etc", &m_titleBook))
    {
        cMyTrace("bool CDataManager::init()", 0x1d1e, 9, true, true)(
            "\t- importTitleBookEtcScript Fail");
        return 0;
    }

    // EventCharacter 两步。
    if (!initEventCharacterScript((char*)"Event/EventCharacter/",
                                  (char*)"EventCharacter.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1d38, 9, true, true)(
            "\t- Load EventCharacter.lst fail");
        return 0;
    }
    if (!m_eventCharacter.load_event_character_list())
    {
        cMyTrace("bool CDataManager::init()", 0x1d3e, 9, true, true)(
            "\t- Load EventCharacter.lst fail");
        return 0;
    }

    // Quest 参数链。
    if (!importQuestParameterScript(&m_questParameter,
                                    "n_Quest/questParameter.etc"))
    {
        cMyTrace("bool CDataManager::init()", 0x1d57, 9, true, true)(
            "\t- importQuestParameterScript Fail");
        return 0;
    }
    if (!importTrainingQuestScript(&m_trainingQuest,
                                   "n_Quest/TrainingQuest.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1d61, 9, true, true)(
            "\t- importTrainingQuestScript Fail");
        return 0;
    }

    // Collect Dungeon Statistic 开关（非 0 才开；log-only TRACE 0x1d84）。
    if (m_serverParameter.CheckCollectDungeonStatisticServer(
            G_CEnvironment()->m_serverEnvir.m_serverGroup, G_CEnvironment()->m_serverEnvir.m_channelNo))
    {
        SetCollectDungeonStatistic(true);
        cMyTrace("bool CDataManager::init()", 0x1d84, 9, true, true)(
            "\t- Collect Dungeon Data Statistic start!");
    }

    // Assault channel 开启。
    pvp_assault::GetInstanceAssaultMgr()->SetAssaultChannel(true);

    // 名字过滤两件套。
    if (!importSlangName((char*)"Etc/slangname.etc", &m_slangName))
    {
        cMyTrace("bool CDataManager::init()", 0x1d8b, 9, true, true)(
            "\t- slangname.etc file load failed");
        return 0;
    }
    if (!importInvalidName((char*)"Etc/invalidName.etc", &m_invalidChar))
    {
        cMyTrace("bool CDataManager::init()", 0x1d90, 9, true, true)(
            "\t- invalidName.etc file load failed");
        return 0;
    }

    // new SlangFilter(0x80) → m_invalidChar.m_slangFilter（destroy 经 vtable D0）。
    m_invalidChar.m_slangFilter = new SlangFilter;

    // FairPVPEquipment.etc → m_stackableMap。
    if (!importFairPVPEquipment((char*)"Etc/FairPVPEquipment.etc",
                                &m_stackableMap))
    {
        cMyTrace("bool CDataManager::init()", 0x1d99, 9, true, true)(
            "\t- FairPVPEquipment.etc file load failed");
        return 0;
    }

    // GrowthPower / BoosterGage。
    if (!importGrowthPower((char*)"Etc/GrowthPowerRewardBuff.etc",
                           &m_growthPower))
    {
        cMyTrace("bool CDataManager::init()", 0x1da1, 9, true, true)(
            "\t- GrowthPowerRewardBuff.etc file load failed");
        return 0;
    }
    if (!importBoosterGage((char*)"Etc/BoosterGage.etc", &m_boosterGage))
    {
        cMyTrace("bool CDataManager::init()", 0x1dac, 9, true, true)(
            "\t- BoosterGage.etc file load failed");
        return 0;
    }

    // AICharacter。
    if (!initAICharacterScript("AICharacter/", "AICharacter.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1db9, 9, true, true)(
            "\t- Load AICharacter.lst fail");
        return 0;
    }
    m_aiCharacterList = new CAICharacterList;
    if (!m_aiCharacterList->init())
    {
        cMyTrace("bool CDataManager::init()", 0x1dbf, 9, true, true)(
            "\t- Load AI Character fail");
        return 0;
    }

    // Light Server 提示（log-only TRACE 0x1dc5 区）。
    if (m_serverParameter.CheckLightServer(G_CEnvironment()->m_serverEnvir.m_serverGroup))
    {
        cMyTrace("bool CDataManager::init()", 0x1dc5, 9, true, true)(
            "\t- Light Server Mode start!");
    }

    // WorldDrop 两张表（无条件导入）。
    ImportWorldDrop(m_worldDrop1, (char*)"Etc/WorldDrop.etc");
    ImportWorldDrop(m_worldDrop2, (char*)"Etc/WorldDropWarArea.etc");

    // PremiumList_New.etc（ORIG +0x2629 区：errPrem 置 0 → `if ((errPrem = ...) != 0)`，
    // 槽 -0x34；TRACE 带行码）。
    errPrem = 0;
    if ((errPrem = WongWork::CUserPremium::LoadPremiumInfo(
             "Etc/PremiumList_New.etc")) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x1e0f, 9, true, true)(
            "\t- LoadPremiumInfo Fail(Line: %d)", errPrem);
        return 0;
    }

    // 非 light server 必须成功加载 WarArea.etc（fail TRACE 0x1e18 ret0）。
    // [b28d-orig-form] ORIG 报告 idx 1930-1940：两独立 `xor $1; test %al;
    // je 同一续链目标`（&& 组合在 4.4 会物化 `mov $1/$0` 尾块，EXTRA 5 行）
    // ⇒ 原码为嵌套形。
    if (!IsLightServer())
    {
        if (!ImportWarAreaCounter(&m_warAreaCounter))
        {
            cMyTrace("bool CDataManager::init()", 0x1e18, 9, true, true)(
                "\t- WarArea.etc file load failed");
            return 0;
        }
    }

    // ---- 大类脚本链（Character→Monster→PassiveObject→Equipment→Stackable→
    //      Map→Dungeon），任一失败 TRACE ret0 ----
    if (!InitCharacterScript("Character/", "Character.lst", "ExpTable.tbl",
                             (const char*)0))
    {
        cMyTrace("bool CDataManager::init()", 0x1e23, 9, true, true)(
            "\t- Load Character.lst fail");
        return 0;
    }
    if (!InitMonsterScript("Monster/", "Monster.lst",
                           "MonsterApcDifficultyBonus.tbl"))
    {
        cMyTrace("bool CDataManager::init()", 0x1e28, 9, true, true)(
            "\t- Load Monster.lst fail");
        return 0;
    }
    if (!InitPassiveObjectScript("PassiveObject/", "PassiveObject.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1e2d, 9, true, true)(
            "\t- Load PassiveObject.lst fail");
        return 0;
    }
    if (!InitEquipmentScript("equipment/", "equipment.lst", "pricetable.tbl"))
    {
        cMyTrace("bool CDataManager::init()", 0x1e33, 9, true, true)(
            "\t- Load equipment.lst fail");
        return 0;
    }
    if (!InitStackableScript("stackable/", "stackable.lst", "pricetable.tbl"))
    {
        // ORIG +0x2928：puts("Load stackable.lst fail")，非 cMyTrace。
        puts("Load stackable.lst fail");
        return 0;
    }
    if (!InitMapScript("Map/", "Map.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1e48, 9, true, true)(
            "\t- Load Map.lst fail\n");
        return 0;
    }
    if (!InitDungeonScript((char*)"Dungeon/", (char*)"Dungeon.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1e4d, 9, true, true)(
            "\t- Load Dungeon.lst fail\n");
        return 0;
    }

    // QuickParty 奖励管理器 init。
    // ORIG +0x2a35..+0x2a41：失败无 TRACE，直接 ret0。
    if (!G_CGameManager()->GetQuickPartyRewardManager()->init())
    {
        return 0;
    }

    // Quest/Skill。
    if (!initQuestScript("n_Quest/", "Quest.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1e5c, 9, true, true)(
            "\t- Load Quest.lst fail\n");
        return 0;
    }
    if (!InitSkillScript("Skill/", "SkillList.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1e61, 9, true, true)(
            "\t- Load SkillList.lst fail\n");
        return 0;
    }

    // ItemShop：new(0x24) → m_itemShop；ItemShop.lst + loadItemShopFiles。
    m_itemShop = new CItemShop;
    if (!initItemShopScript((char*)"ItemShop/", (char*)"ItemShop.lst"))
    {
        cMyTrace("bool CDataManager::init()", 0x1e68, 9, true, true)(
            "\t- Load ItemShop.lst fail\n");
        return 0;
    }
    if (!m_itemShop->loadItemShopFiles())
    {
        cMyTrace("bool CDataManager::init()", 0x1e6d, 9, true, true)(
            "\t- Load ItemShop file fail\n");
        return 0;
    }

    // PvP Mission：initPvpMissionSystem + new(0x54) + Load_list + Load_EtcParameter。
    // ORIG +0x2c55..+0x2c61：失败无 TRACE，直接 ret0。
    if (!initPvpMissionSystem((char*)"PVP_Mission/", (char*)"Mission.lst"))
    {
        return 0;
    }
    m_pvpMissionSystem = new PvP_MissionSystem;
    if (!m_pvpMissionSystem->Load_list())
    {
        cMyTrace("bool CDataManager::init()", 0x1e79, 9, true, true)(
            "\t- Mission Script file fail\n");
        return 0;
    }
    if (!m_pvpMissionSystem->Load_EtcParameter())
    {
        cMyTrace("bool CDataManager::init()", 0x1e7e, 9, true, true)(
            "\t- Mission Parameter Script file fail\n");
        return 0;
    }

    // CashShop 文件名四连（+0x8cb8 / +0xa8c0..0xa8cc）。
    m_scriptPackPath = "Etc/NewCashShop.etc";
    m_packName1 = "CashShop/NewArad_CashShop.shp";
    m_packName2 = "CashShop/Sudden_Shop_1.shp";
    m_packName3 = "CashShop/Sudden_Shop_2.shp";
    m_packName4 = "CashShop/Sudden_Shop_3.shp";

    // Money Limit per Level（fail 仅 TRACE 0x1eb7 继续）。
    {
        cMyTrace("bool CDataManager::init()", 0x1eb4, 9, true, false)(
            "\t- Loading Money Limit per Level ");
        if (!importGoldLimitByLevel(
                reinterpret_cast<unsigned int*>(m_moneyLimit)))
        {
            cMyTrace("bool CDataManager::init()", 0x1eb7, 9, true, true)(
                "\t- Loading Money Limit per Level Fail");
        }
    }

    // Compound Item（bindsphere.etc；失败 TRACE 0x1ebf 后 ret0）。
    cMyTrace("bool CDataManager::init()", 0x1eb9, 9, false, true)("Success");
    {
        cMyTrace("bool CDataManager::init()", 0x1ebb, 9, true, false)(
            "\t- Loading Compound Item ");
        // ORIG +0x2f6a..+0x2f95：new 结果直写全局后再读回作 this，
        // 不留具名局部（不留额外栈槽）。
        GlobalData::s_pCompoundBindCube =
            new ExtreamDungeon::CCompound_ExtreamDun_Item;
        if ((reinterpret_cast<ExtreamDungeon::CCompound_ExtreamDun_Item*>(
                 GlobalData::s_pCompoundBindCube)
                 ->LoadScript("Etc/", "bindsphere.etc")) != 0)
        {
            cMyTrace("bool CDataManager::init()", 0x1ebf, 9, true, true)(
                "\t- Fail load Compound Item Bind Cube Script File");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x1ec2, 9, false, true)("Success");
    }

    // ARAD Event Upgrade Jar（失败 TRACE 0x1ec9 ret0）。
    {
        cMyTrace("bool CDataManager::init()", 0x1ec5, 9, true, false)(
            "\t- Loading ARAD - Event Upgrade Jar ");
        // ORIG +0x30b4..+0x30df：`test %eax,%eax; sete %al` ⇒ 判定为
        // `== 0`（LoadScript 返回真=成功），非 `!= 0`。
        GlobalData::s_pEvent_UpGrade_Jar = new Event_Upgrade_Jar;
        if (GlobalData::s_pEvent_UpGrade_Jar->LoadScript(
                "", "Etc/EventUpgradeJar.etc") == 0)
        {
            cMyTrace("bool CDataManager::init()", 0x1ec9, 9, true, true)(
                "\t- Fail load Event.Upgrad.Jar Script File");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x1ecc, 9, false, true)("Success");
    }

    // Compound Avatar（nothrow new(0x88)；失败 ret0）。
    {
        cMyTrace("bool CDataManager::init()", 0x1ed9, 9, true, false)(
            "\t- Loading Compound Avatar ");
        // ORIG +0x321a..+0x3241：同样先写全局再读回。
        // [b28d-orig-nullfree] ORIG 报告 idx 2493-2503 无判空短路：写全局→读回→
        // 直接 LoadScript→`xor $1`（bool）。旧 `== 0 ||` 守卫系我方发明（EXTRA
        // test/je ×2）。
        GlobalData::s_pCompoundAvatar =
            new (std::nothrow) WongWork::CCompoundAvatar;
        if (!(reinterpret_cast<WongWork::CCompoundAvatar*>(
                  GlobalData::s_pCompoundAvatar)
                  ->LoadScript("Etc/", "CompoundAvatar.etc")))
        {
            cMyTrace("bool CDataManager::init()", 0x1ee2, 9, true, true)(
                "\t- Loading Compound Avatar Fail\n");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x1ee5, 9, false, true)("Success");
    }

    // VendingMachine.lst（无条件）。
    importVendingMachineInfo("Etc/", "VendingMachine.lst", m_vendingMachine);

    // WorldMap/StageMap。
    if (!initWorldMapScript((char*)"WorldMap/", (char*)"WorldMap.lst"))
    {
        // [b28d-solo-CDataManager 真语义缺口修复] ORIG 0x0835b652-59：
        // `movl $0x8c31c07,(%esp); call puts`（串 "Load WorldMap.lst fail"）
        // ⇒ 失败路径有 puts（旧码缺，p167 MISSING puts ×1）。
        puts("Load WorldMap.lst fail");
        return 0;
    }
    initStageMapScript((char*)"StageMap/", (char*)"StageMap.lst");

    // Dungeons：new CDungeonList(0x14) + dimension 区清零 + load_list。
    {
        cMyTrace("bool CDataManager::init()", 0x1ef3, 9, true, false)(
            "\t- Loading Dungeons ");
        m_dungeonList = new CDungeonList;
        memset(m_dimensionInout, 0, 6);
        memset(m_originalDimensionInout, 0, 6);
        memset(m_limitInoutCount, 0, 3);
        if (!m_dungeonList->load_list())
        {
            cMyTrace("bool CDataManager::init()", 0x1f05, 9, true, true)(
                "\t- Loading Dungeons Fail");
            return 0;
        }
    }

    // Tower Of Despair（失败 TRACE 0x1f0f 后 ret0）。
    {
        cMyTrace("bool CDataManager::init()", 0x1f08, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x1f0b, 9, true, false)(
            "\t- Loading Tower Of Despair System ");
        // ORIG +0x355d..+0x3588：写全局→读回→`!= 0` 判定。
        GlobalData::s_pTowerOfDespairMgr = new TowerOfDespairMgr;
        if ((reinterpret_cast<TowerOfDespairMgr*>(
                 GlobalData::s_pTowerOfDespairMgr)
                 ->LoadScript("Etc/", "TowerOfDespair.etc")) != 0)
        {
            cMyTrace("bool CDataManager::init()", 0x1f0f, 9, true, true)(
                "\t- Fail load TowerOfDespair System Script");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x1f12, 9, false, true)("Success");
    }

    // Chatting Emoticon：new(0x48) + lst + Load_list。
    {
        cMyTrace("bool CDataManager::init()", 0x1f16, 9, true, false)(
            "\t- Loading Chatting Emoticon ");
        m_emoticonList = new CChattingEmoticonList;
        if (!initChattingEmoticonScriptList((char*)"ChatEmoticon/",
                                            (char*)"ChatEmoticon.lst"))
        {
            cMyTrace("bool CDataManager::init()", 0x1f1b, 9, true, true)(
                "\t- Emoticon List file error");
            return 0;
        }
        if (!m_emoticonList->Load_list())
        {
            cMyTrace("bool CDataManager::init()", 0x1f20, 9, true, true)(
                "\t- Emoticon Script file fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x1f23, 9, false, true)("Success");
    }

    // PvP 地图基础数据 + Maps。
    // pvpMaps 亦为函数体作用域对象（ORIG 清理梯 +0x6e5a：先 pvpMaps 后
    // schedules），不套嵌套块。
    std::vector<PVPMAP_DATA> pvpMaps;
    if (!loadPvpMapBasicDataScript("Etc/pvpmapbasicdata.etc", &pvpMaps))
    {
        cMyTrace("bool CDataManager::init()", 0x1f29, 9, true, true)(
            "Load Etc/pvpmapbasicdata.etc fail");
        return 0;
    }
    cMyTrace("bool CDataManager::init()", 0x1f2d, 9, true, false)(
        "\t- Loading Maps ");
    m_mapList = new CMapList;
    if (!load_map_files(pvpMaps))
    {
        cMyTrace("bool CDataManager::init()", 0x1f33, 9, true, true)(
            "\t- Loading Maps Fail");
        return 0;
    }

    // Monsters。
    cMyTrace("bool CDataManager::init()", 0x1f36, 9, false, true)("Success");
    cMyTrace("bool CDataManager::init()", 0x1f38, 9, true, false)(
        "\t- Loading Monsters ");
    m_monsterManager = new CMonsterManager;
    if (!m_monsterManager->load_monsters())
    {
        cMyTrace("bool CDataManager::init()", 0x1f3e, 9, true, true)(
            "\t- Loading Monsters Fail");
        return 0;
    }

    // IndependentDrop 缓存清理（无条件）+ Skills。
    cMyTrace("bool CDataManager::init()", 0x1f41, 9, false, true)("Success");
    m_independentDrop.clear();
    {
        cMyTrace("bool CDataManager::init()", 0x1f47, 9, true, false)(
            "\t- Loading Skills ");
        m_skillList = new CSkillList;
        if (!m_skillList->init())
        {
            cMyTrace("bool CDataManager::init()", 0x1f4d, 9, true, true)(
                "\t- Loading Skills Fail");
            return 0;
        }
    }

    // Creature Damage + ActiveStatus（SDCActiveStatusSyncServer 栈局部）。
    cMyTrace("bool CDataManager::init()", 0x1f50, 9, false, true)("Success");
    cMyTrace("bool CDataManager::init()", 0x1f52, 9, true, false)(
        "\t- Loading Creatrue Damage ");
    if (!prepareServerDelayCheckDamage())
    {
        cMyTrace("bool CDataManager::init()", 0x1f55, 9, true, true)(
            "\t- Loading Creatrue Damage Fail");
        return 0;
    }
    cMyTrace("bool CDataManager::init()", 0x1f58, 9, false, true)("Success");

    // activeStatus 同样是函数体作用域对象（ORIG 清理梯 +0x6e32 段；槽
    // -0xf35 仅 1 字节 ⇒ 空类）。
    SDCActiveStatusSyncServer activeStatus;
    cMyTrace("bool CDataManager::init()", 0x1f5c, 9, true, false)(
        "\t- Loading ActiveStatus Damage ");
    if (!activeStatus.loadActiveStatusScript())
    {
        cMyTrace("bool CDataManager::init()", 0x1f5f, 9, true, true)(
            "\t- Loading ActiveStatus Damage Fail");
        return 0;
    }
    cMyTrace("bool CDataManager::init()", 0x1f62, 9, false, true)("Success");

    // ItemGroupName.tbl（返回非 0 = 失败 ret0）+ Items。
    // —— 以下局部按 ORIG 槽序声明（-0xf54 数组 → -0xf58/-0xf5c/-0xf60 三个
    // 迭代器 → -0x107c roulette → -0x1910 STEquipmentScript）：ORIG 清理梯
    // 证明 roulette 是函数体作用域对象，而数组/迭代器槽位比 roulette 更浅
    // ⇒ 源码在 roulette 之前声明、之后才赋值（老式「顶部声明」风格）。
    const char* randomOptionFiles[7];
    // ORIG +0x3100：结果落 errDT 槽（-0x3c，与 DeathTower 同一变量，本处不再
    // 置 0）→ `cmpl $0,-0x3c; setne %al; test %al,%al`。
    if ((errDT = importItmeGroupTable("Etc/", "ItemGroupName.tbl",
                                      getItemGroupNameTable())) != 0)
    {
        return 0;
    }
    {
        cMyTrace("bool CDataManager::init()", 0x1f69, 9, true, false)(
            "\t- Loading Items ");
        m_itemList = new CItemList;
        if (!m_itemList->init())
        {
            cMyTrace("bool CDataManager::init()", 0x1f6f, 9, true, true)(
                "\t- Loading items Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x1f72, 9, false, true)("Success");
    }

    // Avatar Roulette（栈局部 roulette → import → new Server → Init →
    // InitAddCharacSkill，失败 LogManager ret0）。
    // roulette 为函数体作用域对象（ORIG 清理梯顶段 +0x6e0a），不套嵌套块。
    cMyTrace("bool CDataManager::init()", 0x1f8a, 9, true, false)(
        "\t- Loading Avatar Roulette ");
    AvatarRoulette roulette;
    if (!importAvatarRouletteList((char*)"Etc/avatar_roulette_jar.etc",
                                  roulette))
    {
        cMyTrace("bool CDataManager::init()", 0x1f90, 9, true, true)(
            "\t- Loading Avatar Roulette Fail");
        return 0;
    }
    GlobalData::s_pAvatarRoulette = new AvatarRouletteServer;
    if (GlobalData::s_pAvatarRoulette == 0)
    {
        return 0;
    }
    if (!(reinterpret_cast<AvatarRouletteServer*>(GlobalData::s_pAvatarRoulette)
              ->Init(&roulette)))
    {
        cMyTrace("bool CDataManager::init()", 0x1f9a, 9, true, true)(
            "\t- Initalizing Avatar Roulette Fail");
        return 0;
    }
    if (!(reinterpret_cast<AvatarRouletteServer*>(GlobalData::s_pAvatarRoulette)
              ->InitAddCharacSkill()))
    {
        LogManager::logFormat(
            1, "data_manager.cpp", "int CDataManager::init()", 0x1fa1,
            "[AVATAR ROULETTE] initalizing(InitAddCharacSkill) failed.");
        return 0;
    }
    cMyTrace("bool CDataManager::init()", 0x1fa6, 9, false, true)("Success");

    // RandomOption 七文件循环（任一 err!=0 → TRACE ret0）。
    // ORIG +0x3254..+0x3261：err 先置 0，再 7 条字符串指针存入局部数组
    //（槽 -0xf54..-0xf3c），然后才打 “Loading RandomOption”。
    errRO = 0;
    randomOptionFiles[0] = "Etc/RandomOption/OptionGrouping.etc";
    randomOptionFiles[1] = "Etc/RandomOption/OptionGroupSelection.etc";
    randomOptionFiles[2] = "Etc/RandomOption/OptionNumbering.etc";
    randomOptionFiles[3] = "Etc/RandomOption/OptionQuantity.etc";
    randomOptionFiles[4] = "Etc/RandomOption/PartSelection.etc";
    randomOptionFiles[5] = "Etc/RandomOption/RandomizedOptionOverall1.etc";
    randomOptionFiles[6] = "Etc/RandomOption/RandomizedOptionOverall2.etc";
    cMyTrace("bool CDataManager::init()", 0x1fb6, 9, true, false)(
        "\t- Loading RandomOption ");
    // [P310] ORIG 循环尾检 = `mov -S,%eax; cmp $0x6,%eax; setbe; test; jne`
    //（idx 3308-3313）——局部须 int + 显式 (unsigned int) 比较才发两段寄存器形
    //（4.4 对 unsigned 局部恒折 cmpl 直比；探针 /tmp/probe_loop.cpp loop2）。
    for (int i = 0; (unsigned int)i <= 6; ++i)
    {
        if ((errRO = importRandomOptionScript(m_randomOption, randomOptionFiles[i])) != 0)
        {
            cMyTrace("bool CDataManager::init()", 0x1fbb, 9, true, true)(
                "\t- Fail load RandomOption Script File name(%s), "
                "error_line(%d)\n", randomOptionFiles[i], errRO);
            return 0;
        }
    }
    cMyTrace("bool CDataManager::init()", 0x1fbf, 9, false, true)("Success");

    // initRandomOptionItem + random_option_ref DB 同步。
    if (!initRandomOptionItem())
    {
        cMyTrace("bool CDataManager::init()", 0x1fc3, 9, true, true)(
            "Fail load initRandomOptionItem");
        return 0;
    }
    if (!sync_script::GetInstanceSyncScript()->truncate_random_option_ref())
    {
        cMyTrace("bool CDataManager::init()", 0x1fc9, 9, true, true)(
            "\t- Fail truncate_random_option_ref");
        return 0;
    }
    if (!sync_script::GetInstanceSyncScript()->insert_random_option_ref())
    {
        cMyTrace("bool CDataManager::init()", 0x1fcf, 9, true, true)(
            "\t- Fail truncate_random_option_ref");
        return 0;
    }
    if (m_regeneration._0_Load_RegenerationRandomOptionItem() != 0)
    {
        return 0;
    }

    // WorldMapList + Quests + Characters + Disjoints。
    if (!m_worldMapList.load_list())
    {
        cMyTrace("bool CDataManager::init()", 0x1fe1, 9, true, true)(
            "\t- Loading World Map List Fail");
        return 0;
    }
    {
        cMyTrace("bool CDataManager::init()", 0x1fe5, 9, true, false)(
            "\t- Loading Quests ");
        m_questList = new QuestList;
        if (!m_questList->load_list())
        {
            cMyTrace("bool CDataManager::init()", 0x1fe9, 9, true, true)(
                "\t- Loading Quests Fail");
            return 0;
        }
    }
    {
        cMyTrace("bool CDataManager::init()", 0x1fec, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x1fee, 9, true, false)(
            "\t- Loading Characters ");
        m_characList = new CCharacList;
        if (!m_characList->init())
        {
            cMyTrace("bool CDataManager::init()", 0x1ff2, 9, true, true)(
                "\t- Loading Characters Fail");
            return 0;
        }
    }
    {
        cMyTrace("bool CDataManager::init()", 0x1ff5, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x1ff7, 9, true, false)(
            "\t- Loading Disjoints ");
        m_disJoint = new DisJoint;
        if (!m_disJoint->LoadDisJoint())
        {
            cMyTrace("bool CDataManager::init()", 0x1ffb, 9, true, true)(
                "\t- Loading Disjoints Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x1ffe, 9, false, true)("Success");
    }

    // Advance Altar：StageMapList::loadList。
    {
        cMyTrace("bool CDataManager::init()", 0x2001, 9, true, false)(
            "\t- Loading Advance Altar ");
        if (!m_stageMapList.loadList())
        {
            cMyTrace("bool CDataManager::init()", 0x2004, 9, true, true)(
                "\t- Loading Advance Altar Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2007, 9, false, true)("Success");
    }

    // ItemGeneratorMgr / AutoMarket。
    if (m_itemGeneratorMgr == 0)
    {
        m_itemGeneratorMgr = new WongWork::CItemGeneratorMgr;
    }
    if (!m_itemGeneratorMgr->initialize())
    {
        cMyTrace("bool CDataManager::init()", 0x2012, 9, true, true)(
            "\t- Loading Item Generator Manager Fail\n");
        return 0;
    }
    // autoMarket：ORIG 复用 errRO（槽 -0x30，本处不再置 0）。
    if (m_autoMarket == 0)
    {
        m_autoMarket = new CAutoMarketConditionsControl;
    }
    if ((errRO = m_autoMarket->LoadScript("Etc/ItemDropInfo_Control.etc")) != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x201a, 9, true, true)(
            "\t- LoadScript() error Etc/ItemDropInfo_Control.etc line(%d)",
            errRO);
        return 0;
    }

    // LevelExp / MobReward / PvpGrade / RewardSP。
    {
        cMyTrace("bool CDataManager::init()", 0x2022, 9, true, false)(
            "\t- Init Level Exp ");
        // ORIG +0x4b8f..+0x4b99：`call set_level_exp / xor $1 / test %al / jmp
        // <success>`——出厂二进制此处被单字节改写（0x0835cecb 机器码 `eb 56`，
        // 而 `je` 应为 `74 56`，位移完全一致），使 “Init Level Exp Fail” 块成为
        // 不可达死代码。此处按源码原意还原该判定（唯一能生成同型块的合法 C++
        // 写法）；与 ORIG 仅剩 je↔jmp 一条指令之差，见交付说明。
        if (!set_level_exp())
        {
        cMyTrace("bool CDataManager::init()", 0x2025, 9, true, true)(
            "\t- Init Level Exp Fail");
        return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2028, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x202a, 9, true, false)(
            "\t- Init Mob Reward ");
        // 同上：ORIG 0x0835cfcb 机器码 `eb 56`（应为 `74 56` je），失败块被改写
        // 屏蔽；文案沿用 ORIG 的复制粘贴痕迹 “Init Level Exp Fail”（+0x4cd1）。
        if (!set_mob_reward())
        {
        cMyTrace("bool CDataManager::init()", 0x202d, 9, true, true)(
            "\t- Init Level Exp Fail");
        return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2030, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x2032, 9, true, false)(
            "\t- Init PVP Grade ");
        if (!set_pvp_grade())
        {
            cMyTrace("bool CDataManager::init()", 0x2035, 9, true, true)(
                "\t- Init PVP Grade Fail\n");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2038, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x203a, 9, true, false)(
            "\t- Init Reward SP ");
        if (!set_reward_sp())
        {
            cMyTrace("bool CDataManager::init()", 0x203d, 9, true, true)(
                "\t- Init Reward SP Fail\n");
            return 0;
        }
    }

    // Channel 数据/脚本。
    {
        cMyTrace("bool CDataManager::init()", 0x2040, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x2044, 9, true, false)(
            "\t- Init Get Channel Data(channel_info.etc) ");
        if (!GlobalData::s_channel_proxy->SendCheckChannelScript())
        {
            cMyTrace("bool CDataManager::init()", 0x2047, 9, true, true)(
                "\t- Init Get Channel Data(channel_info.etc) Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x204a, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x204e, 9, true, false)(
            "\t- Init Channel Script ");
        // [P248 档①/P292] ORIG 调用点（报告 idx 3963-3966）：`call 88dc008
        // <importChannelScript>; xor $0x1,%eax; test %al,%al; je` = bool 取反形。
        // 探针 /tmp/probe_xor.cpp：int callee 的六种写法恒产 setcc 五条，唯
        // callee 返回 bool 时裸 `!f()` 逐条复现（旧 &1 等价改写多 2 条/处）。
        // callee 声明+定义已 bool 化，返回语句全为常量 0/1（0x088dc062 尾调
        // 透传形态不变）。
        if (!importChannelScript(&m_channelScript))
        {
            cMyTrace("bool CDataManager::init()", 0x2058, 9, true, true)(
                "\t- Init Channel Script Fail");
            return 0;
        }
    }

    // ItemUpgrade / Separate / 3rdChronicle / Amplifier / Reseal。
    {
        cMyTrace("bool CDataManager::init()", 0x205b, 9, false, true)("Success");
        m_itemUpgrade = new WongWork::CItemUpgrade;
        cMyTrace("bool CDataManager::init()", 0x205e, 9, true, false)(
            "\t- Init Item Upgrade Info ");
        // [P248 档①/P292] ORIG idx 4023-4035：`mov 0x20(this),%eax; (%eax)+0x04
        // 取虚槽1; call *%edx; xor $0x1,%eax; test %al` = bool 取反形。虚槽目标
        // ORIG T 0x0854919a（CItemUpgrade）/0x0811eca8（Separate）均尾调透传
        // （无 movzbl ⇒ 整链同型 bool）；声明+定义已同步 bool 化。
        if (!m_itemUpgrade->LoadUpgradeScript("Etc/Upgrade.etc"))
        {
            cMyTrace("bool CDataManager::init()", 0x2061, 9, true, true)(
                "\t- Init Item Upgrade Info Fail\n");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2064, 9, false, true)("Success");
        m_itemUpgradeSeparate = new WongWork::CItemUpgrade_Separate;
        if (!m_itemUpgradeSeparate->LoadUpgradeScript("Etc/Upgrade_Separate.etc"))
        {
            cMyTrace("bool CDataManager::init()", 0x206a, 9, true, true)(
                "\t- Item Upgrade Separate Fail");
            return 0;
        }
        if (!m_itemUpgrade->Load3rdChronicleUpgradeScript("Etc/chronicle.etc"))
        {
            cMyTrace("bool CDataManager::init()", 0x2072, 9, true, true)(
                "\t- Loading 3rd Chronice Upgrade Script Fail");
            return 0;
        }
        GlobalData::s_itemAmplifier_->loadScript("Etc/AmplifyItem.etc",
                                                 "Etc/AmplifyUpgrade.etc");
    }

    // Reseal / UnlimitChallenge / LevelRewardTable / LimitItemUsage。
    {
        cMyTrace("bool CDataManager::init()", 0x2084, 9, true, false)(
            "\t- Init Reseal table ");
        if (!importResealTable("Etc/Reseal.etc", &m_resealTable))
        {
            cMyTrace("bool CDataManager::init()", 0x208c, 9, true, true)(
                "\t- Init Reseal table Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x208f, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x2092, 9, true, false)(
            "\t- Init Unlimit Challenge Info ");
        if (!importUnlimitChallengeScript("Etc/UnlimitChallenge.etc",
                                          &m_unlimitChallenge))
        {
            cMyTrace("bool CDataManager::init()", 0x2095, 9, true, true)(
                "\t- Init Unlimit Challenge Info Fail");
            return 0;
        }
        m_levelRewardTableMgr = CLevelRewardTableMgr::GetInstance();
        m_levelRewardTableMgr->SetChallengeInfo(m_unlimitChallenge);
        cMyTrace("bool CDataManager::init()", 0x209e, 9, false, true)("Success");
        if (!importLimitItemUsageInfo(
                (char*)"Etc/Chn_Server_LimititemUsageinfo.etc",
                &m_limitItemUsage))
        {
            cMyTrace("bool CDataManager::init()", 0x20a5, 9, true, true)(
                "\t- importLimitItemUsageInfo Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x20a8, 9, true, true)(
            "\t- Import Limit Item Usage Info Success");
    }

    // DB Log Item。
    {
        cMyTrace("bool CDataManager::init()", 0x20b7, 9, true, true)(
            "\t- Import DB Log Item - ");
        if (!importDBLogItem((char*)"Etc/Chn_DBLogItem.etc", &m_dbLogItems))
        {
            cMyTrace("bool CDataManager::init()", 0x20be, 9, true, true)(
                "\t- Import DB Log Item  Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x20c1, 9, true, true)(
            "\t- Import DB Log Item Success");
    }

    // Condition Event 两件套。
    {
        cMyTrace("bool CDataManager::init()", 0x20db, 9, true, false)(
            "\t- Import Condition Event Info ");
        if (!importConditionEventScript("Event/ConditionEvent.evt",
                                        &m_conditionEvent))
        {
            cMyTrace("bool CDataManager::init()", 0x20e2, 9, true, true)(
                "\t- Import Condition Event Info Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x20e5, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x2102, 9, true, false)(
            "\t- Import Condition Event Script Dungeon ");
        if (!importConditionEventChkDungeonScript(
                "Event/ConditionEventChkDungeon.evt", &m_conditionDungeon))
        {
            cMyTrace("bool CDataManager::init()", 0x2109, 9, true, true)(
                "\t- Import Condition Event Script Dungeon Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x210c, 9, false, true)("Success");
    }

    // NPCRelation / EmotionTags / EquipGrade。
    {
        cMyTrace("bool CDataManager::init()", 0x2111, 9, true, false)(
            "\t- Import NPC Relation Event Info Success");
        if (!importNPCRelationEventScript("Event/NPCRelationEvent.evt",
                                          &m_npcRelation))
        {
            cMyTrace("bool CDataManager::init()", 0x2118, 9, true, true)(
                "\t- Import NPC Relation Event Info Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x211b, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x211e, 9, true, false)(
            "\t- Import Emotion Type Tags Info ");
        if (!importEmotionTypeTagsScript("Etc/EmotionExpressionData.etc",
                                         &m_emotionTags))
        {
            cMyTrace("bool CDataManager::init()", 0x2121, 9, true, true)(
                "\t- Import Emotion Type Tags Info Fail\n");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2124, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x2126, 9, true, false)(
            "\t- Import Equipment Grade Sample Info ");
        if (!importEquipmentGradeSampleInfo("Etc/EquipmentGradeSample.etc",
                                            &m_equipGrade))
        {
            cMyTrace("bool CDataManager::init()", 0x2129, 9, true, true)(
                "\t- Import Equipment Grade Sample Info Fail\n");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x212c, 9, false, true)("Success");
    }

    // Creature 脚本两步。
    {
        cMyTrace("bool CDataManager::init()", 0x215c, 9, true, false)(
            "\t- Import Creature Script");
        if (!initCreatureScript("Creature/", "Creature.lst", "ExpTable.tbl"))
        {
            cMyTrace("bool CDataManager::init()", 0x215f, 9, true, true)(
                "\t- Import Creature Creature.lst fail");
            return 0;
        }
        if (!user_creature::GetInstanceCreatureScriptMgr()
                  ->LoadCreatureScriptFile())
        {
            cMyTrace("bool CDataManager::init()", 0x2165, 9, true, true)(
                "\t- Import Creature Script Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2168, 9, false, true)("Success");
    }

    // PvP single playable map（静态，无条件）+ NPC Script。
    CMapList::LoadPvPSinglePlayableMap((char*)"extern.sd");
    m_npcScriptList = new CNPCScriptList;
    m_npcDynamicInfo = new CNPCDynamicInfoManager;
    {
        cMyTrace("bool CDataManager::init()", 0x217a, 9, true, false)(
            "\t- Import Npc Script ");
        if (m_npcScriptList->initScript((char*)"Npc/", (char*)"Npc.lst",
                                        (char*)"common.dat") != 0)
        {
            cMyTrace("bool CDataManager::init()", 0x217d, 9, true, true)(
                "\t- Import Npc.lst Fail\n");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2180, 9, false, true)("Success");
    }

    // ItemLimitEditionMgr（m_npcRelation.m_itemLimitEditionMgr）+ MCAP init。
    m_npcRelation.m_itemLimitEditionMgr = new CItemLimitEditionMgr;
    WongWork::CMCAPManager::init();

    // Equipment Part Set：trancate + import + g_equipmentPartSetMap_ 双层回灌。
    sync_script::GetInstanceSyncScript()->trancate_item_parts_set();
    {
        cMyTrace("bool CDataManager::init()", 0x21ac, 9, true, false)(
            "\t- Import Equipment Part Set Info ");
        if (!importEquipmentPartSetInfo("Etc/EquipmentPartSet.etc",
                                        static_cast<ENUM_MODULE_TYPE>(3)))
        {
            cMyTrace("bool CDataManager::init()", 0x21af, 9, true, true)(
                "\t- Import Equipment Part Set Info Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x21b3, 9, false, true)("Success");

        // 迭代器在 for-init 内声明并直接初始化（ORIG `begin` 的 sret 目标即
        // 迭代器槽 -0xf58/-0xf5c/-0xf60，无「临时+拷贝」⇒ 必须是声明初始化）。
        for (std::map<int, STEquipmentPartSetData>::iterator it =
                 g_equipmentPartSetMap_.begin();
             it != g_equipmentPartSetMap_.end(); ++it)
        {
            // [b28d-solo-CDataManager] ORIG 0x0835e4f0 `ptEv; add $0x4;
            // mov %eax,-0x20`（具名引用 data）+ 0x0835e4fe `add $0xc;
            // mov %eax,-0x1c`（具名 pieces 引用）——旧形每处字段访问重发
            // ptEv（p167 EXTRA operator-> ×5）。
            // [N93 封顶] ORIG 槽序 data(-0xf60 深)/pieces(-0xf58 浅) 与本侧相反；
            // 交换声明序实测反劣（5431/5432 且 658 行非槽差，物化顺序被破坏）
            // ⇒ 按 N93（块级对象槽向跨版本相反、具名引用不可翻）封顶。
            STEquipmentPartSetData& data = it->second;
            std::vector<STEquipmentPartSetPiece>& pieces = data.m_pieces;
            for (std::vector<STEquipmentPartSetPiece>::iterator pit =
                     pieces.begin();
                 pit != pieces.end(); ++pit)
            {
                sync_script::GetInstanceSyncScript()->insert_item_parts_set(
                    it->first, *pit);
            }
            // [b28d-orig-nullfree] ORIG 报告 idx 4810-4816 对 m_script 直接
            // `mov 0x8(%eax),%eax` 解引用、无任何判空（旧 `!= 0` 守卫系我方发明，
            // ELF diff +4 行 EXTRA）⇒ 去守卫，与 ORIG 同形无条件进内层 map 循环。
            for (std::map<int, EquipmentParameterInfo>::iterator eit =
                     data.m_script->m_paramMap678.begin();
                 eit != data.m_script->m_paramMap678.end(); ++eit)
            {
                STEquipmentScript es;
                es.m_strac = data.m_script->m_str668;
                // ORIG 0x0835e4f8：`call operator->; add $0x4; mov %eax,-0x20`
                // 具名指针物化后两处复用（+0xc deref/…）——具名引用引导 4.4
                // 同形物化（段②站点对表首个试点）。
                // [b28d-orig-form] ORIG idx 4830-4838 对 eit->second 的
                // `ptEv; add $4` 结果直接落 operator= 实参槽（无具名引用中转，
                // 旧 rParam 引用多发 `mov %eax,-S; mov -S,%eax` 保存/取回 ×2）。
                es.m_param1c0 = eit->second;
                sync_script::GetInstanceSyncScript()
                    ->insert_equiptment_item_to_db(
                        &es, (unsigned int)eit->first, 2, it->first);
            }
        }
    }

    // FairPvPStat + PvP SkillTree 四步 + initFairPVPSkillPoint。
    {
        cMyTrace("bool CDataManager::init()", 0x21ca, 9, true, false)(
            "\t- Import FairPvPStat ");
        if (!importPairPvPStatScript(m_fairPvPStat, "Etc/FairPvPStat.etc"))
        {
            cMyTrace("bool CDataManager::init()", 0x21cd, 9, true, true)(
                "\t- Fail load FairPvPStat Script File");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x21d1, 9, false, true)("Success");
        if (!initPvPSkillTreeScript((char*)"Etc/", (char*)"PvPSkillTree.lst"))
        {
            cMyTrace("bool CDataManager::init()", 0x21d7, 9, true, true)(
                "\t- Load PvPSkillTree.lst fail");
            return 0;
        }
        if (!m_pvpSkillTree.load_pvp_skill_tree_list())
        {
            cMyTrace("bool CDataManager::init()", 0x21dd, 9, true, true)(
                "\t- Load load_pvp_skill_tree_list fail");
            return 0;
        }
        if (!m_pvpSkillTree.importPairPvPSkillPointScript(
                "Etc/PvPSkillPoint.etc"))
        {
            cMyTrace("bool CDataManager::init()", 0x21e3, 9, true, true)(
                "\t- Load PairPvPSkillPointScript fail");
            return 0;
        }
        if (!initFairPVPSkillPoint())
        {
            cMyTrace("bool CDataManager::init()", 0x21eb, 9, true, true)(
                "\t- initFairPVPSkillPoint fail");
            return 0;
        }
    }

    // Ingame Event：new(0x5c) CEventScriptMng → m_eventScriptMng；导入 + init。
    m_liveServerInfo.m_eventScriptMng = new EventClassify::CEventScriptMng;
    {
        cMyTrace("bool CDataManager::init()", 0x221d, 9, true, false)(
            "\t- Loading Ingame Event Script List ");
        // [b28d-orig-form] ORIG 三站点各自独立 `mov 0xb504(%eax)` 重读成员
        // （报告 idx 5059/5093-5099），无局部指针缓存 ⇒ 表达式直接写成员路径。
        if (!importInGameEventInfoList(
                static_cast<InGameEventInfoList*>(m_liveServerInfo.m_eventScriptMng),
                "Event/InGameEventList.evt"))
        {
            cMyTrace("bool CDataManager::init()", 0x2224, 9, true, true)(
                "\t- Fail load ingame_event_script_list");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2228, 9, false, true)("Success");
        if (!m_liveServerInfo.m_eventScriptMng->initScriptlist(
                *m_liveServerInfo.m_eventScriptMng))
        {
            return 0;
        }
    }

    // Event Avatar List / Event Etc。
    {
        cMyTrace("bool CDataManager::init()", 0x2231, 9, true, false)(
            "\t- Loading Event Avatar List ");
        if (!importEventAvatarListScript(&m_eventAvatarList,
                                         "Event/EventAvatarList.evt"))
        {
            cMyTrace("bool CDataManager::init()", 0x223d, 9, true, true)(
                "\t- Fail Loading Event Avatar List");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2240, 9, false, true)("Success");
        cMyTrace("bool CDataManager::init()", 0x2244, 9, true, false)(
            "\t- Loading Event Etc Info ");
        if (!importEventEtcScript(&m_eventEtc, "Event/EventEtc.evt"))
        {
            cMyTrace("bool CDataManager::init()", 0x224e, 9, true, true)(
                "\t- Fail Loading Event Etc Info");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2252, 9, false, true)("Success");
    }

    // AvatarColorVariation。
    if (!AvatarVariation::importAvatarColorVariation(&m_avatarColor))
    {
        cMyTrace("bool CDataManager::init()", 0x227b, 9, true, true)(
            "\t- Loading importAvatarColorVariation  Fail");
        return 0;
    }

    // Mercenary System（ORIG 报告 idx 5231-5239：errMerc 复位(-0x2c) → 独立
    // store → `cmpl $0; je` **两段形**（无 setne，旧「单表达式」判断有误））。
    cMyTrace("bool CDataManager::init()", 0x229b, 9, true, false)(
        "\t- Loading Mercenary System ");
    errMerc = 0;
    errMerc = importMercenarySystemEtcScript("Etc/mercenary.etc",
                                             &m_mercenarySystem);
    if (errMerc != 0)
    {
        cMyTrace("bool CDataManager::init()", 0x22a0, 9, true, true)(
            "\t- Loading Mercenary System Fail - %d line \n", errMerc);
        return 0;
    }
    cMyTrace("bool CDataManager::init()", 0x22a3, 9, false, true)("Success");

    NeedMaterialDataManager::getInstance();

    // PcBang / DimensionActivation。
    if (!importPcBangEquipmentRendar("Etc/PcBangRentalService.etc",
                                     &m_pcBangRentar))
    {
        cMyTrace("bool CDataManager::init()", 0x2304, 9, true, true)(
            "\t- PcBangRentalService Fail!\n");
        return 0;
    }
    if (!importDimensionActivationData("Event/DimensionActivationEvent.evt",
                                       &m_dimensionAct))
    {
        cMyTrace("bool CDataManager::init()", 0x230b, 9, true, true)(
            "\t- DimensionActivationEvent Fail!\n");
        return 0;
    }

    // 租借系统就绪检查。
    if (!reinterpret_cast<InstanceRentalSystem*>(GlobalData::s_rentalSystem)
              ->Ready())
    {
        std::cerr << "[GlobalData::Init] The rental system is not ready!"
                  << std::endl;
        return 0;
    }

    // Delete Invalid Item Script（最后一步）。
    {
        cMyTrace("bool CDataManager::init()", 0x2331, 9, true, false)(
            "\t- Load Delete Invalid Item Script");
        if (!m_deleteInvalidItem.loadScript())
        {
            cMyTrace("bool CDataManager::init()", 0x2334, 9, true, true)(
                "\t- Load Delete Invalid Item Script Fail");
            return 0;
        }
        cMyTrace("bool CDataManager::init()", 0x2337, 9, false, true)("Success");
    }

    return 1;
}

// ===================== 副本次数排行 =====================

// ORIG T 0x083639d2 _ZN12CDataManager26getDungeonClearAverageTimeEii23ENUM_DUNGEON_DIFFICULTY：
//   以 (dungeonIdx, partyCount) 为键查 m_rankSystem.m_dungeonRank.m_map0；
//   未命中返回 0；命中按 difficulty(0..3) 取 stClearTimeAverage::m_v[难度]，
//   其它难度返回 0。
unsigned int CDataManager::getDungeonClearAverageTime(int dungeonIdx, int partyCount,
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

// ORIG T 0x08363ac8 _ZN12CDataManager22getClearTimeBonusPointEii：
//   avgTime/clearTime 任一为 0 → 0；diff = avgTime - clearTime；
//   遍历 m_dungeonRank.m_vec18（vector<stBonusPointValues>）：
//     若 diff < 当前 lo：列表首项 → 0，否则返回上一项 value；
//     否则记录 (lo, value) 继续；循环结束返回最后一个 value。
// [b28d-solo-CDataManager AE] 修复：cur/prev 声明序（cur 先，ORIG 0x08363af0
// 先构造 -0x20）+ diff 赋值后置到两次 ctor 之后 + end 缓存到 e（P13，ORIG
// 0x08363b30 循环外一次 end）复现 ORIG 槽位与循环形。
int CDataManager::getClearTimeBonusPoint(int avgTime, int clearTime)
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
void emitLevelUpRewardWeakSymbols()
{
    LevelUpRewardItemTableKey key;   // C1 → call clear
    (void)key;
    LevelUpRewardItemData data;      // C1 → call clear
    (void)data;
}
}  // namespace
