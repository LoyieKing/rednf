// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItemUpgrade_Separate/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CItemUpgrade_Separate.cpp — WongWork 分离强化系统全链 + GenuineDamageUpgradeTable
//                             （Ctor2_ContB）
//
// 全部实现按 docs/class_func_reports/{WongWork__CItemUpgrade_Separate,
// WongWork__IItemUpgrade,GenuineDamageUpgradeTable,CUser__makeGuildItemSeparateUpgradMessage}/
// 报告 + ORIG objdump 逐段转写。符号对照：
//   WongWork::IItemUpgrade        C1 T 0x080fc834 / D1 T 0x080fc842 /
//                                 item_upgrade_system T 0x080fc850
//   WongWork::STUpgradeInfo       C1 W 0x0811edfe
//   GenuineDamageUpgradeInfo      C1 W 0x0891baf0
//   OutLineEFFECTInfoTemp         C1 W 0x0891bb1e
//   GenuineDamageUpgradeTable     C1 W 0x0811ee5e / D1 W 0x0811eeda /
//                                 getGenuineUpgradeDamage T 0x08912a54 /
//                                 getGenuineUpgradeMaterialCnt T(_ZNK) 0x08912b88 /
//                                 getEffectInfo T 0x08912d10
//   UpgradeSeparateInfo           IncUpgradeSeparate W 0x0811ed32（声明补入 Inven_Item.h）
//   WongWork::CItemUpgrade_Separate C1 T 0x0811dfc0 / D1 T 0x0811e0be /
//                                 ProcUpgrade T 0x0811e66e / LoadUpgradeScript T 0x0811eca8 /
//                                 _CheckCondition T 0x0811e138 / _GetPenaltyType T 0x0811e2ce /
//                                 _GetUpgradeInfo T 0x0811e2e2 / _DoProcUpgrade T 0x0811e468 /
//                                 _NoticeUpgrade T 0x0811e53a / getUpgradeInfoTable W 0x0811ee16
//   CUser::makeGuildItemSeparateUpgradMessage T 0x0867a242（CUser.h 补声明）
// importGenuineUpgradeTable(T 0x08912336) 仅声明——定义归脚本导入域批次。
// ============================================================================
#include "CServerProxyMgr.h"         // CServerProxyMgr<T>::GetServerProxy 模板定义
#include "CItemUpgrade_Separate.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "PacketTypes.h"     // MSG_ITEM_UPGRADE 完整类型（具名字段访问）
#include "Inven_Item.h"
#include "CEquipItem.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CInventory.h"
#include "GameWorld.h"
#include "PvPTypes.h"                 // CMTRand 完整类型（0x9c8）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_.findString

