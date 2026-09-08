#ifndef DNF_GAME_ADVANCE_ALTAR_PROC_STAGE_H_
#define DNF_GAME_ADVANCE_ALTAR_PROC_STAGE_H_

// ============================================================================
// advancealtar_ProcStage.h —— advancealtar::{StageEndType, StageType,
// CharacData, ProcStage, Timer_StageControl, HistoryLog} 权威声明。
//
// 布局证据（df_game_r, elf32-i386）：
//   - ProcStage ctor 0x0812dc18（advancealtar__ProcStage/ProcStage.md 全文）
//   - CharacData ctor 0x0812d024（advancealtar__CharacData/CharacData.md）
#include <cstring>
#include <vector>
//   - StageControl 子对象槽位：getProcStage 0x081348ae（this+0x04）、
//     getCharacData 0x081348a0（this+0x98）——ORIG W thunk objdump 实证。
//   - 访问器 thunk（ORIG W，objdump 实证）：CharacData::setUser 0x08134806
//     写 +0x04、getUser 0x08134814 读 +0x04；ProcStage::getState 0x08134838 /
//     setState 0x08134844（+0x04）、getTimeLine 0x08134852（+0x90）、getMap
//     0x08134860（+0x74）、getStageDifficulty 0x0813486c（+0x68）、
//     getCurrentSurvivalRoundNo 0x08134878（+0x7c+1）、getStartStageTime
//     0x08134886（+0x8c）、getMapBasisLevel 0x08134894（+0x6c）。
//
// 全部子对象已为真实成员（2026-08-25 StageControl 批次）：+0x18
// ProcSummonObjectMs（0x38）、+0x50 map<short,_MapMonsterSummary>、
// +0x80 vector<int>；CharacData +0x10 map<unsigned char,_SlotItem> 同批落地。
// ============================================================================

#include <cstring>

#include "NOTIPacketStruct.h"           // _NotiAdvanceAltarSpawnUnit/_NotiAdvanceAltarClearStage（ProcStage 发送用）
#include "advancealtar_StageDifficulty.h"  // StageDifficulty::T
#include "AdvanceAltarField.h"     // _RewardItem 完整定义（ShopType 幂等块已收敛）
#include "advancealtar_StageDifficulty.h"  // StageDifficulty::T

class CUser;
class CDungeon;
class CMap;
struct MSG_MONSTER_DIE;  // 完整定义 Secu_HackLogCheckByParty.h（killMonster 引用）

namespace advancealtar
{

class StageControl;

// ---- ORIG 枚举 ----
// StageType：权威定义 advancealtar_StageDifficulty.h（STAGE_NORMAL/STAGE_SURVIVAL，
// 自 CDungeon.h 收敛，本头经该叶子取得）。ProcStage::State / StageEndType 仅
// dieCharac/giveUpGame/setState 引用，此前无建模，落在本头。
// StageEndType 权威定义已迁 advancealtar.h（本头 include 该头；此处仅守卫幂等
// 副本，防直接包含本头时缺类型——与权威版同一 DNF_GAME_ADVANCE_ALTAR_STAGE_END_TYPE_DEFINED）。
#ifndef DNF_GAME_ADVANCE_ALTAR_STAGE_END_TYPE_DEFINED
#define DNF_GAME_ADVANCE_ALTAR_STAGE_END_TYPE_DEFINED
struct StageEndType
{
    enum T
    {
        END_N0 = 0,     // isEndStage 全清死亡标记 [名推断]
        END_N1 = 1,     // dieCharac / RegistTimerClearReward
        END_N2 = 2,     // giveUpGame
        END_N3 = 3      // isEndStage 全阶段完成 [名推断]
    };
};
#endif

// ---- ORIG _MapMonsterSummary（sendMapInfo.md/addCreatedMapMonster.md 栈布局实证，
// pack(1) 共 0xd 字节；ctor 0x0813442e memset(this,0,0xd)）----
#pragma pack(push, 1)
struct _MapMonsterSummary
{
    int m_orderIdx;            // +0x00（_mapMonster::m_orderIdx @+0x2c）
    short m_seq;               // +0x04（addCreatedMapMonster 写递增序号；map key 同值）
    short m_towerRefA;         // +0x06（getTowerInfo 第 1 引用出参截断）
    unsigned char m_towerRefB; // +0x08（getTowerInfo 第 2 引用出参截断）
    unsigned char m_roleType;  // +0x09（_mapMonster+0x18 低字节）
    unsigned char m_posX;      // +0x0a（_mapMonster+0x20 低字节；dieMob/isEndStage
                               //   ORIG 兼作已创建怪死亡标记读写同一偏移）
    unsigned char m_champion;  // +0x0b（_mapMonster+0x1c）
    unsigned char m_padC;      // +0x0c（memset 尾字节；发送前无显式写入）

