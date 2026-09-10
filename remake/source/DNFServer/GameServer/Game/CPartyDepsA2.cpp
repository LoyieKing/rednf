// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPartyDepsA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CPartyDepsA2.cpp —— CParty 第一批依赖函数（PartyDepsA2，2026-08-24）
//
// 全部函数逐份完整读取 docs/class_func_reports/CParty/<Func>.md（## 汇编 +
// ## 反编译 C）后转写；依赖叶子函数按其自身报告转写：
//   ConditionLevelChkDungeon/isEventDungeon.md、isApplyLevel.md
//   TowerOfDespairMgr/{Get_CDungeonByLayer,Get_DungeonIndexByLayer,
//                      Get_APCIndexByLayer,GetBaseLayerIndex}.md
//   TOD_UserState/getTodayEnterLayer.md、TOD_Layer/TOD_Layer.md
//   （TOD_Layer::GetLayer 无独立报告，按 nm 0x0822ec40 直读 m_layer 实现，
//     标记"报告缺失"。）
// 去重后本 TU 不定义、仅调用（owner 提供，链接期解析）：
//   TOD_Layer::TOD_Layer(unsigned short)    —— TowerOfDespairMgr.cpp（TOD 域 owner）
//   TowerOfDespairMgr::SendMessageToCUser(CUser*, char*) —— TowerAPCLeafC3.cpp（ORIG T 0x08644366）
// 定义仍缺失（blocked，声明已在权威头）：
//   TowerOfDespairMgr::CheckEnterable   ORIG T 0x086440ee（待 TOD 批次）
//   CUser::MakeTagCharacInfo            ORIG T 0x08690776（待 CUser 批次）
//   CUser::Send(PacketGuard&)           ORIG T 0x086485ba（声明 CUser.h，定义待批次）
// ============================================================================
#include "CParty.h"

#include <algorithm>
#include <stdio.h>
#include <vector>

#include "CDungeon.h"               // get_index/get_min_level/get_standard_level
#include "CDataManager.h"           // G_CDataManager/find_dungeon/m_conditionDungeon
#include "DataManagerInitDepends.h" // TowerOfDespairMgr/TOD_Layer/TOD_UserState
#include "TimerQueue.h"             // G_TimerQueue/TimerQueue::InsertTimer
#include "GlobalData.h"             // GlobalData::s_pTowerOfDespairMgr
#include "CWorldMap.h"              // IsInHellDungeon/m_field1c/m_field34
#include "PacketGuard.h"            // PacketGuard（InterfacePacketBuf 子类）
#include "CInventory.h"             // check_item_exist/delete_item 完整类型
#include "cMyTraceNoop.h"

namespace
{

// ORIG：user+0xcac 为当前 CDungeon*。CUser 该偏移区未建模（无具名字段/访问器），
// 按 DisPatcherCore.cpp 对 party+0xcac 的只读视图同口径处理【推断：布局口径】。
struct CUserCurDungeonView
{
    char m_pad[0xcac];
    CDungeon* m_dungeon;
};

}  // namespace

// ORIG 全局 _Z33getHellPartyEntranceItemNeedCounti（T 0x08891be6），
// 定义在 CParty.cpp；此处仅声明供 UseSealDoorItems 调用。
#include "CPartyDepsA2/getHellPartyEntranceItemNeedCount.inc"
#include "CPartyDepsA2/GetLayer.inc"
}

// ORIG T 0x086439c8 _ZNK9TOD_LayerplEi（报告缺失，按 nm 反汇编实现）：
// 构造 ret(m_layer+n)；ret.GetLayer()>100 → resetLayer()；sret 返回。
TOD_Layer TOD_Layer::operator+(int n) const
{
    TOD_Layer ret(static_cast<unsigned short>(m_layer + n));
    if (ret.GetLayer() > 100)
    {
        ret.resetLayer();
    }
    return ret;
}

// ============================================================================
// ConditionLevelChkDungeon::isEventDungeon —— ORIG T 0x089139d4
//   （报告 ConditionLevelChkDungeon/isEventDungeon.md）
//   m_unableDungeons 为空或不含 dungeonIdx → true。
// ============================================================================
// [b28c-31] AE：正向 `if (*it == dungeonIdx) return false;` 复现 ORIG
// 0x08913a2c 的 `sete %al; test; je <++it>` 正极性物化（46=46）。
#include "CPartyDepsA2/isEventDungeon.inc"
}

