// ============================================================================
// advancealtar_Manager.cpp —— advancealtar::Manager::{selectStage,
// procErrorSelectStage} 真实实现（Prod_GlobalAdv 2026-08-24）。
//
// 证据：docs/class_func_reports/advancealtar__Manager/{selectStage,
// procErrorSelectStage}.md（ORIG 汇编+反编译 C 全文已读）。
// 阻塞符号（声明就位、定义待批次）：StageControl::selectStage(0x0812fa7a)。
// ============================================================================
#include "advancealtar.h"     // advancealtar::Manager / StageDifficulty / ENUM_ERROR
#include "CGameManager.h"     // G_CGameManager / getAdvanceAltar / StageControl 完整声明
#include "PacketGuard.h"      // PacketGuard : InterfacePacketBuf（put_header/put_byte/put_int/finalize）
#include "GameTypes.h"        // cMyTrace
#include "CUser.h"            // CUser 完整类型（m_advanceAltarIdx 字段化写）
#include "advancealtar_ProcStage.h"  // ProcStage / Timer_StageControl / HistoryLog（本批新增叶子头）
#include "GameWorld.h"             // G_GameWorld / out_from_dungeon / send_all（leaveUser）
#include "CGuildServerProxy.h"     // CEventManager / CEventBase 完整类型（isOnEvent）
#include "GlobalData.h"            // GlobalData::s_event_manager（isOnEvent）
#include "CMDPacketStruct.h"         // _SetSlotData 完整定义（setSlot/exchangeSlot/resetSlot 按值访问）

namespace advancealtar

{

// ORIG namespace 内 static 常量（_ZN12advancealtarL<len><name>E，r 段；值实测 ORIG
// .rodata 0x08af2e40..0x08af2e90 / 0x08b6d3cc..）：gcc 4.4 对未使用也发射。
// ----------------------------------------------------------------------------
static const int kMaxSlotCount = 12;            // 0x08af2e70
static const int kPlayMinLevel = 50;            // 0x08af2e64
static const int kMaxStageCount = 50;           // 0x08af2e6c
static const int kUnitSlotCount = 6;            // 0x08af2e50
static const int kSkillSlotCount = 6;           // 0x08af2e54
static const int kMaxBuyItemCount = 50;         // 0x08af2e74
static const int kMaxRewardCount = 30;          // 0x08af2e78
static const int kDefaultTicketFree = 100;      // 0x08af2e4c
static const int kInvalidRewardType = 0;        // 0x08af2e68
static const int kMaxAdvanceAltarDbData = 1;    // 0x08af2e7c
static const int kMaxStrBuffer = 2048;          // 0x08b6d3d0
static const int kMaxZipBuffer = 2048;          // 0x08b6d3cc
static const char* const kDelimterItem = "|";   // 0x08b6d3d8 指针 → '|'(0x7c)
static const char* const kDelimterField = ",";  // 0x08b6d3dc 指针 → ','(0x2c)
static const int KGaugeWidth[4] = { 60, 90, 125, 170 };       // 0x08af2e80
static const unsigned char kUnitSlotNoLock[6] = { 0, 0, 0, 0, 0, 0 };   // 0x08af2e58
static const unsigned char kSkillSlotNoLock[6] = { 0, 0, 0, 0, 1, 1 };  // 0x08af2e5e（实测）

// ----------------------------------------------------------------------------
// ORIG T 0x0813090e _ZN12advancealtar7Manager11selectStageEP5CUseriNS_15StageDifficulty1TE
// user 判空 → 3；难度越界(<0||>2) → 8；getAdvanceAltar() 判空 → cMyTrace +
// "StaticPool Defence DungeonNULL" → 3；
//   StageControl::selectStage != 0 → control->destroy() 后透传错误码；
//   == 0 → setAdvanceAltarIndex(control->getIndex())（ORIG 经 CUser::setAdvanceAltarIndex
//   W 0x0813490e 写 m_advanceAltarIdx，此处字段化等价写）→ 返回 0。
// ----------------------------------------------------------------------------
int Manager::selectStage(CUser* user, int stageId, StageDifficulty::T diff)
{
    if (user == 0)
    {
        return 3;
    }
    if (diff < 0 || diff > 2)
    {
        return 8;
    }

    // ORIG +0x2f `movb $0x0,-0x11(%ebp)`：难度合法后、取 control 前，原码有一枚
    // 具名 char 局部被置 0（P0 槽-局部双向对应律：ORIG 有槽写⇒原码有该具名局部；
    // Ghidra `undefined1 local_15`）。该值后续无读取点（写入即弃），仅占栈槽。
    char isNull = 0;
    (void)isNull;
    StageControl* control = G_CGameManager()->getAdvanceAltar();
    if (control == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x6ab, 5);
        trace("StaticPool Defence DungeonNULL");
        return 3;
    }

