// ============================================================================
// CtorProducers_1.cpp —— df_game_r 还原（Ctor_Group1 批次，2026-08-24）。
// 全部符号经 ORIG nm 核实；实现逐条对照 docs/class_func_reports/ 对应报告的
// ## 汇编 + ## 反编译 C（函数旁标注 ORIG 地址）。
//
// 本 TU 独家提供（勿在他处重复定义）：
//   AvatarRoulette C1/D1、AvatarRouletteServer C1/D1/Init/InitAddCharacSkill/
//                   isAvatarPottery（自 Arad_AvatarRoulette.cpp 迁入）
//   CChattingEmoticonList C1
//   CNPCScriptList C1/initScript
//   CNPCDynamicInfoManager C1/getNPCInfo/add
//   initNpcScript/isExistNPCScript
//   TowerOfDespair_APC_Mgr C1、TowerOfDespairMgr C1/D1/LoadScript/_Set_* 三件
//   CLevelRewardTable C1/SetJobAffection/GetProperJobIndex/GetProperLevelIndex
//   CLevelRewardTableMgr C1/D1/GetInstance/SetChallengeInfo（单例指针定义）
//   CAICharacter::set、CAICharacterList::init（_add 经桥调用）
//   WorldMapScript C1/D1/clear/importWorldMapScript
//   CWorldMap C1/set_world_map、CWorldMapList insert_world_map/load_list
//   CCharacList C1/D1/destroy/init
//   STScriptFileList::GetFileName（ORIG T 0x088bad4e）
//
// [b28d-S435 单文件泳道 2026-09-09] 本轮：①CLevelRewardTable C1 二维化
//   （头 DataManagerInitDepends.h m_rewardPrimary/Secondary → int[9][0x46]，
//   Main 代落；T1 直证 ORIG 0x086975fe imul/lea 形）⇒ DIFF 12→0 归 AE；
//   ②TowerOfDespairMgr C1 m_layerDungeons const 化（头 TowerOfDespairMgr.h，
//   Main 代落；T1 直证 `_ZSt4fillIPPK8CDungeonS2_`）⇒ p167 MISSING/EXTRA→CLEAR；
//   ③importWorldMapScript `second` 三形实测同值（P112 封顶）。
//   TU 计数 377/771/2/12 → 378/774/2/11；清单 11 符号 p167 全 CLEAR。
// ============================================================================

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// AvatarRoulette/Server 权威类经 DataManagerInitDepends.h（含 LevelUpSkill 前置依赖）
#include "CDataManager.h"
#include "CCharacList.h"
#include "CWorldMap.h"
#include "WorldMapScript.h"
#include "NPCScript.h"
#include "STCharacterScript.h"   // STCharacterScript 权威布局（CCharacList::init 真实 C1/D1）
#include "NPCDynamicInfo.h"      // CNPCDynamicInfo 权威布局/ctor（CNPCDynamicInfoManager::add 真实 C1）
#include "CChattingEmoticonList.h"
#include "CGuildServerProxy.h"   // DeliveryMsg 权威声明（迁入定义 NL6）

// ---- 迁入定义（NL6，outgoing 帧塌缩规避；NL4 unit-at-a-time 机制实证）----

// ORIG W 0x0891bce0：两 int 字段清零。原 InitScriptProducers_1.cpp。
void StackableItemData::clear()
{
    m_field0 = -1;   // +0x00 movl $0xffffffff
    m_field4 = 0;    // +0x04
}

// ORIG W 0x086978c0：四字段清零（server group 位为 0xffffffff）。
// 原 CUserDepsB.cpp（BuyItemDeliveryMsgC1 调用点 outgoing 帧 0x18 依赖此迁移）。
DeliveryMsg::DeliveryMsg()
{
    m_type = 0;
    m_accId = 0;
    m_characNo = 0;
    m_serverGroup = -1;
}
#include "EquipmentParameterInfo.h"
#include "STEquipmentScript.h"       // ImportEquipmentScript 声明（5 参）
#include "DNFLexWrapper.h"           // loadRDARScriptFile/ScanType/ScanInt/loadRDARScriptListFile
#include "LogManager.h"
#include "GlobalData.h"
#include "CEnvironment.h"
#include "RDARScriptStringManager.h" // g_scriptStringManager_/findString
#include "sync_script.h"             // STAICharacterCommonScript 占位/STEquipmentPartSetPiece（须先于 deps 头）
#include "DataManagerInitDepends.h"  // TowerOfDespair/CLevelRewardTable 权威类（Ctor_Group1）
#include "CDungeon.h"                // get_index/get_min_level/get_standard_level/getDungeonKind
#include "GameTypes.h"               // cMyTrace
#include "TimerDispatcher.h"         // TimerNPCMoodChange::registNextTimer（权威声明）

// ---- STScriptFileList 镜像（map 视图；NpcScript.h 的 pad 版仅作不透明传递，
//      需要 map 成员访问的三全局改用本镜像类型声明）----
class STFileListMapMirror
{
public:
    std::map<int, std::string> m_fileMap;                // +0x00
    const char* GetFileName(int idx) const;              // ORIG T 0x088bad4e
    int FindIndexByFullScan(const char* path) const;     // ORIG T 0x088bae56
};

const char* STFileListMapMirror::GetFileName(int idx) const
{
    // ORIG 0x088bad4e：find(idx)；命中返回 second.c_str()，否则 0。
    std::map<int, std::string>::const_iterator it = m_fileMap.find(idx);
    if (it == m_fileMap.end())
        return 0;
    return it->second.c_str();
}

// ---- ORIG B 全局脚本文件列表（对象模型，map<int,string>@+0；定义归属
//      InitScriptProducers_4.cpp / InitScriptProducers_5.cpp，此处仅引用）----
extern STFileListMapMirror g_stCharacterScriptFileList;   // B 0x94fe940
extern STScriptFileList g_worldMapScriptFileList;         // 0x9501000（NpcScript.h 类型；方法解析 ORIG 符号）
extern STFileListMapMirror g_aiCharacterScriptFileList;    // 0x94f6a20
extern STScriptFileList g_stEquipmentScriptFileList;       // 0x94fac00（ORIG 类型，GetFileName 解析 0x088bad4e）
extern char g_worldMapScriptBaseDirectory[0x104];          // B（InitScripts_5 定义域）

// ImportCharacterScript（ORIG T 0x08a0ce06，定义 ScriptDepsA.cpp）——CCharacList::init 真实调用。
// §4.5 正解：普通 C++ 自由函数前置声明（非 extern "C"），include STCharacterScript.h 取真实类型。
bool ImportCharacterScript(STCharacterScript* script, const char* path);

// ---- STAICharacterCommonScript 权威布局视图（sizeof 0x1010 —— init 栈帧
//      -0x104c..-0x3c 实证；CAICharacter::set 访问最远字段 +0x100c。
//      sync_script.h 现存 8 字节占位与本视图冲突，故本 TU 不含该头。
//      CAICharacter::set 符号由 sub_CAISet asm 别名提供（见下））----
struct STAICharacterCommonScriptView
{
    int m_field0;                                   // +0x000
    std::string m_str4;                             // +0x004
    int m_level;                                    // +0x008（set 读）
    char m_padC[0xd80 - 0xc];                       // +0x00c..
    int m_d80;                                      // +0xd80（set 读）
    int m_d84;                                      // +0xd84（set 读）
    std::vector<std::pair<int, int> > m_d88;        // +0xd88（set assign）
    char m_padD90[0xda4 - 0xd94];                   // +0xd90..
    int m_da4;                                      // +0xda4（set 读）
    int m_da8;                                      // +0xda8（set 读）
    char m_padDac[0xf2c - 0xdac];                   // +0xdac..
    std::vector<stMonsterIndependentDrop_t> m_f2c;  // +0xf2c（set assign）
    char m_padF38[0xf50 - 0xf38];                   // +0xf3c..
    char m_avatarDrops[0x100c - 0xf50];             // +0xf50（stEnamyDropAvatarInfo_t 区，本 TU 不访问成员）
    unsigned char m_rarityByte;                     // +0x100c（set 读）
    char m_pad100d[3];
    int m_animationJob;                             // +0x1010（ORIG clear 置 0xb）
};  // sizeof 0x1014（ORIG STAICharacterCommonScript 布局视图）


namespace DNFFLib { void Make_Dir(char* dir); }   // T 0x08108ad7

bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* unused);
   // ORIG T 0x088bbd77 _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_
   //   _PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE；定义 STScriptFileList.cpp。
   // 旧本地声明 bool(void*,...) mangled 不匹配（Pv ≠ P16STScriptFileList），
   // 曾产生未解析引用——本批次统一为 ORIG 签名（ScriptFileAndGlobalsB2）。
   // 返回类型按 bool 声明（mangled 不含返回类型；StrikerScript.cpp 先例；
   // ORIG 体 return 0/1，调用侧 xor$1/test %al 岛证实 1 字节语义）。

// ---- STAICharacterCommonScript 权威视图（sizeof 0x1010；ORIG 同名类型，
//      sync_script.h 的 8 字节占位与本视图冲突故本 TU 不含该头）----
// +0x08 字段声明为枚举：P254 判据（ORIG `mov 0x8(%eax),%eax; mov %eax,%edx`
// 两段装载中转 ⇒ 源字段类型是 enum；int 字段 4.4 恒折单段）。
enum AICharLevelEnum { AI_CHAR_LEVEL_MIN = 0 };

struct STAICharScriptView
{
    int m_field0;                                   // +0x000
    std::string m_str4;                             // +0x004
    AICharLevelEnum m_level;                        // +0x008（set 读；enum 见上）
    char m_padC[0xd80 - 0xc];
    int m_d80;                                      // +0xd80
    int m_d84;                                      // +0xd84
    std::vector<std::pair<int, int> > m_d88;        // +0xd88
    char m_padD90[0xda4 - 0xd94];
    int m_da4;                                      // +0xda4
    int m_da8;                                      // +0xda8
    char m_padDac[0xf2c - 0xdac];
    std::vector<stMonsterIndependentDrop_t> m_f2c;  // +0xf2c
    char m_padF38[0xf50 - 0xf38];
    stEnamyDropAvatarInfo_t m_f50;                  // +0xf50（sizeof 0x3c；P288 成员取址 lea）
    char m_padF8c[0x100c - 0xf8c];                  // +0xf8c..+0x100b
    unsigned char m_rarityByte;                     // +0x100c
    char m_pad100d[3];
};  // sizeof 0x1010

// ============================================================================

// （报告 AvatarRoulette/{AvatarRoulette,clear,swap}.md）

// ============================================================================

