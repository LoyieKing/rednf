#ifndef DNF_GAME_SIG_TYPES_B_H_
#define DNF_GAME_SIG_TYPES_B_H_

// ============================================================================
// df_game_r 还原 —— SIG_* 包结构分片 B（COMBO/CREATE/CREATURE/D..I 域）
// 从 SigTypes.h 手动拆分（纯移动，类型定义零改动）；umbrella 见 SigTypes.h。
// ============================================================================

#include <cstddef>  // offsetof static_assert
#include <string.h>  // memset（comoboSkillData/SIG_COMBO_SKILL ctor）

#include "Inven_Item.h"  // SIG_CREATE_CHARAC 三赠礼槽 Inven_Item

// ---- 组合技数据（0xe 字节：技能索引 + 6 个连招技能）----
#pragma pack(push, 1)
struct comoboSkillData
{
    short m_comboSkillIndex;     // +0x00
    short m_comboSkillList[6];   // +0x02

    // ORIG W 0x0848fb90：movw $0,(%eax) + memset(+2, 0xc)。
    comoboSkillData()
    {
        m_comboSkillIndex = 0;
        memset(m_comboSkillList, 0, sizeof(m_comboSkillList));
    }
};
#pragma pack(pop)

// ---- ComboSkill 存档结构 SIG_COMBO_SKILL（pack(1)；+0xa 起奇数偏移由 ORIG 确认）----
// ORIG mangled 名 _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL（0x864dcf2）。
// sizeof = 0xb2；SIG_LOAD_ETC 中 combo 字段占 [0x14aaa, 0x14b60) = 0xb6（尾 4 字节未用）。
// 原定义在 SkillSlot.h（FixStubS2 曾以 pad 镜像占位并因 ODR 冲突移除）；现迁移为本头权威，
// SkillSlot.h 改 #include "SigTypes.h" 引用（FixMissingSigTypes，广播周知）。
#pragma pack(push, 1)
struct SIG_COMBO_SKILL
{
    int m_characNo;                  // +0x00
    unsigned char m_tree1Count;      // +0x04
    unsigned char m_tree2Count;      // +0x05
    int m_job;                       // +0x06
    comoboSkillData m_tree1Data[6];  // +0x0a
    comoboSkillData m_tree2Data[6];  // +0x5e

    // ORIG W 0x0848fbbe：标量 4 清零（+0/+4/+5 movb、+6 = 0xb）→ 两树数组隐式
    // 构造循环（初值 5，P151 口径 6 次）→ memset 两树各 0x54。
    SIG_COMBO_SKILL() : m_characNo(0), m_tree1Count(0), m_tree2Count(0), m_job(0xb)
    {
        memset(m_tree1Data, 0, sizeof(m_tree1Data));
        memset(m_tree2Data, 0, sizeof(m_tree2Data));
    }

    void reset();                    // ORIG W 0x08694d04：characNo=0、两 count=0、
                                     //   m_job=0xb、memset 两树数据各 0x54（W8_CU_E.cpp 定义）
};
#pragma pack(pop)
static_assert(sizeof(SIG_COMBO_SKILL) == 0xb2, "SIG_COMBO_SKILL ABI drift");

struct SIG_CREATE_ACCOUNT_CARGO_RESULT
{
    char m_pad[0x8];
};