    _MapMonsterSummary() { std::memset(this, 0, 0xd); }
};                             // sizeof 0xd
#pragma pack(pop)

typedef char _assert_map_monster_summary_size[(sizeof(_MapMonsterSummary) == 0xd) ? 1 : -1];

// ---- ORIG _SlotItem（ctor W 0x081347e2 objdump：memset(this,0,0xc)；写入点
// 见 CharacAdvanceAltarManager::getSlotItem 0x08132638 报告：+0x00 itemId
// （FieldData::m_value）、+0x04 needGage（CItem::getAdvanceAltarNeedGage））----
struct _SlotItem
{
    // ORIG W C1/C2 @0x081347e2：memset(this,0,0xc)——inline 定义随使用点发射弱符号。
    _SlotItem() { std::memset(this, 0, sizeof(*this)); }

    int m_itemId;                // +0x00
    int m_needGage;              // +0x04
    int m_field8;                // +0x08（ctor 清零；无显式写入点）
};                               // sizeof 0xc
typedef char _assert_slot_item_size[(sizeof(_SlotItem) == 0xc) ? 1 : -1];

// ---- ORIG _SummonObjectMs（makeCurrentPhaseSummonObject 0x812eba8 new 0x38 +
// ctor 0x08134674（_SummonObject ctor 后 memset 0x38）+ dieMob/summonObjectAndSendPacket/
// debugLog 字段访问实证；m_summon 区为 CDataManager.h 权威 _SummonObject 的字节槽
// ——本头被 CDataManager.h include，反向不可达）----
struct _SummonObjectMs
{
    _SummonObjectMs();               // ORIG W 0x08134674：memset(this,0,0x38)

    int m_ms;                        // +0x00（相对 m_field8 基线的 spawn 时刻）
    short m_seq;                     // +0x04（ProcStage::m_field14 递增序号；map key）
    short m_pad6;                    // +0x06
    int m_actionDefineId;            // +0x08（来源 ActionDefine::m_id）
    unsigned char m_deadFlag;        // +0x0c（dieMob 击杀置 1）
    unsigned char m_padD[3];         // +0x0d
    int m_sentFlag;                  // +0x10（summonObjectAndSendPacket 发包后置 1）
    unsigned char m_summon[0x24];    // +0x14 = _SummonObject（memcpy 整体拷贝；
                                     //   +0x00 m_id / +0x1c / +0x20 见权威头）
};                                   // sizeof 0x38

typedef char _assert_summon_object_ms_size[(sizeof(_SummonObjectMs) == 0x38) ? 1 : -1];

// ---- CharacData（ctor 0x0812d024：4 个标量 +0x10 处 map<unsigned char,_SlotItem>；
// 总尺寸 0x28 = StageControl 的 m_characData 槽）----
class CharacData
{
public:
    CharacData();                    // T 0x0812d024：4 标量清零 + map 构造
    ~CharacData();                   // T 0x0812d062：map 析构

    void setUser(CUser* user);       // W 0x08134806：m_user = user
    CUser* getUser() const;          // W 0x08134814：return m_user
    void setSlotItem();              // T 0x0812d246：user 非空时同步槽位物品
    void reset();                    // T 0x0812d078：4 标量清零 + 槽位 map clear
    void upGage();                   // T 0x0812d0b6：CDataManager 配置驱动 gage 推进
    void sendTestCheckGage();        // T 0x0812d372（ORIG 空函数）
    void upgreadGage();                        // T 0x0812d176：等级商城配表扣 gage 升级
    int summonSlotUnit(unsigned char slot, int needGage);  // T 0x0812d278
    int getGage() const;                       // W 0x08142088：return m_gage
    int getGageLevel() const;                  // W 0x08142094：return m_gageLevel

