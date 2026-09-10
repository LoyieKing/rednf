// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CKillMonsterInfo_chain/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CKillMonsterInfo_chain.cpp —— 击杀/奖励链第三阶依赖（LinkKillChain 批次）。
//
// 覆盖 8 个 ORIG 真实符号 + 3 个链上内部依赖（全部经 docs/class_func_reports/
// 对应报告 ## 汇编 + ## 反编译 C 完整读取后忠实转写；无桩）：
//   CKillMonsterInfo::{Check,Register}BossKill        T 0x0854d184 / 0x0854d418
//   CKillMonsterInfo::{Check,Register}APCBossKill     T 0x0854d1e8 / 0x0854d48c
//   CKillMonsterInfo::{Check,Register}NamedKill       T 0x0854d1b6 / 0x0854d452
//   CKillMonsterInfo::GetDungeonDifficultyFlag        T 0x0854d21a（链上依赖）
//   CKillMonsterInfo::{Check,Register}MonsterKill     T 0x0854d0ea / 0x0854d24c（依赖）
//   CHellPartyItemStatistic::SendHellPartyItemStatistic T 0x0860fcc0
//   GetInstanceHellPartyItemStatistic                 T 0x0860fcb6
//
// CKillMonsterInfo 权威头 CKillMonsterInfo.h（继承 charac_expand::CData，
// 三个 std::map<uint,uchar> 子对象位于 +0x8/+0x20/+0x38，ctor 报告实证）；
// 上述谓词/登记/难度标志均以真实成员函数定义（桥清零，b21 批次）。
//
// CHellPartyItemStatistic 权威头 CHellPartyItemStatistic.h；本 TU 提供
// SendHellPartyItemStatistic 真实成员定义（桥清零）。单例 GetInstanceHellPartyItemStatistic
// 为自由函数，提供真实定义（函数级 static，ORIG .bss _ZZ33...E8instance
// @0x0947334c，下一符号距 0x24 → sizeof 0x24）。
//
// 统计包 Packet_HellParty_Statistic_Item（ORIG ctor 0x0860ff9e =
// PacketHeader(0xc31, 0x2a)）本工程仅前向声明（Statistics.h），此处 TU 局部补全；
// 字段语义沿用 StatisticManager::WriteHellPartyStatisticItem 的 Wire 视图。
// ============================================================================

#include <map>
#include "CKillMonsterInfo.h"
#include "../../../shared/packet/include/Packet_HellParty_Statistic_Item.h"
#include <string.h>

#include "CBoosterGage.h"          // charac_expand::CData（alter）
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h         // 经 WarField.h 引入 CAICharacterList/CDataManager 宿主
#include "CDataManager.h"          // G_CDataManager / find_monster / m_aiCharacterList
#include "LogManager.h"            // LogManager::logFormat
#include "CServerProxyMgr.h"      // CServerProxyMgr<T>::GetServerProxy 完整模板
#include "PacketHeader.h"          // PacketHeader(ushort, ushort)
#include "InterfacePacketBuf.h"    // PacketGuard 发送域接口
#include "GlobalData.h"            // GlobalData::s_statistic_proxy_mgr
#include "CStatisticServerProxy.h" // CStatisticServerProxy::SendPacket
#include "CStreamGuard.h"         // CStreamGuard(Stream*,bool) / Stream
#include "MsgQueueMgr.h"          // MsgQueueMgr::put(DB_Q, CStreamGuard&)
#include "SigTypes_C.h"           // SIG_LOAD_KILL_MONSTER_INFO
#include "CHellPartyItemStatistic.h"  // 地狱派对统计（桥清零，权威头）

// ============================================================================
// CKillMonsterInfo 域 —— 桥清零：原 freestanding asm(_ZN…) shim 转为真实成员
// 函数（权威头 CKillMonsterInfo.h，返回型 void→bool 按 ORIG 报告；mangled 不含
// 返回型零 ABI 风险）。槽 map 经成员 m_bossKills/m_namedKills/m_apcBossKills
// 直接访问（+0x8/+0x20/+0x38），与 ORIG add $0x8/0x20/0x38 同形。
// ============================================================================

// ORIG C1 0x0854c9b4：CData 风格 vptr 后三 map 默认构造 + _reset()。
#include "CKillMonsterInfo_chain/CKillMonsterInfo.inc"
}

// ---- GetDungeonDifficultyFlag —— ORIG T 0x0854d21a -------------------------
// diff<=4 ? (1<<diff) : 0（shl %cl 位型实证；返回 int，写 map 值时按 byte 截断）。
#include "CKillMonsterInfo_chain/GetDungeonDifficultyFlag.inc"
}

