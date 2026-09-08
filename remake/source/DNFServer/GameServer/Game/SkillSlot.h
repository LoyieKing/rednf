#ifndef DNF_GAME_SKILLSLOT_H_
#define DNF_GAME_SKILLSLOT_H_

// df_game_r 玩家技能槽 SkillSlot 还原（G1 玩家域第一批）。
// 布局依据 ORIG 构造/析构（0x08694d6a / 0x08694e80 / 0x086996f6 拷贝构造）：
//   +0x00 CUser* m_parent；+0x04/+0x0c int m_remainSP[2]；+0x08 char
//   +0x10 int（构造 -1）；+0x14/+0x18 int m_remainSFP[2]
//   +0x1c ComboSkill（0x2a 字节）；+0x46/+0x1de/+0x376 _Mastered_skill[204]
//   +0x510/+0x514 int m_tempRemainSP/SFP；sizeof = 0x518。
// 依赖的 CUser/CUserCharacInfo 等暂无权威头，此处给最小声明（签名按 ORIG
// mangling 逐一核对；真实定义属后续 G1 玩家批次）。

#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CUser.h"          // 权威 CUser/CUserCharacInfo/cUserHistoryLog（G1-3 统一）
#include "SigTypes.h"       // SIG_COMBO_SKILL/comoboSkillData 权威定义（FixMissingSigTypes 迁移）
#include "MasteredSkill.h"  // _Mastered_skill（原本头定义，抽出供 SigTypes_C.h 共用）
#include "STGuildSkill.h"   // STGuildSkill 5 字节技能条目

class CSkill;
class PacketGuard;
class GameWorld;
class CDataManager;

extern CDataManager* G_CDataManager();
extern GameWorld* G_GameWorld();

// ---- send_skill_info 使用的具名公会技能区域视图 ----
// ORIG 以公会对象 +0x40 为区域基址,技能数组从区域 +0x05 开始；pack(1)
// 保持 m_skillLearnCnt@+0x44、m_skills[i]@+0x45+i*5 的真实布局。
#pragma pack(push, 1)
struct STGuildSkillSectionView
{
    char m_subMasterDataTail[2];       // 公会数据 +0x40..+0x41
    unsigned short m_guildSkillPoint;   // +0x42
    unsigned char m_skillLearnCnt;      // +0x44
    STGuildSkill m_skills[16];          // +0x45, 5 字节/项
};

struct STGuildDBInfoPacketView
{
    char m_guildName[0x17];             // +0x00
    int m_masterNo;                      // +0x17
    unsigned char m_fundLevel;           // +0x1b
    unsigned short m_ability;            // +0x1c
    int m_field1E;                       // +0x1e
    unsigned short m_memberCount;       // +0x22
    int m_guildPoint;                    // +0x24
    unsigned char m_guildRank;           // +0x28
    unsigned int m_fundExp;              // +0x29
    unsigned char m_subMasterCnt;        // +0x2d
    char m_subMasterData[0x12];          // +0x2e..+0x3f
    STGuildSkillSectionView m_skillSection; // +0x40
};
#pragma pack(pop)

// ---- 技能树枚举（mangling 20ENUM_SKILL_TREE_KIND；与 GameEnums.h 同值同 guard）----
#ifndef DNF_ENUM_SKILL_TREE_KIND_DEFINED
#define DNF_ENUM_SKILL_TREE_KIND_DEFINED
enum ENUM_SKILL_TREE_KIND
{
    SKILL_TREE_NONE = -1,
    SKILL_TREE_1 = 0,
    SKILL_TREE_2 = 1,
    SKILL_TREE_SFP_1 = 2,
    SKILL_TREE_SFP_2 = 3
};
#endif

// _Mastered_skill 权威定义已迁至 MasteredSkill.h（本头 include，供 SigTypes_C.h 共用）。
// ---- ComboSkill 存档结构 SIG_COMBO_SKILL / comoboSkillData ----
// 权威定义已迁移到 SigTypes.h（FixMissingSigTypes，广播周知；pack(1) 布局与
// sizeof=0xb2 不变）。此处仅保留使用点引用。

