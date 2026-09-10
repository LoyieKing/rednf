// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CreatureOnlySimple/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CreatureOnlySimple.cpp —— OnlyOrig 清零批 UCSimple（user_creature 辅助类与池）
//
// 独占 TU：每函数定义前注释 ORIG mangled + 地址；逐指令依据报告
// docs/class_func_reports/user_creature__{COverSkillTime,CStomach,CCreatureItem,
// CEgg}/ 或 objdump(-d df_game_r) 转写（缺失报告的函数标【objdump】）。
// 声明追加需求见 /tmp/df_batches/CreatureJob_decl_UCSimple.txt（父代理落头）：
//   - CREATURE_ITME_TYPE 枚举 / CCreatureItem 外联 ctor / SetCreatureItemType(enum)
//   - CEgg 继承式布局 / CDBMsgSender::Cancel+ChangeOwner+ChangeSlotNo+AuctionRegist
//   - COverSkillTime::{ctor,SetTime,GetTime} / CStomach ctor
//     / CCreatureItemFactory::{GetSize,GetRemainSize} 声明
// ============================================================================
#include <string.h>

#include "GlobalData.h"    // s_systemTime_ / s_*_stream_pool
#include "Inven_Item.h"    // Inven_Item / Inven_Item_Compatible
#include "SigTypes.h"      // SIG_CREATURE_ITEM
#include "StreamPool.h"    // Stream(m_poolType) / StreamPool/BigStreamPool/SmallStreamPool
#include "CInventory.h"    // user_creature::CCreatureItem / CCreatureMgr 完整布局
#include "CGameManager.h"  // user_creature::COverSkillTime/CStomach/CEgg/CDBMsgSender/
                           // CCreatureItemFactory/GetInstanceCreatureItemFactory
#include "CUser.h"

namespace user_creature
{

// ============================================================================
// COverSkillTime（4B，内嵌于 CCreature +0x50）
// ============================================================================

// ORIG _ZN13user_creature14COverSkillTimeC1Ev T 0x0833d636（C2 同址）：
// *(int*)this = 0。
#include "CreatureOnlySimple/COverSkillTime.inc"
}

// ORIG _ZN13user_creature14COverSkillTime7SetTimeEv T 0x0833d644：
// call CSystemTime::getCurSec(GlobalData::s_systemTime_) 后 *(int*)this = eax。
#include "CreatureOnlySimple/SetTime.inc"
}

// 【objdump】ORIG _ZN13user_creature14COverSkillTime7GetTimeEv W 0x0833ef04
// （size 0x08）：push %ebp; mov %esp,%ebp; mov 0x8(%ebp),%eax; pop %ebp; ret
//   —— 字面返回 this 本身、不解引用。调用方（CCreatureMgr::SetOverSkillTime
//   0x0833c523/0x0833c637/0x0833c656）在 call 之后立即 `mov (%eax)` 取值，
//   即返回 &m_time。返回类型不参与 mangled；按 ORIG 指令忠实转写为 int*。
#include "CreatureOnlySimple/GetTime.inc"
}

// ============================================================================
// CStomach（8B，内嵌于 CCreature +0x34：m_lastTime@+0 / m_value@+4）
// ============================================================================

// ORIG _ZN13user_creature8CStomachC1Ev T 0x08336cd8（C2 同址）：
// this+0 = getCurSec()；this+4 = 0x42c80000（float 100.0f）。
#include "CreatureOnlySimple/CStomach.inc"
}

// ============================================================================
// CCreatureItem（权威布局见 CInventory.h；本批提供外联 ctor / 类型写入器）
// ============================================================================

// 【objdump】ORIG _ZN13user_creature13CCreatureItemC1Ev W 0x08340c82（C2 同址，
// size 0x0e）：movl $0x8c2f4d8,(%eax) —— 写 CCreatureItem vtable 指针槽。
// [b28d-Main·检查点14] 真多态化后由编译器自动发射该 vptr 写（探针 /tmp/vt_probe.cpp）。
#include "CreatureOnlySimple/CCreatureItem.inc"
}

// 【objdump】ORIG _ZN13user_creature13CCreatureItem19SetCreatureItemTypeENS_18CREATURE_ITME_TYPEE
// W 0x0833efe2（size 0x0e）：this+0x08 = type。
// 调用点 CCreatureMgr::Hatch @0x0833a11d 传 1（孵化成宠物）。枚举参数版本为
// worklist 权威符号（现头 inline int 版产生的 Ei 符号 ORIG 不存在，由 decl
// 条目替换掉）。
#include "CreatureOnlySimple/SetCreatureItemType.inc"
}

// CCreatureItem::GetItemId 定义后置至 CEgg::SaveCreatureItem 之后（P210：4.4 对
// TU 内已见定义的 callee 精确继承 outgoing，SaveCreatureItem 帧塌缩 sub $0x4；
// 未前置时保守大帧=sub $0x18 与 ORIG 4.1 一致）。

