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
#include "CChattingEmoticonList.h"
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
extern STScriptFileList g_worldMapScriptFileList;         // B 0x9501000（NpcScript.h 类型；方法解析 ORIG 符号）
extern STFileListMapMirror g_aiCharacterScriptFileList;    // B 0x94f6a20
extern STFileListMapMirror g_stEquipmentScriptFileList;    // B 0x94fac00
extern char g_worldMapScriptBaseDirectory[0x104];      // B（InitScripts_5 定义域）

// ============================================================================
// 外部真实符号 asm 标签桥（完整类型未在本 TU 建模的 W/T 符号；
// 完整类型落地后由对应批次替换为正常调用并在交付报告销项）
// ============================================================================

// STCharacterScript（sizeof 0xc1c —— CCharacList::init ORIG 栈帧 local_c38[775] 实证）
void sub_STCharacterScriptC1(void*) asm("_ZN17STCharacterScriptC1Ev");
void sub_STCharacterScriptD1(void*) asm("_ZN17STCharacterScriptD1Ev");
bool sub_ImportCharacterScript(void*, const char*)
    asm("_Z21ImportCharacterScriptP17STCharacterScriptPKc");

// ---- STAICharacterCommonScript 权威布局视图（sizeof 0x1010 —— init 栈帧
//      -0x104c..-0x3c 实证；CAICharacter::set 访问最远字段 +0x100c。
//      sync_script.h 现存 8 字节占位与本视图冲突，故本 TU 不含该头，
//      CSyncScript 相关一律走 asm 桥；占位修正归 sync_script 域批次）----
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
    char m_padF38[0xf50 - 0xf3c];                   // +0xf3c..
    char m_avatarDrops[0x100c - 0xf50];             // +0xf50（stEnamyDropAvatarInfo_t 区，本 TU 不访问成员）
    unsigned char m_rarityByte;                     // +0x100c（set 读）
    char m_pad100d[3];
    int m_animationJob;                             // +0x1010（ORIG clear 置 0xb）
};  // sizeof 0x1014（ORIG STAICharacterCommonScript 布局视图）

void sub_STAIScriptC1(void*) asm("_ZN25STAICharacterCommonScriptC1Ev");
void sub_STAIScriptD1(void*) asm("_ZN25STAICharacterCommonScriptD1Ev");
void sub_STAIScriptClear(void*) asm("_ZN25STAICharacterCommonScript5clearEv");
bool sub_ImportAICharacterCommonScript(void*, const char*, bool)
    asm("_Z29importAICharacterCommonScriptP25STAICharacterCommonScriptPKcb");

// sync_script（仅指针域 + 成员方法桥）
namespace sync_script { class CSyncScript; }
sync_script::CSyncScript* sub_GetInstanceSyncScript()
    asm("_ZN11sync_script21GetInstanceSyncScriptEv");
bool sub_truncateAITable(void* self)
    asm("_ZN11sync_script11CSyncScript31truncate_aicharacter_info_tableEv");
bool sub_insertAIInfoToDB(void* self, int idx, const void* script)
    asm("_ZN11sync_script11CSyncScript29insert_aicharacter_info_to_dbEiRK25STAICharacterCommonScript");

// CAICharacterList::_add（T 0x0834a61c：m_charMap/m_byLevelList/m_byAPList
// 登记 + cMyTrace 失败路径 —— 报告 _add.md；归属后续 AI 列表批次收尾）
void sub_CAICharacterList_add(void* self, void* aiChar)
    asm("_ZN16CAICharacterList4_addEP12CAICharacter");

// NPC 动态信息 ctor（W 0x0858191c；CNPCDynamicInfo 完整类型归 NPC 动态域）
void sub_CNPCDynamicInfoC1(void*, const void*) asm("_ZN15CNPCDynamicInfoC1EPK10CNPCScript");


namespace DNFFLib { void Make_Dir(char* dir); }   // T 0x08108ad7

bool loadRDARScriptListFile(void* list, const char* dir, const char* listPath,
                            std::map<int, int>*);   // ORIG T 0x088bbd77

// ---- CCharacter 桥（destroy/set_data 为 T 符号，CCharacter.h 未声明）----
void sub_CCharacterDestroy(void*) asm("_ZN10CCharacter7destroyEv");
void sub_CCharacterSetData(void*, void*) asm("_ZN10CCharacter8set_dataEP17STCharacterScript");