// ============================================================================
// ConditionLevelChkDungeon::isApplyLevel —— ORIG T 0x08913a62
//   （报告 ConditionLevelChkDungeon/isApplyLevel.md）：m_applyLevel <= level。
// ============================================================================
#include "CPartyDepsA2/isApplyLevel.inc"
}

// ============================================================================
// TOD_UserState::getTodayEnterLayer —— ORIG T 0x0864383e
//   （报告 TOD_UserState/getTodayEnterLayer.md）：sret 返回
//   *(const TOD_Layer*)(this+0x14) + 1（经 TOD_Layer(ushort) 构造，>100 回卷 1；
//   构造回卷语义见 TOD_Layer/TOD_Layer.md 与 operator+ 尾部 resetLayer）。
// ============================================================================
#include "CPartyDepsA2/getTodayEnterLayer.inc"
}

// ============================================================================
// TowerOfDespairMgr::Get_CDungeonByLayer —— ORIG T 0x08643d14
//   （报告 TowerOfDespairMgr/Get_CDungeonByLayer.md）
// ============================================================================
// [b28d-Main·检查点17] 返回类型 CDungeon*→const CDungeon*（元素 const 指针，mangled 不变）。
#include "CPartyDepsA2/Get_CDungeonByLayer.inc"
}

// ============================================================================
// TowerOfDespairMgr::Get_DungeonIndexByLayer —— ORIG T 0x08643d76
//   （报告 TowerOfDespairMgr/Get_DungeonIndexByLayer.md）
// ============================================================================
#include "CPartyDepsA2/Get_DungeonIndexByLayer.inc"
}

// ============================================================================
// TowerOfDespairMgr::Get_APCIndexByLayer —— ORIG T 0x08643d9c
//   （报告 TowerOfDespairMgr/Get_APCIndexByLayer.md）：
//   层号在 m_npcLayers 中 → 返回层号；否则 -1；>100 → 0。
// ============================================================================
#include "CPartyDepsA2/Get_APCIndexByLayer.inc"
}

// ============================================================================
// TowerOfDespairMgr::GetBaseLayerIndex —— ORIG T 0x086440a0
//   （报告 TowerOfDespairMgr/GetBaseLayerIndex.md）：m_field358 缓存基础层。
// ============================================================================
#include "CPartyDepsA2/GetBaseLayerIndex.inc"
}

// ============================================================================
// IsExistCharacter —— ORIG T 0x085b624e（报告 CParty/IsExistCharacter.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 比较式交换操作数（call 在左）消除 4.4 跨
// call 缓存形参入 %ebx 的 push/pop 与帧差（P52/P0）。
// ============================================================================
#include "CPartyDepsA2/IsExistCharacter.inc"
}

// ============================================================================
// FindUserByCharId —— ORIG T 0x085a2140（报告 CParty/FindUserByCharId.md）
//   返回命中槽的 CUser*（头文件签名 int；调用方按真值消费）。
// ============================================================================
#include "CPartyDepsA2/FindUserByCharId.inc"
}

// ============================================================================
// BloodHandleDieUser —— ORIG T 0x085b8e5c（报告 CParty/BloodHandleDieUser.md）
// ============================================================================
// [b28d-53 P159] ORIG 尾 `mov $0x1,%eax`、错误路径 `mov $0x0,%eax` ⇒ 返回非 void；
// 声明随头改 bool（mangled 不含返回类型）。
#include "CPartyDepsA2/BloodHandleDieUser.inc"
}

// ============================================================================
// CheckClearQuestHellParty —— ORIG T 0x085a7a72（报告 CParty/CheckClearQuestHellParty.md）
//   out 清空后：对每个有效成员逐一核对 hell 任务清单（m_hellWorldMap+0x1c，
//   即 CWorldMap::m_field1c；无空检查，ORIG 如此）；存在未清任务 →
//   push_back 成员槽号并跳出该成员。
// [b28d-solo-CPartyDepsA2 判词·AE] 删 UserQuest* quest 具名局部（P0：ORIG 结果
// 直传 isClearQuest 的 this）+ 循环变量仅 for-init 初始化一次（ORIG 单条 movl）。
// ============================================================================
#include "CPartyDepsA2/CheckClearQuestHellParty.inc"
}