// ---- CheckMonsterKill(map const&, mobIdx, diff) —— ORIG T 0x0854d0ea -------
// find(mobIdx)!=end 且 (diffFlag & 记录值) 非 0 → true。
// [b28d-solo-CKillMonsterInfo_chain 判词 · AE] ORIG 0x0854d0ea（49 条）：
// const_iterator::operator!=（ORIG `ne(PKi,RKi)`）、字节宽度判定
// `test %al,%al; setne`（uchar 物化）、共享 false 尾块均对齐（P167 CLEAR）。
// 关键旋钮（P231/P248 档②）：`register unsigned char rec = it->second;` +
// 条件 RHS 侧 `(unsigned char)GetDungeonDifficultyFlag(diff)` —— 使 ORIG 0854d149
// 的 `movzbl 0x4(%eax),%ebx` 单段直落 callee-saved 复现；无 register 时 4.4 恒
// `movzbl …,%eax; mov %eax,%ebx`（50/49），RHS 不加 uchar cast 则丢 setne 段。
// 现 49=49 助记符/槽位逐条全等，残余仅 .o 重定位地址显示（P264）。
#include "CKillMonsterInfo_chain/CheckMonsterKill.inc"
}

// ---- RegisterMonsterKill(map&, mobIdx, diff, isAPC) —— ORIG T 0x0854d24c ---
// diff>4 → false；isAPC ? CAICharacterList::get==0 拒绝 : find_monster==0 拒绝；
// 已有记录 → flag |= diffFlag 并 alter()（charac_expand::CData::m_bIsSave=1）；
// 否则 insert(make_pair(idx,diffFlag))，插入失败记日志返回 false，成功 alter()。
// [b28d-solo-CKillMonsterInfo_chain 判词 · NEAR/N93] ORIG 0x0854d24c（129 条）：
//  · 已对齐：`if (!isAPC)` 极性（ORIG `movzbl isAPC; xor $1; test; je <APC>`）、
//    两校验结果具名槽（-0x10 find_monster / -0xc get + `cmpl $0x0,槽; jne`，
//    P0 槽往返）、两成功路径共享尾 `mov $1,%eax`、make_pair 右值 uchar、
//    单次 operator->（P167 CLEAR 16=16）。
//    ORIG 0854d325 `movzbl 0x4(%ebx),%esi` 单段直落亦已复现——复合赋值 RHS 写
//    `(unsigned char)GetDungeonDifficultyFlag(diff)` 即消 `mov %eax,%esi` 中转
//    （P248 档② 同族，129=129）。
//  · 残余（N93/N98 封顶）：仅 `it`(ORIG -0x34/-0x30) 与 `ins`(ORIG -0x2c/-0x28)
//    槽位互换——4.1 按声明序先声明深，4.4 按首次使用序先使用浅；it 的 find 必
//    先于 ins 的 insert，声明序翻转无效（N98），块内提头部需 ins 默认构造 ⇒
//    多 pair C1Ev call + 8 字节拷贝（违反 P215/发明符号禁令）⇒ 结构性封顶。
#include "CKillMonsterInfo_chain/RegisterMonsterKill.inc"
}

// ---- 六个公开谓词 -----------------------------------------------------------
// Check/Register{BossKill,NamedKill,APCBossKill} 分别转发至 +0x8/+0x20/+0x38
// 槽 map；Register 的 APC 标志按 ORIG：Boss=false、Named=false、APCBoss=true
// （见各报告反编译 C）。槽地址由 this 直接偏移（与 ORIG add $0x8/0x20/0x38 同形）。
#include "CKillMonsterInfo_chain/CheckBossKill.inc"
}

#include "CKillMonsterInfo_chain/RegisterBossKill.inc"
}

#include "CKillMonsterInfo_chain/CheckNamedKill.inc"
}

#include "CKillMonsterInfo_chain/RegisterNamedKill.inc"
}

#include "CKillMonsterInfo_chain/CheckAPCBossKill.inc"
}

#include "CKillMonsterInfo_chain/RegisterAPCBossKill.inc"
}

// ============================================================================
// CHellPartyItemStatistic 域
// ============================================================================