// （报告 AvatarRoulette/{AvatarRoulette,clear,swap}.md）
// ============================================================================
// AvatarRouletteServer（报告 AvatarRouletteServer/*.md）
// ============================================================================

// ORIG T 0x0817f354：map ctor 后仅写 +0x18=0（m_pRoulette；+0x1c 总数由 Init 写）。
AvatarRouletteServer::AvatarRouletteServer() : m_addCharacSkillMap()
{
    m_pRoulette = 0;
}

// ORIG T 0x0817f372：仅析构 +0x00 的 map<unsigned long, map<int, vector<LevelUpSkill>>>。
AvatarRouletteServer::~AvatarRouletteServer()
{
}

bool AvatarRouletteServer::Init(AvatarRoulette* roulette)
{
    // [b28d-46 判词·N/N82+N89] ORIG 0x0817f706：264 vs 224；帧/槽整体平移
    // （N82）＋ STL 模板实例内联差（N89）⇒ 封顶。
    // ORIG 0x0817f706。
    m_pRoulette = new AvatarRoulette;
    m_pRoulette->swap(roulette);
    m_totalCount = m_pRoulette->m_upperAvatarRate + m_pRoulette->m_normalAvatarRate;

    // [b28d-solo-CtorProducers_1 判词·N82/P112 残余] 本轮已修：①p167 差集 6 次
    // `inner[type]` 重读（P157/P48，旧具名 itemNo 缓存致 MISSING×6）；②ORIG 两次
    // `outer.size()`（①死调用 P171 + ②循环上界缓存，cond 块只 cmp -0x38）；
    // ③具名 expectedGrade 局部（P0：ORIG `movl $0/$2/$1,-0x1c` 槽往返，旧写立即数）。
    // 残余：4.1 按声明序「先声明更深」分配（result -0x45/job -0x44/…），4.4 按
    // 首次使用序（result -0x41/job -0x40/…）⇒ 全阶梯整体浅 4；dead 标量/char[4]/
    // double pad 三形实测均不能平移（4.4 把新局部放更深，result 不动）⇒ 封顶。
    bool result = true;   // ORIG movb $0x1,-0x45；失败路径写 0，最终返回常量 true
    if (G_CEnvironment()->m_serverEnvir.m_validateScript)
    {
        for (int job = 0; job < 0xb; ++job)
        {
            std::vector<std::vector<std::pair<unsigned long, int> > >& outer =
                m_pRoulette->m_avatars[job];
            // [b28d-solo-CtorProducers_1] ORIG 0x0817f7b1..0x0817f7da 逐条：
            // ① `outer.size()` 死调用（结果丢弃，P171 照抄）；② `li=0`；
            // ③ 第二次 `outer.size()` 存 -0x38 作循环上界（cond 块
            // 0x0817fad8 只 `cmp -0x38`，不再调 size）⇒ 显式缓存 n。
            outer.size();
            size_t li = 0;
            size_t n = outer.size();
            for (; li < n; ++li)
            {
                std::vector<std::pair<unsigned long, int> >& inner = outer[li];
                for (int type = 0; type < 9; ++type)
                {
                    // P157/P48：ORIG 每个使用点重新 `inner[type]`（p167 差集
                    // 实证 operator[] MISSING ×6），禁具名 itemNo 缓存。
                    CItem* item = G_CDataManager()->find_item(
                        static_cast<int>(inner[type].first));
                    if (item == 0)
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x80,
                            "AvatarRouletteServer not exist item_no(%d)",
                            static_cast<int>(inner[type].first));
                        result = false;   // ORIG movb $0x0,-0x45（无 continue）
                    }
                    if (!item->IsAvatarItem())
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x86,
                            "AvatarRouletteServer no avatar item_no(%d)",
                            static_cast<int>(inner[type].first));
                    }
                    result = false;   // ORIG movb $0x0,-0x45
                    if (!item->check_job_type(static_cast<char>(job)))
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x8d,
                            "AvatarRouletteServer item_no(%d) check_job_type(%d)==false",
                            static_cast<int>(inner[type].first), job);
                    }
                    result = false;   // ORIG movb $0x0,-0x45
                    unsigned int itemType = item->GetItemType();
                    if (itemType != static_cast<unsigned int>(type))
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x95,
                            "AvatarRouletteServer item_no(%d) GetItemType(%d)!=%d",
                            static_cast<int>(inner[type].first), itemType, type);
                    }
                    result = false;   // ORIG movb $0x0,-0x45
                    if (type != 8)
                    {
                        int grade = item->get_grade();
                        // P0：ORIG `movl $0x0,-0x1c` 预清 + 两支各 `movl $2/$1,-0x1c`
                        // ⇒ 原码有具名 expectedGrade 局部并在 logFormat 实参处读取
                        // （我们旧写立即数 2/1，缺槽往返与 store）。
                        int expectedGrade = 0;
                        if (li < 2 && grade != 2)
                        {
                            expectedGrade = 2;
                            LogManager::logFormat(
                                1, "localjapan/Arad_AvatarRoulette.cpp",
                                __PRETTY_FUNCTION__, 0xa3,
                                "AvatarRouletteServer item_no(%d) get_grade(%d)!=%d, line_num(%d)",
                                static_cast<int>(inner[type].first), grade,
                                expectedGrade,
                                static_cast<int>(li));
                        
                            result = false;   // ORIG movb $0x0,-0x45
                        }
                        else if (li > 1 && grade != 1)
                        {
                            expectedGrade = 1;
                            LogManager::logFormat(
                                1, "localjapan/Arad_AvatarRoulette.cpp",
                                __PRETTY_FUNCTION__, 0xa9,
                                "AvatarRouletteServer item_no(%d) get_grade(%d)!=%d, line_num(%d)",
                                static_cast<int>(inner[type].first), grade,
                                expectedGrade,
                                static_cast<int>(li));
                        }
                        result = false;   // ORIG movb $0x0,-0x45
                    }
                }
            }
        }
    }
    return true;
}

bool AvatarRouletteServer::isAvatarPottery(unsigned long itemIdx)
{
    // ORIG 0x0817fb0a：**(ulong**)(this+0x18)==itemIdx —— 即
    // m_pRoulette->m_avatarCoinNo（+0x18 指向 roulette 首 8 字节）。
    if (static_cast<unsigned long>(m_pRoulette->m_avatarCoinInfo) == itemIdx)
        return true;
    return false;
    // [推断] ORIG 为 8 字节 ulong 比对；共享头现以 int 建模 coin info 字段。
}

bool AvatarRouletteServer::InitAddCharacSkill()
{
    // [b28d-solo-CtorProducers_1 判词·P171 修复，残余 N82] ORIG 0x0817f386：
    // 本轮修 `outer.size()` 死调用 + 缓存上界两段形（同 Init：ORIG 0x0817f3d5
    // 第一次 size() 结果丢弃、0x0817f3e7 第二次存 -0x20，p167 MISSING×1 清零）。
    // 残余：槽号整体平移（4.1 先声明更深 vs 4.4 首次使用序，N82）⇒ 封顶。
    // ORIG 0x0817f386。
    if (G_CEnvironment()->m_serverEnvir.m_validateScript)
    {

    for (int job = 0; job < 0xb; ++job)
    {
        std::vector<std::vector<std::pair<unsigned long, int> > >& outer =
            m_pRoulette->m_avatars[job];
        // 同 Init：ORIG 0x0817f3d5 `size()` 死调用（结果丢弃，P171）+ `li=0` +
        // 0x0817f3e7 第二次 `size()` 存 -0x20 作循环上界（p167 MISSING×1）。
        outer.size();
        size_t li = 0;
        size_t n = outer.size();
        for (; li < n; ++li)
        {
            std::vector<std::pair<unsigned long, int> >& inner = outer[li];
            for (int type = 0; type < 9; ++type)
            {
                if (!(type == 3 && (job == 9 || job == 0)))
                    continue;
                const char* fileName =
                    g_stEquipmentScriptFileList.GetFileName(
                        static_cast<int>(inner[type].first));
                if (fileName == 0)
                    continue;

                STEquipmentScript eq;
                if (!ImportEquipmentScript(&eq, fileName, false, 0, -1))
                {
                    LogManager::logFormat(
                        1, "localjapan/Arad_AvatarRoulette.cpp",
                        __PRETTY_FUNCTION__, 0x39,
                        "InitAddCharacSkill() - ImportEquipmentScript() fail file - %s",
                        fileName);
                    return false;
                }
                std::map<int, std::vector<LevelUpSkill> > skillMap;
                int count = static_cast<int>(eq.m_param1c0.m_avatar264.size());
                for (int idx = 0; idx < count; ++idx)
                {
                    LevelUpSkill skill;
                    STAvatarAbilityVariation& var = eq.m_param1c0.m_avatar264[idx];
                    skill.m_job = var.m_skill.m_job;        // 元素 +0xc（内层 map 键，ORIG 0x0817f55d find 用 +0）
                    skill.m_field4 = var.m_skill.m_field4;  // 元素 +0x10
                    skill.m_field8 = var.m_skill.m_field8;  // 元素 +0x14
                    // ORIG：先取 end() 再 find(job)（0x0817f548/0x0817f577），
                    // find==end 时先插入空 vector，再 push_back。
                    std::map<int, std::vector<LevelUpSkill> >::iterator itEnd =
                        skillMap.end();
                    std::map<int, std::vector<LevelUpSkill> >::iterator it =
                        skillMap.find(skill.m_job);
                    if (it == itEnd)
                    {
                        std::vector<LevelUpSkill> empty;
                        skillMap[skill.m_job] = empty;
                    }
                    skillMap[skill.m_job].push_back(skill);
                }
                m_addCharacSkillMap[inner[type].first] = skillMap;
            }
        }
    }
    }
    return true;
}