namespace ctors1
{
inline bool IsBlockWorldMapMark(const CDungeon* d)
{
    // ORIG set_world_map 分支：dungeon+0x87a 字节非 0 跳过收敛。
    return *(reinterpret_cast<const char*>(d) + 0x87a) != 0;
}
}  // namespace ctors1


// ---- CEnvironment +0x310 字节（AvatarRouletteServer Init/InitAddCharacSkill
//      报告实证的环境开关；CEnvironment.h 尚未建模该字段——字段化收口归
//      CEnvironment 批次，本 TU 集中一处访问便于替换）----
namespace ctors1
{
inline bool IsEnableAvatarRoulette()
{
    return *(reinterpret_cast<const char*>(G_CEnvironment()) + 0x310) != 0;
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
    char m_padF38[0xf50 - 0xf3c];
    char m_f50[0x100c - 0xf50];                     // stEnamyDropAvatarInfo_t 区
    unsigned char m_rarityByte;                     // +0x100c
    char m_pad100d[3];
};  // sizeof 0x1010

// ============================================================================

// （报告 AvatarRoulette/{AvatarRoulette,clear,swap}.md）
// ============================================================================

// ============================================================================
// AvatarRouletteServer（报告 AvatarRouletteServer/*.md）
// ============================================================================

AvatarRouletteServer::AvatarRouletteServer()
{
    // ORIG 0x0817f354：map 成员自动构造；*(this+0x18)=0。
    m_pRoulette = 0;
}

bool AvatarRouletteServer::Init(AvatarRoulette* roulette)
{
    // ORIG 0x0817f706。
    m_pRoulette = new AvatarRoulette;
    m_pRoulette->swap(roulette);
    m_totalCount = m_pRoulette->m_rareAvatarRate + m_pRoulette->m_upperAvatarRate;

    if (ctors1::IsEnableAvatarRoulette())
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
                            "bool AvatarRouletteServer::Init(AvatarRoulette*)", 0x80,
                            "AvatarRouletteServer not exist item_no(%d)", itemNo);
                        continue;
                    }
                    if (!item->IsAvatarItem())
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            "bool AvatarRouletteServer::Init(AvatarRoulette*)", 0x86,
                            "AvatarRouletteServer no avatar item_no(%d)", itemNo);
                    }
                    if (!item->check_job_type(static_cast<char>(job)))
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            "bool AvatarRouletteServer::Init(AvatarRoulette*)", 0x8d,
                            "AvatarRouletteServer item_no(%d) check_job_type(%d)==false",
                            itemNo, job);
                    }
                    unsigned int itemType = item->GetItemType();
                    if (itemType != static_cast<unsigned int>(type))
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            "bool AvatarRouletteServer::Init(AvatarRoulette*)", 0x95,
                            "AvatarRouletteServer item_no(%d) GetItemType(%d)!=%d",
                            itemNo, itemType, type);
                    }
                    if (type != 8)
                    {
                        int grade = item->get_grade();
                        if (li < 2 && grade != 2)
                        {
                            LogManager::logFormat(
                                1, "localjapan/Arad_AvatarRoulette.cpp",
                                "bool AvatarRouletteServer::Init(AvatarRoulette*)", 0xa3,
                                "AvatarRouletteServer item_no(%d) get_grade(%d)!=%d, line_num(%d)",
                                itemNo, grade, 2, static_cast<int>(li));
                        }
                        else if (li > 1 && grade != 1)
                        {
                            LogManager::logFormat(
                                1, "localjapan/Arad_AvatarRoulette.cpp",
                                "bool AvatarRouletteServer::Init(AvatarRoulette*)", 0xa9,
                                "AvatarRouletteServer item_no(%d) get_grade(%d)!=%d, line_num(%d)",
                                itemNo, grade, 1, static_cast<int>(li));
                        }
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
    return static_cast<unsigned long>(m_pRoulette->m_avatarCoinInfo) == itemIdx;
    // [推断] ORIG 为 8 字节 ulong 比对；共享头现以 int 建模 coin info 字段。
}