#include <cstdio>
#include <cstring>
#include <string>

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN8WongWork21CItemUpgrade_SeparateC2Ev`（P327）。全限定定义可写在
// 命名空间块之外 ⇒ 发射序提前。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// ORIG T 0x0811dfc0：基类 vptr → 本类 vptr → 表构造 → +0x190=100000 →
// new CMTRand(time) 存 +0x18c → 100 次 randInt(m_randRange) 热身。
#include "CItemUpgrade_Separate/CItemUpgrade_Separate.inc"
}

namespace
{

union FloatBits
{
    unsigned int u;
    float f;
};

// ORIG .rodata 常量（位模式实证）：
FloatBits kMatEpsilon = { 0x33d6bf95UL };   // DAT_08e122f4 ≈ 1e-7f（材料放大前加值）
const float kGoldBase = 9840.0f;            // DAT_08b575b0（金币换算基数）
const float kRoundHalf = 0.5f;              // DAT_08e122e8（截断前置）

#include "CItemUpgrade_Separate/truncateToInt.inc"
}

}  // namespace

// ============================ POD 生命周期 ============================

#include "CItemUpgrade_Separate/GenuineDamageUpgradeInfo.inc"
}

#include "CItemUpgrade_Separate/OutLineEFFECTInfoTemp.inc"
}

namespace WongWork
{

#include "CItemUpgrade_Separate/STUpgradeInfo.inc"
}

// ============================ IItemUpgrade ============================

#include "CItemUpgrade_Separate/IItemUpgrade.inc"

#include "CItemUpgrade_Separate/dtor_IItemUpgrade.inc"
#include "CItemUpgrade_Separate/item_upgrade_system.inc"
}

// ============================ GenuineDamageUpgradeTable ============================

}  // namespace WongWork

#include "CItemUpgrade_Separate/GenuineDamageUpgradeTable.inc"

#include "CItemUpgrade_Separate/dtor_GenuineDamageUpgradeTable.inc"
#include "CItemUpgrade_Separate/getGenuineUpgradeDamage.inc"
}

// ORIG T(_ZNK…) 0x08912b88。
// [b28d-solo-CItemUpgrade_Separate] 重写：eps 死局部（-0x14，P232 扩展 float 二段
// 常量初始化）、count 具名预置 0（-0x10）、factor 1.0f 两句式（1.0f 落槽不被折）、
// 乘加拆两句（ORIG 不融合 fmuls/fadds）、第二次 end 检查照抄 ORIG 的 it==m_infos.end()
// 重复形（ORIG 0x08912c90 lea 0x158 实证；mit 无 end 检查，缺键时 ORIG/我方同为
// 越界解引用，行为一致）。p167 8=8 CLEAR。判词 N（N95/N82 + N92/N18a 族）：残余 =
// ①4.4 浅槽基恒比 ORIG 深 8（eps/count/factor -0x1c/-0x18/-0x14 vs -0x14/-0x10/-0xc；
// 帧 0x58 vs 0x68，加 char[0x10]/double pad 只会整体平移 0x10，N95 不可上移）；
// ②ORIG `mov -0x50,%eax; mov -0x4c,%edx` 的 64 位宽读 vs 4.4 只读低 dword（N92）。
// [2026-09-09 SweepCItemUpgrade] 复核（未改）：gap=1（orig 217 / our 216）即 ② 那 1 条
// 高 32 位读（0x08912cf8，fistpll 结果丢弃的高半字）；槽基常数差 8 为 N95。判词维持 N。
#include "CItemUpgrade_Separate/getGenuineUpgradeMaterialCnt.inc"
}

// ORIG T 0x08912d10（sret 返回；静态局部 temp 经 __cxa_guard 初始化）。
// [b28c-32] verdict=DIFF：N86/N82（sret 按值返回类对象 + 函数局部 static guard 临时槽跨版本差）。
// [b28d-solo-CItemUpgrade_Separate] 修布局：`if (m_maxLevel < key) return temp;` 早返形
// 复现 ORIG 0x08912d53 的 `jge <find块>` 正极性（旧 `>=` 包裹形 4.4 产 `jl <temp块>` 反向
// 布局）。残余 N86/N82：ORIG 两处独立 temp 拷贝块 vs 4.4 共享尾块（sret/临时槽跨版本差）。
#include "CItemUpgrade_Separate/getEffectInfo.inc"
}

// ============================ CItemUpgrade_Separate ============================

namespace WongWork
{

// ORIG T 0x0811e0be：delete m_rand 置空 → 表析构 → 基类析构（成员逆序自动）。
#include "CItemUpgrade_Separate/dtor_CItemUpgrade_Separate.inc"
}

// ORIG T 0x0811eca8：import 尾透传（int 返回零开销透传 eax；0=成功）。
#include "CItemUpgrade_Separate/LoadUpgradeScript.inc"
}

// ORIG T 0x0811e2ce：恒返回 1（报告全文仅此）。
#include "CItemUpgrade_Separate/_GetPenaltyType.inc"
}

// ORIG T 0x0811e2e2。
// [2026-09-09 SweepCItemUpgrade] 判词 N（ORIG 0x0811e2e2，setcc-fold + N7 族）：
// 本轮复核（未改）：gap=5（orig 121 / our 116）全部为跨版本形态差——
// ①ORIG 条件物化 `jg <主体>; mov $0,%eax; jmp; mov $1,%eax; test %al,%al; je`
//   （0x0811e303..0x0811e321，6 条）vs 4.4 `setle %al; test; je`（3 条）⇒ setcc-fold；
// ②get_rarity→%ebx / 虚调 GetItemType→%esi 与 4.4 的 %esi/%ebx 相反（N7，P230 判
//   4.4.6 后端敏感族）。p167 11=11 CLEAR（0 missing/0 extra），帧 push esi/ebx +
// sub $0x50 与 ORIG 全等；语义/调用集齐，无源码可翻。
#include "CItemUpgrade_Separate/_GetUpgradeInfo.inc"
}

// ORIG T 0x0811e138（const）。
// [b28d-solo-CItemUpgrade_Separate] 判词 N（ORIG 0x0811e138，N7 族）：修法——haveCount
// 直用 matItem.m_addInfo2（%edi 跨调用保活）、rarity/itemType/sepLv 全部内联实参、
// materialItemIdx 具名槽、m_fieldb 双零扩展 32 位比较、两次独立虚调 GetItemType
// （P282 真语义缺口已补）。p167 10=10 CLEAR；gap=1（133 vs 132）。残余仅 N7：
// 虚调目标 ORIG %edx vs 4.4 %eax、rarity/itemType 的 %ebx/%esi 角色互换及实参槽
// 0xc/0x10 互换（P230 判 4.4.6 后端敏感族）。
// [2026-09-10 SweepCEquipItem2] 本轮修法（P243 求值序）：末检改「sepLv+1 > m_maxLevel」
// 书写序——ORIG 0x0811e29c `lea 0x1(%eax),%edx; mov 0x1c(%ebp),%eax; mov 0x4(%eax),%eax;
// cmp %eax,%edx; setg` 先算左操作数；旧写法 `m_maxLevel < sepLv+1` 使 4.4 提前把
// m_maxLevel 装 %ebx（多 1 对 mov）并反出 setl。改后 clean 132=132、助记符序列全等，
// 本件 DIFF→NEAR（TU 四数 near 1→2 / diff 7→6）；p167 10=10 CLEAR 不变。
#include "CItemUpgrade_Separate/_CheckCondition.inc"
}