    const int err = control->selectStage(user, stageId, diff);
    if (err != 0)
    {
        control->destroy();
        return err;
    }
    // ORIG +0xbf..：`call getIndex; cwtl; mov %eax,0x4(%esp); mov user,%eax;
    // mov %eax,(%esp); call CUser::setAdvanceAltarIndex(short)`。getIndex 返回 short，
    // cwtl 符号扩展入参槽，故必须走真实 setter（直写字段生 `mov %dx,disp` 异形）。
    user->setAdvanceAltarIndex(control->getIndex());
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081309f0 _ZN12advancealtar7Manager20procErrorSelectStageEP5CUser10ENUM_ERROR
// user 判空或 err==0 直接返回；否则 PacketGuard: put_header(1,0x10) /
// put_byte(0) / put_int(err) / finalize(true) → CUser::Send。
// ----------------------------------------------------------------------------
void Manager::procErrorSelectStage(CUser* user, ENUM_ERROR err)
{
    if (user == 0 || err == 0)
    {
        return;
    }

    PacketGuard guard;
    guard.put_header(1, 0x10);
    guard.put_byte(0);
    guard.put_int(static_cast<int>(err));
    guard.finalize(true);
    user->Send(guard);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130ab2 _ZN12advancealtar7Manager9dieCharacEP5CUser
// user 判空 / getAdvanceAltar() 判空直接返回；仅 isRewardState() 时处理：
//   stageType==1 → clearStageAndReward() + setState(5) +
//                  RegistTimerClearReward(getIndex(), END_N1)
//   其它       → setState(6) + RegistTimerEPLP(getIndex(), END_N1) +
//                  HistoryLog::dungeonClearInfo(user, control, false)
// clearStageAndReward(0x0812e3f6) 现按 ORIG 直调（声明已在 advancealtar_ProcStage.h；
//   定义归 ProcStage 批次，链接期解析）。
//
// [b22判词·N族(P16/P63/P112 返回落点岛)] 函数体逐条与 ORIG 一致（含 5 处
//   getProcStage 现取现用、isRewardState 的 `xor $0x1; test; jne` bool 取反、
//   setState/RegistTimer*/dungeonClearInfo 全序）；-O0 下 ORIG 把 control==0 的
//   统一 return 直落 `leave`、user==0/isReward 各占一 nop+jmp 落点岛，本工具链
//   4.4.7 三处 early-return 各生成独立 nop+jmp 岛（多 1 对 nop/jmp，+2 insn）。
//   此为跨 GCC 版本基本块落点布局差异，非可执行语义（call 目标/数据地址经 relink
//   消解，见 §6.3），语义等价，byte 层 N 封顶。
// ----------------------------------------------------------------------------
void Manager::dieCharac(CUser* user)
{
    // ORIG 精确控制流（逐指令对齐，报告 dieCharac.md 全文已读）：
    //   +0x06 `cmpl $0,0x8(%ebp); je +0x100`(user==0 → nop+jmp leave 岛)
    //   +0x1e `cmpl $0,-0xc(%ebp); je +0x104`(control==0 → 直落统一 leave，无中继岛)
    //   +0x3b `xor $0x1,%eax; test; jne +0x103`(isRewardState!=1 → nop;leave 岛)
    //   两 success 支 `jmp +0x104`。
    // P0 槽-局部双向对应律：control 为具名局部（-0xc 槽往返）；ORIG 无 ProcStage
    //   具名局部——每个 ProcStage 调用点现取现用 `control->getProcStage()->…`（结果
    //   寄存器直传不落槽，缓存成命名局部会合并 getProcStage 少发 call）。isReward 用
    //   `if(!x) return` 触发 bool 取反 xor 形（P22）对齐 ORIG。
    // [b28d-D33] 尾块布局 N 封顶：早返形（本形）69 条 vs ORIG 68 条，唯一残差为
    // 多一个 `jmp <epilogue>;nop` 岛（我们 3 岛 vs ORIG 2 岛，ORIG control==0
    // 直跳 leave）。P223 负条件 goto 尾标签形与 P63 && 合并形实测均劣（69/72），
    // 属 4.1 vs 4.4 基本块落点布局差，语义等价（call 序列逐条一致）。
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return;
    }
    if (!control->getProcStage()->isRewardState())
    {
        return;
    }
    if (control->getProcStage()->getStageType() == 1)
    {
        control->getProcStage()->clearStageAndReward();
        control->getProcStage()->setState(ProcStage::State::S_N5);
        Timer_StageControl::RegistTimerClearReward(control->getIndex(),
                                                   StageEndType::END_N1);
    }
    else
    {
        control->getProcStage()->setState(ProcStage::State::S_N6);
        Timer_StageControl::RegistTimerEPLP(control->getIndex(),
                                            StageEndType::END_N1);
        HistoryLog::dungeonClearInfo(*user, *control, false);
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130eb2 _ZN12advancealtar7Manager10finishLoadEP5CUser
// getAdvanceAltar() 判空 → 0；isAvailableTciket() 为假 → 1（原样返回）；
// 否则 setState(1) 并回包 put_header(0,0x1e)/put_int(0)/finalize(true) → Send；
// 恒返回 1。
// ----------------------------------------------------------------------------
bool Manager::finishLoad(CUser* user)  // ORIG T 0x08130eb2（返回值域仅 0/1，bool 化安全）
{
    StageControl* control = user->getAdvanceAltar();
    if (control != 0)
    {
        if (!user->m_advanceAltar.isAvailableTciket())
        {
            return 1;
        }

        control->getProcStage()->setState(ProcStage::State::S_N1);

        PacketGuard guard;
        guard.put_header(0, 0x1e);
        guard.put_int(0);
        guard.finalize(true);
        user->Send(guard);
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130fa8 _ZN12advancealtar7Manager10giveUpGameEP5CUser
// user/getAdvanceAltar() 判空直接返回；setState(6) +
// RegistTimerEPLP(getIndex(), END_N2) + dungeonClearInfo(user, control, false)。
// ----------------------------------------------------------------------------
void Manager::giveUpGame(CUser* user)
{
    if (user == 0)
        goto L_out;
    StageControl* control;
    control = user->getAdvanceAltar();
    if (control != 0)
    {
        control->getProcStage()->setState(ProcStage::State::S_N6);
        Timer_StageControl::RegistTimerEPLP(control->getIndex(),
                                            StageEndType::END_N2);
        HistoryLog::dungeonClearInfo(*user, *control, false);
    }
L_out:;
}


// ============================ OnlyOrig_Altar.AltarMgr 批（2026-08-27）=========================
// 新增符号（ORIG 报告全文转写；证据路径见各函数注记）：
//   Manager::{C1,C2,D1,D2,eplp,pause,dieMob,leaveUser}、isOnEvent、
//   CharacAdvanceAltarManager::{gmSetTicket,decTicket,addRewardData,
//     getClearStageSummary,getCurrentAdvanceAltarData,setUnlockBuyItem,
//     setDefaultBuyItem,buyItem}、
//   AdvanceAltarData::{getSlotMap,getBuyItemMap,set}。
// ============================================================================

// ---- ORIG T/W 0x08130902 / 0x08130908（docs/class_func_reports/advancealtar__Manager/
//      Manager.md、~Manager.md）：push ebp/mov esp,ebp/pop ebp/ret —— 全空体。----
Manager::Manager() {}
Manager::~Manager() {}

// ---- ORIG T 0x0813101e（advancealtar__Manager/eplp.md 全文）----
// [b28d-D33 N1 封顶] ORIG 0x08131041 多一条 `add $0x0,%eax`（4.1 对
// `type + 常量0` 未折叠）；4.4 前端恒折成单条 cmp（探针 /tmp/eplp_probe{,2}.cpp
// f1/f2/g1..g7 七形全折），N1「两步常量算术折叠为单步」判据命中，语义等价。
void Manager::eplp(CUser* user, EplpType::T type)
{
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return;
    }
    if (static_cast<unsigned int>(type) > static_cast<unsigned int>(EplpType::EPLP_N2))
    {
        return;
    }
    control->leaveUser();
}

// ---- ORIG T 0x0813105c（advancealtar__Manager/pause.md 全文）。ORIG 返回
//      undefined4（ProcStage::pause 结果）；协调口径 void，返回值丢弃。----
int Manager::pause(CUser* user, bool flag)
{
    if (user == 0)
    {
        return 0;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control != 0)   // ORIG control 分支 je 前跳尾部 body（4.4 !=0 包裹复现）
    {
        // ORIG 无 procStage 槽无 flag 规范化（P0）：getProcStage 结果经 (%esp)
        // 出参槽直传，flag 压参单条 movzbl（bool→bool 无 test/setne）。
        return control->getProcStage()->pause(flag);
    }
    return 0;
}

// ---- ORIG T 0x08130bb8（advancealtar__Manager/dieMob.md 全文 + 汇编逐块核对）----
// [b28d-D33 N93/P222 封顶] ORIG 0x08130bb8：203 vs 199 条；残差 = 尾部条件极性
// （ORIG je vs 4.4 jne）+ 局部槽 -0x30/-0x2c 装载序（N93 块级槽方向 + P222 块序）。
// p167 差集 CLEAR（41=41 calls），语义完成。
int Manager::dieMob(CUser* user, unsigned short mobSeq, unsigned short param,
                    const MSG_MONSTER_DIE& msg)
{
    if (user == 0)
    {
        return 0;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return 1;
    }
    // ORIG 8130c04..8130e5c：getProcStage 每用点重取（P48 去缓存，共 9 处）
    bool killed = control->getProcStage()->dieMob(mobSeq, param, msg);
    if (killed)
    {
        PacketGuard packet;
        packet.put_header(0, 0x26);
        packet.put_short(mobSeq);
        packet.put_byte(0);           // ×5 个 put_byte(0)（ORIG +0x66..+0x108）
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.finalize(true);
        user->Send(packet);
    }

    StageEndType::T endType = StageEndType::END_N1;
    if (!control->getProcStage()->isEndStage(endType))
    {
        return 1;
    }
    if (!control->getProcStage()->isRewardState())
    {
        return 1;
    }

    if (endType == StageEndType::END_N0)
    {   // ORIG local_18==0：全清死亡分支
        if (control->getProcStage()->getStageType() == 1)
        {
            control->getProcStage()->setState(ProcStage::State::S_N5);
            control->getProcStage()->clearStageAndReward();   // [跨片] clearStage 链定义归 AltarProc 批
            Timer_StageControl::RegistTimerClearReward(control->getIndex(), endType);
            // HistoryLog::dungeonClearInfo(true) 分支 ORIG 无（该段属 endType==3）
        }
        else
        {
            control->getProcStage()->setState(ProcStage::State::S_N6);
            Timer_StageControl::RegistTimerEPLP(control->getIndex(), endType);
            HistoryLog::dungeonClearInfo(*user, *control, false);
        }
    }
    else if (endType == StageEndType::END_N3)
    {   // ORIG local_18==3：全阶段完成
        if (control->getProcStage()->endProcAllKillMonster())
        {
            control->getProcStage()->setState(ProcStage::State::S_N5);
            Timer_StageControl::RegistTimerClearReward(control->getIndex(), endType);
            HistoryLog::dungeonClearInfo(*user, *control, true);
        }
        else
        {
            control->sendMapInfo();
        }
    }
    return 1;
}

// ---- ORIG T 0x081310b2（advancealtar__Manager/leaveUser.md 全文）----
void Manager::leaveUser(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    // [b28d-D33] P0：ORIG 0x081310b2 段 `call G_GameWorld; mov %eax,(%esp); call
    // out_from_dungeon` 无 -0xc 槽往返 ⇒ 原码无 world 具名局部（去缓存）。
    G_GameWorld()->out_from_dungeon(user);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);
}


// ---- ORIG T 0x0813375c（advancealtar__CharacAdvanceAltarManager/gmSetTicket.md）----
void CharacAdvanceAltarManager::gmSetTicket(int ticket)
{
    // ORIG 无槽无调用：this+0 成员逐次重读（P0/P183）。
    if (m_currentAdvanceAltarData == 0)
    {
        return;
    }
    m_currentAdvanceAltarData->m_ticket = ticket;  // ORIG *(cur+4)=param
    m_currentAdvanceAltarData->m_update = 1;       // ORIG *(cur+0xcc)=1
}

// ---- ORIG T 0x081313b6（decTicket.md）：先扣 m_ticket，不足扣 m_freeTicket；
// 任一成功置 update=1 并返 1（头口径 char）。----
char CharacAdvanceAltarManager::decTicket()
{
    // [b28d-D33 P183/P0] ORIG 0x081313b6 无 cur 具名槽：每次 `mov 0x8(%ebp),%eax;
    // mov (%eax),%eax` 重读成员并直接 test ⇒ 去缓存逐次重读。
    if (m_currentAdvanceAltarData == 0)
    {
        return 0;
    }
    bool dec = false;
    if (m_currentAdvanceAltarData->m_ticket >= 1)
    {
        --m_currentAdvanceAltarData->m_ticket;
        dec = true;
    }
    else if (m_currentAdvanceAltarData->m_freeTicket > 0)
    {
        --m_currentAdvanceAltarData->m_freeTicket;
        dec = true;
    }
    if (dec)
    {
        m_currentAdvanceAltarData->m_update = 1;
    }
    // [b28d-D33] ORIG 尾 `movzbl -0x1(%ebp),%eax; leave; ret` 直读 bool 槽 ⇒
    // 原码 `return dec;`（bool→char 隐式转换），非三目物化。
    return dec;
}

// ---- ORIG T 0x08132abe（addRewardData.md）：rewards map 查重后两端各插一份
//      （_Reward{field0=rewardId,byte4=0}），置 m_update=1；返回 1/0。----
// [b28d-D33 P220/P236 封顶] ORIG 0x08132abe：89=89 条；p167 差集 = eq(MISSING) vs
// ne(EXTRA) ⇒ ORIG 用 `it==end` 正极性；实参装载序 `lea map,%ecx; lea key,%edx`
// 与 4.4 相反（P236/P75）。语义等价。
int CharacAdvanceAltarManager::addRewardData(int rewardId, std::map<int, _Reward>& out)
{
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return 0;
    }
    if (cur->m_rewards.find(rewardId) != cur->m_rewards.end())
    {
        return 0;
    }
    _Reward entry;                    // ORIG _Reward ctor W memset 5
    entry.m_field0 = rewardId;
    entry.m_byte4 = 0;
    cur->m_rewards.insert(std::make_pair(rewardId, entry));
    cur->m_update = 1;
    out.insert(std::make_pair(rewardId, entry));
    return 1;
}

// ---- ORIG T 0x081318e2（getClearStageSummary.md）：按 _Stage.m_field0
//      （difficulty 计数）分组累计 out[0..2]。----
// [b28d-D33 P28/N86 封顶] ORIG 0x081318e2：79 vs 66 条；p167 差集 = ppEi(MISSING)
// vs ppEv(EXTRA) ⇒ 迭代器增量形 + N86 copy-elision 两段槽（4.1 临时槽+拷贝）。
void CharacAdvanceAltarManager::getClearStageSummary(int out[3]) const
{
    const AdvanceAltarData* data = m_currentAdvanceAltarData;
    if (data == 0)
    {
        return;
    }
    for (std::map<int, _Stage>::const_iterator it = data->m_stages.begin();
         it != data->m_stages.end(); ++it)
    {
        const int diff = it->second.m_field4;          // ORIG iter+8 → value.f4（pair 内 key+value.f0 之后）
        if (diff == 1)
        {
            ++out[1];
            ++out[0];
        }
        else if (diff == 2)
        {
            ++out[2];
            ++out[1];
            ++out[0];
        }
        else if (diff == 0)
        {
            ++out[0];
        }
    }
}





// ==================== CharacAdvanceAltarManager 补遗 ====================
// ---- ORIG T 0x08132d20（setUnlockBuyItem.md 全文）：遍历商店升级表匹配 stageIdx；
//      minLevel=getMinLevelFieldDataPoint(FT_LEVEL)(缺省回退 level 0→1)；价格点=
//      getFieldDataPoint(minLevel, FT_PRICE)；命中则构建 _BuyItem 入对应 shop 表，
//      需要占槽时置入空槽【BLOCKED 子调用：setUnlockItemToEmptySlot 依赖 _Slot
//      in-map 布局口径】；尾部统一置 update=1。----
// [b28d-D33 P28/N86 封顶] ORIG 0x08132d20：159 vs 163 条；p167 差集 = ppEi(MISSING)
// vs ppEv(EXTRA)（P28 增量侧）+ 局部槽序。语义等价。
void CharacAdvanceAltarManager::setUnlockBuyItem(AdvanceAltarShopType::T shopType,
                                                 int stageIdx,
                                                 std::vector<_BuyItem>* out)
{
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return;
    }
    std::vector<BuyUpgradeData>* upgrades =
        G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(cur->m_ridableId, shopType);
    std::map<int, _BuyItem>* items = cur->getBuyItemMap(shopType);
    if (upgrades == 0 || items == 0)
    {
        return;
    }
    for (std::vector<BuyUpgradeData>::iterator it = upgrades->begin();
         it != upgrades->end(); ++it)
    {
        if (it->m_field4 != stageIdx)
        {
            continue;
        }
        int minLevel = 0;
        FieldData* minPoint = it->getMinLevelFieldDataPoint(FieldType::FT_LEVEL, minLevel);
        FieldData* pricePoint = it->getFieldDataPoint(minLevel, FieldType::FT_PRICE);
        if (minPoint == 0 || pricePoint == 0)
        {
            continue;
        }
        _BuyItem entry;                    // ORIG _BuyItem ctor W memset 12
        entry.m_field0 = it->m_nItemId;
        entry.m_field4 = shopType;
        entry.m_short8 = static_cast<short>(minLevel);
        entry.m_short10 = static_cast<short>(pricePoint->m_value);
        items->insert(std::make_pair(entry.m_field0, entry));
        if (minLevel >= 1 && it->m_field8 != 0)   // ORIG 双条件后才占空槽
        {
            setUnlockItemToEmptySlot(shopType, entry.m_field0);
        }
        if (out != 0)
        {
            out->push_back(entry);
        }
    }
    cur->m_update = 1;
}

// ---- ORIG T 0x08132cb8（setDefaultBuyItem.md）：清对应 shop 表后全量重放。----
void CharacAdvanceAltarManager::setDefaultBuyItem(AdvanceAltarShopType::T type)
{
    if (m_currentAdvanceAltarData == 0)   // ORIG 内联 *(int*)this 判空，无 call W thunk
    {
        return;
    }
    std::map<int, _BuyItem>* items = m_currentAdvanceAltarData->getBuyItemMap(type);
    if (items == 0)
    {
        return;
    }
    items->clear();
    int stageIdx = 0;   // ORIG -0xc 槽：int 局部 = 0 传第 3 参（P0 有槽⇒有局部）
    setUnlockBuyItem(type, stageIdx, 0);
}

// ---- ORIG T 0x08132d20 后续：本函数自上一提交块起沿用同一文件命名空间；
// 以下为第二收口批次（slot 族全真转写）。迭代器偏移按协调者裁定采用
// pair<const u8,_Slot>* 视图：it+0=键(槽号)、it+1=&second(m_byte0)、
// it+2=&m_int1、it+6=&m_byte5、it+7=&m_int6 —— 与现头 sizeof(_Slot)==0xa 自洽。----

// ---- ORIG T 0x08131b58（setSlot.md 全文）----
// [b28d-D33 N93 封顶] ORIG 0x08131b58：253 vs 274 条；残差 = 局部槽 -0x20 vs -0x24
// 起的整组槽位平移（N93/N98 块级槽分配，已全头部声明不可翻）。p167 差集 CLEAR
// （13=13），语义等价。
int CharacAdvanceAltarManager::setSlot(bool isSet, const CMDPacketStruct::_SetSlotData& data)
{
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return 2;
    }
    AdvanceAltarShopType::T shopType;
    if (data.m_type == 1)
    {
        shopType = AdvanceAltarShopType::T_0;
    }
    else if (data.m_type == 2)
    {
        shopType = AdvanceAltarShopType::T_1;
    }
    else
    {
        return 2;
    }
    if (static_cast<unsigned char>(data.m_field4) > 5)
    {
        return 2;
    }

