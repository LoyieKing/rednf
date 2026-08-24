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


class InGameEventInfoList;             // CEventScriptMng.h（全局作用域基类 1）

// ---- RDAR 脚本根基（定义：ScriptExtras.cpp）----
void SetPriorityBaseDirectory(const char* dir);               // _Z24SetPriorityBaseDirectoryPKc 0x088bb40a
bool InitRDARScript(const char* dir, bool compiled,
                    const char* packFile, int* err);          // _Z14InitRDARScriptPKcbS0_Pi 0x088bb5d2
bool InitRDARScriptStringManager(const char* dir, const char* file,
                                 std::vector<ErrorLog>* err); // _Z27InitRDARScriptStringManagerPKcS0_PSt6vectorI8ErrorLogSaIS2_EE 0x088bff9c

void InitMapEquipmentAniScript(void);                         // _Z25InitMapEquipmentAniScriptv 0x089c0586

// ---- 全局环境谓词 ----
char IsLightServer(void);                                     // _Z13IsLightServerv 0x0822ad44（定义 GlobalData.cpp）
void SetCollectDungeonStatistic(bool flag);                   // _Z26SetCollectDungeonStatisticb
bool prepareServerDelayCheckDamage(void);                     // _Z29prepareServerDelayCheckDamagev

// ---- Etc 参数装载 ----
int  importExperienceItemMapInfo(std::map<int, int>& m, bool second);              // _Z27..RSt3mapIii..b 0x089136a4
bool importImageCommunicationInfo(const char* path,
                                  class ImageCommunicationData* out);              // _Z28..PKcP22ImageCommunicationData 0x08913dbc
bool importBoosterGage(char* path, class BoosterGageData* out);                    // _Z17importBoosterGagePcP15BoosterGageData 0x08913a7e
bool importInvalidName(char* path, class InvalidCharData* out);                    // _Z17importInvalidNamePcP15InvalidCharData 0x08908338
bool importSlangName(char* path, class SlangNameData* out);                        // _Z15importSlangNamePcP13SlangNameData
int  importGoldLimitByLevel(unsigned int* table);                                  // _Z22importGoldLimitByLevelPj
bool importOldEquipmentStatInfo(const char* arg);                                  // _Z26importOldEquipmentStatInfoPKc
int  importItmeGroupTable(const char* dir, const char* file,
                          std::map<std::string, int>& table);                      // _Z20importItmeGroupTablePKcS0_RSt3mapISsiSt4lessISsESaISt4pairIKSsiEEE（int 错误码：0=成功/0x1eb2=加载失败/0x1ebd=ScanStr 失败；ORIG 0x0890bb25 返回 undefined4，init.md +0x3e48 以 cmpl $0x0 判失败）

// ---- 事件/Etc 装载（形参类型均为 CDataManager.h 完整类）----
bool importInGameAdvertisementEtcScript(class InGameAdvertisementScript* s, const char* path); // _Z34..
bool importSeriaRoomDecoEvent(class seriaRoomDecoEventScript* s, const char* path);           // _Z24..
bool importGrowthEquipmentScript(class growthEquipmentScript* s, const char* path);           // _Z27..
char IsLightServer(void);                                     // _Z13IsLightServerv 0x0822ad44（定义 GlobalData.cpp）
void SetCollectDungeonStatistic(bool flag);                   // _Z26SetCollectDungeonStatisticb
bool prepareServerDelayCheckDamage(void);                     // _Z29prepareServerDelayCheckDamagev
bool ImportWorldDrop(class WorldDropInfo* out, char* path);   // _Z15ImportWorldDropP13WorldDropInfoPKc
int  ImportWarAreaCounter(class WarAreaCounter* out);         // _Z20ImportWarAreaCounterP14WarAreaCounter（0=成功；定义 InitScriptProducers_2.cpp）
namespace AvatarVariation
{
class AvatarColorInfo;
bool importAvatarColorVariation(AvatarColorInfo* out);        // _ZN15AvatarVariation26importAvatarColorVariationEPNS_15AvatarColorInfoE
}
bool importRandomOptionScript(class RandomOptionScript& script,
                              const char* path);              // _Z24importRandomOptionScriptR18RandomOptionScriptPKc（定义 RandomOptionScript.cpp；本头不引其权威头以避免与 CDataManager.h 本地类冲突）
