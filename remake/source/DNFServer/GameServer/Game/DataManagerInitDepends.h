#ifndef GAME_DATAMANAGER_INIT_DEPENDS_H_
#define GAME_DATAMANAGER_INIT_DEPENDS_H_

// ============================================================================
// CDataManager::init()（ORIG 0x08358332..0x0835f1d5，报告 init.md 全量转写）
// 依赖的脚本装载自由函数声明汇总点。
//
// 规则说明：
//   - 每个符号均为 ORIG nm 真实存在（mangled 逐条核对，注释给出来源）；
//     本头只做「唯一声明点」，不提供任何伪定义。
//   - 定义归属各自数据域 TU（多数尚未落地）；缺失定义清单由 CDataManager TU
//     负责人（RestoreDataManagerInitFull）向 Main 上报。
//   - 本头必须在 CDataManager.h 之后包含（大量形参类型为 CDataManager.h 的
//     完整类型；不在此重复前置声明以免 struct/class 关键字漂移）。
//   - 已有权威头的成员不在此重复：sync_script.h / CEventScriptMng.h /
//     CDailyScheduleManager.h / ServerParameterScript.h / RandomOptionScript.h
//     （importRandomOptionScript）/ CCubeStatistic.h（getItemGroupNameTable）。
// ============================================================================

#include <map>
#include <set>
#include <string>
#include <vector>

#include "RDARScriptStringManager.h"   // ErrorLog（InitRDARScriptStringManager 形参）
#include "CDailyScheduleManager.h"     // _DailySchedule / CDailyScheduleManager
#include "CCompoundAvatar.h"          // ENUM_CHARACTERJOB（guard 枚举）+ tagCompoundAvatar 族声明
#include "NeedMaterialDataManager.h"  // NeedMaterialDataManager（唯一声明点；替代原最小声明）


class InGameEventInfoList;             // CEventScriptMng.h（全局作用域基类 1）

// ---- RDAR 脚本根基（定义：ScriptExtras.cpp）----
void SetPriorityBaseDirectory(const char* dir);               // _Z24SetPriorityBaseDirectoryPKc 0x088bb40a
bool InitRDARScript(const char* dir, bool compiled,
                    const char* packFile, int* err);          // _Z14InitRDARScriptPKcbS0_Pi 0x088bb5d2
bool InitRDARScriptStringManager(const char* dir, const char* file,
                                 std::vector<ErrorLog>* err); // _Z27InitRDARScriptStringManagerPKcS0_PSt6vectorI8ErrorLogSaIS2_EE 0x088bff9c

void InitMapEquipmentAniScript(void);                         // _Z25InitMapEquipmentAniScriptv 0x089c0586

// ---- 全局环境谓词 ----
bool IsLightServer(void);                                     // _Z13IsLightServerv 0x0822ad44（定义 GlobalData.cpp；ORIG 调用点 test %al 无 setne ⇒ bool，giveup_panalty 0x086786cb 实证）
void SetCollectDungeonStatistic(bool flag);                   // _Z26SetCollectDungeonStatisticb
bool prepareServerDelayCheckDamage(void);                     // _Z29prepareServerDelayCheckDamagev

// ---- Etc 参数装载 ----
bool importExperienceItemMapInfo(std::map<int, int>& m, bool second);              // _Z27..RSt3mapIii..b 0x089136a4；[b28d-Main·检查点14 P292] 调用点 xor $1 ⇒ bool
bool importImageCommunicationInfo(const char* path,
                                  class ImageCommunicationData* out);              // _Z28..PKcP22ImageCommunicationData 0x08913dbc
bool importBoosterGage(char* path, class BoosterGageData* out);                    // _Z17importBoosterGagePcP15BoosterGageData 0x08913a7e
bool importInvalidName(char* path, class InvalidCharData* out);                    // _Z17importInvalidNamePcP15InvalidCharData 0x08908338
bool importSlangName(char* path, class SlangNameData* out);                        // _Z15importSlangNamePcP13SlangNameData
bool importGoldLimitByLevel(unsigned int* table);                                  // _Z22importGoldLimitByLevelPj [P330 int→bool] init 调用点 ORIG xor $1 形（idx 2306）
bool importOldEquipmentStatInfo(const char* arg);                                  // _Z26importOldEquipmentStatInfoPKc
int  importItmeGroupTable(const char* dir, const char* file,
                          std::map<std::string, int>& table);                      // _Z20importItmeGroupTablePKcS0_RSt3mapISsiSt4lessISsESaISt4pairIKSsiEEE（int 错误码：0=成功/0x1eb2=加载失败/0x1ebd=ScanStr 失败；ORIG 0x0890bb25 返回 undefined4，init.md +0x3e48 以 cmpl $0x0 判失败）

// ---- 事件/Etc 装载（形参类型均为 CDataManager.h 完整类）----
bool importInGameAdvertisementEtcScript(class InGameAdvertisementScript* s, const char* path); // _Z34..
bool importSeriaRoomDecoEvent(class seriaRoomDecoEventScript* s, const char* path);           // _Z24..
bool importGrowthEquipmentScript(class growthEquipmentScript* s, const char* path);           // _Z27..
bool IsLightServer(void);                                     // _Z13IsLightServerv 0x0822ad44（定义 GlobalData.cpp；ORIG 调用点 test %al 无 setne ⇒ bool，giveup_panalty 0x086786cb 实证）
void SetCollectDungeonStatistic(bool flag);                   // _Z26SetCollectDungeonStatisticb
bool prepareServerDelayCheckDamage(void);                     // _Z29prepareServerDelayCheckDamagev
bool ImportWorldDrop(class WorldDropInfo* out, const char* path);   // _Z15ImportWorldDropP13WorldDropInfoPKc
bool ImportWarAreaCounter(class WarAreaCounter* out);         // _Z20ImportWarAreaCounterP14WarAreaCounter [P330 int→bool] init 调用点 ORIG xor $1 形（idx 1938）；定义 InitScriptProducers_2.cpp
namespace AvatarVariation
{
class AvatarColorInfo;
bool importAvatarColorVariation(AvatarColorInfo* out);        // _ZN15AvatarVariation26importAvatarColorVariationEPNS_15AvatarColorInfoE
}
int  importRandomOptionScript(class RandomOptionScript& script,
                              const char* path);              // _Z24importRandomOptionScriptR18RandomOptionScriptPKc（返回出错行号/0；定义 RandomOptionScript.cpp；本头不引其权威头以避免与 CDataManager.h 本地类冲突）