bool AvatarRouletteServer::Pickup(PickedAvatarInfo& picked, int grade, int index)
{
    // [b28d-solo-CtorProducers_1 判词·P157+P96/P123 修复，残余 N82] ORIG 0x0817fb28：
    // 本轮修（p167 由 5 项差集→CLEAR 31/31）：①`outer[band]` 两次独立
    // `vector<vector<...>>::operator[]`（size 检查 + 取值，P157，去 outer/inner 引用缓存）；
    // ②技能段改**非 const** `_Rb_tree_iterator`（P96）+ `find` 判存在后
    // `m_addCharacSkillMap[uid]`/`skillMap[grade]` 二次 `operator[]`（P123，Ghidra C 同形）。
    // 本轮修：ORIG 0x0817fb42 死写入局部 -0x2c（0/2/1，全程未读，P171 照抄
    // ⇒ bandKind；gap 10→7）。残余：帧 0x7c vs 0x70 与槽号平移（N82）+ 比较
    // 方向 fold（ORIG setbe vs 我们 setae，P196 语义等价；交换书写序 ndiff 反升
    // 295→309 已回退）⇒ 封顶。
    // ORIG 0x0817fb28（报告 AvatarRouletteServer/Pickup.md 全文）。
    // 档位抽取：randVal<roulette+0x0c 时 band∈{0,1}，否则从
    // m_avatars[0].size()-2 偏移取 band（内联 size helper 08180760 实证）。
    int randVal = get_rand_int(m_totalCount);
    // [b28d-solo-CtorProducers_1 修复·P171] ORIG 0x0817fb42 `movl $0x0,-0x2c`
    // 并在两分支写 2/1，但全程**未读**（Ghidra C `local_30` 无消费者）= 原码死
    // 写入，必须照抄（缺它 -0x2c 槽与后续装载形态全异）。
    int bandKind = 0;
    if (randVal < m_pRoulette->m_upperAvatarRate)
    {
        picked.m_gradeBand = static_cast<char>(get_rand_int(2));
        bandKind = 2;   // ORIG 0x0817fb6b
    }
    else
    {
        picked.m_gradeBand = static_cast<char>(
            get_rand_int(static_cast<int>(m_pRoulette->m_avatars[0].size()) - 2) + 2);
        bandKind = 1;   // ORIG 0x0817fb9b
    }
    (void)bandKind;   // 死写入（P171 照抄；无消费者）

    if (grade < 0 || grade > 10)
        return false;

    // [b28d-solo-CtorProducers_1] P157/p167：ORIG 对 `outer[band]` 调
    // `vector<vector<...>>::operator[]` **两次**（size 检查一次、取值一次），
    // 禁具名 outer/inner 引用缓存（旧写法仅 1 次，p167 MISSING×1）。
    // [b28d-solo-CtorProducers_1 判词·P196 折痕] ORIG `cmp band,size; setbe`；
    // 交换书写序（`size()<=band`）虽消 setae/setbe 差，但 jne 块错位使 ndiff
    // 295→309 ⇒ 回退；setbe/setae 属比较方向 fold（语义等价）封顶。
    if (static_cast<unsigned int>(picked.m_gradeBand) >=
        m_pRoulette->m_avatars[grade].size())
        return false;
    if (static_cast<unsigned int>(index) >=
        m_pRoulette->m_avatars[grade][static_cast<int>(picked.m_gradeBand)].size())
        return false;

    std::pair<unsigned long, int>& entry =
        m_pRoulette->m_avatars[grade][static_cast<int>(picked.m_gradeBand)][index];
    picked.m_itemId = static_cast<unsigned int>(entry.first);   // ORIG 仅写低 dword
    picked.m_zoneType = static_cast<char>(entry.second);

    CItem* item = G_CDataManager()->find_item(static_cast<int>(picked.m_itemId));
    if (item == 0)
        return false;
    if (!item->IsAvatarItem())                                  // vtable+0x10==1 才继续
        return false;

    CEquipItem* equip = static_cast<CEquipItem*>(item);
    picked.m_optionOrSkillIdx =
        static_cast<char>(get_rand_int(equip->getAvatarOptionSize()));

    if (!(index == 3 && (grade == 9 || grade == 0)))
    {
        picked.m_pad9 = 0;
        return true;
    }

    // 加成技能选择：ORIG 将 m_addCharacSkillMap[uid] 的 value map 拷贝到栈
    // （copy ctor 08180ca2 实证），找不到 grade 键时直接返回 false（不写 pad9）。
    // p167：ORIG 用**非 const** `_Rb_tree_iterator`（operator==/operator-> 非 const
    // 实例）+ `operator[]` 二次取值（find 判存在 + operator[] 取 map，P123），
    // 旧 const_iterator 多转换构造 ×2 + const 版比较。
    std::map<unsigned long,
             std::map<int, std::vector<LevelUpSkill> > >::iterator it =
        m_addCharacSkillMap.find(static_cast<unsigned long>(picked.m_itemId));
    if (it == m_addCharacSkillMap.end())
        return false;

    std::map<int, std::vector<LevelUpSkill> > skillMap =
        m_addCharacSkillMap[static_cast<unsigned long>(picked.m_itemId)];
    std::map<int, std::vector<LevelUpSkill> >::iterator sit = skillMap.find(grade);
    if (sit == skillMap.end())
        return false;

    std::vector<LevelUpSkill> skillVec = skillMap[grade];
    picked.m_optionOrSkillIdx =
        static_cast<char>(skillVec[get_rand_int(skillVec.size())].m_field4);

    picked.m_pad9 = 0;
    return true;
}

// ============================================================================
// CChattingEmoticonList ctor（ORIG 0x0837e418W；声明见 CDataManager.h：
// map@0/mapVec@0x18/mapInt@0x30 三 map 默认构造，编译器生成）
// ============================================================================

// ============================================================================
// NPC 装载小件（NpcScript.h 提供全局声明）
// ============================================================================

bool initNpcScript(const char* dir, const char* listPath)
{
    // [b28d-solo-CtorProducers_1 判词·P303] ORIG 0x089fc1a8：strncpy(base,dir,0x100)；
    // strlen 三独立重调（P48 无 CSE：'\\' 判定、'/' 判定、追加 dst 各一次），
    // 末字符非 '\\' 且非 '/' 时**目录分隔符追加**；尾部 xor$1/test/je 双 mov 岛
    // = (load 返 1)。旧「N/P75 实参装载时刻封顶」判词作废：该段正是 P303 的
    // `__builtin_strcat(dst,"\\")` 内建展开（movl $2,0x8; movl $str,0x4; movl
    // base,(%esp); call strlen; add base; 双 mov 中转; call memcpy 8 条形）。
    strncpy(g_npcScriptBaseDirectory, dir, 0x100);
    if (g_npcScriptBaseDirectory[strlen(g_npcScriptBaseDirectory) - 1] != '\\')
    {
        if (g_npcScriptBaseDirectory[strlen(g_npcScriptBaseDirectory) - 1] != '/')
        {
            __builtin_strcat(g_npcScriptBaseDirectory, "\\");
        }
    }
    // 岛形复现（同 isExistNPCScript 技巧）：je 落点 = mov $1 ⇒ r==1 返 true。
    // P0/N96 清绑：改 bool callee 裸形（P168a①）。
    if (!loadRDARScriptListFile(&g_npcScriptFileList, g_npcScriptBaseDirectory,
                                listPath, 0))
        return false;
    return true;
}

bool isExistNPCScript(const char* name)
{
    // ORIG 0x089fae70：call loadRDARScriptFile 之后
    // `xor $0x1,%eax; test %al,%al; je +7; mov $0x0,%eax; jmp end; +7: mov $1,%eax`
    // ——je 目标岛是 `mov $1`：load 返 1 ⇒ xor 后 0 ⇒ je ⇒ 返回 1。即 **ORIG 恒等
    // 透传 load 返回值**（早期转写判成 `return !call` 系极性问题，真 bug 修正）。
    // P0/N96 清绑：改 bool callee 裸形（P168a① `if (!call()) return false;`）。
    if (!loadRDARScriptFile(g_npcScriptBaseDirectory, name))
        return false;
    return true;
}

// ============================================================================
// CNPCDynamicInfoManager（ctor/dtor 0x083760f2W/0x08376106W；
// getNPCInfo 0x08581948W；add 0x08580668 报告 CNPCDynamicInfoManager/add.md）
// sizeof 0x18（ctor 0x083760f2W 仅 map 构造）。
class CNPCDynamicInfoManager
{
public:
    CNPCDynamicInfoManager();                       // ORIG 0x083760f2W
    ~CNPCDynamicInfoManager();                      // ORIG 0x08376106W
    CNPCDynamicInfo* getNPCInfo(unsigned int idx);  // ORIG W 0x08581948
    bool add(const CNPCScript* script);             // ORIG T 0x08580668

    std::map<unsigned int, CNPCDynamicInfo*> m_map;   // +0x00
};

CNPCDynamicInfoManager::CNPCDynamicInfoManager()
    : m_map()
{
}

CNPCDynamicInfo* CNPCDynamicInfoManager::getNPCInfo(unsigned int idx)
{
    std::map<unsigned int, CNPCDynamicInfo*>::iterator it = m_map.find(idx);
    return (it == m_map.end()) ? 0 : it->second;
}

bool CNPCDynamicInfoManager::add(const CNPCScript* script)
{
    // [b28d-solo-CtorProducers_1 判词·P0+P236+P292] ORIG 0x08580668：旧「N/P75+N89
    // 封顶」判词作废其可翻部分。ORIG 逐条：①第一次 `getIndex()` **内联实参**
    // （`call getIndex; mov %eax,0x4(%esp)`，无具名 idx 槽往返，P0/P236）；②守卫
    // `test %eax,%eax; setne %al; test %al,%al; je <new 块>` ⇒ 正极性 `!= 0` 早返
    // （我们旧 `== 0` 包裹产 sete + 反向布局，P292 调用点判别）；③第二次
    // `getIndex()` 结果落 -0xc 为编译器物化的 make_pair 实参临时（非具名局部）。
    // 残余：4.4 对 new-expression 布 landing pad（push esi/edi + operator delete +
    // _Unwind_Resume），ORIG 无（N100）；消解需 callee `CNPCDynamicInfo::C1` 加
    // `throw()`（声明 NPCDynamicInfo.h + 定义 CharacterAIA2.cpp）——跨 TU，已上报 Main。
    // ORIG 0x08580668：operator new(0x10) 后直接 C1 构造，无空指针检查
    // （GCC 4.4 -O0 平铺 `new X()` 恰为此形）。真实 ctor
    // _ZN15CNPCDynamicInfoC1EPK10CNPCScript（NPCDynamicInfo.h 权威声明，
    // 定义 CharacterAIA2.cpp W 0x0858191c）。§4.5 禁 extern "C" 桥。
    if (getNPCInfo(script->getIndex()) != 0)
        return false;
    CNPCDynamicInfo* info = new CNPCDynamicInfo(script);
    m_map.insert(std::make_pair(script->getIndex(), info));
    return true;
}

// ============================================================================
// TowerOfDespair（TowerOfDespair_APC_Mgr ctor 0x086444e6W 仅 map 构造；
// TowerOfDespairMgr 报告 TowerOfDespairMgr/*.md）
// ============================================================================

TowerOfDespair_APC_Mgr::TowerOfDespair_APC_Mgr()
    : m_APCInfoMap()
{
}

