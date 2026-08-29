#ifndef DNF_GAME_DB_LOAD_ETC_H_
#define DNF_GAME_DB_LOAD_ETC_H_

// ============================================================================
// df_game_r 还原 —— DB_LoadEtc（SIG_LOAD_ETC DB 装载分发器，DB 线程侧）。
// ORIG 全部符号见 docs/class_func_reports/DB_LoadEtc/（40 mangled）。
//   ctor 0x08403b84（C1/C2 同址：调 DBDispatcher::DBDispatcher 后设 vptr）
//   ~DB_LoadEtc 0x08403ba0（D1/D2）/ 0x08403bd0（D0 deleting）
//   dispatch 0x08403bee
// 对象尺寸 4（仅 vptr），基类 DBDispatcher 见同目录头。
//
// CHARAC_QUEST_SHOP_DATA（_LoadQuestShop 0x08404b72 写点 + SIG_LOAD_ETC 区间
//   [0xd5a7,0xd651)=0xaa 主/tag 双实例证据）建模于本头唯一权威定义。
// ============================================================================

#include "SigTypes.h"      // SIG_LOAD_ETC / SIG_ATTENDANCE_EVENT / comoboSkillData
                           // / SIG_COMBO_SKILL
#include "GameEnums.h"     // ENUM_SKILL_TREE_KIND
#include "DBDispatcher.h"
#include "CHARAC_QUEST_SHOP_DATA.h"  // CHARAC_QUEST_SHOP_DATA（原本头定义，抽出供
                                    //   SigTypes_D.h SIG_QUEST_SHOP_DATA 双实例结构）
class Stream;
class MySQL;

// ---- CHARAC_QUEST_SHOP_DATA（ORIG 布局证据 = _LoadQuestShop 各 get_* 写点）----
// 列序 qp/init_count/max_hp/max_mp/psy_attack/... → 具名字段对齐偏移全部取证。
class DB_LoadEtc : public DBDispatcher
{
public:
    DB_LoadEtc();                          // C1/C2 0x08403b84
    virtual ~DB_LoadEtc();                 // D1/D2 0x08403ba0（D0 deleting
                                           // 0x08403bd0 由编译器产出）

    virtual bool dispatch(int userIdx, int slot, Stream* stream);
                                           // ORIG T 0x08403bee
    void LoadComboSkillInfo(SIG_LOAD_ETC* sig);            // 0x0844a3b2
    int  GetUserRecord(SIG_LOAD_ETC* sig);                 // 0x084075c4（恒返 1）
    int  GetUserPvp(SIG_LOAD_ETC* sig);                    // 0x08407dc2（0=OK 1=无行 -1=错）
    int  GetUserPvpBase(SIG_LOAD_ETC* sig);                // 0x08407db4（恒返 1）
    bool GetUserSkill(SIG_LOAD_ETC* sig);                  // 0x084075d2

    bool LoadLitmitItem(SIG_LOAD_ETC* sig);                // 0x08405c60（恒返 1）
    bool LoadCharacItemStat(SIG_LOAD_ETC* sig);            // 0x08445660
    bool LoadItemMakingSkill(SIG_LOAD_ETC* sig);           // 0x084040b4
    bool LoadProperDungeonClearCount(SIG_LOAD_ETC* sig);   // 0x084073aa
    bool LoadConditionEventInfo(SIG_LOAD_ETC* sig);        // 0x08407296
    int  _loadNPCRelasionship(SIG_LOAD_ETC* sig);          // 0x08407482（返回 LineFunc 错误码）
    bool LoadRedeemItem(SIG_LOAD_ETC* sig);                // 0x08406a6a
    bool LoadPvPMissionList(SIG_LOAD_ETC* sig);            // 0x08406cce
    bool LoadTODUserState(SIG_LOAD_ETC* sig);              // 0x08406eec
    bool LoadVillageAttackInfo(SIG_LOAD_ETC* sig);         // 0x0843f038
    bool LoadAccountFirstLogin(SIG_LOAD_ETC* sig, int eventCode);        // 0x08405c6e
    bool LoadAccountFirstLoginDaily(SIG_LOAD_ETC* sig, int eventCode);   // 0x08405e0a
    bool LoadFairPvPScore(SIG_LOAD_ETC* sig);              // 0x08404290
    bool LoadQuestShop(SIG_LOAD_ETC* sig);                 // 0x08404a68
    static bool _LoadQuestShop(unsigned int characNo,
                               CHARAC_QUEST_SHOP_DATA* out);             // 0x08404b72（static）
    bool LoadTitleBook(SIG_LOAD_ETC* sig);                 // 0x08406092
    bool LoadAchievement(SIG_LOAD_ETC* sig);               // 0x08406422
    bool LoadOnceGiveAvengerTitle(SIG_LOAD_ETC* sig);      // 0x08406600
    bool LoadAuraAvatarOption(SIG_LOAD_ETC* sig);          // 0x0840838e
    bool LoadCharacOption(int userIdx, int slot, SIG_LOAD_ETC* sig);     // 0x084084e0
    bool LoadBoosterGage(SIG_LOAD_ETC* sig);               // 0x08446d9e
    bool LoadPcRoomPlayTimeInfo(SIG_LOAD_ETC* sig);        // 0x084066ae
    bool LoadUsedGiftFatigueInfo(SIG_LOAD_ETC* sig);       // 0x08406810
    bool LoadAttendanceInfo(SIG_ATTENDANCE_EVENT* sig,
                            unsigned int accId);                         // 0x08448944
    bool LoadEventServerMessageInfo(SIG_LOAD_ETC* sig);    // 0x084356a2
    bool LoadComboSkill(SIG_LOAD_ETC* sig, ENUM_SKILL_TREE_KIND kind);   // 0x0844a47a
    bool getComboSkill(MySQL* db, unsigned int accId, int count,
                       comoboSkillData* out);                            // 0x0844a704
    bool LoadSeriaRoomAniDeco(SIG_LOAD_ETC* sig);          // 0x0844a20a
    bool SendResult(int userIdx, int slot, SIG_LOAD_ETC* sig);           // 0x08408766
};

#endif  // DNF_GAME_DB_LOAD_ETC_H_
