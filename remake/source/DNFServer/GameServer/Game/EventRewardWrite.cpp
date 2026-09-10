// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EventRewardWrite/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// EventRewardWrite.cpp —— eventReward 奖励发放链（OrigStStructs 独占）。
//
// 覆盖 ORIG 真实符号（全部 T，报告 docs/class_func_reports/eventReward/*.md 全文转写）：
//   createCharacInvenReward 0x080f0270 / createCharacEquipReward 0x080f033c
//   rewardWriteUserInven    0x080f0408 / rewardWriteUserEquip    0x080f06da
//   sendPostalReward        0x080f0976 / unsealRandomOption      0x080f0e40
//   applyItemOption         0x080f0d6e / StartEndPos             0x080f0d22（报告缺失，
//   objdump 直转）
//
// 注：ORIG 中 createCharac* / sendPostalReward 的调用侧以
//   G_CDataManager()->m_eventReward（CDataManager+0xb628）作为接收者，本 TU 照此书写；
//   this 形参在这些函数中不参与发放（见各报告 Ghidra C）。

#include <string.h>
// ============================================================================
// b28d-52 判词表（本 TU）：
//   已修：rewardKey::operator< 0x080f121a —— P222 正向包裹 + P196 三字节字段
//     signed char（CDataManager.h，已 R2 广播）⇒ 转 AE。
// [b28d-D03 本波 判词]（fresh `check_tu_routed.sh`：identical+ae 56 / near 3 / diff 3，
//   非 identical 件 = unsealRandomOption + applyItemOption + sendPostalReward +
//   rewardWriteUserInven/Equip + _Z41__static_initialization…（N123 逐 TU 撞名豁免））
//   · unsealRandomOption 0x080f0e40 —— **DIFF→NEAR**：ORIG +0x3c `mov 0x10(%ebp),%eax;
//     add $0x25,%eax; mov %eax,-0x1c` ⇒ `&inven.m_random.m_random` 有**具名指针局部**且在
//     首个 getter **之前**物化（P0 槽往返 + P28 声明序）。补 `RandomOption* pOption`
//     并前置后槽位（-0x1c）与 ORIG 对齐；残差 = ①N7 虚调用目标 4.4.7 恒 %eax（ORIG %edx，
//     两处）②第 4 个 getter 值（rarity）4.4 落槽而 ORIG 留 %edx（register/具名 handle 两形
//     实测均劣：78/78→79）。P230：N7 属 4.4.6 后端敏感族，路由由 Main 统管。
//   · applyItemOption 0x080f0d6e —— 77→75 条：①`inven.m_field0` 经 uchar 视图直存
//     （消 4.4 `test;setne` bool 归一，且得 ORIG 的 RHS-first 序）②`if (({ …&&…; }))`
//     P226 语句表达式复现 ORIG +0x91..+0xaf 短路物化岛（mov $1/jmp/mov $0 + test）
//     ③`register unsigned int attachType` 消槽（P0/P231 非绑定形）。残差 = N6 位折叠
//     （ORIG `(1<<t)&0x27` shl 形被 4.4 折 `(0x27>>t)&1` sar）+ N7 + 1 条 LHS/RHS 序；
//     attachType 无槽值复用（ORIG `cmp $5,%eax` 复用同一 %eax 两次）合法 C++ 不可造。
//   · sendPostalReward 0x080f0976 —— 261 条（ORIG 259）：①谓词改读 `inven.m_addInfo`
//     （ORIG +0x2ab `mov -0x87(%ebp),%eax` = inven+0x02，非 itemIdx 副本）②P226 岛
//     ③lvl/len/characNo/titleStr 四值改**实参位内联 + `({ …; })`**（LBL8/P226）⇒ 4.4 分别
//     升格 %ebx/%esi/%ebx/%eax，复现 ORIG 无槽形。残差 = 2 条（ENUM_SERVER_GROUP cast 的
//     中转槽 -0x19c + N7 虚调）；expDate/varargs 求值序为跨版本（P75/P87）。
//   · rewardWriteUserInven 0x080f0408 / rewardWriteUserEquip 0x080f06da —— **NEAR**：
//     p167 调用点差集 0（除 C1/C2、D1/D2 同址别名伪差，P93）；唯一残差 = N7 单处
//     （`citem->make_item(inven)` 虚调 vptr+0x8 目标 4.4.7 恒 %eax，ORIG %edx）。
// ============================================================================

#include <map>
#include <string>
#include <vector>

#include "CDataManager.h"       // eventReward / rewardKey / itemData / G_CDataManager / find_item
#include "Inven_Item.h"         // Inven_Item
#include "CItem.h"              // CItem 虚/非虚访问器
#include "CInventory.h"         // g_slotBoundChecker / OS_API::snprintf
#include "CItemGenerator.h"     // random_option::CRandomOptionItemHandle::give_option / ENUM_RARITY
#include "GlobalData.h"         // g_randomOptionHandle
#include "LogManager.h"         // LogManager::logFormat
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"     // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "GameWorld.h"          // G_GameWorld / IsIntegratedPvPBaseChannel
#include "CEnvironment.h"       // G_CEnvironment
#include "CUser.h"              // CUser（继承 CUserCharacInfo：get_charac_level/getCurCharacNo）

namespace
{

// eventReward+0x18 的 CSlotBoundChecker 视图（P1 组件视图折叠为单 disp 寻址）。
struct EventRewardCheckerPos
{
    char m_pad[0x18];
    CSlotBoundChecker m_checker;
};

}  // unnamed namespace

// ORIG 以 this+0x18 内嵌 CSlotBoundChecker 调用 get_slot_bound；重建 CDataManager.h
// 以同布局 map 成员 m_slotBoundMap 保持 ABI（CSlotBoundChecker 静态实现无成员），
#include "EventRewardWrite/StartEndPos.inc"
}

// ============================================================================
// eventReward::applyItemOption —— ORIG T 0x080f0d6e
// ============================================================================
#include "EventRewardWrite/applyItemOption.inc"
}

// ============================================================================
// eventReward::unsealRandomOption —— ORIG T 0x080f0e40
// ============================================================================
#include "EventRewardWrite/unsealRandomOption.inc"
}

// ============================================================================
// eventReward::rewardWriteUserInven —— ORIG T 0x080f0408
// ============================================================================
#include "EventRewardWrite/rewardWriteUserInven.inc"
}

// ============================================================================
// eventReward::rewardWriteUserEquip —— ORIG T 0x080f06da
// ============================================================================
#include "EventRewardWrite/rewardWriteUserEquip.inc"
}

// ============================================================================
// eventReward::sendPostalReward —— ORIG T 0x080f0976
// ============================================================================
#include "EventRewardWrite/sendPostalReward.inc"
}

// ============================================================================
// eventReward::createCharacInvenReward —— ORIG T 0x080f0270
// ============================================================================
#include "EventRewardWrite/createCharacInvenReward.inc"
}

// ============================================================================
// eventReward::createCharacEquipReward —— ORIG T 0x080f033c
// ============================================================================
#include "EventRewardWrite/createCharacEquipReward.inc"
}