    // +0x00（selectStage 写 getCurrentRidableId()）
    int m_field0;
    CUser* m_user;                   // +0x04（setUser/getUser thunk 实证）
    int m_gageLevel;                 // +0x08（upGage 读取）
    int m_gage;                      // +0x0c（upGage 写入）
    std::map<unsigned char, _SlotItem> m_slotItems;  // +0x10（sizeof 0x18@32位）
};                                   // sizeof 0x28（= StageControl::m_characData 槽）

typedef char _assert_characdata_size[(sizeof(CharacData) == 0x28) ? 1 : -1];


// ---- ProcStage（ctor 0x0812dc18 逐字段布局；总尺寸 0x94，StageControl+0x04 槽）----
class ProcStage
{
public:
    struct State
    {
        enum T
        {
            S_N1 = 1,      // startStage 进入战斗
            S_N2 = 2,      // pause(true) 暂停（0x0812eaa6 实证值）
            S_N3 = 3,      // isRewardState
            S_N4 = 4,      // isRewardState
            S_N5 = 5,      // dieCharac（clearStageAndReward 后）
            S_N6 = 6,      // dieCharac/giveUpGame（EPLP 结束）
            S_N7 = 7       // ctor 初值 / 空闲
        };
    };

    // ---- 本批落地（定义 advancealtar_ProcStage.cpp）----
    void setState(State::T state);                       // W 0x08134844
    bool isRewardState() const;                          // T 0x0812f770
    int getStageType() const;                            // T 0x0812eb04
    void reset(StageType::T type);                       // T 0x0812dde0
    void resetAll();                                     // T 0x0812dd9a
    int startStage(int stageId, StageDifficulty::T diff);// T 0x0812de70
    void setNotSurvivalModeFirstRound();                 // T 0x0812f74c
    bool isSurvivalModeFirstRound() const;               // T 0x0812f70c
    int getStageIndex() const;                           // T 0x0812eb26
    int getStageDifficulty() const;                      // W 0x0813486c
    CMap* getMap() const;                                // W 0x08134860：return m_map
    TimeLine* getTimeLine() const;                       // W 0x08134852：return m_timeLine
    int getCurrentSurvivalRoundNo() const;               // W 0x08134878：return m_survivalRound+1
    int getMapBasisLevel() const;                        // W 0x08134894：return m_mapBasisLevel
    int getStartStageTime() const;                       // W 0x08134886
    void addCreatedMapMonster(_MapMonsterSummary& summary);  // T 0x0812e16e
    ProcStage(CharacData& characData);                   // T 0x0812dc18
    ~ProcStage();                                        // T 0x0812dd26
    bool isEndAllPhase() const;                          // T 0x0812eb5a
    void incCurrentMs();                                 // T 0x0812e146：m_field8 += gmTimeSpeed_*1000
    bool makeCurrentPhaseSummonObject();                 // T 0x0812eba8
    void procTimeLine();                                 // T 0x0812e0d6
    // ---- ORIG _RewardItem（AdvanceAltarField.h）/ map<int,_Reward> 奖励链 ----
    bool dieMob(unsigned short mobSeq, unsigned short param,
                const MSG_MONSTER_DIE& msg);                     // T 0x0812e1de
    bool isEndStage(StageEndType::T& endType) const;             // T 0x0812e2c6
    void clearStageAndReward();                                  // T 0x0812e3f6
    bool endProcAllKillMonster();                                // T 0x0812ea00
    int pause(bool pauseOn);                                     // T 0x0812eaa6
    int getClearSurvivalRoundNo() const;                         // W 0x0812eb4e：return m_survivalRound
    void rewardClearStage(std::vector<_RewardItem>& out);        // T 0x0812f0fa
    void addAchievementReward(std::map<int, _Reward>& out);      // T 0x0812f3ae
    // ORIG 无 this 静态调用（AltarMgr sendAchievementReward asm +0x225 实证）
    static void sendMailRewardItem(CUser* user,
                                   const std::vector<_RewardItem>& items,
                                   const char* senderKey,
                                   const char* titleKey);        // T 0x0812f47a
    void sendClearStage(const std::vector<_RewardItem>& rewards,
                        const std::vector<_BuyItem>& buyItems,
                        const std::map<int, _Reward>& rewardMap,
                        int starCount, bool clearFlag) const;    // T 0x0812e6de
    int getState() const;                                        // W 0x08134838：return m_state