TowerOfDespairMgr::TowerOfDespairMgr()
{
    // [b28d-solo-CtorProducers_1 判词·P70/P253+P171] ORIG 0x08643ac4：旧「N78 memset
    // 内联封顶」判词作废。ORIG 逐条：set C1(+0x328) → vector C1(+0x340)（成员声明序
    // 自动）→ `movw $0,0x34c` → `movw $0,0x34e` → `movl $0,0x358` →
    // **`std::fill<const CDungeon**, const CDungeon*>`(this, this+0x194, &nullDungeon)** →
    // **`std::fill<unsigned*,int>`(this+0x194, this+0x328, &nullIndex)** →
    // `operator new(0x18)`+APC_Mgr C1 → store +0x354。
    // ⇒ 数组清零是具名零局部的 `std::fill` 调用（Ghidra C 段同证），非 memset；
    // 且 ORIG **不写** m_topLayer(+0x350)（P171 照抄不写，补写即多 2 条）。
    // [b28d-S435 修复·T1 ⇒ p167 CLEAR] ORIG 0x08643b33
    // `call _ZSt4fillIPPK8CDungeonS2_EvT_S4_RKT0_`（std::fill<const CDungeon**,
    // const CDungeon*>）⇒ 头 m_layerDungeons 改 `const CDungeon*[101]`（Main 代落）
    // + 本处 nullDungeon 改 const 指针；p167 由 MISSING/EXTRA 各 1 → CLEAR。
    // 残余：new APC_Mgr 的 landing pad（ORIG 亦布，故此处一致）。
    m_enterableCount = 0;
    m_minLevel = 0;
    m_field358 = 0;
    // [b28d-solo-CtorProducers_1 判词·P309] ORIG 0x08643ac4 槽序：
    // nullDungeon=-0x20（深）/ nullIndex=-0x1c（浅）。4.4 对函数头声明按
    // 「先声明浅」分配（与 4.1 相反）⇒ 全头部声明、声明序按 ORIG 槽浅→深
    // （先 nullIndex 后 nullDungeon），赋值（清零）位置保持原位。
    // [b28d-S435 修复·T1] 头 TowerOfDespairMgr.h m_layerDungeons 已改 const CDungeon*：
    // ORIG 0x08643b33 `call _ZSt4fillIPPK8CDungeonS2_EvT_S4_RKT0_`
    // （std::fill<const CDungeon**, const CDungeon*>）⇒ 零局部也须 const 指针。
    int nullIndex;
    const CDungeon* nullDungeon;
    nullDungeon = 0;
    std::fill(m_layerDungeons, m_layerDungeons + 101, nullDungeon);
    nullIndex = 0;
    std::fill(m_dungeonIndexOfLayer, m_dungeonIndexOfLayer + 101, nullIndex);
    m_apcMgr = new TowerOfDespair_APC_Mgr;
}

TowerOfDespairMgr::~TowerOfDespairMgr()
{
    // ORIG 0x08643be2：仅 delete m_apcMgr，**无置空**（0x354 槽无回写）；
    // 其余成员析构自动。4.4 对 vector/set D1 的双层 EH handler 为 N91 结构性。
    delete m_apcMgr;
}

void TowerOfDespairMgr::_Set_LayerByDungeonIndex(unsigned int layer, const CDungeon* dungeon)
{
    // ORIG 0x08643c6c：if (dungeon!=0) m_pLayerDungeon[layer]=dungeon。
    // P223：ORIG 尾空块 nop 岛（je <nop>; body; jmp <pop ebp>; nop）复现形 =
    // goto 尾标签（4.4 对 if/else 恒 jump-thread，不产该岛）。
    if (dungeon == 0)
        goto L_done;
    m_layerDungeons[layer] = dungeon;
L_done:;
}

void TowerOfDespairMgr::_Set_LayerByDungeonIndex(unsigned int layer, unsigned int dungeonIdx)
{
    // ORIG 0x08643c86：写 uint 表；set.insert(idx)；find_dungeon 命中转调重载。
    m_dungeonIndexOfLayer[layer] = dungeonIdx;
    m_layerSet.insert(static_cast<int>(dungeonIdx));
    // 技巧 37 免槽直用：ORIG 0x08643c?? 把 find_dungeon 的 %eax 直接写进
    // 下一 call 的 0x4(%esp)，**没有** `mov %eax,-slot; mov -slot,%eax` 往返
    // （我们具名 dungeon 局部造出该往返，多 2 条）。
    _Set_LayerByDungeonIndex(layer,
                             G_CDataManager()->find_dungeon(static_cast<int>(dungeonIdx)));
}

void TowerOfDespairMgr::_Set_APCIndexByLayer(unsigned int layer)
{
    // ORIG 0x08643cf4：m_npcLayers.push_back(layer)。
    m_npcLayers.push_back(layer);
}

int TowerOfDespairMgr::LoadScript(const char* dir, const char* path)
{
    // [b28d-solo-CtorProducers_1 判词·P164+P171] ORIG 0x08643e7e：旧「N82 内联块
    // 位置封顶」判词作废。本轮修：①外层 `while (ScanType(token,true))` → 4.4 产
    // 无 xor 的 `test; je`；改 `for(;;){ if(!ScanType(...)) break; … }` 复现 ORIG
    // `call; xor $0x1,%eax; test %al,%al; jne <退出块>`（P164 配套形）；②删除两处
    // `ok = false;`（ORIG 仅在函数头预清一次，P171 照抄不写）。
    // 残余：`_Set_LayerByDungeonIndex(layer, dungeonIdx)` 两实参装载序（ORIG 先
    // `mov -0x10,%edx` 再 `mov -0x14,%eax`，4.4 反）——N82/P75 封顶。
    // ORIG 0x08643e7e：失败返回 0x65；段式扫描后成功返回 0。
    if (!loadRDARScriptFile(dir, path))
        return 0x65;
    std::string token;
    bool ok = false;
    // P164 配套形：外层 `for(;;){ if(!ScanType(token,true)) break; … }` 复现
    // ORIG 的 `call ScanType; xor $0x1,%eax; test %al,%al; jne <退出块>` 头部
    // 条件块 + 取反物化（`while (ScanType(...))` 在 4.4 恒产无 xor 的 `test; je`）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[enterable count]")
            m_enterableCount = static_cast<unsigned short>(ScanInt((bool*)0));
        else if (token == "[min Level]")
            m_minLevel = static_cast<unsigned short>(ScanInt((bool*)0));
        else if (token == "[top layer]")
            m_topLayer = static_cast<unsigned short>(ScanInt((bool*)0));
        else if (token == "[each layer matching dungeon]")
        {
            for (;;)
            {
                unsigned int layer = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok) break;
                unsigned int dungeonIdx = static_cast<unsigned int>(ScanInt((bool*)0));
                // [b28d-solo-CtorProducers_1 判词·N/P75] ORIG 0x08643e7e 两实参
                // 先物化 %edx/%eax 再写 esp 槽（4.1 求值-压栈分离）；4.4 逐实参
                // 「载入-压栈」。语句表达式/纯 register 两形实测均劣化
                // （前者零效、后者帧 -8 且槽漂移）⇒ P75 跨版本封顶。
                _Set_LayerByDungeonIndex(layer, dungeonIdx);
            }
        }
        else if (token == "[npc layer]")
        {
            for (;;)
            {
                unsigned int layer = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok) break;
                _Set_APCIndexByLayer(layer);
            }
            std::sort(m_npcLayers.begin(), m_npcLayers.end());
        }
    }
    return 0;
}

// ============================================================================
// CLevelRewardTable（ctor 0x086975feW / SetJobAffection 0x08697766W /
//   GetProperJobIndex 0x08687e78 / GetProperLevelIndex 0x086876c8）
// ============================================================================

CLevelRewardTable::CLevelRewardTable()
{
    // [b28d-S435 修复·T1 二维化 ⇒ 归 AE] ORIG 0x086975fe：67 vs 67，DIFF 12→0。
    // 头 DataManagerInitDepends.h m_rewardPrimary/Secondary 改 `int[9][0x46]`
    // （Main 代落，同尺寸/偏移/mangled 不变），本处写 `m_rewardPrimary[j][l]`：
    // 4.4.6 逐条复现 ORIG `mov -0x10,%ecx; mov -0xc,%edx; mov 0x8,%eax;
    // imul $0x46,%ecx,%ecx; lea (%ecx,%edx,1),%edx; add $0x44,%edx;
    // movl $0x0,0x8(%eax,%edx,4)`（探针 /tmp/p_lrt.cpp f3 vs 一维 f1/f2/f4/f5/f6/f7）。
    // 旧「N4/N102 封顶」判词作废（一维形恒出 `imul; mov %eax,%edx; add 内存`）。
    // [b28d-solo-CtorProducers_1 修复①] ORIG 0x086975fe：67 vs 67。
    // 本轮修：①成员数组**取址形** `&m_nameStrings[21*i]` / `&m_messages[0x100*i]`
    // 复现 ORIG 的 `add $0x14c0; add this; add $8` 三步地址链（旧 `arr + 21*i`
    // 退化形被 4.4 折 `lea 0x14c8(%eax)`）；②零初始化/声明序不变。
    // ORIG 0x086975fe：i∈[0,0x45] 清 levelKeys[i]，名字槽(+0x14c8+21i)/留言槽
    // (+0x1a86+0x100i) 首字节 memcpy '1'（0x31，来自 rodata "127.0.0.1" 首字符，
    // 非 '\0'）；双层清空 primary(@+0x118)/secondary(@+0xaf0) 两块。
    // [b23S3C] 名字/留言槽 ORIG 出 call memcpy(len 1)（4.1 对 1 字节常量拷贝
    // 不内联；`= '1'` 直存产 movb 缺两条 call 形），改 memcpy("1",1) 直调。
    for (int i = 0; i <= 0x45; ++i)
    {
        m_levelKeys[i] = 0;
        memcpy(&m_nameStrings[21 * i], "1", 1);
        memcpy(&m_messages[0x100 * i], "1", 1);
    }
    for (int j = 0; j < 9; ++j)
        for (int l = 0; l <= 0x45; ++l)
        {
            m_rewardPrimary[j][l] = 0;
            m_rewardSecondary[j][l] = 0;
        }
    m_jobAffection = false;   // ORIG 0x086975fe 尾部：movb $0x0,0x6086(%eax)
}

void CLevelRewardTable::SetJobAffection(bool on)
{
    // ORIG 0x08697766W：mov %dl,0x6086(%eax)。
    m_jobAffection = on;
}