#pragma pack(push, 1)
// 真实布局（DisPatcher_CreateCharac::process 0x081c041e 反汇编实证，memset 0x5354）：
struct SIG_CREATE_CHARAC
{
    unsigned int m_accId;              // +0x000
    char m_name[0x1e];                 // +0x004（ORIG strncpy(dst+4, msg+0x12, 0x1e)）
    unsigned char m_slot;              // +0x022（← MSG_CREATE_CHARACTER.m_slot）
    unsigned char m_field23;           // +0x023（← msg+0x2f）
    unsigned char m_eventCharacFlag;   // +0x024（CUserCharacInfo::getUserEventCharacterFlag）
    unsigned char m_pad25;
    unsigned short m_field26;          // +0x026（角色创建表 job*0x7dc +0 字）
    unsigned char m_jobData[80];       // +0x028（同表 +2..0x51 共 80 字节）
    unsigned char m_field78;           // +0x078（同表 +0x7d4 字节）
    unsigned char m_masteredSkillA[0x198]; // +0x079（addSkillOnCreateCharacter 树 A，
                                           //       元素=_Mastered_skill(2B)×204，定义见 SkillSlot.h）
    unsigned char m_masteredSkillB[0x198]; // +0x211（树 B）
    unsigned char m_pad3a9[0x4e01 - 0x3a9]; // +0x3a9（未建模区）
    Inven_Item m_giftItem;             // +0x4e01（makeGift @0x81c0a1a：职业赠礼道具，
                                       //       id=case(job) 映射，find_item 后 make_item 填充）
    unsigned char m_pad4e3e[0x4e7b - 0x4e3e]; // +0x4e3e
    Inven_Item m_weaponItem;           // +0x4e7b（CREATECHARACTEREQUIP[job*2] 初始武器）
    unsigned char m_pad4eb8[0x4ef5 - 0x4eb8]; // +0x4eb8
    Inven_Item m_clothesItem;          // +0x4ef5（CREATECHARACTEREQUIP[job*2+1] 初始衣服）
    unsigned char m_pad4f32[0x533f - 0x4f32]; // +0x4f32
    unsigned char m_giftFindOk;        // +0x533f（makeGift：赠礼 find_item 命中 1/失败 0）
    unsigned char m_equipFindOk;       // +0x5340（makeGift：武器/衣服 find_item 均命中 1/否则 0）
    unsigned char m_pad5341[3];        // +0x5341..+0x5343
    unsigned int m_resultCode;         // +0x5344（DB_CreateCharac::dispatch 各步骤错误码，
                                       //       DB_CreateCharac.md 反汇编实证）
    unsigned int m_characNo;           // +0x5348（GetCharacNo ← WongWork::DBCommon::GetIdentity）
    unsigned char m_ssnCheckFail;      // +0x534c（SsnString 长度/第7位检查→1/0）
    unsigned char m_pad534d[3];        // +0x534d（ORIG +0x534c 字节与 +0x5350 字之间间隙）
    unsigned int m_spAtLevelUp;        // +0x5350（CDataManager::GetSpAtLevelUp(1)）
};
#pragma pack(pop)
static_assert(sizeof(SIG_CREATE_CHARAC) == 0x5354, "SIG_CREATE_CHARAC ABI drift");