bool AvatarRouletteServer::InitAddCharacSkill()
{
    // ORIG 0x0817f386。
    if (!ctors1::IsEnableAvatarRoulette())
        return true;

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
                unsigned long itemNo = inner[type].first;
                const char* fileName =
                    g_stEquipmentScriptFileList.GetFileName(static_cast<int>(itemNo));
                if (fileName == 0)
                    continue;

                STEquipmentScript eq;
                if (!ImportEquipmentScript(&eq, fileName, false, 0, -1))
                {
                    LogManager::logFormat(
                        1, "localjapan/Arad_AvatarRoulette.cpp",
                        "bool AvatarRouletteServer::InitAddCharacSkill()", 0x39,
                        "InitAddCharacSkill() - ImportEquipmentScript() fail file - %s",
                        fileName);
                    return false;
                }
                std::map<int, std::vector<LevelUpSkill> > skillMap;
                size_t count = eq.m_param1c0.m_avatar264.size();
                for (size_t idx = 0; idx < count; ++idx)
                {
                    LevelUpSkill skill;
                    STAvatarAbilityVariation& var = eq.m_param1c0.m_avatar264[idx];
                    skill.m_job = var.m_skill.m_job;        // 元素 +0xc
                    skill.m_field4 = var.m_skill.m_field4;  // 元素 +0x10（内层 map 键）
                    skill.m_field8 = var.m_skill.m_field8;  // 元素 +0x14
                    skillMap[skill.m_field4].push_back(skill);
                }
                m_addCharacSkillMap[itemNo] = skillMap;
            }
        }
    }
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
    // ORIG 0x089fc1a8：strncpy(base,dir,0x100)；末字符非 '\\'/'/' 时补 "/"；
    // loadRDARScriptListFile(&g_npcScriptFileList, base, list, 0)。
    strncpy(g_npcScriptBaseDirectory, dir, 0x100);
    size_t len = strlen(g_npcScriptBaseDirectory);
    char last = g_npcScriptBaseDirectory[len - 1];
    if (last != '\\' && last != '/')
        memcpy(g_npcScriptBaseDirectory + len, "/", 2);
    return loadRDARScriptListFile(&g_npcScriptFileList,
                                  g_npcScriptBaseDirectory, listPath, 0)
               ? true : false;
}

bool isExistNPCScript(const char* name)
{
    // ORIG 0x089fae70：return loadRDARScriptFile(g_npcScriptBaseDirectory,name)。
    return loadRDARScriptFile(g_npcScriptBaseDirectory, name) ? true : false;
}

// ============================================================================
// CNPCDynamicInfoManager（ctor/dtor 0x083760f2W/0x08376106W；
// getNPCInfo 0x08581948W；add 0x08580668 报告 CNPCDynamicInfoManager/add.md）
// ============================================================================

class CNPCDynamicInfo   // 完整类型归 NPC 动态域；本 TU 仅持指针
{
public:
    char m_pad[0x10];
};

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
        void* mem = ::operator new(0x10);
        sub_CNPCDynamicInfoC1(mem, script);
        m_map.insert(std::make_pair(idx,
            reinterpret_cast<CNPCDynamicInfo*>(mem)));
        return true;
    }
    return false;
}

// ============================================================================
// TowerOfDespair（TowerOfDespair_APC_Mgr ctor 0x086444e6W 仅 map 构造；
// TowerOfDespairMgr 报告 TowerOfDespairMgr/*.md）
// ============================================================================

TowerOfDespair_APC_Mgr::TowerOfDespair_APC_Mgr()
    : m_map()
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
    // ORIG 0x08643be2：delete m_apcMgr；其余成员析构自动。
    delete m_apcMgr;
    m_apcMgr = 0;
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
    CDungeon* dungeon = G_CDataManager()->find_dungeon(static_cast<int>(dungeonIdx));
    _Set_LayerByDungeonIndex(layer, dungeon);
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
    // ORIG：i∈[0,0x45] 清 levelKeys[i] 并置名字(+0x14c8+21i)/留言(+0x1a86+21i)
    // 槽首字节 0；双层清空 primary(@+0x118)/secondary(@+0xaf0) 两块。
    for (int i = 0; i <= 0x45; ++i)
    {
        m_levelKeys[i] = 0;
        m_nameStrings[21 * i] = '\0';
        m_messages[21 * i] = '\0';
    }
    for (int j = 0; j < 9; ++j)
        for (int l = 0; l <= 0x45; ++l)
        {
            m_rewardPrimary[j * 0x46 + l] = 0;
            m_rewardSecondary[j * 0x46 + l] = 0;
        }
}

void CLevelRewardTable::SetJobAffection(bool on)
{
    // ORIG 0x08697766W：mov %dl,0x6086(%eax)。
    m_jobAffection = on;
}

int CLevelRewardTable::GetProperJobIndex(char job)
{
    // ORIG 0x08687e78：<0→-1；开关关→0；>8→-1；否则恒等映射 0..8。
    if (job < 0)
        return -1;
    if (!GetJobAffection())
        return 0;
    if (static_cast<unsigned char>(job) > 8)
        return -1;
    return job;
}