// ORIG T 0x0811e468。
// [b28d-solo-CItemUpgrade_Separate] 判词 N（ORIG 0x0811e468，setcc-fold/N2 族；p167
// 6=6 CLEAR）：形 k
// （探针 /tmp/b28d_51/p9.cpp k）复现 gold@-0xc、limit@%ebx 跨 randInt 保活、range@-0x10、
// 「limit 先求值→range 内联赋值→randInt」求值序、push %ebx+sub $0x24 帧。残余 5 条：
// ORIG 的 failed 为 4.1 寄存器化分支赋值（`jae;mov $1,%eax;jmp;mov $0,%eax;test %al,%al;je`），
// 4.4 对 `<` 恒 setcc（setb+test+je）且具名 bool 恒槽化（register/无初始化/两分支赋值
// p/q/m 三形实测：setb+分支写槽或 setb %bl），跨版本后端差不可造。语义/调用集齐。
// [2026-09-09 SweepCItemUpgrade] 复核（未改）：gap=3（orig 69 / our 66）即该 setcc-fold
// 段（ORIG 0x0811e508 `cmp; jae <false岛>; mov $1,%eax; jmp; mov $0,%eax; test; je`
// 6 条 vs 4.4 `cmp; setb %al; test; je` 3 条）。判词维持 N。
#include "CItemUpgrade_Separate/_DoProcUpgrade.inc"
}

// ORIG T 0x0811e53a：header 0x56 全服广播分离强化结果。
#include "CItemUpgrade_Separate/_NoticeUpgrade.inc"
}

// ORIG T 0x0811e66e：vtable 槽 0 主流程。
// [2026-09-09 SweepCItemUpgrade] 判词 N（ORIG 0x0811e66e，N7 + setcc-fold 族）：
// 本轮修法——① sepOld/sepNow 由 unsigned char 改 unsigned int（T1 直证：ORIG
// 0x0811e905 / 0x0811ea93 `movzbl %al,%eax; mov %eax,-0x24/-0x1c` 32 位存；uchar 恒
// 8 位存 + 回读 movzbl，实测多 3 条）；② 删 matSlot/slot 具名局部（P0：ORIG
// 0x0811e93d/0x0811e99e/0x0811eaae/0x0811ead5 每处重读 msg->m_materialSlot /
// m_targetSlot）；③ delete_item receiver 改链式右值（ORIG 0x0811e947 无槽往返）。
// gap 7→3（orig 393 / our 386→390），p167 43=43 CLEAR（0 missing/0 extra）。
// 残余：`cond != 0` 的 setne 物化 1 处（ORIG 0x0811e8e1 cmpl/setne/test/je，4.4 直跳）
// + 虚调目标 %eax vs %edx、rarity/itemType 的 %ebx/%esi 角色互换（N7，P230 判 4.4.6
// 后端敏感族）；帧 push edi/esi/ebx 与 ORIG 全等（sub 立即数差 0x10 = N115）。
// 按 §6.2 判语义完成、byte 层封顶。
// 虚槽核对（Main 2026-09-09 判据）：本 TU 共 5 处 `call *`，槽号逐处与 ORIG 一致——
// ① item_upgrade_system 槽 0（ORIG 0x080fc889 `mov (%eax),%edx`）；② _GetUpgradeInfo
// 0x892 `add $0xc`（ORIG 0x0811e354，CItem::GetItemType）；③④ _CheckCondition
// 0xa65/0xab9 `add $0xc`（ORIG 0x0811e1fc/0x0811e250）；⑤ 本函数 0x1238 `add $0x4c`
// = CItem::IsRandomOption（ORIG 0x0811ebdd）。无槽取错。
// [2026-09-10 SweepCEquipItem2] 本轮修法（P198① 预清两句式）：cond / success 由
// 初始化式改「声明=0/false + 独立赋值」——ORIG 0x0811e894 `movl $0x0,-0x28`、
// 0x0811e90b `movb $0x0,-0x1d` 均为 call 前预清、call 后回填，合写会丢两行。
// gap 3→1（fresh .o orig 393 / our 392），p167 43=43 CLEAR，四数零回退。
// 残余（N 封顶）：setne 物化 1 处、Inven_Item 槽基 -0xf6↔-0xb6（帧 +0x10 = N115）、
// historyLog 取址 `lea 0x79700(%edx),%ecx` vs `add $0x79700,%edx`（N107）、
// success 的 %esi↔%edi（N7）。
#include "CItemUpgrade_Separate/ProcUpgrade.inc"
}

}  // namespace WongWork