// ORIG _ZN13user_creature13CCreatureItem6GetUidEv W 0x0833efbe（S23B：
// SetStomach 0x08339ee0+0x69 实证调用点为 call，内联形违 ORIG，同 GetItemId 案）。
#include "CreatureOnlySimple/GetUid.inc"
}

// ORIG _ZN13user_creature13CCreatureItem11GetNoChargeEv W 0x0833efd6（size 0x0e）：
// movzbl 0x14(%eax),%eax 直返。P9：返回 unsigned char（bool 会生成 setne 规范化）。
// 头内不再保留 inline 定义（无调用点则永不 emission → 仅 ORIG 缺口），外联产出符号。
#include "CreatureOnlySimple/GetNoCharge.inc"
}

// ORIG _ZN13user_creature13CCreatureItem7GetStatEv W 0x0833eff0（size 0x0e）：
// movzbl 0x15(%eax),%eax 直返。
#include "CreatureOnlySimple/GetStat.inc"
}

// ============================================================================
// CEgg（继承 CCreatureItem；布局替换 decl 见 txt）
// ============================================================================

// ORIG _ZN13user_creature4CEggC1Ev T 0x08338f4c（报告 CEgg.md 全汇编转写）：
// ① 基类 ctor（写 CCreatureItem vtable 0x8c2f4d8）
// ② vptr ← &_ZTVN13user_creature4CEggE+0x8（nm V 0x08c2f4a0 → +8 = 0x08c2f4a8）
// ③ Reset()
// 镜像以 int m_vptr 槽模拟多态头，无虚表生成。
// ORIG 只调一次 CCreatureItemC1（编译器隐式基类构造）；显式
// `CCreatureItem();` 会额外构造一次临时对象（NEW 反汇编 2 次 C1），删除。
#include "CreatureOnlySimple/CEgg.inc"
}

// ORIG _ZN13user_creature4CEgg5ResetEv T 0x08338f82（报告 Reset.md 全汇编转写）：
// +0x08/+0x0c/+0x10 清零、+0x14 字节清零（均为基类字段）。
#include "CreatureOnlySimple/Reset.inc"
}

// 【objdump】ORIG _ZN13user_creature4CEgg6IsSaveEv W 0x0833f102（size 0x0a）：
// push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— ORIG 符号字面
// 返回 0（蛋条目默认不可保存态；全二进制无此 W 符号的 call 引用，仅 linkonce
// 导出）。按指令忠实转写，非编造固定桩。
#include "CreatureOnlySimple/IsSave.inc"
}

// ORIG _ZN13user_creature4CEgg16LoadCreatureItemEPNS_12CCreatureMgrEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
// T 0x08338fac（LoadCreatureItem.md 全汇编转写）：蛋条目 DB 载入侧。
#include "CreatureOnlySimple/LoadCreatureItem.inc"
}

// ORIG _ZN13user_creature4CEgg16SaveCreatureItemEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
// T 0x083390c8（SaveCreatureItem.md 全汇编转写）：蛋条目序列化；恒返 1。
// 首写经 GetItemId()（外联 call，见 CCreatureItem::GetItemId 定义）。
// [b28d-26] AE。旧 [b22判词·N82]「movzbl;mov%eax,%edx 双寄存路由不可复现」作废：
//   ORIG 0x083390c8 +0x26 赋值段 `movzbl 0x14(%eax),%eax; mov %eax,%edx; mov %dl,0x26`
//   = uchar→signed char 转换两段；目标字段 GamePODTypes.h 改 signed char 后逐条全等
//   （探针 /tmp/d26p/p_route2.cpp：uchar→char 两段、同型单段）。
#include "CreatureOnlySimple/SaveCreatureItem.inc"
}

// ORIG _ZN13user_creature13CCreatureItem9GetItemIdEv W 0x0833efa4（size 0x0b）：
// mov 0x4(%eax),%eax 直返。ORIG 为外联弱符号，CEgg::SaveCreatureItem 以 call 调
// （不内联）；头内仅声明、此处外联定义产出真实 call。定义**置于 SaveCreatureItem
// 之后**（P210）：调用点不见定义→保守 outgoing 帧 sub $0x18 与 ORIG 一致。
#include "CreatureOnlySimple/GetItemId.inc"
}

// ============================================================================
// CDBMsgSender（Init/C1/dtor/Send 归 CCreatureMgr.cpp；此处补余下消息填充族）
// ============================================================================

// 【objdump】ORIG _ZN13user_creature12CDBMsgSender6CancelEv W 0x0833f994：
// m_stream 空 → 直接结束（两路径均不设置 eax，返回类型取 void）；
// 按 stream->m_poolType(+0x20) 分池归还后置空持有。
//   ==1 → SmallStreamPool::Free(GlobalData::s_small_stream_pool@0x940bda8)
//   ==2 → BigStreamPool::Free  (GlobalData::s_big_stream_pool@0x940bda4)
//   ==0/default → StreamPool::Free      (GlobalData::s_stream_pool@0x940bd6c)
#include "CreatureOnlySimple/Cancel.inc"
}

