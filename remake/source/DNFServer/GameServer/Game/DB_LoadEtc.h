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
class Stream;
class MySQL;

// ---- CHARAC_QUEST_SHOP_DATA（ORIG 布局证据 = _LoadQuestShop 各 get_* 写点）----
// 列序 qp/init_count/max_hp/max_mp/psy_attack/... → 具名字段对齐偏移全部取证。
#pragma pack(push, 1)
struct CHARAC_QUEST_SHOP_DATA
{
    unsigned int   m_field0;                 // +0x000（未访问；前导）
    unsigned int   m_qp;                     // +0x004
    unsigned int   m_initCount;              // +0x008
    unsigned short m_psyAttack;              // +0x00c
    unsigned short m_magAttack;              // +0x00e
    unsigned short m_magDefence;             // +0x010
    unsigned short m_psyDefense;             // +0x012
    unsigned char  m_pad14[0x20 - 0x14];     // +0x014
    unsigned short m_attackSpeed;            // +0x020
    unsigned short m_moveSpeed;              // +0x022
    unsigned char  m_pad24[2];               // +0x024
    unsigned short m_maxHp;                  // +0x026
    unsigned short m_hpRegen;                // +0x028
    unsigned short m_psyCritical;            // +0x02a
    unsigned short m_magCritical;            // +0x02c
    unsigned char  m_pad2e[0x34 - 0x2e];     // +0x02e
    unsigned short m_fireElementResist;      // +0x034
    unsigned short m_waterElementResist;     // +0x036
    unsigned short m_darkElementResist;      // +0x038
    unsigned short m_lightElementResist;     // +0x03a
    unsigned short m_allElementResist;       // +0x03c
    unsigned char  m_pad3e[4];               // +0x03e
    unsigned short m_evasion;                // +0x042
    unsigned char  m_pad44[2];               // +0x044
    unsigned short m_mpRegen;                // +0x046
    unsigned short m_hitRecovery;            // +0x048
    unsigned char  m_pad4a[4];               // +0x04a
    unsigned short m_goodHit;                // +0x04e
    unsigned char  m_pad50[0x60 - 0x50];     // +0x050
    unsigned short m_fireElementAttack;      // +0x060
    unsigned short m_waterElementAttack;     // +0x062
    unsigned short m_darkElementAttack;      // +0x064
    unsigned short m_lightElementAttack;     // +0x066
    unsigned short m_allElementAttack;       // +0x068
    unsigned char  m_pad6a[2];               // +0x06a
    unsigned short m_maxMp;                  // +0x06c
    unsigned char  m_pad6e[0x76 - 0x6e];     // +0x06e
    unsigned short m_separatePsyMagAttack;   // +0x076
    unsigned char  m_pad78[0xa6 - 0x78];     // +0x078
    unsigned int   m_questPiece;             // +0x0a6
};
#pragma pack(pop)
static_assert(sizeof(CHARAC_QUEST_SHOP_DATA) == 0xaa,
              "CHARAC_QUEST_SHOP_DATA ABI drift");

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
