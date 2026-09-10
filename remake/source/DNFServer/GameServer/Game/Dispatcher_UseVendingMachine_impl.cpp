// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Dispatcher_UseVendingMachine_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================

#include "Dispatchers.h"

// ORIG 自由函数 LineFunc(int, const char*, int, unsigned int) @0x085908d7 T；
// 定义由 CompoundEmblemDeps.cpp 提供（见 CompoundEmblemDeps.h）。
extern int LineFunc(int, const char*, int, unsigned int);

// ---------------------------------------------------------------------------
// Dispatcher_UseVendingMachine::dispatch_sig —— 自动售货机（扫地机/道具售货机）
// 购买流程还原。
//
// 依据 docs/class_func_reports/Dispatcher_UseVendingMachine/（ORIG
// dispatch_sig @ 0x0821c2e6，约 1009 指令；连同 6 个辅助 _get/_put/_notify 方法）。
//
// 说明（`IMPL` = 本文件重构决策，非 ORIG 事实）：
//   1. ORIG 的这些辅助方法是类成员（_ZNK28Dispatcher_UseVendingMachine…），
//      Dispatchers.h 类声明未含这些成员且被约定只读，故本文件以 TU 内
//      file-local 辅助函数实现，显式传 CMTRand* 与数据源上下文。
//   2. ORIG 使用 this->m_rand（CMTRand* @ +0x04，由 ctor 以 time 种子创建）。
//      Dispatchers.cpp 的 ctor 为空桩，不改；故 dispatch_sig 内新建本地 CMTRand
//      并按 ORIG ctor 语义以 time 播种，传给各辅助函数。
//   3. 外部依赖已由 producer 在权威头/TU 落地：CVendingMachineStatistic /
//      TimerVendingMachineItemNotify（VendingMachine.h）、CGlobalEffectManager
//      （CGlobalEffectManager.h）、CStackableItem::getGlobalEffectData、
//      user_creature::CCreatureItemConverter::SetEggItem（CInventory.h）。
//      本文件直调这些真实成员，不再定义本地替身。
//   4. 返回码层面的错误路径（ORIG 为 __cxa_throw ENUM_ERROR / LineFunc 断言）
//      在本文件以对应 return 值近似表达，语义与报告一致（见 game_verify_guide）。
// ---------------------------------------------------------------------------

#include <ctime>

#include <cstring>

#include <algorithm>

#include "CDataManager.h"
#include "CInventory.h"
#include "CUserCharacInfo.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "Inven_Item.h"
#include "CUser.h"
#include "GlobalData.h"
// 权威 WongWork::CGenUniqueNo 见 CInventory.h（含 genIPGNo(ENUM_IPGNO_TYPE,uint,char*) @0x084b7750），
// 故不再 include CGenUniqueNo.h（其类签名不同且与 CInventory.h 冲突）。
#include "CCeraShop.h"
#include "PacketGuard.h"
#include "CSystemTime.h"
#include "PvPTypes.h"   // 权威 CMTRand 声明（PvPTypes.h:178；CMTRand.h 不存在，类定义于 CMTRand.cpp）
#include "VendingMachine.h"   // CVendingMachineStatistic / TimerVendingMachineItemNotify 权威声明
#include "CGlobalEffectManager.h"
#include "CUserProc.h"   // APSystem::CUserProc::ClearActionAndSendtoUser 权威声明
#include "CGuildServerProxy.h"   // CEventManager::GetRepeatEvent / CEventBase::IsEventing 权威声明

/* =========================================================================
 * ORIG 嵌套类型 stPutItemResult_t —— _putItemIntoUser/_notifyItemInfo 共用输出
 * （0x10 字节；ORIG _putItemIntoUser 0x0821b71c 开头两 store +0x00/+0x04、
 *  _notifyItemInfo 0x0821c17c 读 +0x00/+0x04 实证）。
 * 声明见 Dispatchers.h（类内 struct stPutItemResult_t; 前向）。
 * ====================================================================== */
struct Dispatcher_UseVendingMachine::stPutItemResult_t
{
    unsigned long  m_itemIdx;   // +0x00
    unsigned int   m_count;     // +0x04
    int            m_result;    // +0x08
    short          m_slot;      // +0x0c
    unsigned char  m_success;   // +0x0e
};

/* =========================================================================
 * 售货机数据直接使用 CDataManager.h 权威结构（不再本地副本）：
 *   stVendingMachine_t      +0x00 m_id；+0x04 m_infoList
 *   stVendingMachineInfo_t  +0x00 m_groupNum；+0x04 m_payType；+0x08 m_payAmount；
 *                           +0x0c m_giveCount；+0x10 m_vecGive；+0x1c m_bonusCount；
 *                           +0x20 m_vecBonus；+0x2c m_lottery
 *   stVendingMachineOutput_t +0x00 m_itemId；+0x04 m_cumWeight；+0x08 m_count
 * ====================================================================== */

// ORIG T 0x0821b60a _ZNK28Dispatcher_UseVendingMachine18_getVendingMachineEj
//   遍历 G_CDataManager()->m_vendingMachine 按 m_id 匹配，返回机器指针（无则 0）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：删除 dm==0 判空（ORIG 无），
//   改 `getVendingMachineTable()` + const_iterator 循环（ORIG 的 begin/end/
//   operator!=/operator++/operator* 调用点逐条对齐；ORIG 0x0821b610 段）。
#include "Dispatcher_UseVendingMachine_impl/_getVendingMachine.inc"
}

// ORIG T 0x0821b698 _ZNK28Dispatcher_UseVendingMachine22_getVendingMachineInfoEPK18stVendingMachine_tj
//   在 vm->m_infoList 中按栏位 id（m_groupNum）匹配，返回栏信息（无则 0）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：成员 vector 在 begin/end 处
//   内联重算（去具名引用，ORIG 0x0821b6a1 `lea 0x4(%eax),%edx` 两次）+ 迭代器循环。
#include "Dispatcher_UseVendingMachine_impl/_getVendingMachineInfo.inc"
}