bool importCharacterCreateGiftScript(class createChracScript* s, const char* path);           // _Z31..
bool importUseJobFatigueGiftScript(class useCharacterJobFatigue* s, const char* path);        // _Z29..
bool importCharacterCreateCreatureGiftScript(class CharacterCreateCreatureGiftScript* s,
                                             const char* path);                               // _Z39..
bool importSeriaRoomAnimationDecoEvent(class seriaRoomDecoAniEventScript* s,
                                       const char* path);                                     // _Z33..
// [P330 bool→int] ORIG caller idx 1053 `mov %eax,-0x38` 4B 全 store 无 movzbl
// （bool caller 必发 movzbl，probe2 实证）⇒ 声明 int；ORIG callee ebx 恒 0
// （文件缺失/成功均 return 0，err 截断风险=日志行号值域）。定义 InitScriptProducers_2.cpp。
int  ImportBingoScript(const char* path, class stBingoScript& out);        // _Z17ImportBingoScriptPKcR13stBingoScript
// 返回 int：init.md 调用点以 32 位与 0 比较并 printf("... Fail(%d)", code)
// （如 +0x131e "\t- ImportSeriaBlessingScript Fail(%d)"、+0x13fb
// "ImportGrowthCapsuleScript Fail(%d)"），bool 会丢失 ORIG 行号错误码。
int ImportBroadCastItemScript(const char* path, class stBroadCastItemScript& out);
int ImportSeriaBlessingScript(const char* path, class stSeriaBlessingScript& out);
int ImportLevelUpRewardItemScript(const char* path, class stLevelUpRewardItemScript& out);
int ImportGrowthCapsuleScript(const char* path, class stGrowthCapsuleScript& out);
int ImportNewAccountLeventUpToJobScript(const char* path,
                                        class stNewAccountLevelUpToJobScript& out); // _Z35ImportNewAccountLeventUpToJobScriptPKcR30..
int  ImportReturnUserRewardScript(const char* path, class stReturnUserRewardScript& out); // _Z28..（0=成功/0x36b4..；定义 InitScriptProducers_2.cpp）
int  ImportItemMakingSkill(const char* path, class stItemMakingSkill& out);               // _Z21..（0=成功/0x279e..；定义 InitScriptProducers_2.cpp）
bool importUltimateDungeonList(const char* path, std::set<int>& out);      // _Z25importUltimateDungeonListPKcRSt3setIiSt4lessIiESaIiEE [P330 int→bool] init 调用点 ORIG xor $1 形（idx 1154）
int  importVillageAttackedScript(const char* path, class stVillageAttacked_t& out); // ORIG 返回错误码（0x197f 打开失败/0x1990..0x1998 Scan 失败），0=成功
int  importCharacLinkSystemScript(const char* path, class stCharacLinkSystem_t& out); // ORIG 返回行号错误码（0x19ab 打开失败/0x19b7..0x19df Scan 失败），0=成功
int  importTowerInfo(const char* dir, const char* list,
                     std::map<unsigned int, struct stDeathTower_t>& out);  // _Z15importTowerInfoPKcS0_RSt3mapIj14stDeathTower_tSt4lessIjE..
int  importDeathTowerScript(const char* path, struct stDeathTower_t& out); // ORIG 返回行号错误码（0x187e 打开失败/0x1887..0x18ca Scan 失败/0x18cd 未知节），0=成功
bool importRankSystemScript(const char* path, class stRankSystemInfo* out);
int  importBloodDungeonScript(const char* path, struct stBloodDungeon_t& out); // ORIG 返回行号错误码（0x191c 打开失败/0x192f..0x196e Scan/重复 key 失败），0=成功
// ---- stHellParty_t（ORIG C1 0x891b242 / D1 0x891c7e2 / 单例 Get_instance_stHellParty_t
// 0x89097c4；importHellPartyScript 0x8909857 全量字段实证）----

// ORIG stHellPartyMonsterGroupList C1 0x891b1d6：memset(-1) 两段 + 三标量清零，sizeof 0x58。
struct stHellPartyMonsterGroupList
{
    stHellPartyMonsterGroupList();
    int m_monsterIdx[16];           // +0x00（ctor memset 0xff）
    unsigned char m_rate[16];       // +0x40（ctor memset 0xff）
    unsigned char m_field50;        // +0x50
    unsigned short m_field52;       // +0x52
    unsigned char m_field54;        // +0x54
};

struct stHellParty_t
{
    stHellParty_t();
    ~stHellParty_t();

