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

namespace ctors1
{
inline bool IsBlockWorldMapMark(const CDungeon* d)
{
    // ORIG set_world_map 分支：dungeon+0x87a（m_villageAttackEventFlag）非 0 跳过收敛。
    return d->m_villageAttackEventFlag != 0;
}
}  // namespace ctors1


// ---- STAICharacterCommonScript 权威视图（sizeof 0x1010；ORIG 同名类型，
//      sync_script.h 的 8 字节占位与本视图冲突故本 TU 不含该头）----
struct STAICharScriptView
{
    int m_field0;                                   // +0x000
    std::string m_str4;                             // +0x004
    int m_level;                                    // +0x008（set 读）
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
    char m_f50[0x100c - 0xf50];                     // stEnamyDropAvatarInfo_t 区
    unsigned char m_rarityByte;                     // +0x100c
    char m_pad100d[3];
};  // sizeof 0x1010

// stEnamyDropAvatarElement_t 同构（3×int，sizeof 0xc；权威定义 STMonsterScript.h）。
// 文件作用域（GCC 4.4 不允许局部类型作模板实参），仅 sub_CAISet 深拷贝用。
struct EnamyAvatarDropElemL
{
    int m0;
    int m4;
    int m8;
};
typedef std::vector<EnamyAvatarDropElemL> EnamyAvatarDropVecL;

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
    // ORIG 0x0817f706。
    m_pRoulette = new AvatarRoulette;
    m_pRoulette->swap(roulette);
    m_totalCount = m_pRoulette->m_upperAvatarRate + m_pRoulette->m_normalAvatarRate;

    bool result = true;   // ORIG movb $0x1,-0x45；失败路径写 0，最终返回常量 true
    // 技巧 31（槽号对齐）：ORIG 把 result 放在 **-0x45**、循环 int 在 -0x44、
    // 指针在 -0x40，我方整体比 ORIG **近 4 字节**（-0x41/-0x40/-0x3c）。
    // 4.4 按声明序由近及远分配 ⇒ 在 result **之后**插一个 4B dead 局部，
    // 即可把 result 及其后所有槽整体推远 4B，一次对齐三处引用。
    unsigned int gapSlot;
    if (G_CEnvironment()->m_serverEnvir.m_validateScript)
    {
        for (int job = 0; job < 0xb; ++job)
        {
            std::vector<std::vector<std::pair<unsigned long, int> > >& outer =
                m_pRoulette->m_avatars[job];
            size_t lines = outer.size();
            for (size_t li = 0; li < lines; ++li)
            {
                std::vector<std::pair<unsigned long, int> >& inner = outer[li];
                for (int type = 0; type < 9; ++type)
                {
                    int itemNo = inner[type].first;
                    CItem* item = G_CDataManager()->find_item(itemNo);
                    if (item == 0)
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x80,
                            "AvatarRouletteServer not exist item_no(%d)", itemNo);
                        result = false;   // ORIG movb $0x0,-0x45（无 continue）
                    }
                    if (!item->IsAvatarItem())
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x86,
                            "AvatarRouletteServer no avatar item_no(%d)", itemNo);
                    }
                    result = false;   // ORIG movb $0x0,-0x45
                    if (!item->check_job_type(static_cast<char>(job)))
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x8d,
                            "AvatarRouletteServer item_no(%d) check_job_type(%d)==false",
                            itemNo, job);
                    }
                    result = false;   // ORIG movb $0x0,-0x45
                    unsigned int itemType = item->GetItemType();
                    if (itemType != static_cast<unsigned int>(type))
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x95,
                            "AvatarRouletteServer item_no(%d) GetItemType(%d)!=%d",
                            itemNo, itemType, type);
                    }
                    result = false;   // ORIG movb $0x0,-0x45
                    if (type != 8)
                    {
                        int grade = item->get_grade();
                        if (li < 2 && grade != 2)
                        {
                            LogManager::logFormat(
                                1, "localjapan/Arad_AvatarRoulette.cpp",
                                __PRETTY_FUNCTION__, 0xa3,
                                "AvatarRouletteServer item_no(%d) get_grade(%d)!=%d, line_num(%d)",
                                itemNo, grade, 2, static_cast<int>(li));
                        
                            result = false;   // ORIG movb $0x0,-0x45
                        }
                        else if (li > 1 && grade != 1)
                        {
                            LogManager::logFormat(
                                1, "localjapan/Arad_AvatarRoulette.cpp",
                                __PRETTY_FUNCTION__, 0xa9,
                                "AvatarRouletteServer item_no(%d) get_grade(%d)!=%d, line_num(%d)",
                                itemNo, grade, 1, static_cast<int>(li));
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
    // ORIG 0x0817f386。
    if (G_CEnvironment()->m_serverEnvir.m_validateScript)
    {

    for (int job = 0; job < 0xb; ++job)
    {
        std::vector<std::vector<std::pair<unsigned long, int> > >& outer =
            m_pRoulette->m_avatars[job];
        size_t lines = outer.size();
        for (size_t li = 0; li < lines; ++li)
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
    // ORIG 0x0817fb28（报告 AvatarRouletteServer/Pickup.md 全文）。
    // 档位抽取：randVal<roulette+0x0c 时 band∈{0,1}，否则从
    // m_avatars[0].size()-2 偏移取 band（内联 size helper 08180760 实证）。
    int randVal = get_rand_int(m_totalCount);
    if (randVal < m_pRoulette->m_upperAvatarRate)
    {
        picked.m_gradeBand = static_cast<char>(get_rand_int(2));
    }
    else
    {
        picked.m_gradeBand = static_cast<char>(
            get_rand_int(static_cast<int>(m_pRoulette->m_avatars[0].size()) - 2) + 2);
    }

    if (grade < 0 || grade > 10)
        return false;

    std::vector<std::vector<std::pair<unsigned long, int> > >& outer =
        m_pRoulette->m_avatars[grade];                 // roulette + grade*0xc + 0x14
    if (static_cast<unsigned int>(picked.m_gradeBand) >= outer.size())
        return false;
    std::vector<std::pair<unsigned long, int> >& inner =
        outer[static_cast<int>(picked.m_gradeBand)];
    if (static_cast<unsigned int>(index) >= inner.size())
        return false;

    std::pair<unsigned long, int>& entry = inner[index];
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
    std::map<unsigned long,
             std::map<int, std::vector<LevelUpSkill> > >::const_iterator it =
        m_addCharacSkillMap.find(static_cast<unsigned long>(picked.m_itemId));
    if (it == m_addCharacSkillMap.end())
        return false;

    std::map<int, std::vector<LevelUpSkill> > skillMap(it->second);
    std::map<int, std::vector<LevelUpSkill> >::const_iterator sit =
        skillMap.find(grade);
    if (sit == skillMap.end())
        return false;

    std::vector<LevelUpSkill> skillVec(sit->second);
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
    // ORIG 0x089fc1a8：strncpy(base,dir,0x100)；strlen 三独立重调（P48 无 CSE：
    // '\\' 判定、'/' 判定、memcpy dst 各一次），末字符非 '\\' 且非 '/' 时
    // memcpy(base+len, "\\", 2)；尾部 xor$1/test/je 双 mov 岛 = (load 返 1)。
    strncpy(g_npcScriptBaseDirectory, dir, 0x100);
    if (g_npcScriptBaseDirectory[strlen(g_npcScriptBaseDirectory) - 1] != '\\')
    {
        if (g_npcScriptBaseDirectory[strlen(g_npcScriptBaseDirectory) - 1] != '/')
        {
            memcpy(g_npcScriptBaseDirectory + strlen(g_npcScriptBaseDirectory), "\\", 2);
        }
    }
    // 岛形复现（同 isExistNPCScript 技巧）：je 落点 = mov $1 ⇒ r==1 返 true。
    register bool hit __asm__("al");
    hit = !loadRDARScriptListFile(&g_npcScriptFileList, g_npcScriptBaseDirectory,
                                  listPath, 0);
    if (hit)
        return false;
    return true;
}

bool isExistNPCScript(const char* name)
{
    // ORIG 0x089fae70：call loadRDARScriptFile 之后
    // `xor $0x1,%eax; test %al,%al; je +7; mov $0x0,%eax; jmp end; +7: mov $1,%eax`
    // ——je 目标岛是 `mov $1`：load 返 1 ⇒ xor 后 0 ⇒ je ⇒ 返回 1。即 **ORIG 恒等
    // 透传 load 返回值**（早期转写判成 `return !call` 系极性问题，真 bug 修正）。
    // 复现岛形用技巧 1/22：register bool asm("al") 绑 !call，再 `if (t) return
    // false; return true;`（4.4 直返会被折掉 xor+岛 6 条）。
    register bool t __asm__("al");
    t = !loadRDARScriptFile(g_npcScriptBaseDirectory, name);
    if (t)
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
    unsigned int idx = const_cast<CNPCScript*>(script)->getIndex();
    if (getNPCInfo(idx) == 0)
    {
        // ORIG 0x08580668：operator new(0x10) 后直接 C1 构造，无空指针检查
        // （GCC 4.4 -O0 平铺 `new X()` 恰为此形）。真实 ctor
        // _ZN15CNPCDynamicInfoC1EPK10CNPCScript（NPCDynamicInfo.h 权威声明，
        // 定义 CharacterAIA2.cpp W 0x0858191c）。§4.5 禁 extern "C" 桥。
        CNPCDynamicInfo* info = new CNPCDynamicInfo(script);
        m_map.insert(std::make_pair(idx, info));
        return true;
    }
    return false;
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
    // ORIG 0x08643ac4：CDungeon*[101]@0 与 uint[101]@0x194 清零；
    // ushort 0x34c/0x34e/0x350=0；dword 0x358=0；new APC_Mgr(0x18)@0x354。
    memset(m_layerDungeons, 0, sizeof(m_layerDungeons));
    memset(m_dungeonIndexOfLayer, 0, sizeof(m_dungeonIndexOfLayer));
    m_enterableCount = 0;
    m_minLevel = 0;
    m_topLayer = 0;
    m_apcMgr = new TowerOfDespair_APC_Mgr;
    m_field358 = 0;
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
    if (dungeon != 0)
        m_layerDungeons[layer] = const_cast<CDungeon*>(dungeon);
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
    // ORIG 0x08643e7e：失败返回 0x65；段式扫描后成功返回 0。
    if (!loadRDARScriptFile(dir, path))
        return 0x65;
    std::string token;
    bool ok = false;
    while (ScanType(token, true))
    {
        if (token == "[enterable count]")
            m_enterableCount = static_cast<unsigned short>(ScanInt((bool*)0));
        else if (token == "[min Level]")
            m_minLevel = static_cast<unsigned short>(ScanInt((bool*)0));
        else if (token == "[top layer]")
            m_topLayer = static_cast<unsigned short>(ScanInt((bool*)0));
        else if (token == "[each layer matching dungeon]")
        {
            ok = false;
            for (;;)
            {
                unsigned int layer = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok) break;
                unsigned int dungeonIdx = static_cast<unsigned int>(ScanInt((bool*)0));
                _Set_LayerByDungeonIndex(layer, dungeonIdx);
            }
        }
        else if (token == "[npc layer]")
        {
            ok = false;
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
    // ORIG 0x086975fe：i∈[0,0x45] 清 levelKeys[i]，名字槽(+0x14c8+21i)/留言槽
    // (+0x1a86+0x100i) 首字节 memcpy '1'（0x31，来自 rodata "127.0.0.1" 首字符，
    // 非 '\0'）；双层清空 primary(@+0x118)/secondary(@+0xaf0) 两块。
    // [b23S3C] 名字/留言槽 ORIG 出 call memcpy(len 1)（4.1 对 1 字节常量拷贝
    // 不内联；`= '1'` 直存产 movb 缺两条 call 形），改 memcpy("1",1) 直调。
    for (int i = 0; i <= 0x45; ++i)
    {
        m_levelKeys[i] = 0;
        memcpy(m_nameStrings + 21 * i, "1", 1);
        memcpy(m_messages + 0x100 * i, "1", 1);
    }
    for (int j = 0; j < 9; ++j)
        for (int l = 0; l <= 0x45; ++l)
        {
            m_rewardPrimary[j * 0x46 + l] = 0;
            m_rewardSecondary[j * 0x46 + l] = 0;
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
    m_flag = *reinterpret_cast<const char*>(&info.m_field0);   // ORIG movzbl (%eax) 读 1 字节
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
            int jobIdx = m_table->GetProperJobIndex(static_cast<char>(it->m_subIdx));
            int levelIdx = m_table->GetProperLevelIndex(level);
            m_table->m_rewardPrimary[jobIdx * 0x46 + levelIdx] = it->m_a;
            jobIdx = m_table->GetProperJobIndex(static_cast<char>(it->m_subIdx));
            levelIdx = m_table->GetProperLevelIndex(level);
            m_table->m_rewardSecondary[jobIdx * 0x46 + levelIdx] = it->m_b;
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
    while (ScanType(token, true))
    {
        if (token == "[name]")
        {
            ScanStr(&script->m_name);
        }
        else if (token == "[map image]")
        {
            ScanStr(&script->m_mapImage);
            // 0x08ab0442..0x08ab0451：ScanInt(NULL) 先落 -0x28（tmp）再
            // `mov %eax,0x10(%edx)` ⇒ 两步经函数级暂存。
            tmp = ScanInt((bool*)0);
            script->m_field10 = tmp;
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
                int second = ScanInt((bool*)0);   // ORIG 独立槽 -0x18
                // ORIG 0x08ab0646 `call _ZNSt4pairIiiEC1IRiiEEOT_OT0_`（pair<int,int>
                // 以两个 **int lvalue 引用** 实例化构造）+ 0x08ab065b
                // `push_back(EOS1_)` ⇒ 原码是嵌套 pair 临时构造，
                // 写 std::make_pair 会换成 _ZSt8make_pair 实例（P134）。
                script->m_freepassItems.push_back(std::pair<int, int>(tmp, second));
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
                int first = 0;
                int secondPoint = 0;
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
    : m_index(0),
      m_isHellDungeon(0),
      m_dungeonIndex(0),
      m_hasDeathTower(0),
      m_dungeonList(),
      m_field1c(),
      m_hellPassItemList(),
      m_field34()
{
}

void CWorldMap::set_world_map(WorldMapScript* script)
{
    // ORIG 0x0836421a：标量回写 + 三段 vector assign + m_dungeonIndex=MAXINT；
    // 遍历 script->m_dungeons：find_dungeon 命中者 push 进 m_dungeonList，
    // 并按 kind==1/kind==2/其它三分支以 min 收敛 m_dungeonIndex（kind==1 另置
    // m_hasDeathTower=1）；kind 其它分支先查 dungeon+0x87a 字节跳过标记，
    // 再按 index==100 与 standard_level 收敛。
    m_index = script->m_index;
    m_isHellDungeon = script->m_isHellDungeon;
    m_field1c.assign(script->m_hellQuests.begin(), script->m_hellQuests.end());
    m_hasDeathTower = 0;
    m_hellPassItemList.assign(script->m_freepassItems.begin(),
                              script->m_freepassItems.end());
    m_field34.assign(script->m_itemConditions.begin(),
                     script->m_itemConditions.end());
    m_dungeonIndex = 0x7fffffff;
    for (std::vector<WorldMapDungeon>::iterator it = script->m_dungeons.begin();
         it != script->m_dungeons.end(); ++it)
    {
        CDungeon* dungeon =
            G_CDataManager()->find_dungeon(it->m_dungeonIndex);
        if (dungeon == 0)
            continue;
        m_dungeonList.push_back(it->m_dungeonIndex);
        int kind = dungeon->getDungeonKind();
        if (kind == 1)
        {
            m_dungeonIndex = std::min(m_dungeonIndex, dungeon->get_min_level());
            m_hasDeathTower = 1;
        }
        else if (kind == 2)
        {
            m_dungeonIndex = std::min(m_dungeonIndex, dungeon->get_min_level());
        }
        else if (!ctors1::IsBlockWorldMapMark(dungeon))
        {
            if (dungeon->get_index() == 0x64)
                m_dungeonIndex =
                    std::min(m_dungeonIndex, dungeon->get_min_level());
            m_dungeonIndex =
                std::min(m_dungeonIndex, dungeon->get_standard_level());
        }
    }
}

// ============================================================================
// CWorldMapList（insert_world_map 0x08364694 / load_list 0x08364506；
// find_world_map/destroy/C1/D1 定义于 CDataManagerScripts.cpp）
// ============================================================================

bool CWorldMapList::insert_world_map(CWorldMap* worldMap)
{
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
    std::map<int, CWorldMap*>::iterator it = m_map.find(worldMap->get_index());
    std::map<int, CWorldMap*>::iterator last = m_map.end();
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
    // ORIG 0x08364506：遍历 g_worldMapScriptFileList，逐个导入并 new CWorldMap(0x40)
    // set_world_map + insert_world_map；导入失败 printf 后返回 0。
    WorldMapScript script;
    for (std::map<int, std::string>::iterator it =
             reinterpret_cast<STFileListMapMirror*>(&g_worldMapScriptFileList)
                 ->m_fileMap.begin();
         it != reinterpret_cast<STFileListMapMirror*>(
                   &g_worldMapScriptFileList)->m_fileMap.end();
         it++)
    {
        const char* name = it->second.c_str();
        if (name == 0)
            break;
        if (!importWorldMapScript(&script, name))
        {
            printf("World Map Script Error: %s\n", name);
            return false;
        }
        CWorldMap* worldMap = new CWorldMap;
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
    // ORIG 0x08349514：栈上构造 STCharacterScript(0xc1c)，遍历
    // g_stCharacterScriptFileList：ImportCharacterScript 失败 printf 返回 0；
    // 否则按脚本首字 job 下标 set_data 到 m_characs[idx]。
    // 真实对象：编译器自动发 C1（0x08368348）于声明处、D1（0x08376362）于
    // 作用域末尾，与 ORIG 单点 C1/D1 重定位精确对齐（§4.5 禁 extern "C" 桥）。
    STCharacterScript script;

    bool result = true;
    for (std::map<int, std::string>::const_iterator it =
             g_stCharacterScriptFileList.m_fileMap.begin();
         it != g_stCharacterScriptFileList.m_fileMap.end(); ++it)
    {
        const char* name = it->second.c_str();
        if (name == 0)
            break;
        if (!ImportCharacterScript(&script, name))
        {
            printf("Charac Script Error: %s\n", name);
            result = false;
            break;
        }
        m_characs[script.m_job].set_data(&script);   // 真实成员调用 _ZN10CCharacter8set_dataEP17STCharacterScript
    }
    return result;
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

    int m_idx;                                          // +0x00
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
void sub_CAISet(void* self, int idx, const void* script)
    asm("_ZN12CAICharacter3setEiRK25STAICharacterCommonScript");

void sub_CAISet(void* self, int idx, const void* scriptRaw)
{
    typedef STAICharScriptView Script;
    const Script& script = *static_cast<const Script*>(scriptRaw);
    CAICharacterFull& selfC = *static_cast<CAICharacterFull*>(self);
    // ORIG 0x0834a066。
    selfC.m_idx = idx;
    selfC.m_level = script.m_level;
    selfC.m_field8 = script.m_d80;
    selfC.m_appearancePoint = script.m_d84;
    selfC.m_dropItems = script.m_d88;
    selfC.m_pvpWinLow = script.m_da4;
    selfC.m_pvpWinHigh = script.m_da8;
    // ORIG 0x08366ea8 stEnamyDropAvatarInfo_t::operator=：POD 区 0x30 拷贝 +
    // +0x30 vector<stEnamyDropAvatarElement_t> 深拷贝（memcpy 浅拷贝会共享
    // 底层数组，与 ORIG 语义不符）。
    memcpy(selfC.m_avatarDrops, script.m_f50, 0x30);
    EnamyAvatarDropVecL* dstV = reinterpret_cast<EnamyAvatarDropVecL*>(
        selfC.m_avatarDrops + 0x30);
    const EnamyAvatarDropVecL* srcV = reinterpret_cast<const EnamyAvatarDropVecL*>(
        script.m_f50 + 0x30);
    *dstV = *srcV;   // 深拷贝（目标 ctor 已 memset，无需先 clear）
    selfC.m_name = script.m_str4;
    selfC.m_rarity = script.m_rarityByte;
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
    // ORIG 0x08581314。
    if (!initNpcScript(dir, list))
        return 0x1a1;

    STFileListMapMirror& npcFiles =
        *reinterpret_cast<STFileListMapMirror*>(&g_npcScriptFileList);
    for (std::map<int, std::string>::const_iterator it =
             npcFiles.m_fileMap.begin();
         it != npcFiles.m_fileMap.end(); ++it)
    {
        const char* name = it->second.c_str();
        if (isExistNPCScript(name))
        {
            CNPCScript* npc = new CNPCScript;
            if (!npc->load(name))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x1b0, 5);
                trace("%s load failed\n", name);
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
            trace("%s file not exists\n", name);
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