// ---- ComboSkill（0x2a 字节 = 2 个 std::map<short, vector<int>>，各 0x15）----
// ORIG 里 map<short,vector<int>> 是在 pack(1) 生效时首次实例化的，故 _Rb_tree
// 头 1 字节（比较器）后紧跟 _M_header，无 4 字节对齐填充 → 0x15/个。
#pragma pack(push, 1)
typedef std::map<short, std::vector<int> > ComboSkillMap;

class ComboSkill
{
public:
    ComboSkill();
    ComboSkill(const ComboSkill& other);
    ~ComboSkill();
    ComboSkill& operator=(const ComboSkill& other);

    bool saveData(SIG_COMBO_SKILL* data) const;
    bool loadData(SIG_COMBO_SKILL* data);
    void resetTree(ENUM_SKILL_TREE_KIND kind);
    void setComboSkill(ENUM_SKILL_TREE_KIND kind, ComboSkillMap& map);
    void getComboSkillSendList(PacketGuard& packet) const;
    void eraseDuplicationComboSKill(ENUM_SKILL_TREE_KIND kind, int slot);

    ComboSkillMap& getComboSkillMap(ENUM_SKILL_TREE_KIND kind)
    {
        return const_cast<ComboSkillMap&>(
            ((const ComboSkill*)this)->getComboSkillMap(kind));
    }

    const ComboSkillMap& getComboSkillMap(ENUM_SKILL_TREE_KIND kind) const
    {
        if (kind == SKILL_TREE_1)
            return m_comboSkillMap;
        if (kind == SKILL_TREE_2)
            return m_comboSkillSfpMap;
        return m_comboSkillMap;
    }

private:
    void convertComboSkillMap(unsigned char count, const comoboSkillData* data,
                              ENUM_SKILL_TREE_KIND kind);
    void convertComboSkillArray(unsigned char& count, comoboSkillData* data,
                                ENUM_SKILL_TREE_KIND kind) const;
    void getComboSkillSendForm(PacketGuard& packet,
                               ENUM_SKILL_TREE_KIND kind) const;

    ComboSkillMap m_comboSkillMap;     // +0x00
    ComboSkillMap m_comboSkillSfpMap;  // +0x15
};
#pragma pack(pop)

// ---- SP 返还信息 ----
struct stSPReturnInfo_t
{
    stSPReturnInfo_t(unsigned short skillIdx, unsigned short levelDiff,
                     unsigned int returnSP, unsigned int returnSFP);
    unsigned short m_skillIdx;   // +0x00
    unsigned short m_levelDiff;  // +0x02
    unsigned int m_returnSP;     // +0x04
    unsigned int m_returnSFP;    // +0x08
};