    int m_maxBaseValue;              // +0x00 [max base value]（==0 → err 0x1a41）
    int m_minBaseValue;              // +0x04 [min base value]
    int m_baseValue;                 // +0x08 [base value]
    int m_baseRisingValue;           // +0x0c [base rising value]
    float m_subBaseRates;            // +0x10 [sub base rates]
    float m_passSubBaseRates;        // +0x14 [pass sub base rates]
    float m_levelDiffRates[4];       // +0x18（ctor memset 0x10；脚本写后三槽 [level difference rates]）
    float m_difficultyRates[5];      // +0x28（ctor memset 0x14 [difficulty rates]）
    float m_partyRates[4];           // +0x3c（ctor memset 0x10；首值 ScanFloat(3,&ok) 缩放）
    float m_ancientDungeonAddRate;   // +0x4c [ancient dungeon add rate]
    unsigned short m_groupIndex;     // +0x50 [group index]（map key）
    std::map<unsigned short, stHellPartyMonsterGroupList> m_groupMap;  // +0x54
    // 难度表（ctor 0x891b242 清零环 i=0..5 实证：byte@0x6c+i、
    //   ushort@0x72+2i（movw 0x2(%eax,%edx,2), edx=i+0x38）、byte@0x7e/0x84/0x8a+i；
    //   各表 6 槽；GetHellPartyDifficulty 0x82ffa2e memcpy 6 字节、
    //   GetHellPartyEventType 0x82ffb06 查 0x8a[diff]、guideLineValue dword@0x90）。
    unsigned char m_field6c[6];      // +0x6c（tag 字节）
    unsigned short m_field72[6];     // +0x72（各槽 monster 数）
    unsigned char m_field7e[6];      // +0x7e
    unsigned char m_field84[6];      // +0x84
    unsigned char m_field8a[6];      // +0x8a
    int m_guideLineValue;            // +0x90 [guide line value]
};                                 // sizeof 0x94

// ORIG 返回错误码（bool 语义 0=成功）：0x1a24 装载失败 / 0x1a2e..0x1b30 各 Scan 失败 /
// 0x1b08 组下标重复 / 0x1aed 未知组名；"[hellparty script end]" → 0。
int importHellPartyScript(const char* path, stHellParty_t& out);           // _Z21importHellPartyScriptPKcR13stHellParty_t
int  importQuestShopScript(const char* path, class stQuestShop& out);      // 返回 0=成功
bool initIndependentDropScript(char* dir, char* list);                     // _Z25initIndependentDropScriptPcS_（[b28d-Main·P321-T3] 调用点 xor $1 ⇒ bool）
int  importVendingMachineInfo(const char* dir, const char* list,
                              std::vector<struct stVendingMachine_t>& out);// _Z24..（ORIG 返回错误码 0x1e73 空列表/0x1e7c Core 失败/0x1e83 列表加载失败，0=成功）
bool initWorldMapScript(char* dir, char* list);                            // _Z18initWorldMapScriptPcS_
bool initStageMapScript(char* dir, char* list);                            // _Z18initStageMapScriptPcS_
int  importDailyScheduleEtcScript(const char* path,
                                  std::vector<_DailySchedule>& out);       // _Z28importDailyScheduleEtcScriptPKcRSt6vectorI14_DailyScheduleSaIS2_EE（返回 0=成功）
int  importMercenarySystemEtcScript(const char* path,
                                    class stMercenarySystemInfo* out);     // ORIG 返回错误码（9 装载/0xc 空指针/0x29..0xd1 各段），0=成功
bool importPcBangEquipmentRendar(const char* path, class PcBangItemRentarData* out);
bool importDimensionActivationData(const char* path, class DimensionActivationData* out);
bool importNPCRelationEventScript(const char* path, class CNPCRelationEventManager* out);
bool importEmotionTypeTagsScript(const char* path, class stEmotionTypeTagsInfo* out);
bool importEquipmentGradeSampleInfo(const char* path, class stEquipmentGradeSample* out);
bool importConditionEventScript(const char* path, class stConditionEventInfo* out);
bool importConditionEventChkDungeonScript(const char* path,
                                          class ConditionLevelChkDungeon* out);
bool importPairPvPStatScript(class FairPvPStatScript& script, const char* path); // _Z23importPairPvPStatScriptR17FairPvPStatScriptPKc
bool importGrowthPower(char* path, class STGrowthPowerData* out);
bool importDBLogItem(char* path, std::vector<struct stDBLogItem>* out);    // _Z15importDBLogItemPcPSt6vectorI11stDBLogItemSaIS1_EE
bool importLimitItemUsageInfo(char* path, class stLimitItemUsageInfoEx* out);
bool importUnlimitChallengeScript(const char* path, class stUnlimitChallengeInfo* out);
bool importResealTable(const char* path, class reseal_table_t* out);       // _Z17importResealTablePKcP14reseal_table_t
bool importAccountCargoScript(class AccountCargoScript* s, const char* path); // _Z24importAccountCargoScriptP18AccountCargoScriptPKc
bool importSecretShopScript(class SecretShopScript* s, const char* path);  // 定义 CSecretShop.cpp
bool importGuildParameterScript(class GuildParameterScript* s, const char* path);
bool importPowerParameterScript(class PowerParameterScript* s, const char* path);
bool importQuestParameterScript(class QuestParameterScript* s, const char* path);
bool importTitleBookEtcScript(const char* path, class stTitleBookInfo* out);
bool importPVPChannelList(char* path, class live_server_info_t* info, int flag); // _Z20importPVPChannelListPcP18live_server_info_ti
bool importChannelScript(class channel_script_t* script);                  // _Z19importChannelScriptP16channel_script_t（[P248 档①] ORIG 调用点 0x8359632 段 `call; xor $1,%eax; test %al,%al; je` ⇒ bool；0x88dc062 尾调透传，体内 return 全常量）
bool importEventAvatarListScript(class EventAvatarListScript* s, const char* path); // _Z27..
bool importEventEtcScript(class EventEtcScript* s, const char* path);      // _Z20..
bool importInGameEventInfoList(InGameEventInfoList* list, const char* path); // _Z25importInGameEventInfoListP19InGameEventInfoListPKc
bool importFairPVPEquipment(
    char* path,
    std::map<int, std::vector<std::vector<struct STStackablePackageData> > >*
        out);                                                              // _Z22importFairPVPEquipmentPcPSt3mapIiSt6vectorIS1_I22STStackablePackageDataSaIS2_EESaIS4_EE..

