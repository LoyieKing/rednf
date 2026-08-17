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

class CSkill;
class PacketGuard;
class GameWorld;
class CDataManager;

extern CDataManager* G_CDataManager();
extern GameWorld* G_GameWorld();

// ---- 技能树枚举（mangling 20ENUM_SKILL_TREE_KIND）----
enum ENUM_SKILL_TREE_KIND
{
    SKILL_TREE_NONE = -1,
    SKILL_TREE_1 = 0,
    SKILL_TREE_2 = 1,
    SKILL_TREE_SFP_1 = 2,
    SKILL_TREE_SFP_2 = 3
};

// ---- 已学技能（2 字节：索引 + 等级）----
struct _Mastered_skill
{
    _Mastered_skill();
    unsigned char m_index;   // +0x00
    char m_level;            // +0x01
};

// ---- 组合技数据（0xe 字节：技能索引 + 6 个连招技能）----
#pragma pack(push, 1)
struct comoboSkillData
{
    short m_comboSkillIndex;     // +0x00
    short m_comboSkillList[6];   // +0x02
};
#pragma pack(pop)

// ---- ComboSkill 存档结构（pack(1)；+0xa 起的奇数偏移由 ORIG 确认）----
#pragma pack(push, 1)
struct SIG_COMBO_SKILL
{
    int m_characNo;                  // +0x00
    unsigned char m_tree1Count;      // +0x04
    unsigned char m_tree2Count;      // +0x05
    int m_job;                       // +0x06
    comoboSkillData m_tree1Data[6];  // +0x0a
    comoboSkillData m_tree2Data[6];  // +0x5e
};
#pragma pack(pop)

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

    char saveData(SIG_COMBO_SKILL* data) const;
    char loadData(SIG_COMBO_SKILL* data);
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
class CSkillChanger
{
public:
    CSkillChanger();
    ~CSkillChanger();
    void SkillInitialize(CUser* user, int kind, bool flag);
};
}

class CCharacter
{
public:
    char get_give_skill(int firstGrow, int secondGrow,
                        std::vector<std::pair<int, int> >& out,
                        unsigned int flag) const;
};

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
    int check_skill_list(int job, std::vector<std::pair<int, int> > list,
                         int level) const;
    int checkComboSkillInsertQuickSlot(int skillIdx) const;
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
    int clear_sfp_skills(char job, ENUM_SKILL_TREE_KIND kind);
    int backupSkill(ENUM_SKILL_TREE_KIND kind);
    int InitSkill(bool sendInfo, ENUM_SKILL_TREE_KIND kind);
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
    int checkskillSlotEmptyCount(int& out) const;

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
    int moveComboSkillExtensionQuickSlot(int skillIdx, int level,
                                         ENUM_SKILL_TREE_KIND kind);
    void pullComboSkillExtensionQuickSlot(ENUM_SKILL_TREE_KIND kind);

    // ---- 布局 ----
    CUser* m_parent;                       // +0x00
    int m_remainSP1;                       // +0x04
    char m_isLoadStrikerSkill;             // +0x08
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
