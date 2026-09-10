// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 advancealtar_Manager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// [b28d-solo-advancealtar_Manager] 单文件泳道收口：8 件 = 5 AE（setSlot
// 274=274、exchangeSlot 243=243、dieMob 199=199、sendAchievementReward 222=222、
// getSlotItem 193=193）+ 1 NEAR（setNextStage 136=136，残槽号 N93/N98）+
// 2 N 封顶（eplp N1 `add $0x0`；resetStar 234=234，槽序+尾块 N93/N98/N93附）。
// 真语义 bug 4 处：SAR 循环 !onEvent 分支、getSlotItem 匹配 m_nItemId、
// _SlotItem 布局错位（+0 byte/+4 itemId/+8 needGage）、resetStar logType 分支。
// .h 2 处（advancealtar.h _Slot 匿名 union；advancealtar_ProcStage.h _SlotItem）
// 均已 R2 广播 + 消费 TU（Dispatchers/advancealtar_ProcStage/_DB/_misc）复验。
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
// [b28d-solo-advancealtar_Manager] 元素类型 bool：ORIG setDefaultSlot 对
// m_byte0(bool) 直接 `mov %al` 无 test/setne 归一化（P300 源同型直存）。
static const bool kUnitSlotNoLock[6] = { false, false, false, false, false, false };   // 0x08af2e58
static const bool kSkillSlotNoLock[6] = { false, false, false, false, true, true };  // 0x08af2e5e（实测）

// ----------------------------------------------------------------------------
// ORIG T 0x0813090e _ZN12advancealtar7Manager11selectStageEP5CUseriNS_15StageDifficulty1TE
// user 判空 → 3；难度越界(<0||>2) → 8；getAdvanceAltar() 判空 → cMyTrace +
// "StaticPool Defence DungeonNULL" → 3；
//   StageControl::selectStage != 0 → control->destroy() 后透传错误码；
//   == 0 → setAdvanceAltarIndex(control->getIndex())（ORIG 经 CUser::setAdvanceAltarIndex
//   W 0x0813490e 写 m_advanceAltarIdx，此处字段化等价写）→ 返回 0。
// ----------------------------------------------------------------------------
#include "advancealtar_Manager/selectStage.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x081309f0 _ZN12advancealtar7Manager20procErrorSelectStageEP5CUser10ENUM_ERROR
// user 判空或 err==0 直接返回；否则 PacketGuard: put_header(1,0x10) /
// put_byte(0) / put_int(err) / finalize(true) → CUser::Send。
// ----------------------------------------------------------------------------
#include "advancealtar_Manager/procErrorSelectStage.inc"
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
#include "advancealtar_Manager/dieCharac.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130eb2 _ZN12advancealtar7Manager10finishLoadEP5CUser
// getAdvanceAltar() 判空 → 0；isAvailableTciket() 为假 → 1（原样返回）；
// 否则 setState(1) 并回包 put_header(0,0x1e)/put_int(0)/finalize(true) → Send；
// 恒返回 1。
// ----------------------------------------------------------------------------
#include "advancealtar_Manager/finishLoad.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130fa8 _ZN12advancealtar7Manager10giveUpGameEP5CUser
// user/getAdvanceAltar() 判空直接返回；setState(6) +
// RegistTimerEPLP(getIndex(), END_N2) + dungeonClearInfo(user, control, false)。
// ----------------------------------------------------------------------------
#include "advancealtar_Manager/giveUpGame.inc"
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
#include "advancealtar_Manager/Manager.inc"
#include "advancealtar_Manager/dtor_Manager.inc"
#include "advancealtar_Manager/eplp.inc"
}

// ---- ORIG T 0x0813105c（advancealtar__Manager/pause.md 全文）。ORIG 返回
//      undefined4（ProcStage::pause 结果）；协调口径 void，返回值丢弃。----
#include "advancealtar_Manager/pause.inc"
}