static_assert(offsetof(SIG_CREATE_CHARAC, m_giftItem) == 0x4e01, "SIG_CREATE_CHARAC m_giftItem offset drift");
static_assert(offsetof(SIG_CREATE_CHARAC, m_weaponItem) == 0x4e7b, "SIG_CREATE_CHARAC m_weaponItem offset drift");
static_assert(offsetof(SIG_CREATE_CHARAC, m_clothesItem) == 0x4ef5, "SIG_CREATE_CHARAC m_clothesItem offset drift");
static_assert(offsetof(SIG_CREATE_CHARAC, m_giftFindOk) == 0x533f, "SIG_CREATE_CHARAC m_giftFindOk offset drift");
static_assert(offsetof(SIG_CREATE_CHARAC, m_equipFindOk) == 0x5340, "SIG_CREATE_CHARAC m_equipFindOk offset drift");
static_assert(offsetof(SIG_CREATE_CHARAC, m_ssnCheckFail) == 0x534c, "SIG_CREATE_CHARAC m_ssnCheckFail offset drift");
static_assert(sizeof(Inven_Item) == 0x3d, "Inven_Item size drift vs SIG_CREATE_CHARAC gift slots");
// 真实布局（CDBMsgSender::{RecvMail,SendMail,UseFeedCreature,InsertCreatureItem}
// 反汇编实证：memset 0x74；slot 写 +0x2c、uid/addInfo2 +0x30、饱食度 +0x44、
// characNo +0x48、flag50 +0x50、reason +0x51、field53 +0x53、days +0x54、
// expireTick +0x58、ceraOrderNo +0x60、field64 +0x64、mapSize +0x68。
// 物品副本区实际为 +0x04..+0x2b 共 0x28 字节（旧声明 [0x26]+对齐使 +0x51 起
// 全部漂移 -8，本批按 ORIG 偏移修正并补齐具名字段）。
struct SIG_CREATURE_ITEM
{
    int m_hdr;                  // +0x00（RecvMail |0x130、SendMail |0x100、
                                //        UseFeedCreature |0x2）
    unsigned char m_item[0x28]; // +0x04（Inven_Item_Compatible 兼容物品副本区）
    int m_slot;                 // +0x2c
    int m_addInfo2;             // +0x30（RecvMail/SendMail ← item+0x07；
                                //        UseFeedCreature 承载宠物 uid）
    // 具名化（W8DB：ORIG 0x833fc1a RenameCreatureItem strncpy(+0x34,0xc)、
    // DB_UpdateCreatureItem::GetField escape_string(+0x34)、dispatch
    // get_str(2,+0x34,0xd)、insertCreatureItem SQL '%s'(+0x34)——+0x34 区
    // 为物品名字段；原 m_pad34）：
    char m_name[0x10];          // +0x34（名字区，ORIG 写 ≤0xd）
    int m_stomachValue;         // +0x44（UseFeedCreature=喂食后饱食度；
                                //        InsertCreatureItem 固定写 100）
    int m_characNo;             // +0x48
    int m_exp;                  // +0x4c（ORIG insertCreatureItem/GetField
                                //   SQL exp=%d 实参 = *(int*)(sig+0x4c)；
                                //   dispatch get_int(4) 回填同槽；
                                //   CCreature.cpp:391 写侧，原 m_pad4c）
    unsigned char m_flag50;     // +0x50（InsertCreatureItem：get_sub_type()==0→1）
    unsigned char m_reason;     // +0x51
    unsigned char m_keepRight;  // +0x52（UseKeepRight W 0x083401ec：flag 低字节）
    unsigned char m_field53;    // +0x53
    int m_days;                 // +0x54（InsertCreatureItem ← 参数 days）
    int m_expireTick;           // +0x58（InsertCreatureItem ← 参数 expireTick）
    unsigned char m_pad5c[4];
    int m_ceraOrderNo;          // +0x60（InsertCreatureItem ← get_cera_item_order_no）
    int m_field64;              // +0x64（RecvMail=4/SendMail=3/InsertCreatureItem=6）
    int m_mapSize;              // +0x68
    int m_field6c;              // +0x6c（CDBMsgSender::ChangeOwner p6；DB 域报告待核对）
    int m_field70;              // +0x70（ChangeOwner p7；同上）→ 总 0x74（ORIG memset 覆盖范围）
};
static_assert(sizeof(SIG_CREATURE_ITEM) == 0x74, "SIG_CREATURE_ITEM ABI drift");
struct SIG_CREATURE_ITEM_LAYOUT_MIRROR
{
    char m_hdr[0x04];
    char m_item[0x28];
    char m_slot[0x04];
    char m_addInfo2[0x04];
    char m_pad34[0x10];
    char m_stomachValue[0x04];
    char m_characNo[0x04];
    char m_pad4c[0x04];
    char m_flag50;
    char m_reason;
    char m_keepRight;
    char m_field53;
    char m_days[0x04];
    char m_expireTick[0x04];
    char m_pad5c[0x04];
    char m_ceraOrderNo[0x04];
    char m_field64[0x04];
    char m_mapSize[0x04];
    char m_pad6c[0x08];
};
static_assert(sizeof(SIG_CREATURE_ITEM) == sizeof(SIG_CREATURE_ITEM_LAYOUT_MIRROR), "SIG_CREATURE_ITEM layout size");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_slot) == 0x2c, "SIG_CREATURE_ITEM m_slot offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_addInfo2) == 0x30, "SIG_CREATURE_ITEM m_addInfo2 offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_stomachValue) == 0x44, "SIG_CREATURE_ITEM m_stomachValue offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_characNo) == 0x48, "SIG_CREATURE_ITEM m_characNo offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_flag50) == 0x50, "SIG_CREATURE_ITEM m_flag50 offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_reason) == 0x51, "SIG_CREATURE_ITEM m_reason offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_field53) == 0x53, "SIG_CREATURE_ITEM m_field53 offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_days) == 0x54, "SIG_CREATURE_ITEM m_days offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_expireTick) == 0x58, "SIG_CREATURE_ITEM m_expireTick offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_ceraOrderNo) == 0x60, "SIG_CREATURE_ITEM m_ceraOrderNo offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_field64) == 0x64, "SIG_CREATURE_ITEM m_field64 offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_mapSize) == 0x68, "SIG_CREATURE_ITEM m_mapSize offset");

// 字段化（W8DB，ORIG 0x0841fd12 DB_UpdateCreatureItemList::dispatch 实证：
// 首 int=条目数；+0x21a4 起行数组（步长 0x38，见 W8LT_DBTask.cpp
// DB_CreatureItemListEntry），行区基址为运行期布局常量，非成员类型可
// 内嵌（行类型定义于消费 TU），故仅具名 count 头）：
struct SIG_CREATURE_ITEM_LIST
{
    int m_count;              // +0x0000（条目数）
    char m_blob[0x4074];      // +0x0004（头 + 行数组，行区 @+0x21a4）
};
static_assert(sizeof(SIG_CREATURE_ITEM_LIST) == 0x4078,
              "SIG_CREATURE_ITEM_LIST ABI drift");

struct SIG_DEATH_TOWER_GET_BEST_RECORD
{
    char m_pad[0x878];
};

struct SIG_DEATH_TOWER_LOAD_TOP_RANKER
{
    char m_pad[0x35b68];
};