int CLevelRewardTable::GetProperJobIndex(char job)
{
    // ORIG 0x08687e78（83 字节）逐条：0x08687e84 `cmpb $0,-0xc; jns` → -1 岛；
    // 0x08687e9c `xor $1,%eax; test; je` → 0 岛（技巧 1）；
    // 0x08687eaa `movsbl -0xc,%eax; cmp $7,%eax; ja 0x08687ef4` +
    // 0x08687eb3 `mov 0x8cf10cc(,%eax,4),%eax; jmp *%eax` —— **8 路跳表**
    // （case 0..7 依次 mov $1..$8; jmp end，default 落 0x08687ef4 `mov $-1`）。
    // ⇒ 直写 switch 才能复现跳表（技巧 34）；`return job + 1` 的算式形不会生成
    //   `jmp *%eax` 与被调表项（原 ndiff 27 全在此）。
    if (job < 0)
        return -1;
    if (!GetJobAffection())
        return 0;
    switch (job)
    {
    case 0:  return 1;
    case 1:  return 2;
    case 2:  return 3;
    case 3:  return 4;
    case 4:  return 5;
    case 5:  return 6;
    case 6:  return 7;
    case 7:  return 8;
    default: return -1;
    }
}

int CLevelRewardTable::GetProperLevelIndex(unsigned int level)
{
    // ORIG 0x086876c8：扫 levelKeys，遇 0 止返回 -1，命中返回下标。
    for (int i = 0; i <= 0x45; ++i)
    {
        if (m_levelKeys[i] == 0)
            break;
        if (m_levelKeys[i] == level)
            return i;
    }
    return -1;
}

// ============================================================================
// CLevelRewardTableMgr（单例指针 B 0x09473978；ctor 0x08687710 /
// D1 0x0868777c / GetInstance 0x0837505bW / SetChallengeInfo 0x08687828）
// ============================================================================

CLevelRewardTableMgr* CLevelRewardTableMgr::levelRewardTableMgr = 0;

CLevelRewardTableMgr::CLevelRewardTableMgr()
{
    // ORIG 0x08687710：vec@+0x08 自动构造；flag=0；m_table 先置 0 再 new(0x6088)。
    m_flag = 0;
    m_table = 0;
    m_table = new CLevelRewardTable;
}

CLevelRewardTableMgr::~CLevelRewardTableMgr()
{
    // ORIG 0x0868777c：仅析构 vec@+0x08，不 delete m_table
}

CLevelRewardTableMgr* CLevelRewardTableMgr::GetInstance()
{
    // ORIG 0x0837505bW：判空 new 单例（无 __cxa_guard）。
    if (levelRewardTableMgr == 0)
        levelRewardTableMgr = new CLevelRewardTableMgr;
    return levelRewardTableMgr;
}

void CLevelRewardTableMgr::SetChallengeInfo(stUnlimitChallengeInfo& info)
{
    // [b28d-solo-CtorProducers_1 判词·P167 修复，残余 N82] ORIG 0x08687828：
    // 本轮修第二循环 `it->m_subIdx` 缓存（ORIG 该循环 operator-> 仅 4 次；
    // 旧写两遍致 EXTRA×1），修后 p167 CLEAR 50/50。
    // 本轮修（③）：`m_flag = info.m_field0` 直赋（P136/P292 bool→bool 直存，
    // 消 test/setne 物化；旧 `*(const char*)&info.m_field0` 形）。gap -4→-2。
    // 残余：帧 0x8c vs 0x90 与三段循环槽号整体平移（N82：4.1 不复用槽）⇒ 封顶。
    // ORIG 0x08687828 全量转写（报告双段 + objdump 逐条）：
    // 0x08687837 `movzbl (%eax)` + 0x0868783d `mov %dl,(%eax)` ⇒ flag 单字节；
    // 0x08687852 vec 赋值、0x08687868 SetJobAffection(true)；随后三段同构循环，
    // 每段 ORIG 都：① size() **只调一次并存具名槽**（-0x3c / -0x38 / -0x34）；
    // ② begin()/end() 各构造一次成持久迭代器槽（-0x64/-0x68、-0x6c/-0x70、
    //   -0x78/-0x7c）⇒ end() 不能写在循环条件里；③ 条件头 =
    //   `mov -0x30,%eax; cmp -0x3c,%eax; jge out`（0x086878ed）短路后再
    //   `call operator!=`（0x08687902）+ bool 物化岛 ⇒ 原码是 `i < sz && it != last`；
    // ④ 体内 `addl $1,-0x30` 之后 `call operator++(int)`（0x086878e5，带临时槽
    //   -0x58）⇒ **i++ 与 it++ 都是后置**，不是 ++it。
    // [b28d-solo-CtorProducers_1 修复·P136/P292] ORIG 0x08687837..0x0868783d
    // `movzbl (%eax),%edx; mov %dl,(%eax)` = bool→bool 直接字节存储（无 setne）；
    // 旧 `*(const char*)&info.m_field0` 引入 uchar→bool 物化（+2 条 test/setne）。
    m_flag = info.m_field0;
    m_vec8 = info.m_vec4;
    m_table->SetJobAffection(true);

    {
        const int sz = static_cast<int>(info.m_vec10.size());   // 0x0868784e → -0x3c
        std::vector<int>::const_iterator it = info.m_vec10.begin();
        std::vector<int>::const_iterator last = info.m_vec10.end();
        for (int i = 0; i < sz && it != last; i++, it++)
            m_table->m_levelKeys[i] = *it;
    }
    {
        // 0x0868794c size()→-0x38；begin→-0x6c、end→-0x70；i=-0x2c
        const int sz =
            static_cast<int>(info.m_vec1c.size());
        std::vector<stUnlimitChallengeRewardInfo>::const_iterator it =
            info.m_vec1c.begin();
        std::vector<stUnlimitChallengeRewardInfo>::const_iterator last =
            info.m_vec1c.end();
        for (int i = 0; i < sz && it != last; i++, it++)
        {
            unsigned int level = static_cast<unsigned int>(it->m_jobLevel);
            // p167：ORIG 该循环 `operator->` 仅 4 次（m_jobLevel/m_subIdx/m_a/m_b）
            // ⇒ `it->m_subIdx` 必须缓存一次后复用（旧写两遍致 EXTRA×1，Ghidra C 的
            // 第二次 GetProperJobIndex 用具名 local_28）。
            char subIdx = static_cast<char>(it->m_subIdx);
            int jobIdx = m_table->GetProperJobIndex(subIdx);
            int levelIdx = m_table->GetProperLevelIndex(level);
            m_table->m_rewardPrimary[jobIdx][levelIdx] = it->m_a;
            jobIdx = m_table->GetProperJobIndex(subIdx);
            levelIdx = m_table->GetProperLevelIndex(level);
            m_table->m_rewardSecondary[jobIdx][levelIdx] = it->m_b;
        }
    }
    {
        // 0x08687a30 size()→-0x34；begin→-0x78、end→-0x7c；i=-0x24
        const int sz =
            static_cast<int>(info.m_vec28.size());
        std::vector<stUnlimitChallengeMessageInfo>::const_iterator it =
            info.m_vec28.begin();
        std::vector<stUnlimitChallengeMessageInfo>::const_iterator last =
            info.m_vec28.end();
        for (int i = 0; i < sz && it != last; i++, it++)
        {
            unsigned int level =
                static_cast<unsigned int>(static_cast<unsigned short>(it->m_id));
            // ORIG 0x08687b8x/0x08687b9x：**两个 std::string 拷贝临时**先各自
            // 构造（local_80←it->m_msg1、local_84←it->m_msg2），再取 c_str() 交
            // findString，块尾按构造逆序析构（0x08687c7c 处 ~string×2，带 EH
            // CatchHandler 0x08687c94/0x08687c7f）⇒ 不能直接对 it->m_msgN 调
            // c_str()（P-H：拷贝临时本身就是 ORIG 的指令来源）。
            std::string msg1 = it->m_msg1;
            std::string msg2 = it->m_msg2;
            const char* p1 = g_scriptStringManager_.findString(4, msg1.c_str(), 0);
            int nameIdx = m_table->GetProperLevelIndex(level);
            strncpy(m_table->m_nameStrings + nameIdx * 0x15, p1, 0x14);
            const char* p2 = g_scriptStringManager_.findString(4, msg2.c_str(), 0);
            int msgIdx = m_table->GetProperLevelIndex(level);
            strncpy(m_table->m_messages + msgIdx * 0x100, p2, 0xff);
        }
    }
}

// ============================================================================
// WorldMapScript（ctor 0x08369f08W / dtor 0x0837f5aeW / clear；
// importWorldMapScript 0x08ab0388 报告 _lt_global_gt_/importWorldMapScript.md）
// ============================================================================

WorldMapScript::WorldMapScript()
{
    // ORIG ctor 0x08369f08W：默认构造全部成员后调用 clear()（O0 下保留 call）。
    clear();
}

WorldMapScript::~WorldMapScript()
{
}

void WorldMapScript::clear()
{
    m_index = 0;
    m_name = "";              // ORIG 0x08369e74W：operator=(const char* "")（_ZNSsaSEPKc）
    m_mapImage = "";
    // ORIG clear 不清 m_uiPath(+0xc)；仅清 +0x4/+0x8 两个 string。
    m_field10 = 0;
    m_dungeons.clear();
    m_isHellDungeon = 0;
    m_hellQuests.clear();
    m_freepassItems.clear();
    m_itemConditions.clear();
    m_minimapPoints.clear();
}

