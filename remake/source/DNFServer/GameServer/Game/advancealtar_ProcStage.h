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
// 未建模子块（后续批次）：ProcStage+0x18 ProcSummonObjectMs、+0x50
// std::map<short,_MapMonsterSummary>、+0x80 std::vector<int> —— 以 pad 占位，
// 不影响已实现方法的偏移与符号。
// ============================================================================

#include <cstring>

#include "advancealtar.h"               // TimeLine / AdvanceAltarData / _Slot
#include "advancealtar_StageDifficulty.h"  // StageDifficulty::T

class CUser;
class CDungeon;
class CMap;

namespace advancealtar
{

class StageControl;

// ---- ORIG 枚举 ----
// StageType：权威定义 advancealtar_StageDifficulty.h（STAGE_NORMAL/STAGE_SURVIVAL，
// 自 CDungeon.h 收敛，本头经该叶子取得）。ProcStage::State / StageEndType 仅
// dieCharac/giveUpGame/setState 引用，此前无建模，落在本头。
struct StageEndType
{
    enum T
    {
        END_N1 = 1,     // dieCharac / RegistTimerClearReward
        END_N2 = 2      // giveUpGame
    };
};

// ---- CharacData（ctor 0x0812d024：4 个标量 +0x10 处 map<unsigned char,_SlotItem>）----
class CharacData
{
public:
    void setUser(CUser* user);       // W 0x08134806：m_user = user
    CUser* getUser() const;          // W 0x08134814：return m_user
    void setSlotItem();              // T 0x0812d246：user 非空时同步槽位物品

    // +0x00（selectStage 写 getCurrentRidableId()；upGage 经 StageControl 偏移访问）
    int m_field0;
    CUser* m_user;                   // +0x04（setUser/getUser thunk 实证）
    int m_gageLevel;                 // +0x08（upGage 读取）
    int m_gage;                      // +0x0c（upGage 写入）
    // +0x10.. std::map<unsigned char,_SlotItem>（_SlotItem 未建模，不占位；
    // 现有 TU 均经 StageControl 裸偏移访问该区，无 sizeof 依赖）
};

// ---- ProcStage（ctor 0x0812dc18 逐字段布局；总尺寸 0x94，StageControl+0x04 槽）----
class ProcStage
{
public:
    struct State
    {
        enum T
        {
            S_N1 = 1,      // startStage 进入战斗
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
    int getStartStageTime() const;                       // W 0x08134886

    CharacData* m_characData;        // +0x00（ctor 存参）
    int m_state;                     // +0x04（State，ctor=7）
    int m_field8;                    // +0x08（reset 清零；ProcSummonObjectMs 引用）
    int m_fieldc;                    // +0x0c（reset 清零）
    int m_field10;                   // +0x10（reset 清零）
    short m_field14;                 // +0x14（ctor/reset 置 0）
    char m_pad18[0x50 - 0x18];       // +0x18 ProcSummonObjectMs（未建模）
    char m_pad50[0x68 - 0x50];       // +0x50 map<short,_MapMonsterSummary> 等（未建模）
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
    static void RegistTimerEPLP(int index, StageEndType::T endType);          // T 0x0812f8ec
    static void RegistTimerClearReward(int index, StageEndType::T endType);   // T 0x0812f89c
};

// ---- HistoryLog（静态语义函数）----
struct HistoryLog
{
    static void dungeonClearInfo(CUser& user, StageControl& control, bool isLast);   // T 0x08133bd4
    static void enterDungeon(CUser& user, const ProcStage& procStage);               // T 0x081339b0
};

// ---- 全局自由函数 ----
void getMemberNames(CUser* user, char* buf);   // T 0x0812cf54

}  // namespace advancealtar

#endif  // DNF_GAME_ADVANCE_ALTAR_PROC_STAGE_H_