// 字段视图（DB_UpdateDeathTowerBestRecord::makeRequest 0x0842a35e / dispatch 0x0842a11e
// 实证：+0x00 accId、+0x04 towerIndex、+0x06 memberCount、+0x08 起 members blob
//（stDeathTowerRecordMemberInfo_t stride 0x17）、+0x64 stage、+0x68 playTime）。
struct SIG_DEATH_TOWER_UPDATE_BEST_RECORD
{
    unsigned int m_accId;        // +0x00
    unsigned short m_towerIndex; // +0x04
    unsigned short m_memberCount;// +0x06
    char m_members[0x5c];        // +0x08..+0x63（0x17×4 上限）
    unsigned int m_stage;        // +0x64
    unsigned int m_playTime;     // +0x68
};
#pragma pack(push, 1)
// 真实布局（DisPatcher_DeleteCharac::process 0x081c1182 反汇编实证，memset 0x14）：
struct SIG_DELETE_CHARAC
{
    unsigned int m_accId;       // +0x00
    unsigned int m_characNo;    // +0x04（==0 → process 置 m_errNo=0x15 并回收流）
    int m_errNo;                // +0x08（DB_DeleteCharac::dispatch/DeleteGuildInfo 报告
                                //       0x08402c38/0x08402f4c movl 写 0/1/2/3 错误码）
    unsigned int m_guildId;     // +0x0c（CUser::GetGuildIdBeforeDeleteCharacter）
    unsigned char m_eventFlag;  // +0x10（dispatch 0x08402e3f/0x08402e6e movb 写 0/1）
    char m_pad11[3];            // +0x11
};
#pragma pack(pop)
static_assert(sizeof(SIG_DELETE_CHARAC) == 0x14, "SIG_DELETE_CHARAC ABI drift");

struct SIG_DOUBLE_CHECK_GUILD_ADDRESS
{
    char m_pad[0xd];
};

struct SIG_DOUBLE_CHECK_GUILD_NAME
{
    char m_pad[0x17];
};

// 部分具名化（证据：CDungeonDataManager::{UpdateCharacter,UpdateMonster,UpdateSkill} 报告
// 0x08485956/0x08485a5a/0x084859d8 —— UpdateMonster/Skill 键 DungeonKey=+0x04（汇编
// lea 0x4(%eax) 传 param_1+4）；CDungeonNode::UpdateSkill 读 *(u32*)(param_1+0x10) 作
// map<int,CDungeonJobNode> 键【推断：job】；UpdateCharacter Character::operator+= 于
// +0x24（Character 继承 PlayInfo，0x50 字节，定义 CUser.h——SigTypes_B 不可 include，
// 保留 blob）；UpdateMonster Monster::operator+= 于 +0x74（Monster 0x0c，
// CDungeonNode.h:18）。TimerDungeonDataStatistic::dispatch_sig memset 0x80 后 +0 写 3。
// 0x14..0x24 无字段级证据保留 pad。
#pragma pack(push, 1)
struct SIG_DUNGEON_DATA_STATISTIC
{
    int m_type;                 // +0x00（TimerDungeonDataStatistic 写 3；节点 map<int,..> 键）
    int m_dungeonIndex;         // +0x04（DungeonKey.m_dungeonIndex，CDungeonNode.h:12）
    int m_dungeonDiff;          // +0x08（DungeonKey.m_dungeonDiff，CDungeonNode.h:13）
    char m_pad0c[4];            // +0x0c（无字段级证据）
    unsigned int m_job;         // +0x10（UpdateSkill map<int,CDungeonJobNode> 键；亦 JobKey.m_job，
                                //   CDungeonSuccessNode::UpdateCharacter 以 param_1+0x10 作 JobKey*）
    unsigned char m_subJob;     // +0x14（JobKey.m_sub；CDungeonSuccessNode::UpdateCharacter JobKey 键）
    char m_pad15[3];            // +0x15..0x17
    int m_monsterKey;           // +0x18（CDungeonNode::UpdateMonster 0x08485628 find 键）【推断】
    int m_skillKey;             // +0x1c（CDungeonJobNode::UpdateSkill 0x08484bac find 键）【推断】
    unsigned char m_successType;// +0x20（CDungeonNode::UpdateCharacter 0x08485564：m_success[byte]）【推断】
    char m_pad21[3];            // +0x21..0x23
    char m_charac[0x50];        // +0x24（Character/PlayInfo 统计区；+0x20 处 int 作
                                //   Party/JobParty find 键（0x084851de/0x08484d4c lea 0x44），
                                //   +0x40 处 Skill 聚合（0x08484bac add $0x64））
    char m_monster[0xc];        // +0x74（Monster 统计区：m_damageTotal/m_damageCount/m_count，
                                //   定义 CDungeonNode.h:18；operator+= 聚合）
};
#pragma pack(pop)
static_assert(sizeof(SIG_DUNGEON_DATA_STATISTIC) == 0x80, "SIG_DUNGEON_DATA_STATISTIC ABI drift");


struct SIG_EVENT_LEVEUP_SUPPORT_INFO
{
    char m_pad[0x2c];
};

// SIG_EVENT_SERVER_MESSAGE 完整定义唯一化至 CSharedServerMessageManager.h
// （真实布局：byte m_count + stSERVER_MESSAGE m_msg[0x64]；原 pad 占位已删除）。
#include "CSharedServerMessageManager.h"