bool importWorldMapScript(WorldMapScript* script, const char* path)
{
    // [b28d-solo-CtorProducers_1 判词·P234/P164/P317/P171/P0，残余 N82]
    // ORIG 0x08ab0388（ndiff 62→12、gap -20→-2）：本轮修
    // ①`std::pair<int,int>(tmp, second)` 第二参右值化（`static_cast<int&&>`，P234）；
    // ②外层 while → `for(;;){ if(!ScanType(...)) break; }` 复现头部
    //   `call; xor $0x1; test; jne 尾`（P164）；
    // ③`script->m_field10 = ScanInt(NULL)` 内联实参（P0，消 tmp 往返 3 条）；
    // ④isType 声明移到 wd.m_dungeonIndex 赋值之后（P171 照抄序）；
    // ⑤删 first/secondPoint 零初始化（P171）；⑥WorldMapScript.h 删
    //   WorldMapDungeon 用户空析构（P317，消 2×out-of-line D1 call+EH 段；
    //   D1/D2 W 符号仍发射、尺寸 5 与 ORIG 同——R2 已广播）。
    // 残余：`second = ScanInt()` 经复用槽 -0x40 的 2 条中转 + minimap 槽
    // -0x44/-0x48 vs ORIG -0x40/-0x44（4.4 块内标量赋值恒经复用槽；register/
    // 函数头声明/初始化式三形实测均无效或劣化）⇒ N82 封顶。
    // ORIG 0x08ab0388。
    if (!loadRDARScriptFile(g_worldMapScriptBaseDirectory, path))
        return false;
    script->clear();

    // 函数级局部序与宽度实证：0x08ab03bf `lea -0x24; call std::string::string()`
    // （token **先**构造）→ 0x08ab03ca `movl $0x0,-0x28`（int 暂存）→
    // 0x08ab03d1 `movb $0x0,-0x29`（bool flag）→ 0x08ab03d5 `jmp` 条件头
    // 0x08ab03d8 `movl $1,0x4; lea token; call ScanType; xor $1; test; jne 尾`。
    // ⇒ 外层是 while(ScanType(...))，且 tmp/flag 是**跨所有子块复用的函数级**
    // 局部（不是每块各一个），故各块写成 `tmp = ScanInt(&flag); if (!flag) break;`
    // （P-H：中间变量必须提到函数级并与 ORIG 同数量，否则槽号全漂）。
    std::string token;
    int tmp = 0;
    bool flag = false;
    // [b28d-solo-CtorProducers_1 修复·P164] ORIG 0x08ab03d5 `jmp` 到头部条件块
    // 0x08ab03d8（`call ScanType; xor $0x1; test; jne 尾`）；4.4 对 while 恒旋转到
    // 尾部 ⇒ 改 `for(;;){ if(!ScanType(...)) break; … }` 复现头部取反物化块。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[name]")
        {
            ScanStr(&script->m_name);
        }
        else if (token == "[map image]")
        {
            ScanStr(&script->m_mapImage);
            // [b28d-solo-CtorProducers_1 修复·P0] ORIG 0x08ab0449..0x08ab0451
            // `call ScanInt; mov 0x8(%ebp),%edx; mov %eax,0x10(%edx)` ⇒ 返回值
            // 直存成员，**无** tmp 槽往返（旧形多 3 条）。
            script->m_field10 = ScanInt((bool*)0);
        }
        else if (token == "[ui path]")
        {
            ScanStr(&script->m_uiPath);
        }
        else if (token == "[dungeon]")
        {
            // 0x08ab04a9..0x08ab055d：wd 基址 -0x38；`movb $0,-0x39` 是**独立 bool**
            // （在 wd 之前一字节），ScanTypeOrInt 实参 = (&b, token, &flag, true)
            // （0x08ab04f2 四实参），返回存 -0x28；`[in progress]` 命中写
            // **wd.m_flag=1**（0x08ab052c `movl $1,-0x30`），随后 `ScanInt(NULL)`
            // 覆盖 -0x28；最后 `mov -0x28,%eax; mov %eax,-0x34` ⇒ wd.m_state=tmp。
            script->m_dungeons.clear();
            for (;;)
            {
                tmp = ScanInt(&flag);
                if (!flag)
                    break;
                // [b28d-solo-CtorProducers_1 修复·P171 照抄序] ORIG 0x08ab04c7
                // 先构造 wd（-0x38）→ 0x08ab04d2 写 wd.m_dungeonIndex →
                // 0x08ab04d8 `movb $0x0,-0x39`（isType 初始化）⇒ isType 声明在
                // wd 之后（槽 -0x39 紧邻 wd 上方由编译器排布，非声明序）。
                WorldMapDungeon wd;
                wd.m_dungeonIndex = tmp;
                bool isType = false;
                // mangled _Z13ScanTypeOrIntRbRSsS_b = (bool&, string&, bool&, bool)，
                // 0x08ab04f2 传的是 `lea -0x39`（&isType）与 0x08ab04e7 的
                // `lea -0x29`（&flag）两个地址。
                tmp = ScanTypeOrInt(isType, token, flag, true);
                if (!flag)
                    break;
                if (isType)
                {
                    if (token == "[in progress]")
                        wd.m_flag = 1;
                    tmp = ScanInt((bool*)0);
                }
                wd.m_state = tmp;
                script->m_dungeons.push_back(wd);
            }
        }
        else if (token == "[hell dungeon]")
        {
            // 0x08ab057e..0x08ab0598：tmp = ScanInt(NULL)；`test;setne %dl`
            // 一次物化后 `mov %dl,0x20(%eax)` ⇒ 赋 bool 表达式（技巧 9）。
            tmp = ScanInt((bool*)0);
            script->m_isHellDungeon = (tmp != 0);
        }
        else if (token == "[hell quest]")
        {
            for (;;)
            {
                // 0x08ab05b7..0x08ab05eb：push_back 的实参是 `lea -0x28` ⇒
                // 传的是函数级 tmp 的**地址**（不是新局部 v）。
                tmp = ScanInt(&flag);
                if (!flag)
                    break;
                script->m_hellQuests.push_back(tmp);
            }
        }
        else if (token == "[hell freepass item]")
        {
            for (;;)
            {
                tmp = ScanInt(&flag);
                if (!flag)
                    break;
                // [b28d-solo-CtorProducers_1 实验·P212] ORIG 0x08ab062a..0x08ab062f
                // `call ScanInt; mov %eax,-0x18` 直存；块内声明致 4.4 中转 -0x40
                // （函数头声明实测槽全漂 ndiff 12→80，回退块内）。
                // [b28d-solo-CtorProducers_1 判词·N82] 残余 2 条中转 + minimap 槽
                // -0x44/-0x48 vs ORIG -0x40/-0x44（4.4 块内标量赋值恒经复用槽；
                // register/函数头声明/初始化式三形实测均无效或劣化）⇒ 封顶。
                // [b28d-S435 实验·三形同值] 块内「声明+赋值」/块内初始化式/分支块级
                // 三形实测 ndiff 均 12（4.4 恒经 -0x40 中转）⇒ 保持原形，P112 封顶。
                int second;
                second = ScanInt((bool*)0);   // ORIG 独立槽 -0x18
                // ORIG 0x08ab0646 `call _ZNSt4pairIiiEC1IRiiEEOT_OT0_`（pair<int,int>
                // 以两个 **int lvalue 引用** 实例化构造）+ 0x08ab065b
                // `push_back(EOS1_)` ⇒ 原码是嵌套 pair 临时构造，
                // 写 std::make_pair 会换成 _ZSt8make_pair 实例（P134）。
                // p167：ORIG 实例 `pair<int,int>::pair<int&,int>(int&, int&&)`
                // （第二参右值）⇒ `static_cast<int&&>(second)`（P234：纯语言构造，
                // 零 call）；旧两左值形产 `pair<int&,int&>` 实例差。
                script->m_freepassItems.push_back(
                    std::pair<int, int>(tmp, static_cast<int&&>(second)));
            }
        }
        else if (token == "[item condition]")
        {
            for (;;)
            {
                // 0x08ab0679..0x08ab06aa：先 ScanInt(&flag) 落 tmp，再无条件
                // ScanInt(NULL)（结果丢弃），之后才判 flag。
                tmp = ScanInt(&flag);
                ScanInt((bool*)0);
                if (!flag)
                    break;
                script->m_itemConditions.push_back(tmp);
            }
        }
        else if (token == "[minimap points]")
        {
            // 0x08ab06d6..0x08ab075c：外层 `tmp = ScanInt(&flag)` 后 while(flag)；
            // 内层 `for (k=0; k<2 && flag; ++k)`（0x08ab0715 `cmpl $1,-0xc; jg`
            // + 0x08ab071b `movzbl -0x29` 复合条件）；k==0 写 -0x44、否则写 -0x48
            // ⇒ 两个**独立具名 int**（不是数组，数组会走变址寻址）。
            tmp = ScanInt(&flag);
            while (flag)
            {
                // [b28d-solo-CtorProducers_1 修复·P171] ORIG 0x08ab06e6 直接进
                // for 头，**无** first/secondPoint 零初始化（多写即 +2 条）。
                int first;
                int secondPoint;
                for (int k = 0; k < 2 && flag; ++k)
                {
                    if (k == 0)
                        first = tmp;
                    else
                        secondPoint = tmp;
                    tmp = ScanInt(&flag);
                }
                script->m_minimapPoints.push_back(
                    std::pair<int, int>(first, secondPoint));
            }
        }
    }
    script->m_index =
        g_worldMapScriptFileList.FindIndexByFullScan(path);
    return true;
}

// ============================================================================
// CWorldMap（ctor 0x0837f6eaW / set_world_map 0x0836421a）
// ============================================================================

CWorldMap::CWorldMap()
{
    // [b28c-35] ORIG 0x0837f6ea 只发 4 个 vector 成员的隐式 C1（+0x10/+0x1c/
    // +0x28/+0x34），**不写** +0x00/+0x04/+0x08/+0x0c 标量；旧初始化列表多 8 条
    // store（63 vs 55）。标量由 set_world_map 赋值，C1 保持空体即逐条对齐。
}