// ---- 大类脚本初始化（Character/Monster/Skill/Map/Dungeon 域）----
bool InitCharacterScript(const char* dir, const char* list,
                         const char* expTable, const char* unused);        // _Z19InitCharacterScriptPKcS0_S0_S0_ 0x08a0cc3c
bool InitMonsterScript(const char* dir, const char* list,
                       const char* apcBonus);                              // _Z17InitMonsterScriptPKcS0_S0_
bool InitPassiveObjectScript(const char* dir, const char* list);          // _Z23InitPassiveObjectScriptPKcS0_
bool InitEquipmentScript(const char* dir, const char* list,
                         const char* priceTable);                         // _Z19InitEquipmentScriptPKcS0_S0_
bool InitStackableScript(const char* dir, const char* list,
                         const char* priceTable);                         // _Z19InitStackableScriptPKcS0_S0_
bool InitMapScript(const char* dir, const char* list);                    // _Z13InitMapScriptPKcS0_
bool InitDungeonScript(char* dir, char* list);                            // _Z17InitDungeonScriptPcS_
bool InitSkillScript(const char* dir, const char* list);                  // _Z15InitSkillScriptPKcS0_
bool initExpertJobScript(char* dir, char* list);                          // _Z19initExpertJobScriptPcS_
bool initAICharacterScript(const char* dir, const char* list);            // _Z21initAICharacterScriptPKcS0_
bool initCreatureScript(const char* dir, const char* list,
                        const char* expTable);                            // _Z18initCreatureScriptPKcS0_S0_
bool initEventCharacterScript(char* dir, char* list);                     // _Z24initEventCharacterScriptPcS_
bool initItemShopScript(char* dir, char* list);                           // _Z18initItemShopScriptPcS_
bool initPvpMissionSystem(char* dir, char* list);                         // _Z20initPvpMissionSystemPcS_
bool initPvPSkillTreeScript(char* dir, char* list);                       // _Z22initPvPSkillTreeScriptPcS_
bool initChattingEmoticonScriptList(char* dir, char* list);               // _Z30initChattingEmoticonScriptListPcS_
bool loadPvpMapBasicDataScript(const char* path,
                               std::vector<struct PVPMAP_DATA>* out);     // _Z25loadPvpMapBasicDataScriptPKcPSt6vectorI11PVPMAP_DATASaIS2_EE

// ---- 装备部件套装全局表（ORIG BSS 符号；定义由 importEquipmentPartSetInfo
//      生产 TU 提供，本头仅 extern 引用 + 最小类视图）----

// ORIG _ZN22STEquipmentPartSetDataC1Ev 0x891b002 / D1 0x08907348：
//   +0x00 int(=-1) +0x04 std::string +0x08 STEquipmentScript*(拥有) +0x0c vector<STEquipmentPartSetPiece>。
// STEquipmentPartSetPiece/STEquipmentScript 完整类型经 sync_script.h 引入。
// 本头独立包含时的最小前置（完整类型见 sync_script.h / STStackableScript.h）：
struct STEquipmentPartSetPiece;
enum ENUM_MODULE_TYPE;
class STEquipmentPartSetData
{
public:
    STEquipmentPartSetData();
    ~STEquipmentPartSetData();

    int m_field0;                                   // +0x00（ctor 置 -1）
    std::string m_name4;                            // +0x04
    STEquipmentScript* m_script;                    // +0x08（dtor delete）
    std::vector<STEquipmentPartSetPiece> m_pieces;  // +0x0c
};

extern std::map<int, STEquipmentPartSetData> g_equipmentPartSetMap_;   // ORIG B 0x094f7130 (0x18)
bool importEquipmentPartSetInfo(const char* path, ENUM_MODULE_TYPE moduleType); // _Z26importEquipmentPartSetInfoPKc16ENUM_MODULE_TYPE 0x089073d9（定义 InitScriptProducers_2.cpp）
int  getSameStringIndex(const char* s, const char** table, int count);           // _Z18getSameStringIndexPKcPS0_i T 0x08990ed5
extern const char* characterEquipmentTypeTags[26];                               // ORIG D 0x093f4fc0（26 个 [xxx avatar]/装备部位标签）
int  LoadRewardDataByGrowType(class stLevelUpRewardItemScript& out, std::string endTag); // _Z24LoadRewardDataByGrowTypeR25stLevelUpRewardItemScriptSs T 0x08917f44
void deleteTagBracketString(std::string& s);                                     // _Z22deleteTagBracketStringRSs T 0x08917db3
int  getCharacterJobToEnumValue(std::string& job);                               // _Z26getCharacterJobToEnumValueRSs T 0x089bbc0f（定义 CDataManager.cpp）
int  makeFairPVPEquipmentPackageKey(int job, int idx);                           // _Z30makeFairPVPEquipmentPackageKeyii T 0x088945ad（(short)job | (short)idx<<16；定义 InitScriptProducers_2.cpp）

// ---- 最小类声明（ORIG 有真实符号；布局与专属批次落地前仅以 new/指针使用）----

// [b28d-SweepDataMgrInit] NeedMaterialDataManager 已统一到权威头 NeedMaterialDataManager.h
// （§4.4 单一声明点；原此处最小类声明与权威头不一致 —— 缺 getNeedMaterial(int)/
//  setNeedMaterialInfo 且私有 ctor/dtor 缺失，构成 ODR 违规）。ORIG 符号：
//   getInstance T 0x081b5ab2、getNeedMaterial(const CItem*) T 0x081b5b70
//   （同族 CItem.cpp 修复已实证可翻正 N7 间接 call 目标寄存器）。
struct SecurityReward;  // CDataManager.h 定义；独立包含本头时仅需指针声明