// ORIG ctor 0x081ba886：5×int=0（+0..+0x10）+ memset(+0x14,0,0x64) → sizeof 0x78。
#pragma pack(push, 1)
struct SIG_EVENT_STAGE_LOG
{
    int m_field0[5];        // +0x000..+0x10（ctor 逐 int 清零）
    char m_data[0x64];      // +0x014（ctor memset 清零）

    SIG_EVENT_STAGE_LOG()   // ORIG W 0x081ba886（C1/C2 同址）
    {
        m_field0[0] = 0;
        m_field0[1] = 0;
        m_field0[2] = 0;
        m_field0[3] = 0;
        m_field0[4] = 0;
        memset(m_data, 0, 0x64);
    }
};
#pragma pack(pop)
static_assert(sizeof(SIG_EVENT_STAGE_LOG) == 0x78, "SIG_EVENT_STAGE_LOG ABI drift");

// DB 载荷 wire 布局（ORIG _saveData 0x0832b0ac / loadData 0x0832b1fe 实证；
// 自然对齐 == packed 布局，总尺寸 0x30 不变）。字段与 CCharacExpertJob 成员
// 一一对应；仅 _saveData/loadData 按此结构读写。
struct SIG_EXPERT_JOB
{
    int            m_characNo;         // +0x00
    unsigned short m_giveupCount;      // +0x04
    unsigned short m_pad06;            // +0x06（wire 保留）
    int            m_endurance;        // +0x08
    int            m_enchantCount;     // +0x0c
    unsigned char  m_recipeFlag[0x1e]; // +0x10（240 位 recipe bitmap）
    char           m_pad2e[2];         // +0x2e（补齐 0x30）
};
static_assert(sizeof(SIG_EXPERT_JOB) == 0x30, "SIG_EXPERT_JOB size");

struct SIG_EXTEND_CHARAC_SLOT
{
    char m_pad[0x8];
};

// SIG_FAIR_PVP_SCORE（总 0x688）。+0x00 characNo、+0x2c job_score 压缩 blob
// 源（0x650）、+0x67c dailyPlayCount；尾部字段按 W8LT_DBTask.cpp
// DB_FairPvPScoreView 实证补具名。
// 2026-09-04 m_pad[0x680] 拆具名（WarField 批）：逐字段宽度/偏移由 ORIG
// fair_pvp::CFairPvPScore::getData 0x084a55a8 与 loadData 0x084a48f8 反汇编
// 实证（docs/class_func_reports/fair_pvp__CFairPvPScore/getData.md、
// loadData.md）：
//   +0x00 movl ← _saveData 0x084a57fe `*(int*)sig = get_charac_no(-1)`；
//   +0x04..+0x18 movl ×6（getData +0x1a4 循环 i*0xc：modeA/B 三统计）；
//   +0x1c/0x20/0x24/0x28 movl（field40/field44/winStreak/bestWinStreak）；
//   +0x2c movzbl+incb（jobScoreCount，getData 每 entry ++）；
//   +0x30.. 100 × entry（getData entry 基址 = (idx+2)*0x10+0x10 = idx*0x10+0x30；
//        movb job / movb sub / movl cnt0..cnt2@+4/+8/+0xc）；
//   +0x670..0x679 movb/movzbl（recentResults ≤10 条）；
//   +0x67c/0x680 movl（dailyPlayCount / lastPlayTime）；+0x684 movb（giveItem）。
// 命名与 fair_pvp::CFairPvPScore 成员一一对应（线格式镜像）。
struct SIG_FAIR_PVP_SCORE_ENTRY   // 职业计分条目（stride 0x10）
{
    unsigned char m_job;      // +0x00（movb 写 / movzbl 读）
    unsigned char m_sub;      // +0x01
    char m_pad02[2];          // +0x02
    int m_cnt0;               // +0x04
    int m_cnt1;               // +0x08
    int m_cnt2;               // +0x0c
};
static_assert(sizeof(SIG_FAIR_PVP_SCORE_ENTRY) == 0x10,
              "SIG_FAIR_PVP_SCORE_ENTRY size");

struct SIG_FAIR_PVP_SCORE
{
    int m_characNo;                            // +0x00
    int m_modeStats[2][3];                     // +0x04..+0x18（modeA0..2 / modeB0..2）
    int m_field1c;                             // +0x1c（↔ CFairPvPScore::m_field40）
    int m_field20;                             // +0x20（↔ m_field44）
    int m_winStreak;                           // +0x24
    int m_bestWinStreak;                       // +0x28
    unsigned char m_jobScoreCount;             // +0x2c
    char m_pad2d[3];                           // +0x2d..0x2f
    SIG_FAIR_PVP_SCORE_ENTRY m_jobScore[100];  // +0x30..+0x66f
    unsigned char m_recentResults[10];         // +0x670..0x679
    char m_pad67a[2];                          // +0x67a..0x67b
    int m_dailyPlayCount;                      // +0x67c
    unsigned int m_lastPlayTime;               // +0x680（from_unixtime 时间戳；CFairPvPScore 写 getCurSec）
    unsigned char m_giveItem;                  // +0x684
    char m_pad685[3];                          // +0x685..0x687
};
static_assert(sizeof(SIG_FAIR_PVP_SCORE) == 0x688, "SIG_FAIR_PVP_SCORE size");