// ============================================================================
// UseSealDoorItems —— ORIG T 0x085a8038（报告 CParty/UseSealDoorItems.md）
//
// [b28d-solo-CPartyDepsA2 判词·N 封顶] 185/185 条、助记符序列逐段同构；
// 已修：result/minLevel/itemId 具名局部删除（P0/P171）、非 const 迭代器
// （P96）、cMyTrace 全表达式临时链（P227）、slot 提到函数头、直接 return。
// 残余三族（语义等价）：
//  ① N95 槽基差 4：ORIG 槽基 local_14@-0x10/slot@-0x14/needCount@-0xc，
//     4.4 恒 ebp-0x18 起（帧顶多留 4B gap；double framePad 反致下移，实测）。
//  ② N88/P197④ 槽比 setne 物化：ORIG 0x085a8153 `cmpl $0xffffffff,-0x14;
//     setne %al; test %al,%al; je`（4.1 槽比零产物）；4.4-O0 对槽比恒出
//     `cmpl;je` 直跳——P197④ 探针实证源形不可复现（P225 while+break 形
//     要求单一条件+循环退出，本处 else 早返含 return false 无法单比较承载）。
//  ③ N7/N18b 寄存器角色差（slot 装载 eax↔edx）。
// ============================================================================
#include "CPartyDepsA2/UseSealDoorItems.inc"
}

// ============================================================================
// CheckHaveHellPartyPassItem —— ORIG T 0x085a7b8a（报告 CParty/CheckHaveHellPartyPassItem.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 负极性短路单 return `!A || B`（P214①/P258）
// + 成功分支正向 if（P59/P222）+ callee CheckDestroyConditionSealDoor 返回 bool
// （P292 2 条形）+ m_sealDoorFlag unsigned char + 本函数返回 unsigned char
// （P9 movzbl 直返；.h 三处已 R2 广播，CParty.cpp 定义已由 Main 代落同步）。
// ============================================================================
#include "CPartyDepsA2/CheckHaveHellPartyPassItem.inc"
}

// ============================================================================
// SendPartyMemberTagChracInfo —— ORIG T 0x085be05a（报告 CParty/SendPartyMemberTagChracInfo.md）
// ============================================================================
#include "CPartyDepsA2/SendPartyMemberTagChracInfo.inc"
}

// ============================================================================
// GetDungeonIndex_isTOD —— ORIG T 0x085bacc0（报告 CParty/GetDungeonIndex_isTOD.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 照抄写后未读 bool 局部（P171/P184）+ 删
// CDungeon* target 具名局部（P0）+ 删显式 cast（P211 LHS-first store）+
// cMyTrace/cMyTraceNoop 全表达式临时链（P221）+ Get_DungeonIndexByLayer 返回
// 类型 unsigned→int（.h 已 R2 广播）。
// ============================================================================
#include "CPartyDepsA2/GetDungeonIndex_isTOD.inc"
}

// ============================================================================
// SetProperDungeonEvent —— ORIG T 0x085bb62a（报告 CParty/SetProperDungeonEvent.md）
// [b28d-solo-CPartyDepsA2 判词·AE] max/min 结果就地写回 minLevel/upperBound
// （P0：4.1 槽复用即源码复用）+ 先声明后赋值、声明序按 ORIG 槽序
// standardLevel/cap/zero/minLevel/upperBound（P198/P244），lower/upper 具名局部删除。
// ============================================================================
#include "CPartyDepsA2/SetProperDungeonEvent.inc"
}

// ============================================================================
// SetProperLevelDungeon —— ORIG T 0x085bb746（报告 CParty/SetProperLevelDungeon.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 同 SetProperDungeonEvent：max/min 就地写回
// + 先声明后赋值、声明序按 ORIG 槽序（P198/P244）。
// ============================================================================
#include "CPartyDepsA2/SetProperLevelDungeon.inc"
}