// ============================================================================
// 共同骨架（ChangeOwner/ChangeSlotNo/AuctionRegist 同构，objdump 逐条核对）：
//   m_stream 空 → 0；buf = GetInBuffer<SIG_CREATURE_ITEM>()；
//   memset(buf, 0, 0x74)；compat(buf+0x04).setCopy(*item)；
//   buf->m_addInfo2(+0x30) ← item+0x07（低半 addInfo2）；随后各自差异字段。
// ============================================================================

// 【objdump】ORIG _ZN13user_creature12CDBMsgSender11ChangeOwnerEP10Inven_Itemiiiiii
// W 0x0833ffe8（nm -S --defined-only 实测；旧注释 0x081fae8a 系历史误记）：差异体——
//   +0x51(byte)=p3(reason)；+0x48=p1(characNo)；hdr|=0x30；+0x2c=p2(slot)；
//   +0x64=1；+0x68=p4(mapSize)；+0x6c=p5；+0x70=p6；返 1。
// [N96 清绑 2026-09-09] 原 `register char* compatEax asm("eax")` 全域禁用（P0②/N96/
// P248 尾条），还原表达式直传 `reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)`。
// 我方 .o 逐条复现 ORIG：`mov -0xc(%ebp),%eax; lea 0x4(%eax),%edx; mov 0xc(%ebp),%eax;
// mov %eax,0x4(%esp); mov %edx,(%esp); call setCopy`。TU 三数零变化
//（identical=9 ae=19 near=0 diff=1），本函数 TU 口径 AE 不变（orig=58 our=59）。
#include "CreatureOnlySimple/ChangeOwner.inc"
}

// 【objdump】ORIG _ZN13user_creature12CDBMsgSender12ChangeSlotNoEP10Inven_Itemiii
// W 0x083400a4（nm -S --defined-only 实测；旧注释 0x081faf4c 系历史误记）：差异体——
//   +0x51(byte)=p3(reason)；+0x48=p1(characNo)；hdr|=0x20；+0x2c=p2(slot)；
//   +0x64=2；返 1（无 +0x68 区写入）。
// [N96 清绑 2026-09-09] 原 `register char* compatEax asm("eax")` 全域禁用（P0②/N96/
// P248 尾条），还原表达式直传 `reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)`。
// 我方 .o 逐条复现 ORIG `lea 0x4(%eax),%edx` setCopy 段。TU 三数零变化
//（identical=9 ae=19 near=0 diff=1），本函数 TU 口径 AE 不变（orig=49 our=49）。
#include "CreatureOnlySimple/ChangeSlotNo.inc"
}

// ORIG _ZN13user_creature12CDBMsgSender13AuctionRegistEP10Inven_Itemiiii
// W 0x08340144（nm -S --defined-only 实测；旧注释 0x081faff2 系历史误记）：差异体——
//   +0x48=p1(characNo)；hdr|=0x100（or $0x1,%dh）；+0x53=1；+0x64=5；
//   +0x51(byte)=p3(@0x18)；+0x68=p4(mapSize)；返 1。p2(@0x14) 未被使用。
// [N96 清绑 2026-09-09] 原 `register char* compatEax asm("eax")` 全域禁用（P0②/N96/
// P248 尾条），还原表达式直传 `reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)`。
// 我方 .o 逐条复现 ORIG `lea 0x4(%eax),%edx` setCopy 段。TU 三数零变化
//（identical=9 ae=19 near=0 diff=1），本函数 TU 口径 AE 不变（orig=51 our=51）。
#include "CreatureOnlySimple/AuctionRegist.inc"
}

// ============================================================================
// 全局池尺寸包装器（均转发 CCreatureItemFactory 单例查询）
// ============================================================================

// 【objdump】ORIG _ZN13user_creature22GetCreatureEggPoolSizeEv T 0x08336c63：
// GetInstanceCreatureItemFactory()->GetSize(type=0)（egg）。
#include "CreatureOnlySimple/GetCreatureEggPoolSize.inc"
}

// 【objdump】ORIG _ZN13user_creature19GetCreaturePoolSizeEv T 0x08336c80：
// GetInstanceCreatureItemFactory()->GetSize(type=1)（creature）。
#include "CreatureOnlySimple/GetCreaturePoolSize.inc"
}

// 【objdump】ORIG _ZN13user_creature28GetCreatureEggPoolRemainSizeEv T 0x08336c9d：
// GetInstanceCreatureItemFactory()->GetRemainSize(type=0)。
#include "CreatureOnlySimple/GetCreatureEggPoolRemainSize.inc"
}

// 【objdump】ORIG _ZN13user_creature25GetCreaturePoolRemainSizeEv T 0x08336cba：
// GetInstanceCreatureItemFactory()->GetRemainSize(type=1)。
#include "CreatureOnlySimple/GetCreaturePoolRemainSize.inc"
}

}  // namespace user_creature