bool importCharacterCreateGiftScript(class createChracScript* s, const char* path);           // _Z31..
bool importUseJobFatigueGiftScript(class useCharacterJobFatigue* s, const char* path);        // _Z29..
bool importCharacterCreateCreatureGiftScript(class CharacterCreateCreatureGiftScript* s,
                                             const char* path);                               // _Z39..
bool importSeriaRoomAnimationDecoEvent(class seriaRoomDecoAniEventScript* s,
                                       const char* path);                                     // _Z33..
bool ImportBingoScript(const char* path, class stBingoScript& out);        // _Z17ImportBingoScriptPKcR13stBingoScript
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
int  importUltimateDungeonList(const char* path, std::set<int>& out);      // _Z25importUltimateDungeonListPKcRSt3setIiSt4lessIiESaIiEE
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
    // 难度表：ctor 循环 i=0..5 清零五组；[difficulty] 以 1 基下标写入。
    unsigned char m_field6c[7];      // +0x6c（+0x6d..0x72 = 'A'..'E' 槽 tag 字节）
    unsigned short m_field72[7];     // +0x72（+0x74..0x7e = 各槽 monster 数）
    unsigned char m_field7e[7];      // +0x7e
    unsigned char m_field84[7];      // +0x84
    unsigned char m_field8a[7];      // +0x8a
    int m_guideLineValue;            // +0x90 [guide line value]
};                                 // sizeof 0x94

// ORIG 返回错误码（bool 语义 0=成功）：0x1a24 装载失败 / 0x1a2e..0x1b30 各 Scan 失败 /
// 0x1b08 组下标重复 / 0x1aed 未知组名；"[hellparty script end]" → 0。
int importHellPartyScript(const char* path, stHellParty_t& out);           // _Z21importHellPartyScriptPKcR13stHellParty_t
int  importQuestShopScript(const char* path, class stQuestShop& out);      // 返回 0=成功
int  initIndependentDropScript(char* dir, char* list);                     // _Z25initIndependentDropScriptPcS_
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
bool importChannelScript(class channel_script_t* script);                  // _Z19importChannelScriptP16channel_script_t
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

// ORIG ImportSecurityReward(SecurityReward*) 写入 0x30 字节宿主
// （CDataManager::m_securityReward）。专属 TU 未落地，先给可编译最小视图。
// ORIG NeedMaterialDataManager::getInstance T 0x081b5ab2（单例；定义 CItem.cpp 域）
class NeedMaterialDataManager
{
public:
    static NeedMaterialDataManager* getInstance();
};

// ORIG CmmChannelProxy（GlobalData.h 前向；完整布局在 GlobalData.cpp/GameMain.cpp
// 各自局部镜像——本 TU 仅需 SendCheckChannelScript 一员，ORIG
// _ZN15CmmChannelProxy22SendCheckChannelScriptEv，init 以 s_channel_proxy 调用）
class CmmChannelProxy
{
public:
    bool SendCheckChannelScript();
};
class SecurityReward
{
public:
    // ORIG 无 ctor/dtor 符号（纯 POD，0x30）。ImportSecurityReward(0x089113ec)
    // 三段各写 4 个 int：[GoblinPad Reward]/[SecurityCard Reward]/[OTP Reward]。
    int m_goblinPad[4];    // +0x00..+0x0f
    int m_securityCard[4]; // +0x10..+0x1f
    int m_otp[4];          // +0x20..+0x2f
};
bool ImportSecurityReward(SecurityReward* out);   // _Z20ImportSecurityRewardP14SecurityReward（定义 InitScriptProducers_2.cpp）