namespace WongWork
{
// ORIG 0x092 字节：4 组 kind 状态（各 0x24 对齐 8），+0x90 bool，+0x91 char。
class CSkillChanger
{
public:
    CSkillChanger();
    ~CSkillChanger();
    void SkillInitialize(CUser* user, int kind, bool flag);
    void _ResetSkillPoint(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _ResetSFPoint(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _GetUserItemCount(const CUser* user);  // [b23S3C] ORIG 0x0860a34a leave;ret 无返回值
    void _ApplyUserData(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _CountSkillBook(Inven_Item& item);
    // ORIG 0x0860a1e6：计算角色总 SP（基础 + 任务点）。
    int GetCurCharacTotalSP(const CUser* user, bool flag);
    int _GetBaseSkillPoint(const CUser* user) const;
    int _GetQuestSkillPoint(const CUser* user);
    // ORIG T 0x08609d10：mangled 与成员形态一致，但调用/被调双方均为单参 cdecl
    // （increase_status 0x08666162 等实证；函数体不消费隐式 this）→ 实为静态。
    static bool CheckCondition(const CUser* user);
    void CountElixer(CUser* user);                                     // ORIG T 0x08609d9c _ZN8WongWork13CSkillChanger11CountElixerEP5CUser
    int* GetElixerCount();                                             // ORIG W 0x0869780e _ZN8WongWork13CSkillChanger14GetElixerCountEv（返回 this，即第一组计数数组）
    int* GetNeedElixerCount();                                         // ORIG W 0x08697816 _ZN8WongWork13CSkillChanger18GetNeedElixerCountEv（返回 this+0x48）
    int* GetNeedElixerAbility();                                       // ORIG W 0x08697822 _ZN8WongWork13CSkillChanger20GetNeedElixerAbilityEv（返回 this+0x6c）
    static bool _fnEnumQuestClear(unsigned int questIdx, char* args);  // ORIG T 0x0860a6da _ZN8WongWork13CSkillChanger17_fnEnumQuestClearEjPc（enumQuestClear 回调，静态）
    void _CountQuestSPBook(const CUser* user, int& a, int& b, int& c, int& d);  // ORIG T 0x0860a680 _ZN8WongWork13CSkillChanger17_CountQuestSPBookEPK5CUserRiS4_S4_S4_
    void _addSPRelatedItem(int* a, int* b, int itemIdx, int count);    // ORIG T 0x0860a868 _ZN8WongWork13CSkillChanger17_addSPRelatedItemEPiS1_ii
    void _addSFPRelatedItem(int* a, int* b, int itemIdx, int count);   // ORIG T 0x0860a918 _ZN8WongWork13CSkillChanger18_addSFPRelatedItemEPiS1_ii
    // GetCurCharacTotalSP 0x0860a1e6/SkillInitialize 0x08609e90 以 4 段独立
    // memset(&sec,0,0x24) 清零 kind 状态区 ⇒ 每节为独立 0x24 子对象。
    struct STKindSection
    {
        int m_kindState;
        char m_pad04[0x20];
    };
    union
    {
        char m_pad[0x92];
        // ORIG 实证视图（WongWork__CSkillChanger 报告）：
        //   _CountSkillBook 0x0860a2ec 写 (this+i*4) 与 (this+(i+8)*4+4)=0x24+i*4；
        //   _addSPRelatedItem 0x0860a868 累加 (this+(i+0x10)*4+8)=0x48+i*4 并写
        //   g_nElixerAbility 至 (this+(i+0x18)*4+0xc)=0x6c+i*4；
        //   GetElixerCount=this / GetNeedElixerCount=+0x48 / GetNeedElixerAbility=+0x6c。
        struct
        {
            int m_elixerCount[9];       // +0x00
            int m_skillBookCount[9];    // +0x24
            int m_needElixerCount[9];   // +0x48
            int m_needElixerAbility[9]; // +0x6c
        } m_data;
        struct
        {
            STKindSection m_sec0;                  // +0x00
            STKindSection m_sec1;                  // +0x24
            STKindSection m_sec2;                  // +0x48
            STKindSection m_sec3;                  // +0x6c
            bool m_initialized;    // +0x90（ORIG SkillInitialize 0x08609e90 保存序 截断→载 this→回读→store，P33）
            unsigned char m_reserved91;
        } m_state;
    };
    // ORIG D 0x093acdc0 / 0x093ace00：CSkillChanger 静态成员表，[0]=计数（8）。
    // _CountSkillBook/_addSPRelatedItem 以 g_nStatUpItemIdx[0] 为循环上界。
    static int g_nStatUpItemIdx[9];
    static int g_nElixerAbility[9];
};
}

#include "CCharacter.h"  // 权威 CCharacter（get_give_skill 定义，独立 TU CCharacter.cpp）

extern void addSkillOnCreateCharacter(_Mastered_skill* skill, int job);
extern int getSkillChecksum(int slot, int skillIdx, int level);
extern int finishSkillChecksum(int* sum, int key);
extern int getSkillSpendSPOnSteal(int sp, int penalty);
extern char* NumberToString(unsigned int value, int radix);

// ===================== SkillSlot =====================

class SkillSlot
{
public:
    SkillSlot();
    ~SkillSlot();

    void set_parent(CUser* parent);
    void set_remain_sp(int sp1, int sp2, int sfp1, int sfp2);
    void get_remain_sp(int* sp1, int* sp2, int* sfp1, int* sfp2) const;

    // ---- 查询 ----
    int FindSkill(int skillIdx) const;
    int FindSkill(int skillIdx, ENUM_SKILL_TREE_KIND kind) const;
    int GetSkillTreeIndex() const;
    int get_skill_count() const;
    unsigned int get_skill_index(int slot) const;
    int get_skill_level(int slot) const;
    int* get_cur_remain_sp(ENUM_SKILL_TREE_KIND kind) const;
    _Mastered_skill* get_skillslot_buf(ENUM_SKILL_TREE_KIND kind) const;
    int get_skillslot_group(int slot) const;
    int get_skillslot_no(int skillIdx, int group, ENUM_SKILL_TREE_KIND kind,
                         bool checkCombo) const;
    int get_skillslot_no(_Mastered_skill* buf, int skillIdx, int group,
                         bool checkCombo) const;
    int getSkillSlotIndex(ENUM_SKILL_TREE_KIND kind, int slot) const;
    int get_remain_sp_at_index(ENUM_SKILL_TREE_KIND kind) const;
    int get_remain_sfp_at_index(ENUM_SKILL_TREE_KIND kind) const;
    int* get_temp_remain_sp();
    int* get_temp_remain_sfp();
    _Mastered_skill* get_temp_skill_slot();
    char IsEmptySkillTree(ENUM_SKILL_TREE_KIND kind) const;
    char IsLoadStrikerSkill();
    void SetLoadStrikerSkill(bool flag);
    void get_skill_slot(char* out1, char* out2, int size) const;
    void get_skill_slot_at_index(char* out, int size,
                                 ENUM_SKILL_TREE_KIND kind) const;
    int makeCheckSum(int key) const;
    int check_skill_mastered(int skillIdx, int level, int group) const;
    int check_skill_mastered(int skillIdx, int level, int group,
                             ENUM_SKILL_TREE_KIND kind) const;
    int check_skill_mastered(_Mastered_skill* buf, int skillIdx, int level,
                             int group) const;
    bool check_skill_list(int job, std::vector<std::pair<int, int> > list,   // [b28c-21 SIGFIX]
                         int level) const;
    bool checkComboSkillInsertQuickSlot(int skillIdx) const;
    bool checkMoveComboSkillSlot(unsigned char slot1, unsigned char slot2);
    void calcReturnSpSfpPerOneSkill(int& sp, int& sfp, int level,
                                    const _Mastered_skill* buf, int slot,
                                    const CSkill* skill, bool isSteal) const;
    void calcUsedSP(const _Mastered_skill* buf, int& sp, int& sfp) const;

    // ---- 修改 ----
    void set_skill_slot(char* in1, char* in2, int size);
    void set_skill_slot_at_index(char* in, int size,
                                 ENUM_SKILL_TREE_KIND kind);
    void set_remain_sp_at_index(int value, ENUM_SKILL_TREE_KIND kind);
    void set_remain_sfp_at_index(int value, ENUM_SKILL_TREE_KIND kind);
    int insert_skill(int skillIdx, int group, bool isActive,
                     ENUM_SKILL_TREE_KIND kind, int level);
    int insert_skill(_Mastered_skill* buf, int skillIdx, int group,
                     bool isActive, int level);
    int insert_skill_with_slot_index(int slot, int skillIdx, bool isActive,
                                     int level);
    int delete_skill(int skillIdx, int job, ENUM_SKILL_TREE_KIND kind);
    int skill_move(unsigned char slot1, unsigned char slot2, int job);
    int growtype_skill(int job, int skillIdx, int level,
                       ENUM_SKILL_TREE_KIND kind);
    int clear_all_skills(ENUM_SKILL_TREE_KIND kind);
    int clear_all_skills_both();
    bool clear_sfp_skills(char job, ENUM_SKILL_TREE_KIND kind);   // [b28c-21 SIGFIX]
    bool backupSkill(ENUM_SKILL_TREE_KIND kind);  // ORIG 返回 _Bool（DisPatcher_BuySkill::process 调用点 xor $1 反证；int/bool 函数体代码生成相同）
    bool InitSkill(bool sendInfo, ENUM_SKILL_TREE_KIND kind);   // [b28c-21 SIGFIX]
    void InitSkillHistoryLog(ENUM_SKILL_TREE_KIND kind, int reason);
    void SendSkillInfo(int characIdx, ENUM_SKILL_TREE_KIND kind);
    int master_new_skill(int skillIdx, int level, int job, int firstGrow,
                         int secondGrow, int addLevel);
    int try_master_new_skills(int skillIdx, int level, int job, int firstGrow,
                              int secondGrow, int addLevel);
    int master_new_guild_skill(int skillIdx, int job, int type,
                               int secondGrow,
                               const STGuildDBInfoOnly& guildInfo,
                               int availableGSP, int& needGold, int& learnCnt,
                               short& needGSP, int guildLevel);
    int refund_skill(int skillIdx, int level, int job, int firstGrow,
                     int secondGrow, int refundCount, int& result);
    int try_refund_skill(int skillIdx, int level, int job, int firstGrow,
                         int secondGrow, int refundCount, int& result);
    int get_give_skill(int job, int firstGrow, int secondGrow,
                       std::vector<std::pair<int, int> >& out);
    char checkOverLevelSkill(std::vector<stSPReturnInfo_t>& out,
                             ENUM_SKILL_TREE_KIND kind, int level);
    void initialize_sfp_skill();
    void debugCheckGrowTypeSkill(int level, int job, int firstGrow,
                                 int secondGrow);
    void debugCommandResetSkill();
    int checkskillSlotEmptyCount(int& out);

    // ---- Combo 技能 ----
    void loadComboSkill(SIG_COMBO_SKILL* data);
    void getComboSkillSaveData(SIG_COMBO_SKILL* data) const;
    void getComboSkillSendInfo(PacketGuard& packet) const;
    void setComboSkillInfo(ENUM_SKILL_TREE_KIND kind,
                           std::map<short, std::vector<int> >& map);
    void resetComboSkill(ENUM_SKILL_TREE_KIND kind);
    void verifyComboSkill();
    void verifyComboSkillTree(ENUM_SKILL_TREE_KIND kind);
    void eraseDuplicationComboSKill();
    void eraseDuplicationComboSKillTree(ENUM_SKILL_TREE_KIND kind);
    bool moveComboSkillExtensionQuickSlot(int skillIdx, int level,
                                         ENUM_SKILL_TREE_KIND kind);
    void pullComboSkillExtensionQuickSlot(ENUM_SKILL_TREE_KIND kind);

    // ---- 布局 ----
    CUser* m_parent;                       // +0x00
    int m_remainSP1;                       // +0x04
    bool m_isLoadStrikerSkill;             // +0x08（ORIG SetLoadStrikerSkill 0x086086b6 保存序：截断→载 this→回读→store，P33）
    int m_remainSP2;                       // +0x0c
    int m_field10;                         // +0x10
    int m_remainSFP1;                      // +0x14
    int m_remainSFP2;                      // +0x18
    ComboSkill m_comboSkill;               // +0x1c
    _Mastered_skill m_skillSlot[204];      // +0x46
    _Mastered_skill m_skillSlot2[204];     // +0x1de
    _Mastered_skill m_tempSkillSlot[204];  // +0x376
    int m_tempRemainSP;                    // +0x510
    int m_tempRemainSFP;                   // +0x514
};

#endif  // DNF_GAME_SKILLSLOT_H_