    std::vector<BuyUpgradeData>* upgrades =
        G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(cur->m_ridableId, shopType);
    if (upgrades == 0)
    {
        return 3;
    }
    const BuyUpgradeData* chosen = 0;
    for (std::vector<BuyUpgradeData>::const_iterator it = upgrades->begin();
         it != upgrades->end(); ++it)
    {
        if (it->m_nItemId == data.m_field8)
        {
            chosen = &(*it);
            break;
        }
    }
    if (chosen == 0)
    {
        return 3;
    }
    if (chosen->m_field8 == 0)          // “is settable slot”未开放
    {
        return 3;
    }
    if (!isSet)
    {
        return resetSlot(*cur, data);   // ORIG 尾 jmp 进入 resetSlot 共用出口
    }

    std::map<int, _BuyItem>* items = cur->getBuyItemMap(shopType);
    if (items == 0)
    {
        return 3;
    }
    if (items->find(data.m_field8) != items->end())
    {
        return 3;                        // 已购买过
    }

    std::map<unsigned char, _Slot>* slots =
        cur->getSlotMap(static_cast<AdvanceAltarSlotType::T>(data.m_type));
    if (slots == 0)
    {
        return 3;
    }
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); ++it)
    {
        if (it->second.m_int6 == data.m_field8)
        {
            return 4;                    // 同一物品已在其它槽装备
        }
    }
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); ++it)
    {
        _Slot& slot = it->second;
        if (slot.m_byte5 == static_cast<unsigned char>(data.m_field4) &&
            slot.m_int1 == data.m_type &&
            slot.m_byte0 != 1)           // 未锁定
        {
            slot.m_int6 = data.m_field8;              // ORIG it+7 ← data+8
            cur->m_update = 1;
            return 0;
        }
    }
    return 2;
}