// 字段化（W8DB，ORIG 0x08436ba0 DB_FindFactoryHub::dispatch 实证：+0 uint
// m_id（与 charac_info 查得 m_id 比对）、+4 角色名（escape_string/strncpy
// 0x1d）、+0x21 byte（转发 FATIGUE_CHECK+4）；尾 2 字节为对齐驻留）：
struct SIG_FIND_FACTORY_HUB
{
    unsigned int m_mId;         // +0x00
    char m_characName[0x1d];    // +0x04
    char m_field21;             // +0x21
    char m_pad22[2];            // +0x22..0x23
};
static_assert(sizeof(SIG_FIND_FACTORY_HUB) == 0x24,
              "SIG_FIND_FACTORY_HUB ABI drift");

// 字段化（W8DB，ORIG 0x08436ba0 出站组包实证：+0 uint mId、+4 byte
// （← SIG_FIND_FACTORY_HUB+0x21）、+5 strncpy(name,0x1d)；尾 2 对齐驻留）：
struct SIG_FIND_FACTORY_HUB_FATIGUE_CHECK
{
    unsigned int m_mId;         // +0x00
    char m_flag;                // +0x04
    char m_characName[0x1d];    // +0x05
    char m_pad22[2];            // +0x22..0x23
};
static_assert(sizeof(SIG_FIND_FACTORY_HUB_FATIGUE_CHECK) == 0x24,
              "SIG_FIND_FACTORY_HUB_FATIGUE_CHECK ABI drift");

// 字段化（W8DB，ORIG 0x08437c50 makeRequest 实证：strncpy(+0,name,0x1d)、
// +0x1e ushort flag）：
struct SIG_FIND_FACTORY_HUB_USER
{
    char m_characName[0x1e];    // +0x00（strncpy 0x1d + 尾 0）
    unsigned short m_flag;      // +0x1e
};
static_assert(sizeof(SIG_FIND_FACTORY_HUB_USER) == 0x20,
              "SIG_FIND_FACTORY_HUB_USER ABI drift");

struct SIG_FIND_FACTORY_HUB_USER_NO_CONN
{
    char m_pad[0xc];
};

struct SIG_FORBID_USER_TO_PLAY
{
    char m_pad[0x9];
};

struct SIG_GET_PREMIUM_SELECTED_CUBE_INFO
{
    unsigned int m_characNo;   // requestCubePremiumInfoIfContracted 0x086467ee
                               // 写 getCurCharacR() 首字段整型（characNo）
};

// 字段化（W8DB，ORIG 0x0844c4e2 dispatch 实证：+0 uint characNo、
// +4 ubyte（get_ubyte(0)←cube_premium.selected）、+5 ubyte
// （get_ubyte(1)←cube_type）；+6..7 驻留）：
struct SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES
{
    unsigned int m_characNo;      // +0x00
    unsigned char m_selected;     // +0x04
    unsigned char m_cubeType;     // +0x05
    char m_pad6[2];               // +0x06..0x07
};
static_assert(sizeof(SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES) == 0x8,
              "SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES ABI drift");

struct SIG_GET_USER_REGDATE
{
    // ORIG makeRequest(0x8442fae) 写 +0x00=acc_id；dispatch(0x8442e68) 写
    // +0x04=DATE_FORMAT 列/100、+0x08=reg_date 原始值（member_info 查询）。
    unsigned int m_id;        // +0x00（SQL where m_id=%d）
    unsigned int m_regDay;    // +0x04（yyyymmdd，列 0 /100；ORIG 0x8442f72
                              // 商原地 shr $0x5,%edx ⇒ 与无符号商同号）
    unsigned int m_regDate;   // +0x08（member_info.reg_date 原始 epoch）
    // ORIG GetInBuffer 实例化 sizeof=0xc：仅 3 个 dword，无尾部 pad
};
static_assert(sizeof(SIG_GET_USER_REGDATE) == 0xc,
              "SIG_GET_USER_REGDATE ABI drift");
static_assert(offsetof(SIG_GET_USER_REGDATE, m_id) == 0x00,
              "SIG_GET_USER_REGDATE m_id offset drift");
static_assert(offsetof(SIG_GET_USER_REGDATE, m_regDay) == 0x04,
              "SIG_GET_USER_REGDATE m_regDay offset drift");