// ORIG 包类 Packet_HellParty_Statistic_Item（ctor 0x0860ff9e = PacketHeader
// (0xc31, 0x2a)）：权威定义见 shared/packet/include/Packet_HellParty_Statistic_Item.h
//（布局：+0xa bool type / +0xb int dungeonIndex / +0xf char diff /
//  +0x10 char partyCount / +0x11 char hellpartyDiff / +0x12 int data[6]，
//  与 ORIG mangled _ZN31Packet_HellParty_Statistic_ItemC1Ev 一致）。

// ---- SendHellPartyItemStatistic —— ORIG T 0x0860fcc0 -----------------------
// dungeonDiff>4 或 scriptIdx==0 → false；否则组包经统计代理（组 0）发送 0x2a 字节。
// [b28d-solo-CKillMonsterInfo_chain 判词 · AE] ORIG 0x0860fcc0（50 条）：
// 删具名 proxy 局部、改单条链式表达式（P52）后 `lea -0x32(%ebp),%ebx` 跨
// GetServerProxy call 保活复现，push %ebx + 帧 $0x54 + 槽 -0x28/-0x32 全对齐
// （旧版具名 proxy 帧 $0x58 无 push）。50=50 助记符/槽位逐条全等，P167 CLEAR；
// 残余仅 .o 重定位地址显示（P264）。
#include "CKillMonsterInfo_chain/SendHellPartyItemStatistic.inc"
}

// ---- GetInstanceHellPartyItemStatistic —— ORIG T 0x0860fcb6 ---------------
// 函数级 static 单例（ORIG .bss instance 0x0947334c，占 0x24 字节）。
void* GetInstanceHellPartyItemStatistic()
{
    static unsigned char instance[0x24];
    return instance;
}

#include "CKillMonsterInfo_chain/dtor_CKillMonsterInfo.inc"
}

// ORIG T 0x0854d0b8：三 map clear。
#include "CKillMonsterInfo_chain/_reset.inc"
}
// ============================================================================
// ORIG 数据序列化域 —— ConvertStructToMap / ConvertMapToStrcut / getData /
// loadData / _saveData（OrigKillMob 批次；逐报告 ## 汇编 + ## 反编译 C 转写）
// ============================================================================


// ---- ConvertStructToMap —— ORIG T 0x0854cb42 ------------------------------
// for 循环遍历 st 数组 count 项；mobIdx==0 提前返回 1。bAPC ? 经
// CAICharacterList::get 校验 : CDataManager::find_monster 校验；失败 logFormat
// 后 continue（ORIG jmp +0x158 循环增量实证）；通过则共用插入块
// insert(make_pair(mobIdx, diff))（make_pair 实参序 = 0x4(%esp) mobIdx /
// 0x8(%esp) diff 实证）。
// [b28d-solo-CKillMonsterInfo_chain 判词 · AE] ORIG 0x0854cb42：100/100
// 助记符/槽位逐条全等（check_tu_routed 路由口径），P167 CLEAR。清单旧标
// NEAR ndiff=9 系 .o 重定位地址显示差（P264：字符串/函数地址未解析），
// 非助记符差异；TU 口径归 AE。
#include "CKillMonsterInfo_chain/ConvertStructToMap.inc"
}

// ---- ConvertMapToStrcut —— ORIG T 0x0854ccbc（const）----------------------
// empty→1；size>count→0；const_iterator 默认构造后 = begin()（P28 临时中转），
// while (it != end && i < count) 逐项回写 st[i]（i*5 步长、两次独立重算）。
#include "CKillMonsterInfo_chain/ConvertMapToStrcut.inc"
}

// ---- getData —— ORIG T 0x0854ce90（const，void）---------------------------
// 三 map 顺序落盘：+0x0 / +0x1d4c / +0x927c（1500 / 6000 / 1500 项 × 5 字节）。
#include "CKillMonsterInfo_chain/getData.inc"
}

// ---- loadData —— ORIG T 0x0854cddc ----------------------------------------
// 开头虚调 vptr[+0x1c] = _reset()（ORIG vtable 槽 7 实证）；三块顺序加载，
// APC 槽 bAPC=true（第三块）；返回 true。
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
#include "CKillMonsterInfo_chain/loadData.inc"
}

// ---- _saveData —— ORIG T 0x0854cf16 ---------------------------------------
// BigStreamPool::Acquire → CStreamGuard(stream,true) → <<0x281/<<UID →
// GetInBuffer<SIG_LOAD_KILL_MONSTER_INFO>；非空则清零 0xafcc、写 characNo、
// 三 map 落 SIG 缓冲，put(DB_Q, guard)；返回 pSig!=0。
#include "CKillMonsterInfo_chain/_saveData.inc"
}