// ---- ORIG T 0x08131f0a（exchangeSlot.md 全文）----
// [b28d-D33 N7/N109 + 帧量化] ORIG 0x08131f0a：219 vs 243 条；ORIG 帧 sub $0x48
// （16k+8）vs 4.4 push %ebx + sub $0x54（callee-saved 需求不同，N7/N109）。
// p167 差集 CLEAR（10=10），语义等价。
unsigned int CharacAdvanceAltarManager::exchangeSlot(CMDPacketStruct::_SetSlotData& slotA,
                                            CMDPacketStruct::_SetSlotData& slotB)
{
    if (slotA.m_type != slotB.m_type)
    {
        return 2;
    }
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return 3;
    }
    std::map<unsigned char, _Slot>* slots =
        cur->getSlotMap(static_cast<AdvanceAltarSlotType::T>(slotA.m_type));
    if (slots == 0)
    {
        return 4;
    }

    std::map<unsigned char, _Slot>::iterator foundA = slots->end();
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); ++it)
    {
        const _Slot& slot = it->second;
        if (slot.m_byte5 == static_cast<unsigned char>(slotA.m_field4) &&
            slot.m_int1 == slotA.m_type &&
            slot.m_int6 == slotA.m_field8)
        {
            foundA = it;
            break;
        }
    }
    std::map<unsigned char, _Slot>::iterator foundB = slots->end();
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); ++it)
    {
        const _Slot& slot = it->second;
        if (slot.m_byte5 == static_cast<unsigned char>(slotB.m_field4) &&
            slot.m_int1 == slotB.m_type &&
            slot.m_int6 == slotB.m_field8)
        {
            foundB = it;
            break;
        }
    }
    if (foundA == slots->end() || foundB == slots->end())
    {
        return 3;
    }
    if (foundA->second.m_byte0 != 0 || foundB->second.m_byte0 != 0)
    {
        return 3;                         // 任一处于锁定态不可交换
    }

    const int tmp = foundA->second.m_int6;
    foundA->second.m_int6 = foundB->second.m_int6;
    foundB->second.m_int6 = tmp;
    cur->m_update = 1;
    return 0;
}