    // ---- ProcStage::ProcSummonObjectMs（+0x18 子对象，sizeof 0x38；ctor
    // 0x0812d378 / dtor 0x0812d3f0，方法报告 advancealtar__ProcStage__ProcSummonObjectMs/）----
    struct ProcSummonObjectMs
{
    ProcSummonObjectMs(CharacData& characData, int const& currentMs);   // T 0x0812d378
    ~ProcSummonObjectMs();                                        // T 0x0812d3f0
    void reset();                                                 // T 0x0812d43c
    bool isEndSummon() const;                                     // T 0x0812d50a
    int getAliveMonsterCount() const;                             // W 0x08134820：m_aliveDelta-m_deadCount
    void insertSummonObject(_SummonObjectMs& so);                 // T 0x0812d540
    int getLastInsertDataIndex();                                 // T 0x0812d5ba
    void completePushSummonObject(int firstIndex);                // T 0x0812d5d0
    static bool compare(const _SummonObjectMs* lhs, const _SummonObjectMs* rhs);  // T 0x0812daec
    void summonObjectAndSendPacket();                             // T 0x0812d658
    bool dieMob(unsigned short mobSeq, unsigned short param,
               const MSG_MONSTER_DIE& msg);                       // T 0x0812db00
    void debugLog() const;                                        // T 0x0812d804
    int m_currentMs;                                   // +0x00（ctor/reset 清零）
    std::vector<_SummonObjectMs*> m_summons;           // +0x04
    std::map<short, _SummonObjectMs*> m_summonMap;     // +0x10（key=_SummonObjectMs::m_seq）
    int m_deadCount;                                   // +0x28（dieMob 计数）
    int m_aliveDelta;                                  // +0x2c（makeCurrentPhase 累计）
    CharacData* m_characData;                          // +0x30（ctor 存参）
    int* m_pCurrentMs;                                 // +0x34（= &ProcStage::m_field8）
    };                                                     // sizeof 0x38

    typedef char _assert_proc_summon_ms_size[(sizeof(ProcSummonObjectMs) == 0x38) ? 1 : -1];


    CharacData* m_characData;        // +0x00（ctor 存参）
    int m_state;                     // +0x04（State，ctor=7）
    int m_field8;                    // +0x08（reset 清零；ProcSummonObjectMs 引用）
    int m_fieldc;                    // +0x0c（reset 清零）
    int m_field10;                   // +0x10（reset 清零）
    short m_field14;                 // +0x14（ctor/reset 置 0；addCreatedMapMonster 序号源）
    ProcSummonObjectMs m_summonObjectMs;  // +0x18（sizeof 0x38，ctor 注入 m_characData/&m_field8）
    std::map<short, _MapMonsterSummary> m_createdMapMonsters;  // +0x50（sizeof 0x18@32位）
    int m_difficulty;                // +0x68（ctor=-1；startStage 写 diff）
    int m_mapBasisLevel;             // +0x6c（getMapBasisLevel）
    CDungeon* m_dungeon;             // +0x70（startStage find_dungeon）
    CMap* m_map;                     // +0x74（getMap/startStage find_map）
    unsigned char m_survivalFirst;   // +0x78（ctor/reset=1；首轮标记）
    int m_survivalRound;             // +0x7c（当前生存轮次下标）
    std::vector<int> m_roundMaps;    // +0x80（ctor 构造 vector<int>；startStage 由 CDungeon+0x814 拷贝）
    int m_startStageTime;            // +0x8c（getCurSec 起点；getStartStageTime）
    TimeLine* m_timeLine;            // +0x90（startStage 写 m_map+0xf4）
};                                       // sizeof 0x94

typedef char _assert_procstage_size[(sizeof(ProcStage) == 0x94) ? 1 : -1];

// ---- Timer_StageControl（静态语义函数，asm 不使用 this）----
struct Timer_StageControl
{
    static void RegistTimerCheckFinsh(int index);                             // T 0x0812f850
    static void RegistTimerEPLP(int index, StageEndType::T endType);          // T 0x0812f8ec
    static void RegistTimerEPLPReturnVillage(int index);                      // T 0x0812f956
    static void RegistTimerClearReward(int index, StageEndType::T endType);   // T 0x0812f89c
};

// ---- 全局自由函数 ----
void getMemberNames(CUser* user, char* buf);   // T 0x0812cf54

}  // namespace advancealtar

#endif  // DNF_GAME_ADVANCE_ALTAR_PROC_STAGE_H_