// ORIG CmmChannelProxy（GlobalData.h 前向；完整布局在 GlobalData.cpp/GameMain.cpp
// 各自局部镜像——本 TU 仅需 SendCheckChannelScript 一员，ORIG
// _ZN15CmmChannelProxy22SendCheckChannelScriptEv，init 以 s_channel_proxy 调用）
class CmmChannelProxy
{
public:
    bool SendCheckChannelScript();
};
bool ImportSecurityReward(SecurityReward* out);   // _Z20ImportSecurityRewardP14SecurityReward（定义 InitScriptProducers_2.cpp）

// ---- TOD 域权威定义（TOD_Layer/SendingAPC_Info/TowerOfDespair_APC_Mgr/
//      TowerOfDespairMgr）已统一至 TowerOfDespairMgr.h（2026-08-25 合并，
//      原此处镜像删除；TOD_UserState 保留本头）。----
#include "TowerOfDespairMgr.h"
#include "CBoosterGage.h"               // charac_expand::CData（TOD_UserState 基类）
#include "SigTypes_D.h"                 // SIG_REQUEST_TOD_USER_STATE（TOD_UserState 内嵌数据成员）

// ORIG TOD_UserState（角色扩展数据 0xd 槽对象；布局按报告逐字段建模；
// ORIG vtable 0x08cec600 槽序 = charac_expand::CData 完全一致
// [ResetDailyMidnight, loadData, getData, ResetDaily, ~, ~, _saveData, _reset]，
// 故继承该基类产生正确 ABI）。字段依据：
//   TOD_UserState/getEnterCount.md     —— +0x10 int（ORIG T 0x08643872）
//   TOD_UserState/getLastClearTime.md  —— +0x1c int（ORIG T 0x0864387e）
//   TOD_UserState/getTodayEnterLayer.md —— +0x14 ushort（当日层）
//   CDungeon.cpp tod 注                —— +0x16 ushort lastEnterLayer【推断】
class TOD_UserState : public charac_expand::CData
{
public:
    TOD_UserState();            // ORIG T 0x08643460：CData ctor 后写本类 vptr、
                                // SIG_REQUEST_TOD_USER_STATE(this+8) 内嵌构造
    ~TOD_UserState();           // ORIG D1 0x086434ae / D0 0x086434de

    // ORIG T 0x0864383e（报告 TOD_UserState/getTodayEnterLayer.md）：sret 返回
    // *(const TOD_Layer*)(this+0x14) + 1（经 TOD_Layer(ushort) 构造，>100 回卷 1）。
    TOD_Layer getTodayEnterLayer() const;

    // ---- CheckEnterable 链访问器（定义 BossTowerDepsC2.cpp）----
    int getEnterCount() const;      // ORIG T 0x08643872：*(int*)(this+0x10)
    int getLastClearTime() const;   // ORIG T 0x0864387e：*(int*)(this+0x1c)
    // ORIG T 0x08643942 const：sret 构造 TOD_Layer(m_lastEnterLayer@+0x16)。
    TOD_Layer getLastEnterLayer() const;

    // ORIG T 0x0864388a（objdump 转写，定义暂寄 CDungeon.cpp）：
    // enterCount/lastEnterLayer 递增、写 lastEnterTime、当日层为 0 时写
    // firstEnterTime，末尾 charac_expand::CData::alter 脏标记。
    void IncreaseEnterCount();
    // ORIG T 0x0864390e（本批次定义 CPartyDepsD2.cpp）：层推进+脏标记+清层时间。
    void ClearLayer();
    // ORIG T 0x086438fc _ZN13TOD_UserState13setEnterLayerERK9TOD_Layer：
    // 写 lastEnterLayer(+0x16)/当日层（oldGmRoutine "wjfakd" GM 命令调用）。
    void setEnterLayer(const TOD_Layer& layer);

    // ---- 虚槽覆盖/生命周期（W9_StructMisc 批次实现）----
    virtual void ResetDaily();                  // ORIG T 0x08643624：enterCount=0 + alter
    virtual bool loadData(CUser* pUser, char* pData);     // ORIG T 0x086434fc
    virtual void getData(char* pData) const;              // ORIG T 0x086435ca
    virtual bool _saveData(CUser* pUser);                 // ORIG T 0x08643642
    virtual void _reset();                                // ORIG T 0x086437ec

    // 数据区（+0x08..+0x1f = ORIG 内嵌 SIG_REQUEST_TOD_USER_STATE 同布局；
    // loadData/_saveData/_reset 全部按此段操作）。ORIG ctor 0x8643460 实证本段是
    // 一个内嵌 SIG_REQUEST_TOD_USER_STATE 子对象（this+0x8），其 C1 0x08491f68 构造
    // 当日层 TOD_Layer(0)，故以真实成员建模（非平铺标量），令 ctor 发 call SIG::C1。
    SIG_REQUEST_TOD_USER_STATE m_data;   // +0x08（m_characNo@+0/m_firstEnterTime@+4/
                                          // m_enterCount@+8/m_todayLayer@+c/
                                          // m_lastEnterLayer@+e/m_lastEnterTime@+10/
                                          // m_lastClearTime@+14）
};