int CLevelRewardTable::GetProperLevelIndex(unsigned int level)
{
    // ORIG 0x086876c8：扫 levelKeys，遇 0 止返回 -1，命中返回下标。
    for (int i = 0; i <= 0x45; ++i)
    {
        if (m_levelKeys[i] == 0)
            return -1;
        if (m_levelKeys[i] == static_cast<int>(level))
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
    // ORIG 0x08687710：vec@+0x08 自动构造；flag=0；table=new(0x6088)。
    m_flag = 0;
    m_table = new CLevelRewardTable;
}

CLevelRewardTableMgr::~CLevelRewardTableMgr()
{
    delete m_table;
    m_table = 0;
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
    // ORIG 0x08687828：flag←info 首字节；vec8=info.vec4；SetJobAffection(true)；
    // levelKeys←vec10；rewards 写 primary/secondary 两块；messages 经
    // findString 写名字/留言区。
    m_flag = static_cast<char>(info.m_field0);
    m_vec8 = info.m_vec4;
    m_table->SetJobAffection(true);

    {
        std::vector<int>::const_iterator it = info.m_vec10.begin();
        for (int i = 0; it != info.m_vec10.end(); ++i, ++it)
            m_table->m_levelKeys[i] = *it;
    }
    for (std::vector<stUnlimitChallengeRewardInfo>::const_iterator it =
             info.m_vec1c.begin();
         it != info.m_vec1c.end(); ++it)
    {
        unsigned int level = static_cast<unsigned int>(it->m_jobLevel);
        int jobIdx = m_table->GetProperJobIndex(static_cast<char>(it->m_subIdx));
        int levelIdx = m_table->GetProperLevelIndex(level);
        m_table->m_rewardPrimary[jobIdx * 0x46 + levelIdx] = it->m_a;
        jobIdx = m_table->GetProperJobIndex(static_cast<char>(it->m_subIdx));
        levelIdx = m_table->GetProperLevelIndex(level);
        m_table->m_rewardSecondary[jobIdx * 0x46 + levelIdx] = it->m_b;
    }
    for (std::vector<stUnlimitChallengeMessageInfo>::const_iterator it =
             info.m_vec28.begin();
         it != info.m_vec28.end(); ++it)
    {
        unsigned int level =
            static_cast<unsigned int>(static_cast<unsigned short>(it->m_id));
        int levelIdx = m_table->GetProperLevelIndex(level);
        const char* p1 = g_scriptStringManager_.findString(4, it->m_msg1.c_str(), 0);
        strncpy(m_table->m_nameStrings + levelIdx * 0x15, p1, 0x14);
        const char* p2 = g_scriptStringManager_.findString(4, it->m_msg2.c_str(), 0);
        strncpy(m_table->m_messages + levelIdx * 0x100, p2, 0xff);
    }
}

// ============================================================================
// WorldMapScript（ctor 0x08369f08W / dtor 0x0837f5aeW / clear；
// importWorldMapScript 0x08ab0388 报告 _lt_global_gt_/importWorldMapScript.md）
// ============================================================================

WorldMapScript::WorldMapScript()
    : m_index(0),
      m_name(),
      m_mapImage(),
      m_uiPath(),
      m_field10(0),
      m_dungeons(),
      m_isHellDungeon(0),
      m_hellQuests(),
      m_freepassItems(),
      m_itemConditions(),
      m_minimapPoints()
{
}

WorldMapScript::~WorldMapScript()
{
}

void WorldMapScript::clear()
{
    m_index = 0;
    m_name.clear();
    m_mapImage.clear();
    m_uiPath.clear();
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

    bool ok = false;
    std::string token;
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
            script->m_field10 = ScanInt((bool*)0);
        }
        else if (token == "[ui path]")
        {
            ScanStr(&script->m_uiPath);
        }
        else if (token == "[dungeon]")
        {
            script->m_dungeons.clear();
            for (;;)
            {
                bool isType = false;
                bool isInt = false;
                WorldMapDungeon wd;
                wd.m_dungeonIndex = ScanInt(&ok);
                if (!ok)
                    break;
                isInt = ScanTypeOrInt(isType, token, isType, true);
                if (!(isType || isInt))
                    break;
                wd.m_state = atoi(token.c_str());
                script->m_dungeons.push_back(wd);
            }
        }
        else if (token == "[hell dungeon]")
        {
            script->m_isHellDungeon = ScanInt((bool*)0) != 0 ? 1 : 0;
        }
        else if (token == "[hell quest]")
        {
            for (;;)
            {
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_hellQuests.push_back(v);
            }
        }
        else if (token == "[hell freepass item]")
        {
            for (;;)
            {
                int a = ScanInt(&ok);
                if (!ok)
                    break;
                int b = ScanInt((bool*)0);
                script->m_freepassItems.push_back(std::make_pair(a, b));
            }
        }
        else if (token == "[item condition]")
        {
            for (;;)
            {
                int a = ScanInt(&ok);
                ScanInt((bool*)0);
                if (!ok)
                    break;
                script->m_itemConditions.push_back(a);
            }
        }
        else if (token == "[minimap points]")
        {
            int v = ScanInt(&ok);
            while (ok)
            {
                int pt[2] = { 0, 0 };
                for (int k = 0; k < 2 && ok; ++k)
                {
                    pt[k] = v;
                    v = ScanInt(&ok);
                }
                script->m_minimapPoints.push_back(std::make_pair(pt[0], pt[1]));
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
    // ORIG 0x08364694：重复 index → logFormat 后返回 false；否则 insert。
    int idx = worldMap->get_index();
    if (m_map.find(idx) != m_map.end())
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            "bool CWorldMapList::insert_world_map(CWorldMap*)", 0x2be9,
            "Already registed world map index detected : %d", idx);
        return false;
    }
    m_map.insert(std::make_pair(idx, worldMap));
    return true;
}