// ---- ORIG T 0x08132bfa（resetSlot.md 全文）：按槽号查找；找不到或锁定返 2；
// 命中清空槽数据并置 update=1 返 0。----
int CharacAdvanceAltarManager::resetSlot(advancealtar::AdvanceAltarData& cur,
                                         const CMDPacketStruct::_SetSlotData& data)
{
    std::map<unsigned char, _Slot>* slots =
        cur.getSlotMap(static_cast<AdvanceAltarSlotType::T>(data.m_type));
    if (slots == 0)
    {
        return 3;
    }
    std::map<unsigned char, _Slot>::iterator it = slots->find(data.m_field4);
    if (it == slots->end())
    {
        return 2;
    }
    // [b28d-D33] ORIG 0x08132c39 `xor $1,%eax; test %al,%al; je` ⇒ 裸 !field 形
    // （P168a 档②；值域 {0,1}，与 !=1 语义等价）。
    if (!it->second.m_byte0)   // P222 反向包裹：ORIG body-fall、mov$2 尾块
    {
        it->second.m_int6 = 0;
        cur.m_update = 1;
        return 0;
    }
    return 2;                          // 锁定槽
}

// ---- ORIG T 0x08132234（setUnlockItemToEmptySlot.md 全文）----
// [b28d-D33 P209/P28 封顶] ORIG 0x08132234：71 vs 82 条；残差 = ①枚举形参比较
// ORIG `mov 0xc(%ebp),%eax; test`（寄存器化）vs 4.4 mem 直比（P209 反例）；
// ②p167 差集 ppEi/ptEv MISSING vs ppEv/ne EXTRA（P28 + P167 新判据）。语义等价。
void CharacAdvanceAltarManager::setUnlockItemToEmptySlot(AdvanceAltarShopType::T shopType,
                                                         int itemIdx)
{
    AdvanceAltarSlotType::T slotType;
    if (shopType == AdvanceAltarShopType::T_0)
    {
        slotType = AdvanceAltarSlotType::SLOT_N1;
    }
    else if (shopType == AdvanceAltarShopType::T_1)
    {
        slotType = AdvanceAltarSlotType::SLOT_N2;
    }
    else
    {
        return;
    }
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return;
    }
    std::map<unsigned char, _Slot>* slots = cur->getSlotMap(slotType);
    if (slots == 0)
    {
        return;
    }
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); ++it)
    {
        _Slot& slot = it->second;
        if (slot.m_byte0 == 1)             // ORIG 加锁位/已占用任一不满足即跳过
        {
            continue;
        }
        if (slot.m_int6 != 0)
        {
            continue;
        }
        slot.m_int6 = itemIdx;
        cur->m_update = 1;
        return;
    }
}

// ---- ORIG W 0x08132f8e（setDefaultSlot.md 全文）：unit/skill 两张槽表各扫 0..5；
// 命中则清空槽数据；缺失则以 kUnitSlotNoLock/kSkillSlotNoLock[k] 为模板补插，
// 类型标记分别为 SLOT_N1/SLOT_N2，槽内槽号=k 写入 m_byte5。----
// [b28d-D33 P183/P220 封顶] ORIG 0x08132f8e：161 vs 156 条；ORIG
// `mov (%eax),%eax; test %eax,%eax` 直判（无 cur 槽，P183）vs 4.4 -0x1c 槽 +
// p167 差集 eq/make_pair 左值性（P61/P220）。语义等价。
void CharacAdvanceAltarManager::setDefaultSlot()
{
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return;
    }
    static const unsigned char* kUnit = kUnitSlotNoLock;    // ORIG .rodata 0x08af2e58（6×00，实测）
    static const unsigned char* kSkill = kSkillSlotNoLock;  // ORIG .rodata 0x08af2e5e（实测 00 00 00 00 01 01）

    for (int k = 0; k <= 5; ++k)
    {
        std::map<unsigned char, _Slot>& slots = cur->m_slots1;
        std::map<unsigned char, _Slot>::iterator it = slots.find(
            static_cast<unsigned char>(k));
        if (it != slots.end())
        {
            it->second.m_int6 = 0;
            continue;
        }
        _Slot slot;                                  // ORIG _Slot ctor memset
        slot.m_byte0 = kUnit[k];
        slot.m_int1 = static_cast<SlotInt1_t>(1);    // 槽类型标记（SLOT_N1）
        slot.m_byte5 = static_cast<unsigned char>(k);
        slot.m_int6 = 0;
        slots.insert(std::make_pair(static_cast<unsigned char>(k), slot));
    }
    for (int k = 0; k <= 5; ++k)
    {
        std::map<unsigned char, _Slot>& slots = cur->m_slots2;
        std::map<unsigned char, _Slot>::iterator it = slots.find(
            static_cast<unsigned char>(k));
        if (it != slots.end())
        {
            it->second.m_int6 = 0;
            continue;
        }
        _Slot slot;
        slot.m_byte0 = kSkill[k];
        slot.m_int1 = static_cast<SlotInt1_t>(2);    // 槽类型标记（SLOT_N2）
        slot.m_byte5 = static_cast<unsigned char>(k);
        slot.m_int6 = 0;
        slots.insert(std::make_pair(static_cast<unsigned char>(k), slot));
    }
}