// ---- ORIG TowerOfDespairMgr（sizeof 0x35c；ctor 0x08643ac4 / dtor 0x08643be2 /
//      LoadScript 0x08643e7e；报告 TowerOfDespairMgr/*.md。Ctor_Group1 权威化）----
struct TOD_Layer            // ORIG resetLayer 0x085fe852：首 short 置 1
{
    TOD_Layer(unsigned short layer);   // ORIG T 0x085fe7b4（报告 TOD_Layer/TOD_Layer.md）
    void resetLayer() { m_layer = 1; }
    unsigned short GetLayer() const;   // ORIG T 0x0822ec40（定义 PartyDepsA2.cpp）
    // ORIG T 0x086439c8（报告缺失，按 nm 实现）：层 +n，>100 回卷 1。
    TOD_Layer operator+(int n) const;

    unsigned short m_layer;
};

// ORIG TOD_UserState（角色扩展数据 0xd 槽对象；完整布局未建模，此处仅声明
// GetDungeonIndex_isTOD 所需一员。对象由 CUserCharacInfo 数据区承载，本工程不构造）。
class TOD_UserState
{
public:
    // ORIG T 0x0864383e（报告 TOD_UserState/getTodayEnterLayer.md）：sret 返回
    // *(const TOD_Layer*)(this+0x14) + 1（经 TOD_Layer(ushort) 构造，>100 回卷 1）。
    TOD_Layer getTodayEnterLayer() const;
};
class PacketBuf;            // 完整定义见 PacketBuf.h（SendingAPC_Info 内嵌）
struct SendingAPC_Info      // ORIG ctor 0x085fe860：PacketBuf@+4 构造+clear，
{                           // +0x17338 起 0xbc 清零，sizeof 0x1733c
    SendingAPC_Info();
    ~SendingAPC_Info();
    char m_pad0[4];                       // +0x00
    PacketBuf* m_bufView;                 // 占位指针域【布局见下方 static_assert】
    char m_pad8[0x1733c - 8];
};
class TowerOfDespair_APC_Mgr   // ORIG = map<TOD_Layer,SendingAPC_Info> @0
{
public:
    TowerOfDespair_APC_Mgr();
    ~TowerOfDespair_APC_Mgr();
    std::map<int, SendingAPC_Info*> m_map;   // 值改堆指针避免 0x1733c 栈值拷贝【推断】
};

// ORIG sizeof 0x35c：CDungeon*[101]@0x000、uint[101]@0x194、set@0x328、
// vector<uint>@0x340、ushort 0x34c/0x34e/0x350、APC_Mgr*@0x354、int@0x358。
class CDungeon;
class CUser;
class TowerOfDespairMgr
{
public:
    TowerOfDespairMgr();                              // ORIG 0x08643ac4
    ~TowerOfDespairMgr();                             // ORIG 0x08643be2
    int LoadScript(const char* dir, const char* path); // ORIG 0x08643e7e（0=成功）
    void _Set_LayerByDungeonIndex(unsigned int layer, const CDungeon* dungeon); // 0x08643c6c
    void _Set_LayerByDungeonIndex(unsigned int layer, unsigned int idx);        // 0x08643c86
    void _Set_APCIndexByLayer(unsigned int layer);     // 0x08643cf4

    // ---- 查询族（报告 TowerOfDespairMgr/*.md；定义 PartyDepsA2.cpp，
    //      CheckEnterable 定义 blocked 待 TOD 批次）----
    CDungeon* Get_CDungeonByLayer(unsigned int layer);          // ORIG T 0x08643d14
    unsigned int Get_DungeonIndexByLayer(unsigned int layer);   // ORIG T 0x08643d76
    int Get_APCIndexByLayer(unsigned int layer);                // ORIG T 0x08643d9c（不在表=−1）
    int GetBaseLayerIndex();                                    // ORIG T 0x086440a0（缓存 m_field358）
    int CheckEnterable(CUser* user, const CDungeon* dungeon);   // ORIG T 0x086440ee【blocked】

    CDungeon* m_layerDungeons[101];                    // +0x000
    unsigned int m_dungeonIndexOfLayer[101];           // +0x194
    std::set<int> m_layerSet;                          // +0x328
    std::vector<unsigned int> m_npcLayers;             // +0x340
    unsigned short m_enterableCount;                   // +0x34c [enterable count]
    unsigned short m_minLevel;                         // +0x34e [min Level]
    unsigned short m_topLayer;                         // +0x350 [top layer]
    TowerOfDespair_APC_Mgr* m_apcMgr;                  // +0x354
    int m_field358;                                    // +0x358（GetBaseLayerIndex 缓存）
    static void SendMessageToCUser(CUser* user, char* msg);   // ORIG T 0x08644366（静态成员：
                                                              //   ORIG asm 无 this 槽，user@0x8；依赖 CUser::Send）
};  // sizeof 0x35c