static_assert(offsetof(SIG_GET_USER_REGDATE, m_regDate) == 0x08,
              "SIG_GET_USER_REGDATE m_regDate offset drift");

struct SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD
{
    char m_pad[0x4];
};

struct SIG_GOBLIN_UPDATE_REWARD
{
    char m_pad[0x18];
};

// 字段化（W8DB，ORIG makeRequest 0x0842f2ae：+0 uint accId、+4 uint flag；
// dispatch 0x0842f22a：member_mousepass.m_id=%s(+0)、+%d(+4)）：
struct SIG_GOBLIN_UPDATE_VALIDITYTIME
{
    unsigned int m_mId;      // +0x00（member_mousepass.m_id ← accId）
    unsigned int m_flag;     // +0x04（validity_time 秒差）
};
static_assert(sizeof(SIG_GOBLIN_UPDATE_VALIDITYTIME) == 0x8,
              "SIG_GOBLIN_UPDATE_VALIDITYTIME ABI drift");

struct SIG_GOLD_CONTROL_EXCEPTION_MID
{
    char m_pad[0x2];
};

// 字段化（SecuLink 批次 2026-09-05，裸偏移清理；union 叠加保留 m_pad 基名）。
// 布局证据：DB_GuildJoinList::dispatch ORIG 0x0843d87c（本类型唯一 producer；
// GetInBuffer 后 MySQL 逐列填充，偏移/宽度按 get_uint/get_str/get_int/get_byte
// 实参 disp 实证：entry stride 0x44：+0x00 charac_no、+0x04 charac_name(0x1e)、
// +0x22..0x25 四列 get_byte(1..4)（SQL 序 charac_name,lev,job,grow_type,sex）、
// +0x26 memo(0x1a)、+0x40 now-occ_time；count@+0x00，条目区 +0x04 起 0x32 项）。
// sizeof 0xd4c 不变；自然对齐即可（各 int 成员偏移均为 4 的倍数）。
struct SIG_GUILD_JOIN_LIST
{
    union
    {
        char m_pad[0xd4c];
        struct
        {
            int m_count;                             // +0x00
            struct
            {
                unsigned int m_characNo;             // e+0x00（get_uint col0）
                char m_name[0x1e];                   // e+0x04（get_str col0）
                char m_job;                          // e+0x22（get_byte col2）【推断：SQL 列序】
                char m_growType;                     // e+0x23（get_byte col3）【推断】
                char m_sex;                          // e+0x24（get_byte col4）【推断】
                char m_lev;                          // e+0x25（get_byte col1）【推断】
                char m_memo[0x1a];                   // e+0x26（get_str col1）
                int m_remainTime;                    // e+0x40（now - occ_time）
            } m_entry[0x32];                         // +0x04 起
        } m_list;
    };
};
static_assert(sizeof(SIG_GUILD_JOIN_LIST) == 0xd4c,
              "SIG_GUILD_JOIN_LIST ABI drift");
static_assert(offsetof(SIG_GUILD_JOIN_LIST, m_list.m_entry[1]) == 0x48,
              "SIG_GUILD_JOIN_LIST entry stride drift");
static_assert(offsetof(SIG_GUILD_JOIN_LIST, m_list.m_entry[0].m_remainTime) == 0x44,
              "SIG_GUILD_JOIN_LIST remainTime offset drift");

struct SIG_HACK_IP_SAVE
{
    char m_pad[0x28];
};

struct SIG_HADES_PUNISH
{
    char m_pad[0x24];
};

#pragma pack(push, 1)
struct SIG_INSERT_0712_WINTER_EVENT_COUPON
{
    unsigned int m_accId;           // +0x00
    unsigned int m_characNo;        // +0x04
    unsigned int m_count;           // +0x08
};
#pragma pack(pop)

struct SIG_INSERT_ACHIEVE_EVENT
{
    char m_pad[0xc];

    // ORIG 嵌套枚举（_ZN24SIG_INSERT_ACHIEVE_EVENT17ENUM_ACHIEVE_TYPEE）。
    // 调用点观察值：1=CUser::_onLevelUp level 0x12；2=_onLevelUp level 0x30；
    // _onChangeGrowType/_onQuestClear 亦传小整数。ORIG 无成员名符号信息，取中性名。
    enum ENUM_ACHIEVE_TYPE
    {
        ENUM_ACHIEVE_TYPE_NONE = 0,
        ENUM_ACHIEVE_TYPE_1 = 1,
        ENUM_ACHIEVE_TYPE_2 = 2
    };
};

struct SIG_INSERT_AUTO_PUNISH_FIRST_USER
{
    char m_pad[0x2c];
};

struct SIG_INSERT_AUTO_PUNISH_SECOND_USER
{
    char m_pad[0x14];
};