/* =========================================================================
 * 辅助：按权重随机选一个给/赏物品，写入 out（Inven_Item）。
 * ==================================================================== */

// ORIG T 0x0821bf82 _ZNK28Dispatcher_UseVendingMachine12_getGiveItemEPK22stVendingMachineInfo_t
//   依 m_giveCount 权重（范围 = m_giveCount-1）经 this->m_rand 随机选 m_vecGive 中
//   一个输出；命中写 m_itemId → m_addInfo(+0x02)、m_count → m_addInfo2(+0x07)。
//   按值返回 Inven_Item（sret，0x3d 字节；ORIG 调用处逐字段拷贝实证）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：删除 range<0 防御（ORIG 无）、
//   randInt 直传具名 range（去中间 r）、迭代器循环（operator-> 每点重调）。
#include "Dispatcher_UseVendingMachine_impl/_getGiveItem.inc"
}

// ORIG T 0x0821c058 _ZNK28Dispatcher_UseVendingMachine13_getBonusItemEPK22stVendingMachineInfo_t
//   依 m_bonusCount 权重随机选 m_vecBonus 中一个输出（事件开启检查见上版推断注释）。
//   命中写 m_itemId → m_addInfo(+0x02)、m_count → m_addInfo2(+0x07)。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：事件判活改内联链式
//   `s_event_manager->GetRepeatEvent(0x23)->IsEventing(0)`（ORIG 无 ev 判空/无具名槽，
//   0x0821c071 段），range 防御删除、迭代器循环同 _getGiveItem。
#include "Dispatcher_UseVendingMachine_impl/_getBonusItem.inc"
}

/* =========================================================================
 * 辅助：把物品/货币/胜点交给用户，填充 result。
 * ==================================================================== */
// ORIG T 0x0821b71c _ZNK28Dispatcher_UseVendingMachine16_putItemIntoUserEP5CUsermjRNS_17stPutItemResult_tE
//   把 itemIdx（0=金币/1=活动币/2=胜点/else=实物）交给用户，填充 result。
// b28d-solo-Dispatcher_UseVendingMachine_impl [N7+N18a+setcc-fold]：ORIG 547 insn /
//   我方 552；p167 调用点差集 CLEAR 62/62（0 缺 0 多）⇒ 语义完成，byte 层封顶（§6.2）。
//   残差：①4 处虚调用 vptr 链目标寄存器（ORIG `mov (%eax),%edx; … call *%edx`
//   vs 4.4.7 恒目标 %eax/this %edx，N7；本 TU 未路由 446）；②`mov 0x8(%eax),%eax;
//   mov %eax,%edi` 被 4.4 折成单步（N18a）；③`A && B` 第二条件 ORIG setne 物化
//   vs 4.4 分支+常量物化（setcc-fold 镜像）；④registNewEffect arg1（int 字段→
//   unsigned 形参）恒多 `mov %eax,%edx` 中转（P265 族；字段侧需动共享头，未改）。
//   语义修正：补 `outItem.m_addInfo = itemIdx;`（ORIG 0x0821b883 实证，旧实现缺失）。
#include "Dispatcher_UseVendingMachine_impl/_putItemIntoUser.inc"
}

/* =========================================================================
 * 辅助：给用户发"物品已放入"计时通知（若该物品尚未出现在用户拥有的列表）。
 * ==================================================================== */
// ORIG T 0x0821c17c _ZNK28Dispatcher_UseVendingMachine15_notifyItemInfoEPK22stVendingMachineInfo_tP5CUserRKNS_17stPutItemResult_tE
//   在 info->m_lottery（vector<pair<int,int>>）中查找 (itemIdx,count)；命中才
//   registTimer 延迟发放（ORIG _notifyItemInfo 汇编：pair 构造 + find + operator==
//   判等后跳 registTimer）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：pair 直传 result 字段（去 (int)
//   cast ⇒ 复现 pair<unsigned long const&,unsigned int const&> 转换构造）、m_lottery
//   内联 begin/end（去具名 lot 引用）、具名 it + `== end` + goto 尾空标签复现
//   ORIG 0x0821c21f `jne <nop>` 岛（P223）。
#include "Dispatcher_UseVendingMachine_impl/_notifyItemInfo.inc"
}

// §6.2 巨件核销：P167 112/112 [0缺0多]；gap 1008→976；残差归因 N82
// （帧 0x1c0→0x200 全帧槽号平移 + 未链接 .o 地址归一，详见
//   docs/asm_translation_patterns.md 负例表）。
/* =========================================================================
 * dispatch_sig —— 主入口。
 * ==================================================================== */
#include "Dispatcher_UseVendingMachine_impl/dispatch_sig.inc"
}

// ============================================================================
// ORIG 0x08233dbc W _ZN28Dispatcher_UseVendingMachine7processEP5CUserR8MSG_BASER9ParamBase
//   objdump 逐指令核实：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
//   —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x0821c2e6，本文件上方)
//   及 6 个 _get/_put/_notify 辅助方法。忠实转写空返回，勿臆造。
#include "Dispatcher_UseVendingMachine_impl/process.inc"
}

// ORIG 0x08233db2 W _ZN28Dispatcher_UseVendingMachine4readER9PacketBufR8MSG_BASE
//   objdump 核实同为纯空 ret0（0x08233db2..0x08233dbc）；ORIG 的包读取在
//   dispatch_sig 内完成（get_int/get_int/get_short 直读 PacketBuf）。
#include "Dispatcher_UseVendingMachine_impl/read.inc"
}