// ---- ORIG T 0x081333b2（resetStar.md 全文）：非 0 类型→返 2；0 型扣重置费
// （费用表按 m_resetCount 查，miss 回退 getMaxStarResetGoldFee）→ 不足返 3 →
// use_money(fee,0x32,1) 失败返 3 → SendUpdateItemList(1,0,0) → ++m_resetCount
// → setDefaultSlot + 三商店 default → 遍历 stages（m_field4!=-1）三商店重放
// unlock → 星池差值入账 + HistoryLog::starAdd。----
// [b28d-D33 N82/帧量化] ORIG 0x081333b2：220 vs 234 条；帧 sub $0x48 vs $0x58
// （N82 帧区差）+ 局部槽组平移。语义等价。
int CharacAdvanceAltarManager::resetStar(CUser* user, CMDPacketStruct::ResetStarType::T type)
{
    if (user == 0 || m_currentAdvanceAltarData == 0)
    {
        return 2;
    }
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (type != static_cast<CMDPacketStruct::ResetStarType::T>(0))
    {
        return 2;
    }

    int fee = 0;
    std::map<int, _StarRestFeeByGold>::iterator feeIt =
        G_CDataManager()->m_altarShop.m_starRestMap.find(
            static_cast<int>(cur->m_resetCount));
    if (feeIt != G_CDataManager()->m_altarShop.m_starRestMap.end())
    {
        fee = feeIt->second.m_field0;                       // pair 视图 *(feeIter+4)=second.m_field0
    }
    else
    {
        fee = G_CDataManager()->m_altarShop.getMaxStarResetGoldFee();
    }

    const CInventory* invenR = user->getCurCharacInvenR();
    if (invenR->get_money() < fee)
    {
        return 3;
    }
    CInventory* invenW = user->getCurCharacInvenW();
    if (!invenW->use_money(fee, static_cast<eMoneySubReason>(0x32), true))
    {
        return 3;
    }
    user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                             static_cast<ENUM_ITEMSPACE>(0), 0);

    ++cur->m_resetCount;                                    // ORIG short 自增
    setDefaultSlot();
    setDefaultBuyItem(AdvanceAltarShopType::T_0);
    setDefaultBuyItem(AdvanceAltarShopType::T_1);
    setDefaultBuyItem(AdvanceAltarShopType::T_2);

    for (std::map<int, _Stage>::const_iterator it = cur->m_stages.begin();
         it != cur->m_stages.end(); ++it)
    {
        if (it->second.m_field4 == -1)
        {
            continue;
        }
        setUnlockBuyItem(AdvanceAltarShopType::T_0, it->second.m_field0, 0);
        setUnlockBuyItem(AdvanceAltarShopType::T_1, it->second.m_field0, 0);
        setUnlockBuyItem(AdvanceAltarShopType::T_2, it->second.m_field0, 0);
    }

    const int delta = (cur->m_curStar + cur->m_starPoolBase) - cur->m_starPoint;
    cur->m_starPoint = cur->m_curStar + cur->m_starPoolBase;
    cur->m_update = 1;

    advancealtar::AddStarType::T logType = advancealtar::AddStarType::ADD_BY_RESET;
    if (delta > 0)
    {
        HistoryLog::starAdd(*user, logType, 0, delta);
    }
    return 0;
}

// ---- ORIG T 0x08133720（gmSetStar.md）：当前星=v、星池点=基数+v、update=1。----
void CharacAdvanceAltarManager::gmSetStar(int star)
{
    // ORIG 无槽无调用：this+0 成员逐次重读（P0/P183）。
    if (m_currentAdvanceAltarData == 0)
    {
        return;
    }
    m_currentAdvanceAltarData->m_curStar = star;
    m_currentAdvanceAltarData->m_starPoint =
        m_currentAdvanceAltarData->m_starPoolBase + star;
    m_currentAdvanceAltarData->m_update = 1;
}

// ---- ORIG T 0x08131426（checkAndCreateDefaultRidable.md 全文）----
// [b28d-D33 P197④/N82 封顶] ORIG 0x08131426：92 vs 101 条；帧 sub $0x28 vs $0x38
// （N82 帧/槽区差）+ p167 差集 = size(MISSING x3) vs empty(EXTRA x3) ⇒ 源形须
// `map.size()==0` 而非 `map.empty()`（P197④）。语义等价。
void CharacAdvanceAltarManager::checkAndCreateDefaultRidable()
{
    bool skipInit = false;
    AdvanceAltarData* target = 0;
    for (int i = 0; i <= 0; ++i)
    {
        AdvanceAltarData* data =
            reinterpret_cast<AdvanceAltarData*>(reinterpret_cast<char*>(this) + 4 + i * 0xd0);
        if (data->m_ridableId != RidableId::RIDEABLE)
        {
            continue;
        }
        const bool allEmpty = data->m_buyItems1.empty() &&
                              data->m_buyItems2.empty() &&
                              data->m_buyItems3.empty();
        if (allEmpty)
        {
            target = data;               // ORIG local_14 = this+i*0xd0+4
            break;
        }
        skipInit = true;                 // 已有内容不重置
    }
    if (skipInit)
    {
        return;
    }
    if (target == 0)
    {
        target = &m_data[0];            // ORIG 回退 this+4
    }
    target->m_update = 1;
    target->m_ridableId = RidableId::RIDEABLE;
    setCurrentAdvanceAltarData(RidableId::RIDEABLE);
    setDefaultSlot();
    setDefaultStage();
    setDefaultBuyItem(AdvanceAltarShopType::T_0);
    setDefaultBuyItem(AdvanceAltarShopType::T_1);
    setDefaultBuyItem(AdvanceAltarShopType::T_2);
    target->m_ticket = G_CDataManager()->m_altarConfig.m_0;   // ORIG +0x6364 配置票数
}

// ---- ORIG T 0x081319ae（buyItem.md 全文）----
// [b28d-D33 P220/P75 封顶] ORIG 0x081319ae：130 vs 125 条；p167 差集 = eq(MISSING)
// vs ne/ptEv(EXTRA) + 实参/槽装载序（P75 跨版本调度）。语义等价。
int CharacAdvanceAltarManager::buyItem(AdvanceAltarShopType::T shopType, int level,
                                       short fromSlot, short toSlot, int totalCost,
                                       int& outReward)
{
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return 3;
    }
    if (cur->m_starPoint < totalCost)
    {
        return 2;
    }
    std::map<int, _BuyItem>* items = cur->getBuyItemMap(shopType);
    if (items == 0)
    {
        return 3;
    }

    std::map<int, _BuyItem>::iterator it = items->find(level);
    if (it != items->end())
    {
        it->second.m_short8 = toSlot;                          // ORIG +0xc ← param_5 截断
        it->second.m_short10 =
            static_cast<short>(it->second.m_short10 + totalCost);   // +0xe 累加
    }
    else
    {
        _BuyItem entry;                  // ORIG _BuyItem ctor W memset 12
        entry.m_field0 = shopType;       // 序按 asm：param_2→f0 / param_3→f4 /
        entry.m_field4 = level;          //   toSlot→short8 / totalCost→shortA；
        entry.m_short8 = toSlot;         //   map 键=level（make_pair int& &param_3）
        entry.m_short10 = static_cast<short>(totalCost);
        items->insert(std::make_pair(entry.m_field4, entry));
    }

    cur->m_starPoint -= totalCost;                              // ORIG +0x14 扣减
    cur->m_update = 1;
    outReward = cur->m_starPoint;
    HistoryLog::buyItem(shopType, level, fromSlot, toSlot, totalCost);
    return 0;
}