bool CWorldMapList::load_list()
{
    // ORIG 0x08364506：遍历 g_worldMapScriptFileList，逐个导入并 new CWorldMap(0x40)
    // set_world_map + insert_world_map；导入失败 printf 后返回 0。
    WorldMapScript script;
    for (std::map<int, std::string>::const_iterator it =
             reinterpret_cast<STFileListMapMirror*>(&g_worldMapScriptFileList)
                 ->m_fileMap.begin();
         it != reinterpret_cast<STFileListMapMirror*>(
                   &g_worldMapScriptFileList)->m_fileMap.end();
         ++it)
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
    // ORIG 0x083494da：for i in [0,10]: m_characs[i].destroy()。
    for (int i = 0; i <= 0xa; ++i)
        sub_CCharacterDestroy(&m_characs[i]);
}

bool CCharacList::init()
{
    // ORIG 0x08349514：栈上构造 STCharacterScript(0xc1c)，遍历
    // g_stCharacterScriptFileList：ImportCharacterScript 失败 printf 返回 0；
    // 否则按脚本首字 job 下标 set_data 到 m_characs[idx]。
    char scriptStorage[0xc1c];
    void* script = scriptStorage;
    sub_STCharacterScriptC1(script);

    bool result = true;
    for (std::map<int, std::string>::const_iterator it =
             g_stCharacterScriptFileList.m_fileMap.begin();
         it != g_stCharacterScriptFileList.m_fileMap.end(); ++it)
    {
        const char* name = it->second.c_str();
        if (name == 0)
            break;
        if (!sub_ImportCharacterScript(script, name))
        {
            printf("Charac Script Error: %s\n", name);
            result = false;
            break;
        }
        int jobIdx = *static_cast<int*>(script);
        sub_CCharacterSetData(&m_characs[jobIdx], script);
    }
    sub_STCharacterScriptD1(script);
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
    selfC.m_independentDrops = script.m_f2c;
    memcpy(selfC.m_avatarDrops, script.m_f50, sizeof(selfC.m_avatarDrops));
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
                cMyTrace trace("int CNPCScriptList::initScript(NCHAR*, NCHAR*, NCHAR*)", 0x1b0, 5);
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
            cMyTrace trace("int CNPCScriptList::initScript(NCHAR*, NCHAR*, NCHAR*)", 0x1a9, 5);
            trace("%s file not exists\n", name);
        }
    }

    if (loadNPCCommonData(m_npcCommon, common) != 0)
    {
        cMyTrace trace("int CNPCScriptList::initScript(NCHAR*, NCHAR*, NCHAR*)", 0x1be, 5);
        trace("%s load failed\n", common);
        return 0x1bf;
    }
    if (m_npcCommon.m_vec18.size() > 0x32)
    {
        cMyTrace trace("int CNPCScriptList::initScript(NCHAR*, NCHAR*, NCHAR*)", 0x1c3, 5);
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