// ---- ORIG T 0x08130bb8（advancealtar__Manager/dieMob.md 全文 + 汇编逐块核对）----
// [b28d-solo-advancealtar_Manager AE] 本次修：control 守卫正向包裹（ORIG +0x38
// `je <共享尾 return 1>`）+ endType 去初值（ORIG 无 `movl $1,-0x14` 写点）+
// 具名 bool isEnd（ORIG +0x174 `mov %al,-0x9; cmpb $0,-0x9`）+ endType 派发改
// switch（ORIG 决策树 `test;je END_N0` + N3 直落；P46 叶块序 N3 在前）⇒
// 199=199 逐条全等。p167 差集 CLEAR。
#include "advancealtar_Manager/dieMob.inc"
}

// ---- ORIG T 0x081310b2（advancealtar__Manager/leaveUser.md 全文）----
#include "advancealtar_Manager/leaveUser.inc"
}


// ---- ORIG T 0x0813375c（advancealtar__CharacAdvanceAltarManager/gmSetTicket.md）----
#include "advancealtar_Manager/gmSetTicket.inc"
}

// ---- ORIG T 0x081313b6（decTicket.md）：先扣 m_ticket，不足扣 m_freeTicket；
// 任一成功置 update=1 并返 1（头口径 char）。----
#include "advancealtar_Manager/decTicket.inc"
}

// ---- ORIG T 0x08132abe（addRewardData.md）：rewards map 查重后两端各插一份
//      （_Reward{field0=rewardId,byte4=0}），置 m_update=1；返回 1/0。----
// [b28d-solo-advancealtar_Manager AE] 本次修：正极性 `if (find==end){插入;return 1}`
// （P220/P293）+ 具名 iterator 恢复 find-先/end-后求值序 ⇒ 89=89 逐条全等。
#include "advancealtar_Manager/addRewardData.inc"
}

// ---- ORIG T 0x081318e2（getClearStageSummary.md）：按 _Stage.m_field0
//      （difficulty 计数）分组累计 out[0..2]。----
// [b28d-solo-advancealtar_Manager AE] 本次修：switch(cast int) + fall-through 共享
// 叶块（P46/P251，case 2→1→0）+ it++ 后置（P28 ppEi）⇒ 66=66 逐条全等。
#include "advancealtar_Manager/getClearStageSummary.inc"
}





// ==================== CharacAdvanceAltarManager 补遗 ====================
// ---- ORIG T 0x08132d20（setUnlockBuyItem.md 全文）：遍历商店升级表匹配 stageIdx；
//      minLevel=getMinLevelFieldDataPoint(FT_LEVEL)(缺省回退 level 0→1)；价格点=
//      getFieldDataPoint(minLevel, FT_PRICE)；命中则构建 _BuyItem 入对应 shop 表，
//      需要占槽时置入空槽【BLOCKED 子调用：setUnlockItemToEmptySlot 依赖 _Slot
//      in-map 布局口径】；尾部统一置 update=1。----
// [b28d-solo-advancealtar_Manager AE] 本次修：两段式指针局部（P197③）+ upgrades/items
// 独立守卫（P133）+ it++ + entry 序 field4→field0 + make_pair(entry.m_field4)；
// 并修真语义 bug：getMinLevelFieldDataPoint 首参 ORIG 立即数 1=FT_INDEX（原写
// FT_LEVEL=0 读错字段列）⇒ 163=163 逐条全等。
#include "advancealtar_Manager/setUnlockBuyItem.inc"
}

// ---- ORIG T 0x08132cb8（setDefaultBuyItem.md）：清对应 shop 表后全量重放。----
#include "advancealtar_Manager/setDefaultBuyItem.inc"
}

// ---- ORIG T 0x08132d20 后续：本函数自上一提交块起沿用同一文件命名空间；
// 以下为第二收口批次（slot 族全真转写）。迭代器偏移按协调者裁定采用
// pair<const u8,_Slot>* 视图：it+0=键(槽号)、it+1=&second(m_byte0)、
// it+2=&m_int1、it+6=&m_byte5、it+7=&m_int6 —— 与现头 sizeof(_Slot)==0xa 自洽。----