// ---- ORIG CLevelRewardTable(0x6088)/CLevelRewardTableMgr(0x14) 权威
//      （ctor 0x086975feW/0x08687710、GetInstance W 0x0837505b、
//       SetChallengeInfo 0x08687828；Ctor_Group1）----
class CLevelRewardTable
{
public:
    CLevelRewardTable();                              // ORIG 0x086975feW
    void SetJobAffection(bool on);                    // ORIG 0x08697766W
    bool GetJobAffection();                         // ORIG W 0x08697782 _ZN…Ev 非 const（定义 CUserDepsB.cpp）
    int GetProperJobIndex(char job);                  // ORIG 0x08687e78
    int GetProperLevelIndex(unsigned int level);      // ORIG 0x086876c8
    int GetRewardItemNo(int jobIdx, int levelIdx);    // ORIG W 0x086976f6（定义 CUserDepsB.cpp）
    int GetItemCount(int jobIdx, int levelIdx);       // ORIG W 0x08697712（定义 CUserDepsB.cpp）
    char* GetMailTitle(int levelIdx);                 // ORIG W 0x08697730（定义 CUserDepsB.cpp）
    char* GetMailString(int levelIdx);                // ORIG W 0x08697750（定义 CUserDepsB.cpp）

    int m_levelKeys[0x46];                // +0x000
    // [b28d-Main·检查点17 T1] 一维→二维（同尺寸/偏移/mangled 不变）：
    //   ORIG 0x086975fe `mov -0x10(%ebp),%ecx; mov -0xc(%ebp),%edx; imul $0x46,%ecx,%ecx;
    //   lea (%ecx,%edx,1),%edx; add $0x44,%edx; movl $0x0,0x8(%eax,%edx,4)`
    //   二维 `m[j][l]` 逐条复现（探针 /tmp/p_lrt.cpp，4.4.6 后端；一维/交换加序/unsigned/cast 6 形全败）。
    int m_rewardPrimary[9][0x46];         // +0x118..0xae7
    int m_rewardSecondary[9][0x46];       // +0xaf0..0x14bf
    char m_nameStrings[0x1a86 - 0x14c8];  // +0x14c8（21B/级 strncpy 槽）
    char m_messages[0x6086 - 0x1a86];     // +0x1a86（0x100/级 strncpy 槽）
    bool m_jobAffection;                  // +0x6086
};  // sizeof 0x6088

class CLevelRewardTableMgr
{
public:
    CLevelRewardTableMgr();    // ORIG 0x08687710
    ~CLevelRewardTableMgr();   // ORIG 0x0868777c
    static CLevelRewardTableMgr* GetInstance();                  // ORIG W 0x0837505b
    void SetChallengeInfo(stUnlimitChallengeInfo& info);         // ORIG 0x08687828
    bool IsOn();                             // ORIG W 0x08697792 体=movzbl (this)+ret 直返（bool 字段直返同形）；P292 调用点判别：ORIG _processLevelReward 0x0868748b / CUserOnlyOrig 0x0864a5c8 均为 `call; xor $0x1,%eax; test %al,%al` ⇒ 返回类型 bool（uchar 恒产 test;sete 两段）。m_flag 同步 bool 化保直返形（mangled 无返回类型，零 ABI 风险）
    bool IsChallengingJob(char job);                  // ORIG T 0x08687792（定义 CUserDepsB.cpp）
    bool IsOnServer(int serverIdx);                   // ORIG T 0x08687d6c（W7_CUserFix 补；实现 owner=CUserDepsB.cpp）
    bool IsHaveStartLevelReward();                    // ORIG T 0x08687d18（同上）
    CLevelRewardTable* GetProperLevelRewardTableR() const;  // ORIG T 0x08687e62（返回 m_table；定义 CUserDepsB.cpp）
    CLevelRewardTable* GetProperLevelRewardTableR(int) const; // ORIG T 0x08687e6e（恒返 0；定义 CUserDepsB.cpp）
    static CLevelRewardTableMgr* levelRewardTableMgr;            // ORIG B 0x09473978

    bool m_flag;                          // +0x00（ctor 写 0；IsOn 直返 movzbl）
    CLevelRewardTable* m_table;           // +0x04
    std::vector<int> m_vec8;              // +0x08
};  // sizeof 0x14

// ORIG _ZN14AvatarRouletteC1Ev 0x08911b98 / D1 0x0837e484；init 栈上 0x11c 字节。
// 布局（ctor/dtor/clear 0x8911cf6 逐指令实证）：
//   +0x00..+0x10 五个 int（[avatar coin info]/[rare clone avatar rate]/
//   [rare avatar rate]/[upper avatar rate]/[normal avatar rate] 写入），
//   +0x14 起 11 个 vector<vector<pair<unsigned long,int>>>（步长 0xc，按
//   ENUM_CHARACTERJOB 0..10 索引），+0x98 起 11 个 vector<int>（sticker），
//   sizeof 0x11c。clear 仅清 avatar 数组（sticker 数组不动，ORIG 如此）。
class AvatarRoulette
{
public:
    AvatarRoulette();
    ~AvatarRoulette();
    void clear();                       // ORIG 0x8911cf6 T（仅清 m_avatars）