// ---- ORIG T 0x0813234e（sendAchievementReward.md 全文）：奖励领奖入口。----
// [b28d-D33 N82/帧量化] ORIG 0x0813234e：210 vs 222 条；帧 sub $0x50 vs $0x60
// （N82 帧区差）+ 局部槽组平移。语义等价。
int CharacAdvanceAltarManager::sendAchievementReward(
    CUser* user, int achievementId,
    const std::vector<_RewardItem>** outItems)
{
    if (user == 0 || m_currentAdvanceAltarData == 0)
    {
        return 2;
    }
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    std::map<int, _Reward>& rewards = cur->m_rewards;
    std::map<int, _Reward>::iterator it = rewards.find(achievementId);
    if (it == rewards.end())
    {
        return 2;
    }
    if (it->second.m_byte4 != 0)         // 已领取标记（_Reward+4）
    {
        return 3;
    }
    AchievementRewardData* reward =
        G_CDataManager()->m_altarReward.getAchievementRewardData(achievementId);
    if (reward == 0)
    {
        return 3;
    }

    std::vector<_RewardItem> normalItems;      // ORIG local_34/local_40 双缓冲分流
    std::vector<_RewardItem> eventItems;
    for (std::vector<_RewardItem>::const_iterator rit = reward->m_rewardItems.begin();
         rit != reward->m_rewardItems.end(); ++rit)
    {
        const bool onEvent = advancealtar::isOnEvent();
        if (onEvent && rit->m_type != 0)
        {
            eventItems.push_back(*rit);
        }
        else
        {
            normalItems.push_back(*rit);
        }
    }
    const char* postalTitle = reward->m_postalTitle.c_str();     // +0x20
    const char* postalLetter = reward->m_postalLetter.c_str();   // +0x24
    if (!normalItems.empty())
    {
        ProcStage::sendMailRewardItem(user, normalItems, postalTitle, postalLetter);
    }
    if (!eventItems.empty())
    {
        ProcStage::sendMailRewardItem(user, eventItems,
                                      "chn_game_server_msg_86", postalLetter);
    }
    *outItems = &reward->m_rewardItems;
    it->second.m_byte4 = 1;                                     // 置已领取
    cur->m_update = 1;
    return 0;
}

// ==================== 终态收口批（OnlyOrig_Altar.AltarMgr，2026-08-27）====================
// ---- ORIG T 0x081316e8（getAdvanceAltarDataByIndex.md；任务文本 0x8133194 为
//      笔误，nm -C 权威地址 0x081316e8）：idx 负或超界→0，否则 this+4+i*0xd0。----
AdvanceAltarData* CharacAdvanceAltarManager::getAdvanceAltarDataByIndex(int idx)
{
    if (idx < 0 || idx > 0)
    {
        return 0;
    }
    // ORIG：mov 0xc(%ebp),%eax; imul $0xd0,%eax,%eax; add 0x8(%ebp),%eax; add $0x4,%eax
    //（getAdvanceAltarDataByIndex.md）。成员数组下标寻址（同 setCurrentAdvanceAltarData
    // 的 m_data[i] 自然寻址形）；char* 乘法+加 4 混排会触发 4.4 恒折 (i*0x34+1)<<2。
    return &m_data[idx];
}

// ---- ORIG T 0x08131254（setCurrentAdvanceAltarData.md 全文）----
bool CharacAdvanceAltarManager::setCurrentAdvanceAltarData(RidableId::T ridableId)
{
    if (!isValidRidableId(ridableId))
    {
        return false;
    }
    for (int i = 0; i <= 0; ++i)
    {
        if (m_data[i].m_ridableId == ridableId)   // 自然数组寻址：ORIG imul $0xd0 字节乘
        {
            m_currentAdvanceAltarData = &m_data[i];
            return true;
        }
    }
    m_currentAdvanceAltarData = 0;
    return false;
}

// ---- ORIG T 0x08132f72（setDefaultStage.md）：setDefaultStage ⇒ setNextStage(0)。----
void CharacAdvanceAltarManager::setDefaultStage()
{
    setNextStage(0);
}

// ---- ORIG T 0x08131616（CharacAdvanceAltarManager/set.md）：单实例分派，
//      db.m_db(+4) → m_data.set。----
void CharacAdvanceAltarManager::set(const _CharacAdvanceAltarDbData& db)
{
    for (int i = 0; i < 1; ++i)
    {
        m_data[i].set(db.m_db[i]);
    }
}

// ---- ORIG T 0x08132638（getSlotItem.md 全文）。报告内 `local_78[0] =
// *(_SlotItem*)(slotIterElem+6)` 的 pair 基址记法按 ProcStage.h _SlotItem
// 写入点口径还原：itemId=等级点 FT_INDEX 的 FieldData::m_value、needGage=
// CItem::getAdvanceAltarNeedGage；键=槽表 first（槽号）。局部 tmp _SlotItem
// 构造自然发射 ORIG W 0x081347e2 的 C1/C2 弱符号。----
// [b28d-D33 N86/P28/P167 封顶] ORIG 0x08132638：183 vs 193 条；p167 差集 =
// ne/ptEv/ppEi/make_pair 非 const(MISSING) vs eq/ppEv/make_pair const(EXTRA)
// （P28/P167/P61/N86 copy-elision 槽号连锁）。语义等价。
void CharacAdvanceAltarManager::getSlotItem(
    std::map<unsigned char, _SlotItem>& out) const
{
    const AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return;
    }
    const std::map<unsigned char, _Slot>& slots = cur->m_slots1;      // ORIG base+0x84
    for (std::map<unsigned char, _Slot>::const_iterator sit = slots.begin();
         sit != slots.end(); ++sit)
    {
        std::map<int, _BuyItem>::const_iterator bit =
            cur->m_buyItems1.find(sit->second.m_int6);                 // 槽内物品 id 查购买表
        if (bit == cur->m_buyItems1.end())
        {
            continue;
        }
        std::vector<BuyUpgradeData>* upgrades =
            G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(cur->m_ridableId,
                                                               AdvanceAltarShopType::T_0);
        if (upgrades == 0)
        {
            continue;
        }
        BuyUpgradeData* chosen = 0;
        for (std::vector<BuyUpgradeData>::iterator uit = upgrades->begin();
             uit != upgrades->end(); ++uit)
        {
            if (uit->m_field4 == bit->second.m_field4)
            {
                chosen = &(*uit);                     // ORIG 升级表匹配 *(buyIter+8)
                break;
            }
        }
        if (chosen == 0)
        {
            continue;
        }
        const int level = bit->second.m_short8;                        // ORIG +0xc 截断
        FieldData* point = chosen->getFieldDataPoint(level, FieldType::FT_INDEX);
        if (point == 0)
        {
            continue;
        }
        const CItem* item = G_CDataManager()->find_item(point->m_value);
        if (item == 0)
        {
            continue;
        }
        _SlotItem slotItem;                                            // ORIG ctor memset(0xc)
        slotItem.m_itemId = point->m_value;
        slotItem.m_needGage = item->getAdvanceAltarNeedGage();
        out.insert(std::make_pair(sit->first, slotItem));
    }
}

// ---- ORIG W 0x081348c4（objdump thunk：mov (%eax),%eax）——const 版定义，
//      头内唯一声明（非 const 版已删，避免调用方产出错误 mangled）。----
AdvanceAltarData* CharacAdvanceAltarManager::getCurrentAdvanceAltarData() const
{
    // ORIG W thunk mov (%eax),%eax；NK 成员但返回非 const 指针（调用方原地改写），
    // C++ 下需显式去除 const 视图（与 ORIG 语义一致）。
        return reinterpret_cast<AdvanceAltarData*>(m_field0);
}