// ---- ORIG CLevelRewardTable(0x6088)/CLevelRewardTableMgr(0x14) 权威
//      （ctor 0x086975feW/0x08687710、GetInstance W 0x0837505b、
//       SetChallengeInfo 0x08687828；Ctor_Group1）----
class CLevelRewardTable
{
public:
    CLevelRewardTable();                              // ORIG 0x086975feW
    void SetJobAffection(bool on);                    // ORIG 0x08697766W
    bool GetJobAffection() const { return m_jobAffection; }
    int GetProperJobIndex(char job);                  // ORIG 0x08687e78
    int GetProperLevelIndex(unsigned int level);      // ORIG 0x086876c8
    int GetRewardItemNo(int jobIdx, int levelIdx);    // ORIG W 0x086976f6（定义 CUserDepsB.cpp）
    int GetItemCount(int jobIdx, int levelIdx);       // ORIG W 0x08697712（定义 CUserDepsB.cpp）
    char* GetMailTitle(int levelIdx);                 // ORIG W 0x08697730（定义 CUserDepsB.cpp）
    char* GetMailString(int levelIdx);                // ORIG W 0x08697750（定义 CUserDepsB.cpp）

    int m_levelKeys[0x46];                // +0x000
    int m_rewardPrimary[9 * 0x46];        // +0x118..0xae7
    int m_rewardSecondary[9 * 0x46];      // +0xaf0..0x14bf
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
    bool IsOn();                                      // ORIG W 0x08697792（读 m_flag；定义 CUserDepsB.cpp）
    bool IsChallengingJob(char job);                  // ORIG T 0x08687792（定义 CUserDepsB.cpp）
    CLevelRewardTable* GetProperLevelRewardTableR() const;  // ORIG T 0x08687e62（返回 m_table；定义 CUserDepsB.cpp）
    static CLevelRewardTableMgr* levelRewardTableMgr;            // ORIG B 0x09473978

    char m_flag;                          // +0x00
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
    void swap(AvatarRoulette* other);   // ORIG 0x8911e4e T：前 5 dword + 11 桶 swap，返回 1

    int m_avatarCoinInfo;               // +0x00 [avatar coin info]
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
    AvatarRouletteServer();                       // ORIG 0x0817f354 T
    ~AvatarRouletteServer();                      // ORIG 0x0817f372 T
    bool Init(AvatarRoulette* roulette);          // ORIG 0x0817f706 T
    bool InitAddCharacSkill();                    // ORIG 0x0817f386 T
    bool isAvatarPottery(unsigned long itemIdx);  // ORIG 0x0817fb0a T

    std::map<unsigned long, std::map<int, std::vector<LevelUpSkill> > >
        m_addCharacSkillMap;   // +0x000
    AvatarRoulette* m_pRoulette;   // +0x018
    int m_totalCount;              // +0x01c
};  // sizeof 0x20

// ORIG SDCActiveStatusSyncCommon：Server 基类（ctor W 0x08234870 / dtor W
// 0x08234876，均空体 ret）。定义：CtorProducers_2.cpp。
class SDCActiveStatusSyncCommon
{
public:
    SDCActiveStatusSyncCommon();
    ~SDCActiveStatusSyncCommon();
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

// ORIG WongWork::CCompoundAvatar（0x88）：ctor T 0x08331d74 + LoadScript
// ("Etc/","CompoundAvatar.etc") T 0x08333ea6。权威完整布局（含
// tagCompoundAvatar/tagCompoundAvatarTable/importCompoundAvatarTable 声明）
// 收敛至 CCompoundAvatar.h（Ctor2_ContB）。
#include "CCompoundAvatar.h"

#endif  // GAME_DATAMANAGER_INIT_DEPENDS_H_