    bool isAvatarCoin(unsigned long avatarCoin);  // ORIG 0x8911da0 T（定义 AvatarCoin.cpp）
    int swap(AvatarRoulette* other);       // ORIG 0x08911e4e T
    // ORIG 0x8911db0 T _ZN14AvatarRoulette13getAvatarListERb17ENUM_CHARACTERJOBRSt6vectorIS2_ISt4pairImiESaIS4_EESaIS6_EE
    // ORIG 0x8911dfe T _ZN14AvatarRoulette13getStikerListERb17ENUM_CHARACTERJOBRSt6vectorIiSaIiEE
    // 定义 Arad_AvatarRoulette.cpp（本 TU）。
    void getAvatarList(bool& isOk, ENUM_CHARACTERJOB job,
                       std::vector<std::vector<std::pair<unsigned long, int> > >& out);
    void getStikerList(bool& isOk, ENUM_CHARACTERJOB job, std::vector<int>& out);
    // [L16 2026-09-10] P235 同址 union 双视角：ORIG importAvatarRouletteList
    // 0x0891179f 段 store 为 `mov %eax,%edx; mov this,%eax; mov %edx,(%eax)`
    // （RHS-first + 中转 = 字段与 RHS 异型，P211 档③）⇒ 原名 int 保留。
    union {
        int m_avatarCoinInfo;           // +0x00 [avatar coin info]
        unsigned int m_avatarCoinInfoU; // +0x00 同址 unsigned 视角（[L16]）
    };
    int m_rareCloneAvatarRate;          // +0x04 [rare clone avatar rate]
    int m_rareAvatarRate;               // +0x08 [rare avatar rate]
    int m_upperAvatarRate;              // +0x0c [upper avatar rate]
    int m_normalAvatarRate;             // +0x10 [normal avatar rate]
    std::vector<std::vector<std::pair<unsigned long, int> > >
        m_avatars[11];                  // +0x14（job 桶）
    std::vector<int> m_stikers[11];     // +0x98（getStikerList 同名语义）
};
bool importAvatarRouletteList(char* path, AvatarRoulette& out);   // _Z24importAvatarRouletteListPcR14AvatarRoulette
// _Z30importAvatarRouletteListDetailR14AvatarRouletteRSs17ENUM_CHARACTERJOB
// 0x089115b1：各 job 节内层装载器（本 TU 提供）。

// ORIG _ZN20AvatarRouletteServerC1Ev 0x0817f354 / 4InitEP14AvatarRoulette 0x0817f706 /
//      18InitAddCharacSkillEv 0x0817f386
// sizeof 0x20：map<ulong,map<int,vector<LevelUpSkill>>>@0（ctor 首指令）、
// AvatarRoulette*@0x18、int 总数@0x1c（Pickup get_rand_int 上界）。
class AvatarRouletteServer
{
public:
    // ORIG W ctor _ZN20AvatarRouletteServer16PickedAvatarInfoC1Ev @0x0819a336。
    // 布局为 Arad_INTER_kAvatarRoulette::dispatch_sig / Pickup 写读字节实证：
    //   +0x04 unsigned int m_itemId；+0x08 char option/skill idx；+0x09 char pad；
    //   +0x0a char zoneType（pair.second 低字节）；+0x0b char gradeBand。
    struct PickedAvatarInfo
    {
        PickedAvatarInfo();                          // ORIG W 0x0819a336
        int m_pad0;                 // +0x00【推断：ORIG 未见读写】
        // [A19·T1 直证] ORIG 0x0817fcab / 0x08198d46 均为 `mov 0x4(%eax),%esi`
        // **单段直落** callee-saved（形参与 CDataManager::find_item(int)、
        // CInventory::AddAvatarItem(int,...) 同型）；unsigned int 版在两侧调用点
        // 各多 1 条 `mov %eax,%esi` 转换中转。
        unsigned int m_itemId;      // +0x04（A19 A/B 临时基线态：净账核算中）
        char m_optionOrSkillIdx;    // +0x08
        char m_pad9;                // +0x09
        char m_zoneType;            // +0x0a
        char m_gradeBand;           // +0x0b
    };  // sizeof 0x10【推断：dispatch_sig 栈对象占用 ebp-0x54..ebp-0x45】

    AvatarRouletteServer();                       // ORIG 0x0817f354 T
    ~AvatarRouletteServer();                      // ORIG 0x0817f372 T
    bool Init(AvatarRoulette* roulette);          // ORIG 0x0817f706 T
    bool InitAddCharacSkill();                    // ORIG 0x0817f386 T
    bool isAvatarPottery(unsigned long itemIdx);  // ORIG 0x0817fb0a T
    // ORIG 0x0817fb28 T _ZN20AvatarRouletteServer6PickupERNS_16PickedAvatarInfoEii
    // 定义由 AvatarRouletteServer TU 批次提供。
    bool Pickup(PickedAvatarInfo& picked, int grade, int index);


    std::map<unsigned long, std::map<int, std::vector<LevelUpSkill> > >
        m_addCharacSkillMap;   // +0x000
    AvatarRoulette* m_pRoulette;   // +0x018
    int m_totalCount;              // +0x01c
};  // sizeof 0x20
// [A19 实测] PickedAvatarInfo 6 个成员合计 0xc 字节（4+4+1+1+1+1，4 对齐 ⇒
// sizeof 0xc）。ORIG dispatch_sig 里该栈对象占 ebp-0x54..ebp-0x49，紧邻的
// `now` 落在 ebp-0x48（Arad_InternalDispatcher_INTER.cpp:642 段实证）⇒ 0xc 自洽，
// 旧注释「ebp-0x45」把下一个变量算进来了。
static_assert(sizeof(AvatarRouletteServer::PickedAvatarInfo) == 0xc,
              "PickedAvatarInfo ABI drift");
static_assert(offsetof(AvatarRouletteServer::PickedAvatarInfo, m_itemId) == 0x04,
              "PickedAvatarInfo::m_itemId offset drift");

// ORIG SDCActiveStatusSyncCommon：Server 基类（ctor W 0x08234870 / dtor W
// 0x08234876，均空体 ret）。定义：CtorProducers_2.cpp。
class STSDCActiveStatusSyncDataSend;   // 布局见下方 packed 定义
class STSDCActiveStatusSyncDataCLONG;  // 完整定义见 STSDCActiveStatusSyncDataCLONG.h（Send::operator= 形参）
class IRDActiveObject;                 // 仅前置（getActiveStatusDurationClient 不解引用）

namespace _SDC
{
bool isObjectType(int objectType, int code);   // T 0x080950e8（b28d-lane15 迁定义至 SDCLink.cpp）
}

float GetUniformVelocityFloatSDC(float v0, float v1, int cur, int max);
                                                // _Z26GetUniformVelocityFloatSDCffii T 0x088924c1

