// df_game_r Game/ advancealtar::StageControl 专属 TU（从 GameStubs.cpp 拆出）。
// 声明唯一归口 advancealtar.h（具名布局：m_index@+0x00 / ProcStage 槽@+0x04 /
// CharacData 槽@+0x98，sizeof 0xc0）；本 TU 全部函数依据
// docs/class_func_reports/advancealtar__StageControl/ 报告翻译。
#include "CGameManager.h"
#include "CInventory.h"  // PacketGuard 完整定义 / CInventory::SendItemLockListInven
#include "CMap.h"  // CMap::get_index/get_monster_list、_mapMonster（sendMapInfo）
#include "GameWorld.h"
#include <cstring>        // std::memset / std::memcpy
#include <new>            // placement new（StageControl 子对象构造）
#include "advancealtar_ProcStage.h"  // ProcStage/CharacData 完整类型（getProcStage/selectStage）

namespace advancealtar
{
// ----------------------------------------------------------------------------
// ORIG T 0x0812f9a2（StageControl.md 全文）：m_index=-1 →
//   ProcStage::ProcStage(this+4, characData@this+0x98)（T 0x812dc18）+
//   CharacData::CharacData(this+0x98)（T 0x812d024）。
// 子对象以字节槽持有（advancealtar.h 布局注释），此处 placement 构造。
// [b28d-solo-StageControl] C1：ORIG 34 / ours 62。ORIG 为真成员直构（`lea 0x98(%eax),
//   %edx` + `add $0x4,%eax` + call ProcStage::C1(CharacData&) + call CharacData::C1，
//   仅 esi/ebx + sub 0x10 帧）。我方字节槽 + placement new ⇒ N83：4.4-O0 恒发
//   `movl $0x94,(%esp); call _ZnwjPv`（size 装载行）+ test/je + 每子对象一段 EH
//   cleanup（~28 条）。修法唯一路径 = StageControl 成员改真实类型，但
//   advancealtar.h → advancealtar_ProcStage.h → NOTIPacketStruct.h → advancealtar.h
//   为双向 include 环（NOTIPacketStruct.h 需 advancealtar::_Stage），成员化须拆环
//   （波及全仓 include CGameManager.h/advancealtar.h 的 TU）。语义等价（m_index=-1 +
//   ProcStage(*characData)@+0x04 + CharacData@+0x98，槽宽恰 0x94/0x28）。N 封顶（N83/P67）。
// [b28d-sweep 2026-09-09 复核] 解环路径已定位（供 Main 决策，本泳道未落）：①把
//   advancealtar::_Stage（advancealtar.h:71-73 + 断言 491）抽到叶子头
//   advancealtar_Stage.h；②NOTIPacketStruct.h 改 include 该叶子头（去 advancealtar.h
//   依赖）；③advancealtar.h 便可 include advancealtar_ProcStage.h，StageControl 成员改
//   真实类型 ProcStage m_procStage / CharacData m_characData。波及面：include
//   advancealtar.h 的 17 个 .cpp（含经 CGameManager.h/CUser.h 的传递消费）须复编；
//   4.4 生成的 EH landing 布局是否与 ORIG 4.1 逐条一致亦需实测（N91 族）。风险与
//   波及超出单 TU 泳道 ⇒ 维持 N83/P67 封顶，路径交 Main 定夺。
StageControl::StageControl()
{
    m_index = -1;
    CharacData* characData = reinterpret_cast<CharacData*>(
        reinterpret_cast<char*>(this) + 0x98);
    new (reinterpret_cast<void*>(m_procStage)) ProcStage(*characData);
    new (characData) CharacData;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fa06（~StageControl.md 全文）：先 CharacData::~CharacData(this+0x98)
//   再 ProcStage::~ProcStage(this+0x04)（成员逆序析构）。两处子对象指针均直算
//   `mov this,%eax; add $0x98/0x4,%eax; mov %eax,(%esp)`，ORIG 无具名局部（P0：
//   指针全程寄存器/出参槽直传）→ 此处亦内联 reinterpret_cast，不落 -0xc 槽。
//
// [b28d-solo-StageControl] D1：ORIG 29 / ours 13。ORIG（真成员自动 D1）把
//   CharacData::~CharacData(+0x98) 建 try 区，抛时落点 `mov %edx,%ebx; mov %eax,%esi;
//   ProcStage~D1(+0x04); _Unwind_Resume` 后仍走正常路径 ProcStage~D1 ⇒ push esi/ebx +
//   sub 0x10 帧 + 整段 landing pad。我方字节槽 + 显式析构调用无 EH 清理（P67/N91：
//   4.4 对用户显式析构体不建该 pad）。与 C1 同根因（成员化须拆 include 环，见 C1 判词）。
//   可观察语义（逆序销毁 CharacData→ProcStage，指针/调用序/出参槽逐条同 ORIG）等价。
//   N 封顶（P67/N91）。
StageControl::~StageControl()
{
    reinterpret_cast<CharacData*>(reinterpret_cast<char*>(this) + 0x98)->~CharacData();
    reinterpret_cast<ProcStage*>(m_procStage)->~ProcStage();
}

// ORIG W 0x81348ba：(const) return *(int*)this;
int StageControl::getIndex() const
{
    return m_index;
}

// ORIG W 0x82a669c：*(int*)this = idx;
void StageControl::setIndex(int idx)
{
    m_index = idx;
}

// ORIG T 0x0812fc4e（leaveUser.md 全文）：getUser/getCurCharacR 判空 → 0；
//   [blocker] HistoryLog::leaveDungeon(user, procStage)（报告 0x8133abc 在，
//   源码声明/定义缺，随 ProcStage 域批次）→ out_from_dungeon → 广播
//   basic_info(0,2) → send_itemspace(0) → getCurCharacInvenR().
//   SendItemLockListInven() → setAdvanceAltarIndex(-1)（CUser+0x8d012）→
//   destroy(this) → 1。
bool StageControl::leaveUser()
{
    CUser* user =
        reinterpret_cast<CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    HistoryLog::leaveDungeon(*user, *getProcStage());
    G_GameWorld()->out_from_dungeon(user);
    PacketGuard guard;
    guard.put_header(0, 2);
    guard.put_byte(0);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 0);
    guard.finalize(true);
    G_GameWorld()->send_all(guard, user);
    user->send_itemspace(0);
    user->getCurCharacInvenR()->SendItemLockListInven();
    user->setAdvanceAltarIndex(-1);  // ORIG CUser::setAdvanceAltarIndex(user,-1)
    destroy();                // ORIG 尾部真实调用 destroy(this)（leaveUser.md 实证）
    return 1;                 // ORIG uVar4 = 1
}

// ORIG T 0x0812fb98（onTimerStageTick.md 全文）：ProcStage::getState 为 3/4 时
//   CharacData::upGage() + ProcStage::procTimeLine()，恒返回 1。
bool StageControl::onTimerStageTick()   // 返回类型 bool（advancealtar.h:513 同源，StrC13 会签）
{
    if (reinterpret_cast<ProcStage*>(m_procStage)->getState() == ProcStage::State::S_N3 ||
        reinterpret_cast<ProcStage*>(m_procStage)->getState() == ProcStage::State::S_N4)
    {
        reinterpret_cast<CharacData*>(m_characData)->upGage();
        reinterpret_cast<ProcStage*>(m_procStage)->procTimeLine();
    }
    return true;  // ORIG 恒返回 1
}

// ORIG T 0x0812fa54（reset.md 全文）：ProcStage::resetAll() + CharacData::reset()。
void StageControl::reset()
{
    // ORIG 直访 this+0x04 / this+0x98（不进 getProcStage/getCharacData thunk 函数；P-O0 内联访问器直访）。
    reinterpret_cast<ProcStage*>(m_procStage)->resetAll();
    reinterpret_cast<CharacData*>(m_characData)->reset();
}

// ORIG T 0x081300ce（docs/class_func_reports/advancealtar__StageControl/destroy.md
// 全文，Prod_GlobalAdv 2026-08-24）：reset(this) → G_CGameManager()->returnAdvanceAltar(this)。
void StageControl::destroy()
{
    reset();
    G_CGameManager()->returnAdvanceAltar(this);
}

// ORIG W 0x081348ae：return (ProcStage*)(this + 0x04)。
ProcStage* StageControl::getProcStage()
{
    return reinterpret_cast<ProcStage*>(m_procStage);
}

// ORIG W 0x081348a0：return (CharacData*)(this + 0x98)。
CharacData* StageControl::getCharacData()
{
    return reinterpret_cast<CharacData*>(m_characData);
}

// ----------------------------------------------------------------------------
// ORIG T 0x081300f6（docs/class_func_reports/advancealtar__StageControl/
// sendStageInfo.md 全文）：getUser 判空直接返回；put_header(0,0x209) +
//   put_binary({stageIndex, stageDifficulty}, 8) + finalize(true) → Send。
// ----------------------------------------------------------------------------
// [B24D判词 DONE_AE] 语义正确：getUser 判空返回 + _NotiAdvanceAltarStageInfo(8B)+
//   put_header(0,0x209)+put_binary(&body,8)+finalize(true)+Send。已改用与 ORIG 同源
//   struct 写法（去 body[]+memcpy 二次拷贝，68/68 指令对齐）；仅剩 call/符号地址
//   AE 占位差异。
void StageControl::sendStageInfo()
{
    CUser* user = reinterpret_cast<CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return;
    }