// ---- ORIG T 0x08131b58（setSlot.md 全文）----
// [b28d-solo-advancealtar_Manager AE] 本次修：shopType 派发改 switch（ORIG 平铺
// `cmp $1; je; cmp $2; je; jmp` 决策链 + 各 case 内联字段检查）+ upgrades 循环
// const_iterator→iterator（ORIG IPN 实例）+ 每用点重取 operator->（P48）+
// items 侧 const map 视图 + 具名 find 迭代器（ORIG find-先/end-后 + const eq）+
// 两遍 slots 扫描共用同一 it（ORIG 同槽 -0x44，第二遍 `it = begin()` 经临时）⇒
// 274=274 逐条全等。p167：items 处 ORIG 用 const_iterator::eq 而我方 `!=` 发 ne
// （语义等价模板实例差；`!(==)` 形实测 275 vs 274 劣化，保 AE）。
#include "advancealtar_Manager/setSlot.inc"
}

// ---- ORIG T 0x08131f0a（exchangeSlot.md 全文）----
// [b28d-solo-advancealtar_Manager AE] 本次修：两遍扫描改「begin 直构 + break」
// （ORIG 无 foundX=it 记录）+ 每用点重取 operator->（P48）+ m_int1i int 视角
// （P235 匿名 union，消 enum→int 两段中转）+ 交换写两侧 m_field8（ORIG +0x2f1/
// +0x305 直读实参）+ 冗余 `!=end && !=end` 守卫照抄（P171）⇒ 243=243 逐条全等。
#include "advancealtar_Manager/exchangeSlot.inc"
}

// ---- ORIG T 0x08132bfa（resetSlot.md 全文）：按槽号查找；找不到或锁定返 2；
// 命中清空槽数据并置 update=1 返 0。----
#include "advancealtar_Manager/resetSlot.inc"
}

// ---- ORIG T 0x08132234（setUnlockItemToEmptySlot.md 全文）----
// [b28d-solo-advancealtar_Manager AE] 本次修：switch(slotType 具名槽) + 合并短路
// `m_byte0!=1 && m_int6==0`（P133/P258）+ 去 _Slot& 引用缓存（P48）+ it++（ppEi）
// ⇒ 82=82 逐条全等。
#include "advancealtar_Manager/setUnlockItemToEmptySlot.inc"
}

// ---- ORIG W 0x08132f8e（setDefaultSlot.md 全文）：unit/skill 两张槽表各扫 0..5；
// 命中则清空槽数据；缺失则以 kUnitSlotNoLock/kSkillSlotNoLock[k] 为模板补插，
// 类型标记分别为 SLOT_N1/SLOT_N2，槽内槽号=k 写入 m_byte5。----
// [b28d-solo-advancealtar_Manager AE] 本次修：while 形（P25/P51 setle 尾测）+
// m_currentAdvanceAltarData 每处重读（P183）+ `== end` 正极性 if/else + 键数组直接
// 下标（去 static 指针局部）+ 字段序 byte0/int1/int6/byte5 + make_pair(slot.m_byte5)；
// kUnit/kSkillSlotNoLock 元素 uchar→bool 使 m_byte0(bool) 直存（P300）⇒ 156=156 全等。
#include "advancealtar_Manager/setDefaultSlot.inc"
}