void CWorldMap::set_world_map(WorldMapScript* script)
{
    // [b28d-solo-CtorProducers_1 判词·P96+P48+P292，残余 N82] ORIG 0x0836421a：
    // 旧「N/P168a③b+N82 封顶」判词作废其可翻部分。本轮修（p167 由 5 项差集→CLEAR
    // 31/31）：①迭代器改 **const_iterator** + 具名 `last`（ORIG 两枚 const 迭代器
    // 各自经转换构造 + 同型 ne）；②`getDungeonKind()` 两次独立调用（P48 无 CSE）；
    // ③else 分支直接读 `!dungeon->m_villageAttackEventFlag`（+0x87a bool，P292），
    // 删除 `ctors1::IsBlockWorldMapMark` helper（EXTRA×1）；④`if (dungeon != 0)`
    // 正向包裹替 continue（P296，Ghidra C 同形）。
    // 本轮修（②）：三段 assign 的 begin/end 临时 ORIG 各占独立一对
    // （-0x3c/-0x38、-0x34/-0x30、-0x2c/-0x28），4.4 原复用同一对 ⇒ 改**具名
    // 迭代器**拉长生命周期阻止复用（ndiff 211→151；end-first 声明序 155 稍差
    // 已回退）。残余：帧/槽基差（ORIG it=-0x40/last=-0x44 vs 我们 -0x30 区）+
    // 极性 fold（ORIG-only jmp×2/jne×1 vs OURS-only je/xor）⇒ N82 封顶。
    // ORIG 0x0836421a：标量回写 + 三段 vector assign + m_dungeonIndex=MAXINT；
    // 遍历 script->m_dungeons：find_dungeon 命中者 push 进 m_dungeonList，
    // 并按 kind==1/kind==2/其它三分支以 min 收敛 m_dungeonIndex（kind==1 另置
    // m_hasDeathTower=1）；kind 其它分支先查 dungeon+0x87a 字节跳过标记，
    // 再按 index==100 与 standard_level 收敛。
    m_index = script->m_index;
    m_isHellDungeon = script->m_isHellDungeon;
    // [b28d-solo-CtorProducers_1 实验·N82] 三段 assign 的 begin/end 临时槽
    // ORIG 各占独立一对（-0x3c/-0x38、-0x34/-0x30、-0x2c/-0x28），4.4 复用同槽。
    // 试具名迭代器拉长生命周期阻止复用。
    std::vector<int>::iterator hqBegin = script->m_hellQuests.begin();
    std::vector<int>::iterator hqEnd = script->m_hellQuests.end();
    m_field1c.assign(hqBegin, hqEnd);
    m_hasDeathTower = 0;
    std::vector<std::pair<int, int> >::iterator fpBegin = script->m_freepassItems.begin();
    std::vector<std::pair<int, int> >::iterator fpEnd = script->m_freepassItems.end();
    m_hellPassItemList.assign(fpBegin, fpEnd);
    std::vector<int>::iterator icBegin = script->m_itemConditions.begin();
    std::vector<int>::iterator icEnd = script->m_itemConditions.end();
    m_field34.assign(icBegin, icEnd);
    m_dungeonIndex = 0x7fffffff;
    // [b28d-solo-CtorProducers_1] Ghidra C + p167 实证三处修：
    // ①迭代器 **const_iterator**（ORIG `__normal_iterator<WorldMapDungeon const*>`；
    //   非 const 版多转换构造/实例差，P96/P270）；②`getDungeonKind()` **两次独立
    //   调用**（ORIG else 内再调一次，P48 无 CSE；旧 `int kind=` 缓存致 MISSING×1）；
    // ③else 分支条件直接读字段 `!dungeon->m_villageAttackEventFlag`（+0x87a bool，
    //   P292），旧 `ctors1::IsBlockWorldMapMark()` inline 调用致 EXTRA×1（已删该 helper）。
    // ORIG 两枚 const_iterator 各自经「非 const→const」转换构造（Ghidra C 两处
    // `__normal_iterator<const*>::__normal_iterator<WorldMapDungeon*>`）+ 同型
    // `ne(const,const)`；`it != script->m_dungeons.end()` 直写会产混合实例差。
    std::vector<WorldMapDungeon>::const_iterator it = script->m_dungeons.begin();
    std::vector<WorldMapDungeon>::const_iterator last = script->m_dungeons.end();
    while (it != last)
    {
        CDungeon* dungeon =
            G_CDataManager()->find_dungeon(it->m_dungeonIndex);
        // P296：Ghidra C 为正向包裹 `if (local_10 != 0) { body }`，无 continue 岛。
        if (dungeon != 0)
        {
            m_dungeonList.push_back(it->m_dungeonIndex);
            if (dungeon->getDungeonKind() == 1)
            {
                int minLevel = dungeon->get_min_level();
                m_dungeonIndex = std::min(m_dungeonIndex, minLevel);
                m_hasDeathTower = 1;
            }
            else if (dungeon->getDungeonKind() == 2)
            {
                int minLevel = dungeon->get_min_level();
                m_dungeonIndex = std::min(m_dungeonIndex, minLevel);
            }
            else if (!dungeon->m_villageAttackEventFlag)
            {
                if (dungeon->get_index() == 0x64)
                {
                    int minLevel = dungeon->get_min_level();
                    m_dungeonIndex = std::min(m_dungeonIndex, minLevel);
                }
                int standardLevel = dungeon->get_standard_level();
                m_dungeonIndex = std::min(m_dungeonIndex, standardLevel);
            }
        }
        ++it;
    }
}

// ============================================================================
// CWorldMapList（insert_world_map 0x08364694 / load_list 0x08364506；
// find_world_map/destroy/C1/D1 定义于 CDataManagerScripts.cpp）
// ============================================================================

bool CWorldMapList::insert_world_map(CWorldMap* worldMap)
{
    // [b28d-46 判词·N/N86] 沿用 b28c-lane25：73 vs 68，map::insert 的 4.1
    // 「临时槽+拷贝」vs 4.4 copy-elision（N86）⇒ 封顶。
    // b28c-lane25 判词 N86：ORIG 4.1 对 map::insert 的 pair/迭代器恒「临时槽+
    // 拷贝」两段，4.4 直构进目标槽（copy-elision）⇒ 73/68，无源码解（patterns N86）。
    // ORIG 0x08364694 逐条（报告 call 标注被 report_resolve 误标，以地址与 C 段为准）：
    //  ① 0x083646a5 `call get_index` → 结果落 -0x2c 作 find 实参；
    //  ② 0x083646c7 `call map::find` 到临时 -0x3c，再 0x083646d2 拷进持久迭代器槽 -0x30；
    //  ③ 0x083646e2 `call map::end` → 持久槽 -0x28；
    //  ④ 0x083646f7 `call _Rb_tree_iterator::operator==` + `test; je 0x0836475f`
    //     ⇒ **真路径 fall-through 是"未命中→插入"**（技巧 17），不是我们的"命中→报错"；
    //  ⑤ 插入分支 0x08364700 **再 call 一次 get_index** 落 -0xc，
    //     0x08364722 make_pair<int,CWorldMap*&>、0x08364737 pair<int const,CWorldMap*>::pair
    //     两级临时、0x08364750 map::insert；
    //  ⑥ 报错分支 0x0836475f **又 call 一次 get_index**（共 3 次）⇒ 不得缓存 idx 局部
    //     （P-H），且 find/end 都要具名持久槽（技巧 30 + 技巧 21 槽序）。
    // P50 复活（b28c C28L29）：ORIG 有独立 `_Rb_tree_iterator::C1Ev`（-0x30 槽）
    // + find 结果经 sret -0x3c 拷入 ⇒ 源形必为**先声明后赋值**；初始化式会被
    // 4.4 折叠成直构（少 C1Ev 与拷贝两段）。
    std::map<int, CWorldMap*>::iterator it;
    it = m_map.find(worldMap->get_index());
    std::map<int, CWorldMap*>::iterator last = m_map.end();
    // [b28d-solo-CtorProducers_1 判词·N93] ORIG it=-0x30（深）/last=-0x28（浅）
    // 为 4.1「先声明深」；4.4 对头声明 it（C1Ev 首用）给浅槽 -0x2c。last 必须保持
    // 初始化式（ORIG end 直构无 C1Ev），无法提前声明 ⇒ 槽方向不可翻（N93 类对象，
    // 标量例外不适用）。方案 A（last 先声明后赋值）实测 73→78 且多一条 C1Ev ⇒ 回退。
    if (it == last)
    {
        m_map.insert(std::make_pair(worldMap->get_index(), worldMap));
        return true;
    }
    LogManager::logFormat(
        1, "data_manager.cpp",
        __PRETTY_FUNCTION__, 0x2be9,
        "Already registed world map index detected : %d",
        worldMap->get_index());
    return false;
}

bool CWorldMapList::load_list()
{
    // [b28d-solo-CtorProducers_1 判词·P304+P50+P197③] ORIG 0x08364506：旧「N82 帧差
    // 封顶」判词作废。ORIG 逐条：①`movl $0x0,-0x20` 预清 name 在 script C1 **之前**、
    // `movl $0x0,-0x1c` 预清 worldMap 在 iterator C1Ev 之前（P197③ 声明=0 + 循环内
    // 独立赋值两段式）；②iterator 默认构造 + `it = begin()` 经 sret -0x8c 拷入
    // （P50 先声明后赋值）；③begin/end 的 this 直接 `movl $0x9501000,0x4(%esp)`
    // —— 旧 `reinterpret_cast<STFileListMapMirror*>(&g_worldMapScriptFileList)`
    // 使 4.4 物化寄存器（`mov $0,%eax; mov %eax,%edx`，P304）；STScriptFileList
    // 的 m_fileMap@+0 是权威同型成员，直接成员访问即消 cast。
    const char* name = 0;
    WorldMapScript script;
    CWorldMap* worldMap = 0;
    std::map<int, std::string>::iterator it;
    it = g_worldMapScriptFileList.m_fileMap.begin();
    for (; it != g_worldMapScriptFileList.m_fileMap.end(); it++)
    {
        name = it->second.c_str();
        if (name == 0)
            break;
        if (!importWorldMapScript(&script, name))
        {
            printf("World Map Script Error: %s\n", name);
            return false;
        }
        worldMap = new CWorldMap;
        worldMap->set_world_map(&script);
        insert_world_map(worldMap);
    }
    return true;
}

// ============================================================================
// CCharacList（ctor 0x083493ea / dtor 0x0834945e / destroy 0x083494da /
// init 0x08349514；声明见 CCharacList.h）
// ============================================================================

CCharacList::CCharacList()
{
    // ORIG：11 个 CCharacter 连续构造（步长 0x7dc），编译器生成。
}

CCharacList::~CCharacList()
{
    // ORIG 0x0834945e：destroy() 后逆序析构各元素（成员析构自动生成）。
    destroy();
}

void CCharacList::destroy()
{
    // ORIG 0x083494da：循环内**两个**函数级槽 —— i(-0x10) 与元素地址(-0xc)：
    // `imul $0x7dc; add this; mov %eax,-0xc; mov -0xc,%eax; mov (%esp); call`。
    // 4.4 默认把地址直接留在 %eax（copy-prop）⇒ 少 2 条，需具名中间指针
    //   才能保住那对 `mov %eax,-slot; mov -slot,%eax`（技巧 36 的反向用法）。
    for (int i = 0; i <= 0xa; ++i)
    {
        CCharacter* p = &m_characs[i];
        p->destroy();   // ORIG 0x083494da：_ZN10CCharacter7destroyEv 真实调用（CCharacter.h 声明）
    }
}