// ORIG 布局按 operator=(0x808d18c) 字节映射重构 [推断字段名]；packed，
// sizeof 0x35。damage 侧访问语义实证：+3 baseDamage、+7 tick、
// +0xb attacker 类型(==10)、+0xc PvP 标志、+0x1d HP 百分比。
#pragma pack(push, 1)
struct STSDCActiveStatusSyncDataSend
{
    unsigned char m_field00;      // +0x00
    unsigned char m_field01;      // +0x01
    unsigned char m_field02;      // +0x02
    int m_baseDamage;             // +0x03
    int m_durationTick;           // +0x07
    unsigned char m_attackerType; // +0x0b（getBurn/… ==10 判定）
    unsigned char m_isPvp;        // +0x0c（!=0 → ×userToUserRate）
    int m_field0d;                // +0x0d
    int m_field11;                // +0x11
    int m_field15;                // +0x15
    int m_field19;                // +0x19
    int m_hpPercent;              // +0x1d（<100 才产出伤害）
    int m_field21;                // +0x21
    int m_field25;                // +0x25
    int m_field29;                // +0x29
    int m_field2d;                // +0x2d
    int m_field31;                // +0x31

    // ORIG T 0x0808d18c：CLONG（0x44 展开）→ 本 packed 块字节截断回填。
    STSDCActiveStatusSyncDataSend& operator=(
        const STSDCActiveStatusSyncDataCLONG& o);
};                                // sizeof = 0x35
static_assert(sizeof(STSDCActiveStatusSyncDataSend) == 0x35,
              "STSDCActiveStatusSyncDataSend ABI drift");
#pragma pack(pop)

#ifndef DNF_ENUM_ACTIVESTATUS_DEFINED
#define DNF_ENUM_ACTIVESTATUS_DEFINED
enum ENUM_ACTIVESTATUS
{
    ACTIVE_STATUS_SLOW = 0, ACTIVE_STATUS_FREEZE = 1, ACTIVE_STATUS_POISON = 2,
    ACTIVE_STATUS_STUN = 3, ACTIVE_STATUS_CURSE = 4,   ACTIVE_STATUS_BLIND = 5,
    ACTIVE_STATUS_LIGHTNING = 6, ACTIVE_STATUS_STONE = 7, ACTIVE_STATUS_SLEEP = 8,
    ACTIVE_STATUS_BURN = 9, ACTIVE_STATUS_WEAPON_BREAK = 10,
    ACTIVE_STATUS_BLEEDING = 11, ACTIVE_STATUS_PIERCING = 12,
    ACTIVE_STATUS_STUCK = 13, ACTIVE_STATUS_CONFUSE = 14,
    ACTIVE_STATUS_HOLD = 15, ACTIVE_STATUS_ARMOR_BREAK = 16,
    ACTIVE_STATUS_17 = 17   // [推断]
};
#endif  // DNF_ENUM_ACTIVESTATUS_DEFINED

class SDCActiveStatusSyncCommon
{
public:
    SDCActiveStatusSyncCommon();
    ~SDCActiveStatusSyncCommon();

    // 全部非虚；ORIG 地址注释逐方法标注。
    int getDuration(ENUM_ACTIVESTATUS status, int level, void* param);   // 0x0808dc7a（透传返回值）
    int getBurnDamage(bool& flag, STSDCActiveStatusSyncDataSend* d,
                      void* obj, int t);                          // 0x0808de08
    int getPoisonDamage(STSDCActiveStatusSyncDataSend* d, void* obj,
                        int t);                                    // 0x0808dcc4
    int getDiseaseDamage(STSDCActiveStatusSyncDataSend* d, int t); // 0x0808dd5c
    void makeEtcParameters(ENUM_ACTIVESTATUS status, int level, void* param,
                           std::vector<int>* out, int* outPtr);    // 0x0808d4b4
    void makeEtcParameters(ENUM_ACTIVESTATUS status, int level,
                           std::vector<int>& out);                 // 0x0808d50e
    float getActiveStatusPower1(int idx, int level);               // 0x0808d37a
    float getActiveStatusPower2(int idx, int level);               // 0x0808d3dc
    int getBleedingDamageClient(STSDCActiveStatusSyncDataSend* d,
                                bool flag, void* obj);             // 0x0808de9e
    int getBleedingDamageServer(STSDCActiveStatusSyncDataSend* d); // 0x0808df1a
    float getUserToUserDamageRate(bool deathTower);                // 0x0808df5c
    float getActiveStatusEtcParameter(int idx, int level, int j);  // 0x0808d43e
    int getActiveStatusDurationClient(int idx, int level,
                                      IRDActiveObject* obj);       // 0x0808d268（纯 ret 0）
    int getActiveStatusDurationServer(int status, int level,
                                      int objectType, int a4,
                                      int hpPenalty);              // 0x0808d272
    void getActiveStatusDamageDurationForSDC(
        char attackType, int status, int* outA, int* outB,
        STSDCActiveStatusSyncDataSend* d, void* obj);              // 0x0808df82
};

// ORIG SDCActiveStatusSyncServer：栈局部对象（ctor W 0x0823487c / dtor W
// 0x08234890，均仅委托基类），loadActiveStatusScript T 0x0808e13c 尾透传。
// 定义：CtorProducers_2.cpp。
class SDCActiveStatusSyncServer : public SDCActiveStatusSyncCommon
{
public:
    SDCActiveStatusSyncServer();
    ~SDCActiveStatusSyncServer();
    bool loadActiveStatusScript();   // return importActiveStatusTable("Etc/ActiveStatusTable_ex.etc")
};

// ---- ActiveStatus 表装载（定义待数据域批次）----
bool importActiveStatusTable(const char* path);   // _Z23importActiveStatusTablePKc T 0x08906f88（成功 ret1/失败 logFormat ret0）

#endif  // GAME_DATAMANAGER_INIT_DEPENDS_H_