// ---- ORIG 内部只读表 _ZN12advancealtarL11kRewardStarE（rodata 0xc，
//      nm 多副本同值）：按难度 0/1/2 → 通关奖励星 {3,2,1}。ORIG 每个使用 TU
//      各持一份 internal 副本，此处以 static 复刻（无 ODR 冲突）。----
static const int kRewardStar[3] = {3, 2, 1};

// ---- ORIG T 0x0813175a _ZN12advancealtar25CharacAdvanceAltarManager10clearStageEi
//      NS_15StageDifficulty1TENS_9StageType1TEiRiRbS6_（clearStage.md 全文；
//      2026-08-27 LfAltarStage 批）。----
bool CharacAdvanceAltarManager::clearStage(int stageIdx, StageDifficulty::T diff,
                                           StageType::T type, int clearRound,
                                           int& starDelta, bool& unlockBuy,
                                           bool& bOut)
{
    unlockBuy = false;   // ORIG 首指令 *param_7=0（其余引用路径不改写）
    AdvanceAltarData* cur = m_currentAdvanceAltarData;   // ORIG 0x813176a mov (%eax) 直读 this+0（无 getter call）
    if (cur == 0)
    {
        return false;
    }
    if (!G_CDataManager()->m_stageMapList.isExist(stageIdx))
    {
        return false;
    }
    std::map<int, _Stage>::iterator it = cur->m_stages.find(stageIdx);
    if (it == cur->m_stages.end())
    {
        return false;
    }
    if (type == StageType::STAGE_SURVIVAL)                  // 生存关：只刷新最高轮数
    {
        if (static_cast<int>(cur->m_maxClearRound) < clearRound)   // ORIG sext(u16) 比较
        {
            cur->m_maxClearRound = static_cast<short>(clearRound);
            bOut = true;
            cur->m_update = 1;
        }
        return true;
    }
    if (it->second.m_field4 == -1)     // 首次记录 → 解锁商店购买项
    {
        unlockBuy = true;
    }
    if (it->second.m_field4 < static_cast<int>(diff))    // 未达该难度 → 晋级发星
    {
        it->second.m_field4 = static_cast<StageField4_t>(
            StageDifficulty::getNextDifficulty(
                static_cast<StageDifficulty::T>(it->second.m_field4)));   // b28d：m_field4 已 enum 化
        starDelta = kRewardStar[diff];
        cur->m_curStar += starDelta;      // +0x0c
        cur->m_starPoint += starDelta;    // +0x14
        cur->m_update = 1;
    }
    return true;
}

// ---- ORIG T 0x081331d4 _ZN12advancealtar25CharacAdvanceAltarManager12setNextStageEi
//      （setNextStage.md 全文；2026-08-27 LfAltarStage 批）。stageIdx==0 走首幕
//      占位；否则按 StageMapList 链推进，下一幕未入 map 时写入占位记录并按链
//      尾标志递归。----
// [b28d-D33 P28/P61 封顶] ORIG 0x081331d4：130 vs 136 条（D26 已去 cur 缓存）；
// p167 差集 = 迭代器转换 C1ERKS/eq/make_pair 非 const(MISSING) vs ne/make_pair
// const(EXTRA)（P61/P81 左值性 + P220 极性）。语义等价。
void CharacAdvanceAltarManager::setNextStage(int stageIdx)
{
    // [b28d-26] ORIG 0x081331d4 每处 `mov 0x8(%ebp),%eax; mov (%eax),%eax`
    // 重读 this+0（P183 地址重物化，无 cur 具名局部）⇒ 直读成员。
    if (m_currentAdvanceAltarData == 0)
    {
        return;
    }
    if (stageIdx == 0)   // 开场：插入 (firstStageId, {firstStageId, -1}) 占位
    {
        StageMapList& stageMapList = G_CDataManager()->m_stageMapList;
        _Stage stage;
        stage.m_field0 = stageMapList.getFirstStageIndex();
        stage.m_field4 = static_cast<StageField4_t>(-1);   // b28d：enum 化（-1 哨兵）
        m_currentAdvanceAltarData->m_stages.insert(std::make_pair(stage.m_field0, stage));
        return;
    }
    bool chainFlags = false;
    const int nextStage =
        G_CDataManager()->m_stageMapList.getNextStage(stageIdx, chainFlags);
    if (nextStage == 0)
    {
        return;
    }
    std::map<int, _Stage>::iterator it = m_currentAdvanceAltarData->m_stages.find(nextStage);
    if (it != m_currentAdvanceAltarData->m_stages.end())
    {
        return;   // 已有该幕记录则不再推进
    }
    m_currentAdvanceAltarData->m_field20 = 1;   // ORIG *(char*)(cur+0x20)=1（语义【推断】）
    _Stage stage;
    stage.m_field0 = nextStage;
    stage.m_field4 = static_cast<StageField4_t>(-1);   // b28d：enum 化（-1 哨兵）
    m_currentAdvanceAltarData->m_stages.insert(std::make_pair(nextStage, stage));
    m_currentAdvanceAltarData->m_update = 1;
    if (chainFlags)
    {
        setNextStage(nextStage);
    }
}
}  // namespace advancealtar
// ============================================================================
// StageMapList::{isExist,getNextStage,getFirstStageIndex} —— LfAltarStage 批
// 定义归本 TU；声明在 CDataManager.h::StageMapList。
// ============================================================================

// ---- ORIG T 0x08364df6 _ZNK12StageMapList7isExistEi ----
bool StageMapList::isExist(int stageIdx) const
{
    // [b28d-D33] ORIG 0x08364e40：find 直构 -0x10（this）、end 临时 -0xc（rhs），
    // 调 operator==（eq）后 test/je/mov$0/mov$1 分支形（P56 + P47）。
    std::map<int, _StageMapSummary>::const_iterator it = m_map.find(stageIdx);
    if (it == m_map.end())
    {
        return false;
    }
    return true;
}

// ---- ORIG W 0x081347c0 _ZNK12StageMapList18getFirstStageIndexEv（inline thunk）----
int StageMapList::getFirstStageIndex() const
{
    return m_firstStageId;
}

// ---- ORIG T 0x08364e58 _ZNK12StageMapList12getNextStageEiRb ----
int StageMapList::getNextStage(int stageIdx, bool& outFlags) const
{
    std::map<int, _StageMapSummary>::const_iterator it = m_map.find(stageIdx);
    if (it == m_map.end())
    {
        return 0;
    }
    std::map<int, _StageMapSummary>::const_iterator next =
        m_map.find(it->second.m_nextId);
    if (next != m_map.end())
    {
        // [b28c-21] ORIG `movzbl 0x10(%eax),%edx; mov %dl,(%eax)` 无归一化；
        // 4.4 对 uchar→bool 赋值恒插 test/setne（跨版本 bool ABI，N 族）；
        // 字节视图形（引用/指针）实测均致 push %ebx + 帧 0x28→0x24，已回退。
        outFlags = next->second.m_flags;
    }
    return it->second.m_nextId;
}