bool CCharacList::init()
{
    // [b28d-solo-CtorProducers_1 判词·P50+P96+P0] ORIG 0x08349514：旧「N89 迭代器
    // 实例差封顶」判词作废。ORIG 逐条实证：①`movl $0x0,-0xc(%ebp)` 先清 char* 局部
    // （P244 具名局部，声明在最前）；②iterator **默认构造** `_Rb_tree_iterator::C1Ev`
    // 占 -0x18，再 `it = begin()` 经 sret -0xc3c 拷入（P50 复活：先声明后赋值）；
    // ③STCharacterScript 在 -0xc34（声明序在 iterator 之后）；④`operator->`/`ne`
    // 均为 **_Rb_tree_iterator（非 const）** 的 const 成员 `_ZNKSt17_Rb_tree_iterator…`
    // （P96/P270：勿用 const_iterator，否则多转换构造）；⑤返回值 `mov $0/%ebx` 直返，
    // 无 bool 局部槽（P0 槽-局部双向对应律）。
    // 增量段必须写 `it++`（P28 增量侧推论：ORIG 增量段调 `_Rb_tree_iterator::ppEi`
    // 后置版并带独立 sret 临时槽 -0x10 ⇒ 迭代器/end/临时三槽阶梯；`++it` 走 ppEv
    // 免临时槽，后续槽整体上移一格）。
    const char* name = 0;
    std::map<int, std::string>::iterator it;
    STCharacterScript script;
    for (it = g_stCharacterScriptFileList.m_fileMap.begin();
         it != g_stCharacterScriptFileList.m_fileMap.end(); it++)
    {
        name = it->second.c_str();
        if (name == 0)
            break;
        if (!ImportCharacterScript(&script, name))
        {
            printf("Charac Script Error: %s\n", name);
            return false;
        }
        m_characs[script.m_job].set_data(&script);   // 真实成员调用 _ZN10CCharacter8set_dataEP17STCharacterScript
    }
    return true;
}

// ============================================================================
// CAICharacter::set（0x0834a066 报告 CAICharacter/set.md）
// 本 TU 局部完整视图（与 MonsterIndependentDrop.h 镜像共享前缀 0x30，
// 补齐 +0x30..+0x74 尾段至 ORIG 尺寸；跨 TU 同前缀保证既有实现不变）。
// ============================================================================

class CAICharacterFull   // ORIG 0x74；前缀 0x30 与 MonsterIndependentDrop.h 镜像一致
{
public:
    CAICharacterFull();                                  // ORIG W 0x08373a9a（成员自动）
    ~CAICharacterFull();                                 // ORIG W 0x08373b32

    // [b28d-solo-CtorProducers_1] +0x00 声明 unsigned：P211 异型化旋钮——ORIG
    // `mov 0xc(%ebp),%edx; mov 0x8(%ebp),%eax; mov %edx,(%eax)` 是 RHS-first
    // 直存；同型 int 字段在 4.4 恒 LHS-first（`mov 0x8(%ebp),%eax` 先）。
    unsigned int m_idx;                                 // +0x00
    int m_level;                                        // +0x04
    int m_field8;                                       // +0x08
    int m_appearancePoint;                              // +0x0c
    std::vector<std::pair<int, int> > m_dropItems;      // +0x10
    int m_pvpWinLow;                                    // +0x1c
    int m_pvpWinHigh;                                   // +0x20
    std::vector<stMonsterIndependentDrop_t> m_independentDrops;  // +0x24
    char m_avatarDrops[0x6c - 0x30];                    // +0x30（stEnamyDropAvatarInfo_t 区）
    std::string m_name;                                 // +0x6c
    unsigned char m_rarity;                             // +0x70
    char m_pad71[3];                                    // → sizeof 0x74
};

CAICharacterFull::CAICharacterFull()
    : m_idx(0), m_level(0), m_field8(0), m_appearancePoint(0),
      m_dropItems(), m_pvpWinLow(0), m_pvpWinHigh(0),
      m_independentDrops(), m_avatarDrops(), m_name(), m_rarity(0)
{
    memset(m_avatarDrops, 0, sizeof(m_avatarDrops));
}

CAICharacterFull::~CAICharacterFull() {}

// §4.5 例外：CAICharacter::set 符号经 asm 别名桥提供（保留）。
// 无法转真实成员调用的原因：`STAICharacterCommonScript` 完整 0x1010 布局仅
// 存在本 TU / CAICharacterList.cpp 的局部视图，sync_script.h 现存的 8 字节
// 占位（m_field0+m_str4）与本 TU 冲突（本 TU 已 include sync_script.h），
// 无法在此重定义全布局类型以产生精确 mangled `_ZN12CAICharacter3setEiRK25
// STAICharacterCommonScript`。`CAICharacter` 亦无跨 TU 权威头。待 sync_script
// 域统一 STAICharacterCommonScript 权威全布局 + 建 CAICharacter 权威头后，
// 改真实成员定义。
// 【b22 建模评估·维持例外】统一权威头专项核账发现本域四视图（本 TU 两 view/
// CAICharacterList 0x1014/sync_script 占位/CharacterAIA2 完整类）存在未裁决
// 的头部偏移矛盾：ORIG C1 0x088c8a78 首指令 `movl $0x8e0a1f0,(%eax)` 实证
// vptr@+0x00（多态），而三个消费视图以 +0x00 作 m_field0/int 承载；且
// +0x04 槽在 sync_script.cpp:921 按 std::string(m_str4) 消费、在
// CharacterAIA2 类视角为 int——视图仅靠"各自函数不触矛盾字段"维持二进制
// 兼容。统一即需逐字段重校四视图+迁移 sync_script.h 全部 10+ 消费 TU 与
// CharacterAIA2/S22x 在途工作面，非本单粒度可收口；盲改任何一处即静默偏移
// 错位。判定：asm 别名桥保留为终判，域统一另开 AI script 专项单（本注释即
// 其工单输入）。
// §4.5 asm 别名桥例外（四视图偏移矛盾未裁决，无法转真实成员定义）。
// 形参直接取权威视图类型：ABI 与 ORIG 成员函数 `(int, const&)` 同布局
// （this@0x8 / idx@0xc / script@0x10），避免旧 `void*` 形参 + 具名 cast 局部
// 产生的两级槽往返（P0：ORIG 无槽往返 ⇒ 原码直接使用形参）。
void sub_CAISet(CAICharacterFull* self, int idx, const STAICharScriptView* script)
    asm("_ZN12CAICharacter3setEiRK25STAICharacterCommonScript");

void sub_CAISet(CAICharacterFull* self, int idx, const STAICharScriptView* script)
{
    // [b28d-solo-CtorProducers_1 判词·P0+真bug] ORIG 0x0834a066 逐条：
    // ①`self+0x24 ← script+0xf2c` = `vector<stMonsterIndependentDrop_t>::operator=`
    //   （0x08388558）——**旧实现完全漏掉此赋值**（真语义 bug：独立掉落表不更新）；
    // ②`self+0x30 ← script+0xf50` = 单次 `stEnamyDropAvatarInfo_t::operator=`
    //   （0x08366ea8，POD 0x30 + vector 深拷贝在 callee 内），旧实现拆成
    //   memcpy+手工 vector 赋值两个调用（多 0x30 实参 + 视图指针槽）。
    // 顺序照 ORIG：idx/level/d80/d84/d88/da4/da8/f2c/f50/str4/rarity。
    self->m_idx = idx;
    self->m_level = script->m_level;
    self->m_field8 = script->m_d80;
    self->m_appearancePoint = script->m_d84;
    self->m_dropItems = script->m_d88;
    self->m_pvpWinLow = script->m_da4;
    self->m_pvpWinHigh = script->m_da8;
    self->m_independentDrops = script->m_f2c;
    *reinterpret_cast<stEnamyDropAvatarInfo_t*>(self->m_avatarDrops) = script->m_f50;
    self->m_name = script->m_str4;
    self->m_rarity = script->m_rarityByte;
}

// ============================================================================
// CNPCScriptList ctor/initScript（ctor 0x08376262W；initScript 0x08581314；
// find/_destroy/~ 定义于 CDataManagerScripts.cpp）
// ============================================================================

CNPCScriptList::CNPCScriptList()
    : m_npcMap(),
      m_npcCommon()
{
}

int CNPCScriptList::initScript(char* dir, char* list, char* common)
{
    // [b28d-solo-CtorProducers_1 判词·P304+P157/P48，残余 N82] ORIG 0x08581314：
    // 旧「N82+N89 封顶」判词作废其可翻部分。本轮修：①去掉
    // `reinterpret_cast<STFileListMapMirror*>(&g_npcScriptFileList)` 的 P304 寄存器
    // 物化（STScriptFileList::m_fileMap@+0 权威同型成员直访问）；②p167 MISSING
    // 3×`operator->`+3×`c_str` ⇒ `it->second.c_str()` 4 处全部内联（P157/P48，
    // 旧具名 name 缓存）；修后 p167 CLEAR（39/39）。
    // 残余：4.1 不复用栈槽（begin/end sret 落 -0x80/-0x7c，共 ~0x48 额外槽）vs
    // 4.4 复用（-0x44 起）⇒ 全帧槽号错位（N82），语义/调用点已闭合。
    // ORIG 0x08581314。
    if (!initNpcScript(dir, list))
        return 0x1a1;

    // [b28d-solo-CtorProducers_1] P304：旧 `reinterpret_cast<STFileListMapMirror*>(
    // &g_npcScriptFileList)` 使 begin/end 的 this 经寄存器物化；STScriptFileList
    // 的 m_fileMap@+0 是权威同型成员 ⇒ 直接成员访问发 `movl $0x94fe780,0x4(%esp)`。
    // P157/P48：p167 差集 MISSING 3×`operator->`+3×`c_str` ⇒ ORIG 每个使用点重读
    // `it->second.c_str()`（4 处），禁具名 name 缓存。
    for (std::map<int, std::string>::const_iterator it =
             g_npcScriptFileList.m_fileMap.begin();
         it != g_npcScriptFileList.m_fileMap.end(); ++it)
    {
        if (isExistNPCScript(it->second.c_str()))
        {
            CNPCScript* npc = new CNPCScript;
            if (!npc->load(it->second.c_str()))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x1b0, 5);
                trace("%s load failed\n", it->second.c_str());
                return 0x1b4;
            }
            m_npcMap.insert(
                std::make_pair(static_cast<unsigned int>(npc->getIndex()), npc));
            if (npc->isFavorableNPC())
                G_CDataManager()->m_npcDynamicInfo->add(npc);
        }
        else
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1a9, 5);
            trace("%s file not exists\n", it->second.c_str());
        }
    }

    if (loadNPCCommonData(m_npcCommon, common) != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1be, 5);
        trace("%s load failed\n", common);
        return 0x1bf;
    }
    if (m_npcCommon.m_favorableNpcList.size() > 0x32)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1c3, 5);
        trace("%s Favorable npc count over\n", common);
        return 0x1c4;
    }
    TimerNPCMoodChange::registNextTimer(0);
    return 0;
}

// ============================================================================
// CChattingEmoticonList ctor / CNPCDynamicInfoManager dtor /
// TowerOfDespair_APC_Mgr dtor（编译器生成等价体）
// ============================================================================

CChattingEmoticonList::CChattingEmoticonList() {}

CNPCDynamicInfoManager::~CNPCDynamicInfoManager() {}

TowerOfDespair_APC_Mgr::~TowerOfDespair_APC_Mgr() {}
