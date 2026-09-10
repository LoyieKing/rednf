// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Dispatcher_ModItemAttr_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "Dispatchers.h"

// ============================================================================
// Dispatcher_ModItemAttr::dispatch_sig  —— 还原 ORIG 0x08200b08 T（约 829 insn）
//
// 语义（依据 docs/class_func_reports/Dispatcher_ModItemAttr/dispatch_sig.md）：
//   客户端请求「修改装备属性（封印再铸 / 一次性再铸）」。包格式：
//     get_short  → equipSlot（目标装备槽，INVEN_TYPE_EQUIP=1）
//     get_int    → itemIdx （目标装备 item idx）
//     get_short  → matSlot （消耗材料槽：封印卷 / 再铸材料）
//   校验链：state==3 且当前角色存在 → ServiceRestrict(1,0xf) → 安全保护(0x26)
//           → CheckInTrade → 目标装备非空/ItemType 10..0x15/匹配 idx →
//           → 材料非空 → CheckItemLock(1,equipSlot) → 按材料 item idx 选再铸模式
//           → 扣材料（delete_item）→ 修改装备（update_item）→ _SendResult 回包
//           → 会员返还材料（GetReturnItemRate / insertItemIntoInventory）。
//
// ORIG 的全部校验失败均为「抛出 int / ENUM_ERROR 异常 → 函数尾部 catch」：
//   ENUM_ERROR（正值）→ catch1：SendCmdErrorPacket(0x54, 码&0xff) 后 return 0；
//   int(0)            → catch2：e>=0 → return 0（不发包）；
//   int(-1/-2/-3)     → catch2：e<0 → LineFunc(0x9706,__PRETTY_FUNCTION__,0,0)（仅日志）。
//   本实现用等价的 SendCmdErrorPacket / return / LineFunc 直接表达。
// ============================================================================

#include "InterfacePacketBuf.h"      // InterfacePacketBuf（PacketGuard 基类，put_*）
#include "PacketGuard.h"             // PacketGuard 权威声明（CUser::Send(PacketGuard&) 参数类型）
#include "CDataManager.h"            // G_CDataManager、find_item、reseal_table_t、CItem
#include "CInventory.h"              // INVEN_TYPE / eItemDelReason / eItemAddReason、
                                     // CSecu_ProtectionField、GlobalData::s_pSecuProtectionField
#include "GlobalData.h"              // s_pSecuProtectionField
#include "ServiceRestrictManager.h"  // ServiceRestrictManager
#include "Arad_DataManager.h"        // ARAD::Singleton<ServiceRestrictManager>::Get()
#include "CItemList.h"               // get_rand_int(int)
#include <algorithm>                 // std::min<int>
// LineFunc（包分发断言日志；权威声明 CompoundEmblemDeps.h:35）
extern int LineFunc(int line, const char* func, int errNo, unsigned int mid);


namespace
{
// ---- 还原 Dispatcher_ModItemAttr::_SendResult（ORIG 0x08201938 T，独立成员函数，
//      Dispatchers.h 类声明未含该方法，故以 TU 局部静态函数表达相同包行为）----
//   参数：(resultType, matSlot, equipSlot) 对应 ORIG _SendResult(this,user,param_2,param_3,param_4)。
//   包：header(1,0x54) | byte(1) | short(matSlot) | int(该槽 item.get_add_info()) |
//       short(resultType)，随后 CUser::Send + SendUpdateItemList(1,0,equipSlot)。
#include "Dispatcher_ModItemAttr_impl/sSendResult.inc"
}
}  // namespace

// b28d-36（修法 P48/P282 + N82 封顶，ORIG 0x08200b08 T size 0xcfc）：
// P282 候选已核——ORIG vptr+0xc 虚调 4 处（0x08200f22/0x08200f39/0x082011b9/
// 0x082012d9），我方旧实现仅 3 处（itype 具名缓存把 0x08200f22+0x08200f39 合并为
// 单次）⇒ 改两次独立 `citem->GetItemType()` 短路比较后 vcall 4/4 与 ORIG 同槽
// 同形。三件套：①p167 调用点差集 102=102 CLEAR；②帧尺寸同 0x1ac；③gap 17
// 全为 4.4 槽复用/声明序（N82）导致的槽号平移（358 行 %ebp 槽差），无 MISSING/
// EXTRA 调用。语义完成，byte 层 N82 封顶。
// [N96 清绑 2026-09-09，本函数 4 处绑定一次清完]
// 删除：`register int ret asm("ebx")` / `register bool bad asm("al")` /
//       `register char* idStr asm("ebx")` / `register int st asm("esi")`
//       （P0 限定② + N96 全域禁绑；P248 尾条：任何情形不得恢复）。
// 还原（按 P0 双推律：ORIG 侧 ret 走 %ebx、bad 走 %al、idStr 走 %ebx、st 走
// %esi，四者均无栈槽）：
//   · ret  → 具名栈槽局部 `int ret;`（ORIG 有统一出口 `mov %ebx,%eax`，原码确有
//            统一返回局部，故取具名槽形而非逐点 return 常量）；
//   · bad  → 具名栈槽局部 `bool bad = (A || B); if (bad)`，4.4 产同构双岛物化
//            （`mov $1,%eax / mov $0,%eax / mov %al,-0x45(%ebp) / cmpb` 对比 ORIG
//            的 `mov $1,%eax / mov $0,%eax / test %al,%al`）；
//   · idStr/st → 右值直传 `tr(fmt, user->get_state(), NumberToString(...))`，4.4
//            产 `call NumberToString; mov %eax,%ebx; call get_state;
//            mov %ebx,0xc(%esp); mov %eax,0x8(%esp)`，与 ORIG 同序同槽位
//            （ORIG 多一条 `mov %eax,%esi` 中转）。
// 残差 = N96+N18a 封顶：bad 的物化结果 4.4 必落栈槽（ORIG 停在 %al），ret 每次
// 返回多 1 条槽读（`movl $0,-0x4c(%ebp); mov -0x4c(%ebp),%ebx` 对比 ORIG
// `mov $0,%ebx`），绑定外不可达。
// 读数（并列）：清绑前 verdict=DIFF orig829/our812 gap17，四数
//   [identical=2 ae=2 合计4 near=0 diff=2]；清绑后 verdict=DIFF orig829/our816
//   gap13，四数 [identical=2 ae=2 合计4 near=0 diff=2]（不变）。
#include "Dispatcher_ModItemAttr_impl/dispatch_sig.inc"
}

// ============================================================================
// ORIG 0x082335da W _ZN22Dispatcher_ModItemAttr7processEP5CUserR8MSG_BASER9ParamBase
//   objdump 逐指令核实：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
//   —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x08200b08，本文件上方)
//   与 _SendResult(0x08201938)。忠实转写空返回，勿臆造。
#include "Dispatcher_ModItemAttr_impl/process.inc"
}

// ORIG 0x082335d0 W _ZN22Dispatcher_ModItemAttr4readER9PacketBufR8MSG_BASE
//   objdump 核实同为纯空 ret0（0x082335d0..0x082335da）；ORIG 的包读取在
//   dispatch_sig 内完成（get_short/get_int 直读 PacketBuf），read 不承担职责。
#include "Dispatcher_ModItemAttr_impl/read.inc"
}