// ============================ UpgradeSeparateInfo ============================

// ORIG W 0x0811ed32：低 5 位自增（模 32），保留高位（bit5=交易限制等）。
#include "CItemUpgrade_Separate/IncUpgradeSeparate.inc"
}

// ============================ CUser 公会公告 ============================

// ORIG T 0x0867a242：成功且新等级>5 用 game_server_msg_248；失败且旧等级>5 用
// game_server_msg_249；前缀 game_server_msg_247 取 10 字节；长度<120 才发公会板。
// [2026-09-09 SweepCItemUpgrade] 判词 N（ORIG 0x0867a242，4.1 EH bool 归一化族）：
// 本轮修法——① 删 prefixStr 具名局部（P0：ORIG 0x0867a299→0x0867a2b7 findString 结果
// 经 %eax 直传 strncpy 第 2 参，无槽往返）；② boardReady 改裸 `register bool`（P231：
// ORIG 全程 %ebx 承载无栈槽，4.4 由 movb×3+cmpb 槽往返改寄存器保活）。p167 71=71
// CLEAR（0 missing/0 extra）。残余（gap=29，orig 512 / our 483）：ORIG 在 4 个 string
// 析构点各发 6 条 bool 归一化块（`test %ebx,%ebx; jne; mov $0,%ebx; jmp; mov $1,%ebx`，
// 0x0867a556 / 0x0867a588 / 0x0867a872 / 0x0867a8a4，共 24 条）——4.1 对 bool 局部在
// EH cleanup 点的规范化，4.4 不生成，跨版本不可复现；余下为槽基常数差
// （board -0xb8 ↔ -0xc4、帧 0x2dc ↔ 0x2fc = N115 outgoing 预留）。按 §6.2 判语义完成。
// [2026-09-10 SweepCEquipItem2] 本轮修法（P0/P52 + P198① + P222）：
// ① 删 guildkey/characNo/proxy 三个具名局部改单条链式表达式（ORIG 0x0867a8ec/
//    0x0867a8f9 把两 getter 结果留 %esi/%ebx 跨 GetServerProxy 保活、无槽往返）；
// ② boardReady 去初始化式改「未初始化声明 + if/else 双赋值」（ORIG 0x0867a4fa/
//    0x0867a52f/0x0867a816/0x0867a84b 两臂各 mov $0/$1,%ebx、函数头无预置）；
// ③ sprintf 分支改先写 upgrade != 0（ORIG 0x0867a455 `je` 落 "%s %d" 臂）。
// 改后帧 0x2ec→0x2dc（与 ORIG 全等）、board/prefix/msgBuf/tail 槽位与 ORIG 对齐
// （msgBuf 差 1 字节）、tail 清零慢路逐条相同；p167 71=71 CLEAR（仅 C1/C2 同址别名伪差）。
// 残余（gap=32，orig 512 / our 481，N 封顶）：3 个 bool EH 归一化块 15 条 + 尾 test 2 条
// （4.1 独有）、`seta`↔`setbe`（setcc-fold）、success 的 `xor $1` 物化 3 条、msgBuf 槽
// 差 1 字节（2 条 lea）、boardReady 末尾 `test %ebx`↔`test %bl`。
#include "CItemUpgrade_Separate/makeGuildItemSeparateUpgradMessage.inc"
}