// ---- ORIG T 0x081333b2（resetStar.md 全文）：非 0 类型→返 2；0 型扣重置费
// （费用表按 m_resetCount 查，miss 回退 getMaxStarResetGoldFee）→ 不足返 3 →
// use_money(fee,0x32,1) 失败返 3 → SendUpdateItemList(1,0,0) → ++m_resetCount
// → setDefaultSlot + 三商店 default → 遍历 stages（m_field4!=-1）三商店重放
// unlock → 星池差值入账 + HistoryLog::starAdd。----
// [b28d-solo-advancealtar_Manager N 封顶] 本次修：type 守卫正向包裹（P222/P257）、
// 缓存 &m_starRestMap（ORIG `add $0x6334`=0x631c+0x18，非 &m_altarShop）、具名
// int key（ORIG -0x30）、`== end` 分支前置（ORIG `eq;test;je`）、去 invenR/invenW
// 具名指针（内联）、fee 声明移到 find 后（ORIG `movl $0,-0xc` 位置）、logType
// 反极性 if/else + unsigned 比较（ORIG `mov 0x10(%ebp),%eax; cmp $1,%eax; je`）、
// feeMap const 视图 + const_iterator（p167：ORIG const find/end/eq/ptEv）⇒
// 234=234 条 + p167 CLEAR。残差 = 块级槽逆序（fee/feeMap/logType/delta，
// N93/N98）+ return 2 块位置（ORIG 中间 +0x1c3 vs 我方函数尾，N93附 尾部双
// return 块序）⇒ DIFF 封顶，语义等价。
// [R22 2026-09-10 复测] 235=235，残差 13 行全为 feeMap/fee/logType 三变量槽位
// **完全倒置**（ORIG logType@-0x18,feeMap@-0x10,fee@-0xc vs 我方 feeMap@-0x18,
// fee@-0x10,logType@-0xc）；实测「裸声明前置（赋值留原位）」两形（logType 单独/
// logType+fee 同时）4.4 产物**零变化** ⇒ 4.4 按首次使用序分配、混合区声明序不可翻
// （P112/P331 限定），维持 N 封顶。
#include "advancealtar_Manager/resetStar.inc"
}

// ---- ORIG T 0x08133720（gmSetStar.md）：当前星=v、星池点=基数+v、update=1。----
#include "advancealtar_Manager/gmSetStar.inc"
}

// ---- ORIG T 0x08131426（checkAndCreateDefaultRidable.md 全文）----
// [b28d-solo-advancealtar_Manager AE] 本次修：while 形（P25/P51）+ m_data[i] 自然
// 寻址（去 char* 混排）+ `size()==0` 短路链（P197④，去 empty()）+ 去 data/allEmpty
// 具名局部 + setCurrentAdvanceAltarData(target->m_ridableId) 重读 ⇒ 101=101 全等。
#include "advancealtar_Manager/checkAndCreateDefaultRidable.inc"
}

// ---- ORIG T 0x081319ae（buyItem.md 全文）----
// [b28d-solo-advancealtar_Manager AE] 本次修：正极性 `if (it==end){插入}else{更新}`
// （P220）+ make_pair(level, entry) 首参取形参地址 + 更新分支 `m_short10 +=`（单次
// operator-> 读写同址，P38）⇒ 125=125 逐条全等。
#include "advancealtar_Manager/buyItem.inc"
}

// ---- ORIG T 0x0813234e（sendAchievementReward.md 全文）：奖励领奖入口。----
// [b28d-solo-advancealtar_Manager AE] 本次修：items 引用缓存先于双 vector 声明
// （ORIG +0xcb）+ dead int 局部（ORIG +0x0ea `movl $0,-0xc` 后无读点，P171/P232
// 撑帧）+ 循环体嵌套 if 语义修正（见下）+ 内层 `== 0` 反极性（ORIG sete）+
// rit++（ppEi）⇒ 222=222 逐条全等。真语义 bug：原 `onEvent && type!=0 ? event :
// normal` 在 `!onEvent && type!=0` 误推 normalItems，ORIG 该分支无 push。
#include "advancealtar_Manager/sendAchievementReward.inc"
}

// ==================== 终态收口批（OnlyOrig_Altar.AltarMgr，2026-08-27）====================
// ---- ORIG T 0x081316e8（getAdvanceAltarDataByIndex.md；任务文本 0x8133194 为
//      笔误，nm -C 权威地址 0x081316e8）：idx 负或超界→0，否则 this+4+i*0xd0。----
#include "advancealtar_Manager/getAdvanceAltarDataByIndex.inc"
}

// ---- ORIG T 0x08131254（setCurrentAdvanceAltarData.md 全文）----
#include "advancealtar_Manager/setCurrentAdvanceAltarData.inc"
}

// ---- ORIG T 0x08132f72（setDefaultStage.md）：setDefaultStage ⇒ setNextStage(0)。----
#include "advancealtar_Manager/setDefaultStage.inc"
}

// ---- ORIG T 0x08131616（CharacAdvanceAltarManager/set.md）：单实例分派，
//      db.m_db(+4) → m_data.set。----
#include "advancealtar_Manager/set.inc"
}