struct SIG_INSERT_RESPONSE_TIME
{
    char m_pad[0x7d8];
};

struct SIG_INSERT_SERVER_LOAD_STATISTIC
{
    char m_pad[0xa];
};

struct SIG_INSERT_UNLIMIT_SUPPORT_LOG
{
    char m_pad[0x2c];
};

// EventBattleLink 批次按 CInGameAdvertisementManager/WriteLog.md 汇编
// （0x080fda56 GetInBuffer 写入序列 +0x0..+0x1f）字段化，总大小保持 0x20。
struct SIG_IN_GAME_ADVERTISEMENT_LOG
{
    unsigned int m_accId;            // +0x00（get_acc_id）
    unsigned int m_characNo;         // +0x04（get_charac_no(-1)）
    unsigned char m_type;            // +0x08（param_2，写入字节）
    unsigned char m_pad09;           // +0x09
    short m_adType;                  // +0x0a（param_3）
    unsigned char m_flag;            // +0x0c（param_4，写入字节）
    unsigned char m_pad0d[3];        // +0x0d
    unsigned int m_extra;            // +0x10（param_5）
    int m_dungeonIdx;                // +0x14（state=5 时副本 idx，否则 -1）
    int m_village;                   // +0x18（恒 -1；state=3 分支 ORIG 未写入）
    int m_pad1c;                     // +0x1c（恒 -1）
};

struct SIG_IP_MONITOR_PUNISH
{
    char m_pad[0xc88];
};

// SIG_ITEM_DICTIONARY_FLAG —— 物品词典位图存档（ORIG int characNo + 5000 字节位图，共 0x138c）。
// CItemDictionary.cpp 曾以本地匿名命名空间 struct 定义（与权威冲突）；按 ORIG 迁移至本头。
#pragma pack(push, 1)
struct SIG_ITEM_DICTIONARY_FLAG
{
    int m_characNo;              // +0x00
    unsigned char m_data[5000];  // +0x04

    // ORIG W 0x0848fab2：movl 0→+0 + memset(+0x4,0,0x1388)。
    SIG_ITEM_DICTIONARY_FLAG()
    {
        m_characNo = 0;
        memset(m_data, 0, 5000);
    }
};
#pragma pack(pop)
static_assert(sizeof(SIG_ITEM_DICTIONARY_FLAG) == 0x138c, "SIG_ITEM_DICTIONARY_FLAG ABI drift");

struct SIG_ITEM_LOCK_UPDATE
{
    char m_pad[0x324];
};

// ORIG ctor 0x0848fa82：movl 0→+0 + memset(+0x4,0,0x16)。
#pragma pack(push, 1)
struct SIG_ITEM_MAKING_SKILL
{
    int m_field0;            // +0x00（ctor 清零）
    unsigned char m_data[0x16];  // +0x04（ctor memset 清零）
    char m_pad1a[2];         // +0x1a（尾部对齐）

    SIG_ITEM_MAKING_SKILL()  // ORIG W 0x0848fa82（C1/C2 同址）
    {
        m_field0 = 0;
        memset(m_data, 0, 0x16);
    }
};
#pragma pack(pop)
static_assert(sizeof(SIG_ITEM_MAKING_SKILL) == 0x1c, "SIG_ITEM_MAKING_SKILL ABI drift");

struct SIG_JOIN_GUILD_INFO
{
    char m_pad[0x64];
};

// 具名化（证据：docs/class_func_reports/DB_ChangeLetterStat/{makeRequest,dispatch}.md 与
// WongWork__CMailBoxHelper/ReqChangeLetterStat.md：makeRequest 写 +0 characNo(int) /
// +4 mailIdx(int)；dispatch 读 *(short*)(sig+8) 作 state 判 0）。
struct SIG_LETTER_STAT
{
    int m_characNo;     // +0x00
    int m_mailIdx;      // +0x04
    short m_state;      // +0x08（dispatch 读取判 0）
    char m_padA[2];     // +0x0a（对齐尾）
};
static_assert(sizeof(SIG_LETTER_STAT) == 0xc, "SIG_LETTER_STAT ABI drift");


// 字段化（W8DB，ORIG 0x0843199a DB_LoadBloodBestRecord::dispatch 实证：
// +0 uint characNo、+4 uint best_time(get_uint)、+8 int best_round、
// +0xc int dungeon_index；组包注释 14047-14052 同款）：
struct SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD
{
    unsigned int m_characNo;     // +0x00
    unsigned int m_bestTime;     // +0x04
    int m_bestRound;             // +0x08
    int m_dungeonIndex;          // +0x0c
};
static_assert(sizeof(SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD) == 0x10,
              "SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD ABI drift");

struct SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE
{
    char m_pad[0x80];
};

#endif  // DNF_GAME_SIG_TYPES_B_H_