    // ORIG 用 _NotiAdvanceAltarStageInfo（memset 0,8 的 ctor + 两 int 直赋），
    // 栈上两 int 连续成 8B payload；P48 去 body[]+memcpy 的二次拷贝形。
    NOTIPacketStruct::_NotiAdvanceAltarStageInfo body;
    body.m_stageIndex =
        reinterpret_cast<ProcStage*>(m_procStage)->getStageIndex();
    body.m_stageDifficulty =
        reinterpret_cast<ProcStage*>(m_procStage)->getStageDifficulty();

    PacketGuard guard;
    guard.put_header(0, 0x209);
    guard.put_binary(reinterpret_cast<char*>(&body), 8);
    guard.finalize(true);
    user->Send(guard);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fa7a（docs/class_func_reports/advancealtar__StageControl/
// selectStage.md 全文，Prod_GlobalAdv 批次 2026-08-24）：
//   user 判空 → 3；!isAvailableTciket → 0xf5；!isEnterStage(stageId) → 8；
//   CharacData(+0x98)：m_field0 = getCurrentRidableId()、setUser(user)、
//     setSlotItem()；ProcStage::resetAll → startStage(stageId, diff)；
//   失败透传错误码；成功 → sendStageInfo() → sendMapInfo()【未建模批次跳过】
//     → setNotSurvivalModeFirstRound() → set_state(0xd) → 0。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] selectStage：ORIG 78 / ours 83。已按 P183/P0 去掉
//   getCharacData()/getProcStage() out-of-line call 与具名指针槽（ORIG 每访问点重算
//   this+0x98 / this+0x4），并补 P198① `int err = 0; err = startStage(...)` 预清两句式。
//   残差 = N76：`m_field0 = getCurrentRidableId()` 4.4 恒 LHS-first（地址升格 %ebx 跨
//   call ⇒ push ebx/sub $0x24、+5 条），ORIG 4.1 为 RHS-first 直存 `mov %eax,0x98(%edx)`；
//   P211 档②/③（字段 unsigned 化、RHS (unsigned) cast）实测均不翻（4.4.6 同 LHS-first）。
//   另 setUser/startStage 实参寄存器角色 eax↔edx 互换（P75 跨版本）。语义逐条等价。
// [round1-L29 2026-09-10 复核] 判据① CLEAR。残差 = `mov this,%edx; mov %eax,0x98(%edx)`（ORIG
//   RHS-first）vs 我方 `push %ebx; mov this,%ebx; mov %eax,(%ebx)`（LHS-first+指针缓存，
//   N76/P183 反向不可补）；另 `lea 0x98(%eax),%edx` vs `add $0x98,%eax`（N107 六形探针全败）。
//   ORIG 0x0812fdd4 / 0x0812fe1c。
int StageControl::selectStage(CUser* user, int stageId,
                              advancealtar::StageDifficulty::T diff)
{
    if (user == 0)
    {
        return 3;
    }
    // [b28d-D9] ORIG 0x0812fdd4 段每次重算 `add $0x8df60,%eax`（P183 地址重物化，
    // 无具名引用槽）；具名 CharacAdvanceAltarManager& 使 4.4 落 -0x18 槽往返。
    if (!user->m_advanceAltar.isAvailableTciket())
    {
        return 0xf5;
    }
    if (!user->m_advanceAltar.isEnterStage(stageId))
    {
        return 8;
    }

    // [b28d-solo-StageControl] P183/P0：ORIG 每个访问点重算 this+0x98 / this+0x4
    // （无 getCharacData/getProcStage call、无具名指针槽）；逐点表达式直用。
    reinterpret_cast<CharacData*>(m_characData)->m_field0 =
        user->m_advanceAltar.getCurrentRidableId();
    reinterpret_cast<CharacData*>(m_characData)->setUser(user);
    reinterpret_cast<CharacData*>(m_characData)->setSlotItem();

    reinterpret_cast<ProcStage*>(m_procStage)->resetAll();
    // P198①：ORIG 在 startStage 前有 `movl $0x0,-0xc(%ebp)` 预清 ⇒ 两句式。
    int err = 0;
    err = reinterpret_cast<ProcStage*>(m_procStage)->startStage(stageId, diff);
    if (err != 0)
    {
        return err;
    }

    sendStageInfo();
    sendMapInfo();
    reinterpret_cast<ProcStage*>(m_procStage)->setNotSurvivalModeFirstRound();
    user->set_state((ch_state)0xd);
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081306be（sendEplp.md 全文）：getUser 判空直接返回；
//   available = user->m_advanceAltar.isAvailableTciket()（CUser+0x8df60）；
//   char isAvailableTicket; pad[3]}（8B，权威定义 NOTIPacketStruct.h）；
//   char isAvailableTicket; pad[3]}（8B，权威定义 advancealtar.h）；
//   put_header(0,0x20e) + put_binary(8) + finalize(true) → Send。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] P183/P0：ORIG 直接 `this+0x98` 调 CharacData::getUser
//   （无 getCharacData thunk call）；isAvailableTciket 结果先落具名 bool 槽 -0x9
//   （`mov %al,-0x9` 直存、body 字段处 `movzbl -0x9; mov %al` 读回），且声明在 body
//   之前 ⇒ 调用序在 body ctor 之前；旧 `? 1 : 0` 三目被物化成 test/je/mov$1/jmp/mov$0。
void StageControl::sendEplp(StageEndType::T endType)
{
    CUser* user = reinterpret_cast<CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return;
    }

    const bool isAvailableTicket = user->m_advanceAltar.isAvailableTciket();
    NOTIPacketStruct::_NotiAdvanceAltarEplpState body;
    body.m_eplpType = endType;
    body.m_isAvailableTicket = isAvailableTicket;

    PacketGuard guard;
    guard.put_header(0, 0x20e);
    guard.put_binary((char*)&body, 8);
    guard.finalize(true);
    user->Send(guard);
}

// ORIG T 0x08130644（onEplp.md 全文）：sendEplp(endType) +
//   Timer_StageControl::RegistTimerEPLPReturnVillage(getIndex())
//   （ORIG T 0x0812f956，定义 advancealtar_ProcStage.cpp）。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] onEplp：ORIG 27 / ours 29（对齐 nop 剥离后 28）。P222 否定
//   守卫形已把块序/跳转方向逐条对齐（js/jle/je/jmp/movb/jmp/movb 全等，含 e>3 兜底 jmp）。
//   唯一残差 = 4.4 语句级重载形参多一条 `mov 0xc(%ebp),%eax` + 第二比较 `cmpl $3,mem`
//   vs ORIG `cmp $3,%eax`（P85：ORIG 4.1 跨语句保留 %eax；P0 禁具名局部/绑定形）。
//   语义逐值等价（e<0/e>3 均不写 flag，e∈[0,2]→0、e==3→1）。N 封顶（P85/N82 族）。
// [round1-L29 2026-09-10 复核] 判据① jcc 目标逐条核对 CLEAR；残差 = 4.4 语句级重载形参多 1 条
//   `mov 0xc(%ebp),%eax` + 第二比较 `cmpl $3,mem` vs ORIG `cmp $3,%eax`（P85 跨语句 %eax 保活；
//   P0 禁绑定）⇒ N（ORIG 0x08130644）。
void StageControl::onEplp(StageEndType::T endType)
{
    // ORIG 0x813064a：bool 岛 -0x9（预清 movb $0x0、条件链物化 movb $0/$1、无读者——
    // 4.1 保留死 store；volatile 强制 4.4 保留写与槽）。分支内独立 movb 直存复现
    // ORIG 两 arm `movb $0/$1,-0x9`（三目形被物化折成 mov%al）。
    volatile bool flag = false;
    if (endType >= 0)
    {
        if (endType > 2)
        {
            if (endType != 3)
                goto flag_done;
            goto flag_true;
        }
        flag = false;
        goto flag_done;
    flag_true:
        flag = true;
    flag_done:;
    }
    sendEplp(endType);
    Timer_StageControl::RegistTimerEPLPReturnVillage(getIndex());
}

// ORIG T 0x08130692（onEplpReturnVillage.md 全文）：ProcStage::getState()==6 → leaveUser()。
void StageControl::onEplpReturnVillage()
{
    if (reinterpret_cast<ProcStage*>(m_procStage)->getState() ==
        ProcStage::State::S_N6)
    {
        leaveUser();
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x081305f6（onClearReward.md 全文）：(-1 < e && 2 < e) && e == 3 时
//   ProcStage::setState(6) + Timer_StageControl::RegistTimerEPLP(getIndex(), 3)
//   （第二参为字面 3，非透传）。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] onClearReward：ORIG 30 / ours 30。P223 多岛形（三条独立否定
//   守卫 goto + 尾标签串 island1/2/3）逐条复现 ORIG 三岛（js/jle/jne 分落 nop+jmp 岛，
//   第三岛直落；body 尾 jmp join 同形）。唯一残差 = 4.4 语句级重载形参多一条
//   `mov 0xc(%ebp),%eax` + 第三比较 `cmpl $3,mem` vs ORIG `cmp $3,%eax`（P85 跨语句
//   %eax 保活；P0 禁具名局部/绑定形）。语义逐值等价。N 封顶（P85 族）。
// [round1-L29 2026-09-10 复核] 判据① CLEAR；P286 单 case switch 已实测（27→28 条反劣，已回退）。
//   残差同 onEplp（P85 族）⇒ N（ORIG 0x081305f6）。
void StageControl::onClearReward(StageEndType::T endType)
{
    if (endType < 0)
        goto island1;
    if (endType <= 2)
        goto island2;
    // [b28d-sweep] P286 单 case switch 实测：得 `mov 0xc(%ebp),%eax; cmp $0x3,%eax`
    //   但整函数 27→28 条（第二次比较前仍重载形参，ORIG 26 条为单次装载三连比较）
    //   ⇒ 反向回退，维持 P85「4.1 跨语句 %eax 保活不可复现」封顶。
    if (endType != 3)
        goto island3;
    // ORIG 0x0813060e：this+4 直访问 m_procStage 槽（无 getProcStage call）、
    // mov (%eax) 直读 m_index（无 getIndex call）、第二参透传 endType（0x81330629）。
    reinterpret_cast<ProcStage*>(m_procStage)->setState(ProcStage::State::S_N6);
    Timer_StageControl::RegistTimerEPLP(m_index, endType);
island1:
    ;
island2:
    ;
island3:
    ;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fbfa（onTimer.md 全文）：msg 0xa9 → onEplp(endType)；
//   0xaa → onEplpReturnVillage()；0xa8 → onClearReward(endType)；其余无操作。
//   TIMER_MESSAGE 0xa8-0xaa 枚举器名待 TimerDispatcher 批次定名，暂用字面值。
// ----------------------------------------------------------------------------
// [b28d-D9] P85/P209：ORIG 线性比较链值全程 %eax；4.4 signed 形参 mem 直比（unsigned 强转实测反劣 29>27）（ORIG 0x0812fbfa sz 0x00000053）
void StageControl::onTimer(TIMER_MESSAGE msg, StageEndType::T endType)
{
    // [b28d-solo-StageControl] P251：ORIG 值全程 %eax 无槽 + cmp 链 ⇒ 原码是
    // switch((int)msg)（窄枚举需 (int) 规避 P305 越界 case 静默删除）；叶块顺序 =
    // case 书写顺序（P46）照 ORIG：a8 → a9 → aa。
    switch ((int)msg)
    {
    case 0xa8:
        onClearReward(endType);
        break;
    case 0xa9:
        onEplp(endType);
        break;
    case 0xaa:
        onEplpReturnVillage();
        break;
    default:
        break;
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fdd4（gotoStage.md 全文；静态语义，asm 不使用 this）：
//   getUserVillage 判空 → cMyTrace(0x585 行, vill) 返回；getArea(get_area(false))
//   判空 → cMyTrace(0x58c 行, area) 返回；否则 erase_user →
//   包1 0x17{short uid, byte vill, byte 0xff, short posX, short posY,
//   byte direction, byte visible_values} Send → clear →
//   包2 3{byte 1, short uid, byte 1} Send + send_all(guard, user) → set_state(0xd)。
// ----------------------------------------------------------------------------
// 头声明 static（ORIG 无 this）；类外定义不带 static。
// [b28d-D9] 槽差/P227：trace 临时链位置 + 槽序差（ORIG 0x0812fdd4 sz 0x000002fa）
void StageControl::gotoStage(CUser* user)
{
    Village* vill = G_GameWorld()->getUserVillage(user);
    if (vill == 0)
    {
        // [b28d-solo-StageControl] P227：ORIG 在 ctor 前求 vararg 实参并以 %ebx 跨
        //   ctor 保活（`call getCurCharacVill; movsbl %al,%ebx; ... call C1; mov %ebx,
        //   0x8(%esp)`）⇒ 全表达式临时链形。
        cMyTrace(__PRETTY_FUNCTION__, 0x585, 5)(
            "%d", (int)user->getCurCharacVill());
        return;
    }
    // [b23S3C] P48：ORIG 两次独立 get_area(false)——+0x80 结果 eax 直落
    // getArea 实参槽（无 areaId 槽）、判空分支 +0xab 再 call 一次喂 trace。
    Area* area = vill->getArea(user->get_area(false));
    if (area == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x58c, 5)(
            "%d", user->get_area(false));
        return;
    }

    area->erase_user(user);
    PacketGuard guard;
    guard.put_header(0, 0x17);
    guard.put_short(user->get_unique_id() & 0xffff);
    guard.put_byte(user->getCurCharacVill());
    guard.put_byte(0xff);
    guard.put_short(user->get_posX() & 0xffff);
    guard.put_short(user->get_posY() & 0xffff);
    guard.put_byte(user->get_direction());
    guard.put_byte(user->get_charac_visible_values());
    guard.finalize(true);
    user->Send(guard);

    guard.clear();
    guard.put_header(0, 3);
    guard.put_byte(1);
    guard.put_short(user->get_unique_id() & 0xffff);
    guard.put_byte(1);
    guard.finalize(true);
    user->Send(guard);
    G_GameWorld()->send_all(guard, user);
    user->set_state((ch_state)0xd);
}


// ----------------------------------------------------------------------------
// ORIG W 0x081307a6（getCenterCount.md 全文）：map/user 判空 → 0；
 // 遍历 CMap::get_monster_list()（按值返回 list<_mapMonster>），对每只怪
 // getTowerInfo(monster.m_orderIdx, out, out)（+0x2c），命中即计数。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] getCenterCount：ORIG 103 / ours 99。已对齐：P183 去
//   procStage/characData 具名指针槽（直接 this+4 / this+0x98）；P28 增量段 it++（ppEi
//   后置带 sret 临时槽）；P96 非 const iterator（去转换构造临时）；ORIG -0x10 list 指针
//   具名槽（monsterList = &monsters，begin/end 从槽取 this）逐条复现。
//   唯一残差 = hit 取反物化：ORIG `movzbl -0x9,%eax; xor $1,%eax; test %al,%al; jne
//   <skip>; ++count; jmp; nop`（8 条）vs 4.4 `cmpb $1,-0x9; jne; ++count`（P168a④：
//   寄存器 xor 物化无绑定不可达，X2 两语句落栈槽；`hit^1` 被折 mem 直比 N18b）。
//   语义等价（hit!=0 ⇒ ++count）。N 封顶（P168a④/N18b 族）。
// [b28d-sweep 2026-09-09] 复验：归一化后唯一差异仍是上述 4↔1 条；另试 const 容器 +
//   const_iterator（ORIG 0x08135d88/0x08135dae 段确用 `_ZNKSt4list...` const 版）⇒
//   verdict 无改善（仍 DIFF）且本 TU 少发射 iterator→const_iterator 转换构造
//   ⇒ check_tu identical 80→79 回退，已回退维持非 const iterator。
// [round1-L29 2026-09-10 修复] 真因：callee `getTowerInfo` 权威声明**已是 bool**
//   （advancealtar.h:421 / advancealtar_DB.cpp:314），我方却用 `unsigned char hit` 接收 ⇒
//   4.4 折 `cmpb $1,-0x9`。改为 `const bool hit` + P223 goto 尾空标签形
//   （`if (!hit) goto hit_done; ++count; hit_done:;`）⇒ **AE 102/102 ndiff=0**。
//   ORIG 0x08130869 证据形：`mov %al,-0x9; movzbl -0x9,%eax; xor $1,%eax; test %al,%al;
//   jne <skip>; addl $1,-0x14; jmp <join>; <skip>: nop`。
int StageControl::getCenterCount() const
{
    // [b28d-solo-StageControl] P183：ORIG 直接 `this+4` 调 getMap、`this+0x98` 调
    //   getUser（无具名 procStage/characData 指针槽）；P28：ORIG 增量段调 ppEi（后置
    //   迭代器带独立 sret 临时槽）⇒ 源形必写 `it++`。
    CMap* map = reinterpret_cast<const ProcStage*>(m_procStage)->getMap();
    if (map == 0)
    {
        return 0;
    }
    CUser* user = reinterpret_cast<const CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return 0;
    }

    int count = 0;                                      // -0x14
    // [回滚记录 b28d-sweep] 曾试 const 容器 + const_iterator（ORIG 0x08135d88 段确用
    //   `_ZNKSt4list...5beginEv`/`_ZNKSt20_List_const_iterator...`）；verdict 无改善
    //   （残差仍是下方 hit 物化，仍 DIFF），且本 TU 不再发射 ORIG 别处实存的非 const
    //   `_ZNSt14_List_iteratorI11_mapMonsterEppEi/EptEv`（0x08311764/0x08311756）⇒
    //   check_tu_routed identical 80→78 回退，故维持非 const iterator 形。
    std::list<_mapMonster> monsters = map->get_monster_list();
    // ORIG 0x0813080e：`lea -0x2c,%eax; mov %eax,-0x10(%ebp)` —— 具名 list 指针槽
    // -0x10，begin/end 均从该槽取 this（Ghidra local_14 = local_30）。
    std::list<_mapMonster>* monsterList = &monsters;
    for (std::list<_mapMonster>::iterator it = monsterList->begin();
         it != monsterList->end(); it++)
    {
        int valueA = 0;                                 // local_88[1]
        int valueB = 0;                                 // local_88[0]
        // ORIG 0x08130869：getTowerInfo 结果先落具名字节槽 -0x9（`mov %al,-0x9`），
        // 再 `movzbl -0x9; xor $1; test; jne <skip>; ++count; jmp; nop` 取反物化。
        const bool hit =
            user->m_advanceAltar.getTowerInfo(it->m_mobId, valueA, valueB);
        // ORIG 0x08130869：jne <skip 岛> + body 尾 jmp <join> ⇒ P223 goto 尾空标签形。
        if (!hit)
            goto hit_done;
        ++count;
    hit_done:;
    }
    return count;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081301e0（sendMapInfo.md 全文，0416B）：user/map/timeLine 三重判空；
 // _NotiAdvanceAltarMapInfo(0x14 packed){stageType, clearSurvivalRoundNo,
 // mapIndex=CMap::get_index, currentPhaseIndex=timeline+0xc, mapBasisLevel}；
 // put_header(0,0x20a)+put_binary(0x14)；put_int(phaseSummarys.size()) 后逐
 // _PhaseSummary put_binary(0xc)；生存模式且非首轮：put_int(getCenterCount())
 // 并遍历怪物表逐只构造 _MapMonsterSummary（仅 getTowerInfo 命中者
 // addCreatedMapMonster + put_binary(0xd)）；否则 put_int(0)；finalize→Send。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] sendMapInfo：ORIG 293 / ours 290。已修：P183 去
//   getCharacData/getProcStage out-of-line call 与具名 procStage 槽（每点重算
//   this+0x98/this+0x4）；body ctor 移到 map 判空后/getTimeLine 前（ORIG 0x0813026e）；
//   P214① sendMonsters 改直接 `if (a||b)`（无具名 bool 槽）；else 分支具名 zero 局部；
//   Send 前重新 `this+0x98` 调 getUser。
// [b28d-sweep 2026-09-09 重判] 旧判词「-0x20 死计数器/-0x18 list 指针槽/it++ 引入即
//   反劣」**全部作废**——旧读数是在 3 处真 bug + put_binary 重载 + hit 槽未补的前提下
//   测的。本轮修后（clean 口径 ORIG 289 / ours 292）：
//   ①真 bug：`!isSurvivalModeFirstRound()` 极性反（ORIG je/jne 同落 L_true，P224）；
//   ②真 bug：phase 循环 put_binary 误选 PKc 重载（ORIG 三处均 Pc @0x0811df08，P167）；
//   ③真 bug：getTowerInfo 首实参误传 m_orderIdx，ORIG 取 it->+0x00=m_mobId（0x08130454）；
//   ④补 hit 具名 uchar 槽（ORIG 0x0813047c `mov %al,-0xd`，P0）；
//   ⑤phase/monsters 两循环改后置 `it++`（ORIG 调 ppEi，P28 增量侧）；
//   ⑥补 phase 死计数器（ORIG 0x08130308 `movl $0,-0x20` + 每轮 `addl $1,-0x20`，P171）；
//   ⑦补 monsters 具名 list 指针槽（ORIG 0x081303d9 `mov %eax,-0x18`）；
//   ⑧删 cam 具名引用槽（ORIG 每轮重算 lea 0x8df60，P183）；
//   ⑨删原两个撑帧 double（补 ④⑥⑦ 后 4.4 帧自涨到 0xa0，删后回 0x90 与 ORIG 逐字对齐）。
//   残余 = hit 取反物化（ORIG movzbl/xor/test/jne 4 条 vs 4.4 `cmpb $0` 2 条，P168a④
//   /N18b 封顶）+ 槽号分配差（N95/N126 版本常数）。p167：put_binary 差集清零，仅剩
//   D1/D2、C1/C2 同址别名与 list const/非 const 迭代器符号选择差（非语义缺口）。
// [round1-L29 2026-09-10 复核] 我方 292 / ORIG 289；判据① 分支数 66 vs 67，无目标错位。
//   残差含 getTowerInfo 的 X 族（同 getCenterCount，待 callee bool 化）+ 槽号 N95/N126。
//   ORIG 0x081301e0。
void StageControl::sendMapInfo()
{
    // [b28d-solo-StageControl] P183：ORIG 每访问点重算 this+0x98 / this+0x4
    // （无 getCharacData/getProcStage out-of-line call、无具名 procStage 指针槽）。
    // [b28d-sweep] 原 P190 两个取址 double 撑帧（0x80→0x90）；补 hit/phaseIndex/
    //   monsterList 具名局部后 4.4 帧已自涨到 0xa0（ORIG 0x90）⇒ 撑帧局部转为冗余，
    //   删除后帧回落 0x90（实测 sub 立即数逐字对齐）。
    CUser* user = reinterpret_cast<CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return;
    }
    CMap* map = reinterpret_cast<ProcStage*>(m_procStage)->getMap();
    if (map == 0)
    {
        return;
    }
    // ORIG 0x0813026e：body ctor 在 map 判空之后、getTimeLine 之前（Ghidra local_5c）。
    NOTIPacketStruct::_NotiAdvanceAltarMapInfo body;
    const TimeLine* timeLine =   // [b28d-Main 代落·S374] getTimeLine 返回 const TimeLine*（ORIG 调用点 0x8130237 为 `_ZNK...getTimeLineEv`，消费端仅读）
        reinterpret_cast<ProcStage*>(m_procStage)->getTimeLine();
    if (timeLine == 0)
    {
        return;
    }

    body.m_stageType =
        reinterpret_cast<ProcStage*>(m_procStage)->getStageType();
    body.m_currentSurvivalRoundNo =
        reinterpret_cast<ProcStage*>(m_procStage)->getCurrentSurvivalRoundNo();
    body.m_mapIndex = map->get_index();
    // TimeLine+0x0c = m_summary（TimeLineSummary）+0x00 = m_field0（int，`mov 0xc(%eax)`）。
    body.m_currentPhaseIndex = timeLine->m_summary.m_field0;
    body.m_mapBasisLevel =
        reinterpret_cast<ProcStage*>(m_procStage)->getMapBasisLevel();

    PacketGuard guard;
    guard.put_header(0, 0x20a);
    guard.put_binary(reinterpret_cast<char*>(&body), 0x14);
    guard.put_int(static_cast<int>(timeLine->m_summary.m_phaseSummarys.size()));
    // [b28d-sweep] P167：ORIG 三处 put_binary 目标均为 `_ZN18InterfacePacketBuf10put_binaryEPci`
    //   （char* 重载 @0x0811df08），此前 phase 循环误用 `put_binary(const char*,int)`
    //   （@0x0815096a）⇒ 调用点差集 MISSING/EXTRA 各 1。改传 char*（const_cast 去 const）。
    // [b28d-sweep] P28 增量侧：ORIG 0x08130393 增量段调
    //   `_ZN9__gnu_cxx17__normal_iteratorIPKN...EEEppEi`（后置，带独立 sret 临时槽
    //   -0x40）⇒ 源形写 `it++`。
    // P328⑥/P171：ORIG 在 `call begin`（sret -0x68）之后发 `movl $0x0,-0x20(%ebp)`，
    //   每轮增量段 `addl $0x1,-0x20(%ebp)`（死计数器，全函数无读者）⇒ 照抄死写入。
    std::vector<_PhaseSummary>::const_iterator it =
        timeLine->m_summary.m_phaseSummarys.begin();
    int phaseIndex = 0;
    for (; it != timeLine->m_summary.m_phaseSummarys.end(); it++, ++phaseIndex)
    {
        guard.put_binary(reinterpret_cast<char*>(
                             const_cast<_PhaseSummary*>(&*it)), 0xc);
    }

    // ORIG 0x08130391：`cmp $1,%eax; jne L_true; call; test; je L_false; L_true: mov $1;
    // jmp; L_false: mov $0; test %al,%al; je L_else` —— 无具名 bool 槽（P214①）。
    // 极性：jne/je 均落 L_true ⇒ monsters 分支 = `stageType != 1 || isSurvivalModeFirstRound()`
    // （P224：je 落点定极性；此前误写 `!isSurvivalModeFirstRound()` = 真语义 bug）。
    if (reinterpret_cast<ProcStage*>(m_procStage)->getStageType() != 1 ||
        reinterpret_cast<ProcStage*>(m_procStage)->isSurvivalModeFirstRound())
    {
        // [b28d-sweep] 试过 const 化 monsters（ORIG 0x081303ce 段确调 `_ZNKSt4list...5beginEv`
        //   const 版）：p167 调用差集 48→46=ORIG，但本 TU 少发射
        //   `_ZNSt20_List_const_iteratorI11_mapMonsterEC1ERKSt14_List_iteratorIS0_E`
        //   ⇒ check_tu identical 80→79 回退，故维持非 const 形（语义等价，仅符号选择差）。
        std::list<_mapMonster> monsters = map->get_monster_list();
        // ORIG 0x081303d6：`lea -0x3c(%ebp),%eax; mov %eax,-0x18(%ebp)` ⇒ 具名 list
        //   指针槽（begin/end 均从该槽取 this）。
        std::list<_mapMonster>* monsterList = &monsters;
        guard.put_int(getCenterCount());
        // [b28d-sweep] P183：ORIG 0x08130456 每轮重算 `mov -0x2c(%ebp),%edx;
        //   lea 0x8df60(%edx),%ecx`（无具名引用槽）⇒ 去掉 CharacAdvanceAltarManager& 局部。
        // [b28d-sweep] P28 增量侧：ORIG 增量段调 `_ZNSt20_List_const_iteratorI11_mapMonsterEppEi`
        //   （后置，带 sret 临时槽）⇒ 源形写 `it++`。
        for (std::list<_mapMonster>::const_iterator it = monsterList->begin();
             it != monsterList->end(); it++)
        {
            advancealtar::_MapMonsterSummary summary;
            summary.m_orderIdx = it->m_orderIdx;          // _mapMonster+0x2c
            summary.m_seq = 0;                            // addCreatedMapMonster 覆写
            int towerValue = 0;                           // getTowerInfo 出参 1
            int towerCount = 0;                           // getTowerInfo 出参 2
            // [b28d-sweep] 真 bug：ORIG 0x08130454 `mov (%eax),%eax` 取 it->+0x00
            //   （_mapMonster::m_mobId）作 getTowerInfo 首实参；此前误传 m_orderIdx
            //   （+0x2c，仅用于下方 summary.m_orderIdx 赋值，ORIG 0x0813042c 实证）。
            //   ORIG 0x0813047c `mov %al,-0xd(%ebp)` + 回读 ⇒ 具名字节局部（P0 槽往返）。
            unsigned char hit = user->m_advanceAltar.getTowerInfo(
                it->m_mobId, towerValue, towerCount);
            // [round1-L29 2026-09-10 复核] bool 具名局部 + P223 goto 尾标签形实测劣化
            //   （295 vs ORIG 289）⇒ 回退保持 uchar 形；残差归 N（ORIG 0x0813047c）。
            if (hit)
            {
                summary.m_towerRefA =
                    static_cast<short>(towerValue);       // u16 截断 [+0x06]
                summary.m_towerRefB =
                    static_cast<unsigned char>(towerCount);  // 低字节 [+0x08]
                summary.m_roleType = static_cast<unsigned char>(
                    it->m_roleType);                      // +0x18 低字节
                summary.m_posX = static_cast<unsigned char>(
                    it->m_posX);                          // +0x20 低字节
                summary.m_champion = it->m_champion;      // +0x1c
                reinterpret_cast<ProcStage*>(m_procStage)->
                    addCreatedMapMonster(summary);
                guard.put_binary(reinterpret_cast<char*>(&summary), 0xd);
            }
        }
    }
    else
    {
        // ORIG 0x08130570：`movl $0x0,-0xc(%ebp); mov -0xc(%ebp),%eax; ... put_int`
        // —— 具名零局部（P0 槽往返），非字面量直压。
        int zero = 0;
        guard.put_int(zero);
    }
    guard.finalize(true);
    // ORIG 0x0813059c：Send 前重新 `this+0x98` 调 getUser（不复用开头 user 槽）。
    reinterpret_cast<CharacData*>(m_characData)->getUser()->Send(guard);
}

// ---- TimeLine / Phase / TimeLineSummary（ORIG advancealtar__TimeLine 报告）----
Phase::Phase()
    : m_field0(0), m_field4(0), m_assigns()
{
}

void Phase::reset()
{
    // ORIG T 0x08899cf8：仅 m_field0 清零 + m_assigns.clear()（+0x04 不清）。
    m_field0 = 0;
    m_assigns.clear();
}

TimeLineSummary::TimeLineSummary()
    : m_field0(0), m_phaseSummarys()
{
}

TimeLineSummary::~TimeLineSummary()
{
}

TimeLineSummary::TimeLineSummary(const TimeLineSummary& other)
    : m_field0(other.m_field0), m_phaseSummarys(other.m_phaseSummarys)
{
}

TimeLineSummary& TimeLineSummary::operator=(const TimeLineSummary& other)
{
    m_field0 = other.m_field0;
    m_phaseSummarys = other.m_phaseSummarys;
    return *this;
}

void TimeLineSummary::reset()
{
    // ORIG T 0x08899d4e：int 清零 + vector clear。
    m_field0 = 0;
    m_phaseSummarys.clear();
}

// TimeLine ctor（ORIG T 0x08899d6e）：vector<Phase> 默认构造 + summary@+0xc 构造。
TimeLine::TimeLine()
    : m_phases(), m_summary()
{
}

// TimeLine dtor（ORIG T 0x08899db4）：summary 先、vector 后自动析构。
TimeLine::~TimeLine()
{
}

// 拷贝构造（ORIG W 0x0837e642）：vector/summary 成员拷贝。
TimeLine::TimeLine(const TimeLine& other)
    : m_phases(other.m_phases), m_summary(other.m_summary)
{
}

TimeLine& TimeLine::operator=(const TimeLine& other)
{
    m_phases = other.m_phases;
    m_summary = other.m_summary;
    return *this;
}

void TimeLine::reset()
{
    // ORIG T 0x08899dfa：vector<Phase>::clear + TimeLineSummary::reset。
    m_phases.clear();
    m_summary.reset();
}

}  // namespace advancealtar