// ---- ORIG T 0x08132638（getSlotItem.md 全文）。报告内 `local_78[0] =
// *(_SlotItem*)(slotIterElem+6)` 的 pair 基址记法按 ProcStage.h _SlotItem
// 写入点口径还原：itemId=等级点 FT_INDEX 的 FieldData::m_value、needGage=
// CItem::getAdvanceAltarNeedGage；键=槽表 first（槽号）。局部 tmp _SlotItem
// 构造自然发射 ORIG W 0x081347e2 的 C1/C2 弱符号。----
// [b28d-solo-advancealtar_Manager AE] 本次修：_SlotItem 构造提前到 find 检查后
// （ORIG +0x0a0）+ `slotItem.m_byte0 = sit->second.m_byte5`（ORIG +0x0ab movzbl
// 0x6）+ make_pair(slotItem.m_byte0, slotItem)（ORIG 两实参同址 &slotItem）+
// sit++/uit++（ppEi）+ 首个守卫正向包裹（P222/P257）⇒ 193=193 逐条全等。
// 真语义 bug：升级表匹配 ORIG 为 `uit->m_nItemId(+0x00)`，原实现误用 m_field4。
#include "advancealtar_Manager/getSlotItem.inc"
}

// ---- ORIG W 0x081348c4（objdump thunk：mov (%eax),%eax）——const 版定义，
//      头内唯一声明（非 const 版已删，避免调用方产出错误 mangled）。----
#include "advancealtar_Manager/getCurrentAdvanceAltarData.inc"
}

// ---- ORIG 内部只读表 _ZN12advancealtarL11kRewardStarE（rodata 0xc，
//      nm 多副本同值）：按难度 0/1/2 → 通关奖励星 {3,2,1}。ORIG 每个使用 TU
//      各持一份 internal 副本，此处以 static 复刻（无 ODR 冲突）。----
static const int kRewardStar[3] = {3, 2, 1};

// ---- ORIG T 0x0813175a _ZN12advancealtar25CharacAdvanceAltarManager10clearStageEi
//      NS_15StageDifficulty1TENS_9StageType1TEiRiRbS6_（clearStage.md 全文；
//      2026-08-27 LfAltarStage 批）。----
#include "advancealtar_Manager/clearStage.inc"
}

// ---- ORIG T 0x081331d4 _ZN12advancealtar25CharacAdvanceAltarManager12setNextStageEi
//      （setNextStage.md 全文；2026-08-27 LfAltarStage 批）。stageIdx==0 走首幕
//      占位；否则按 StageMapList 链推进，下一幕未入 map 时写入占位记录并按链
//      尾标志递归。----
// [b28d-solo-advancealtar_Manager NEAR·N93/N98 封顶] 本次修：正向包裹外层守卫
// （P222/P257，消 4.4 多出的尾 nop+jmp 岛）⇒ 136=136 助记符序列全等。残差 =
// 兄弟块槽方向（then stage -0x68 vs ORIG -0x5c、chainFlags -0x55 vs -0x5d、
// nextStage -0x5c vs -0x64、it -0x60 vs -0x68，4.1/4.4 块级槽分配镜像，N93/N98；
// stage 提头部会改 _Stage::C1 时机、it 提头部会引 C1Ev 发射）⇒ NEAR。
#include "advancealtar_Manager/setNextStage.inc"
}
}  // namespace advancealtar
// ============================================================================
// StageMapList::{isExist,getNextStage,getFirstStageIndex} —— LfAltarStage 批
// 定义归本 TU；声明在 CDataManager.h::StageMapList。
// ============================================================================

// ---- ORIG T 0x08364df6 _ZNK12StageMapList7isExistEi ----
#include "advancealtar_Manager/isExist.inc"
}

// ---- ORIG W 0x081347c0 _ZNK12StageMapList18getFirstStageIndexEv（inline thunk）----
#include "advancealtar_Manager/getFirstStageIndex.inc"
}

// ---- ORIG T 0x08364e58 _ZNK12StageMapList12getNextStageEiRb ----
#include "advancealtar_Manager/getNextStage.inc"
}

