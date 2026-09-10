// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_LoadInventory/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_LoadInventory.cpp
// 目标：清零 /tmp/only_orig/DB_LoadInventory.txt 全部 14 个 ORIG 符号。
// 每函数按 docs/class_func_reports/DB_LoadInventory/<fn>.md（汇编为准）转写。
// SIG_LOAD_INVENTORY 权威布局见 SigTypes_C.h（pack(1)，size 0x1544c）：
//   +0x004 m_characNo | +0x008 m_field8(tag charac_no) | +0x00c m_tagInven
//   | +0x4030 m_result | +0x4034 m_money | +0x4044 m_invenType2（avatar 主区）
//   | +0x5949 m_invenType1 | +0xa3a1 m_equipMirror | +0xa8e0 m_cargoCapacity
//   | +0xa8e4 m_cargo | +0xcd1c m_avatarCount | +0xcd20 m_avatarDetail
//   | +0xeed1 m_invenType3（creature 槽位区，步长 0x3d）| +0x1287c m_creatureCount
//   | +0x12880 m_creature（0x38 步长详情条目）| +0x14780 m_avatarCoin
//   | +0x14784 m_expandData2（GetItemLock 输出区）| +0x1533c m_inventoryCapacity
//   | +0x15448 m_maxEquipLevel
// ============================================================================

#include <string.h>
#include <stdio.h>
#include <vector>

#include "DB_LoadInventory.h"

#include "MySQL.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"          // cMyTrace
#include "LogManager.h"         // logFormat
#include "AdvanceAltarZip.h"    // uncompress_zip 权威声明
#include "Inven_Item.h"
#include "GamePODTypes.h"      // SIG_CREATURE_ITEM_DETAIL_INFO/AvatarDetailInfo 权威
                              // 轻量叶子头（ORIG 本 TU 无 ios_base static-init）
#include "CItemAmplifier.h"     // stAmplifyOption_t::SetLock/GetLock
#include "W8LT_DBTask.h"        // DB_SwapCreatureItem 权威声明（makeRequest）

// ---- 本 TU 转接声明（UNLINKED_CALLEE，源码暂无 owner；照常调用，保持
// ORIG 全局 mangled 名以便链接器命中真实 T 符号）----
// ORIG T _Z24get_compressed_blob_dataP5MySQLiPcii @0x083fbc79
// （定义在 DB_Login.cpp:1967）。
#include "DB_LoadInventory/get_compressed_blob_data.inc"

extern char* NumberToString(unsigned int value, int radix);  // C++ linkage

// ============================================================================
// DB_LoadInventory::DB_LoadInventory @0x0840a604（C1/C2 同址）
// ORIG：call DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D1 0x0840a620 设 vptr→基类空 dtor→恒假 delete 分支；
// D0 0x0840a650 再 operator delete）由类定义自然产出同形符号。
// ============================================================================
#include "DB_LoadInventory/DB_LoadInventory.inc"
}

// ============================================================================
// ~DB_LoadInventory @0x0840a620（D1/D2）/ deleting dtor @0x0840a650（D0）
// ============================================================================
#include "DB_LoadInventory/dtor_DB_LoadInventory.inc"
}

// ============================================================================
// DB_LoadInventory::dispatch @0x0840a66e
// 基类 dispatch 通过 → GetOutBuffer<SIG_LOAD_INVENTORY> → GetUserInven/
// GetUserAvatar/creature 槽 reset/GetUserCreature/_getCharacInvenExpand/
// GetItemLock/_GetEquipMaxItemLevel，任一步失败 throw int（1/2/3/4/6/7），
// catch 后置 sig->m_result=1 继续 SendResult。错误行号 0x2364..0x23a4。
// b28d-solo-DB_LoadInventory：P227 全表达式临时链（6 处 trace 的 ebx 保活）
// + 尾 SendResult 具名 bool 早返已归位；残差 = 4.4 EH 帧/槽布局（sub 0x94 vs
// 0x84、sig -0x1c vs -0x14、catch 段 __cxa_get_exception_ptr + _Unwind_Resume）
// ⇒ N104/跨版本 EH 封顶；p167 差集 MISSING=0（语义完整）。
// ============================================================================
#include "DB_LoadInventory/dispatch.inc"
}

// ============================================================================
// DB_LoadInventory::SendResult @0x0840da6e：直接转发 SendResult_forchina。
// ============================================================================
#include "DB_LoadInventory/SendResult.inc"
}

// ============================================================================
// DB_LoadInventory::SendResult_forchina @0x0840d98c
// BigStreamPool::Acquire("DBThread.cpp",0x2939) → guard<<userIdx<<slot →
// put_binary(sig,0x1544c) → MsgQueueMgr::put(s_msgq_mgr,(QUEUE_IDX)1,guard)。
// ============================================================================
#include "DB_LoadInventory/SendResult_forchina.inc"
}

// ============================================================================
// DB_LoadInventory::GetUserInven @0x0840b0d8
// 列序 money(0)/coin(1)/inventory(2)/equipslot(3)/pay_coin(4)/event_coin(5)/
// creature(6)/creature_flag(7)/inventory_capacity(8)。三 blob 分别解压到
// +0x5949 / +0xa603 / +0x1102d，容量 0x473e/0x37a/0x1d36；尺寸校验
// equip 0x53e / inven 0x4a58 / creature 0x4a58 对 0x3d 对齐。随后
// member_avatar_coin 聚合查询入 +0x14780；tag charac（+0x8）非零走
// _GetTagCharacInven。blob 溢出/uncompress 失败 throw unsigned int
// （0x255a/0x255c/0x2563/0x2565/0x2567/0x256e），catch 后 logFormat
// 0x259c 并返 0。错误行号 0x24cb..0x25b2。
// b28d-solo-DB_LoadInventory：ok 变量（14 处共用槽）+ P227 临时链 +
// len/binLen 副本（P171，声明序对齐 ORIG 槽）已归位；残差 = 4.4 标量分区
// （ok -0x25 vs -0x31、binLen 副本整体深 4）⇒ N82 封顶；p167 MISSING=0。
// 本轮复核修复（真语义 bug）：creature blob 解压目标。ORIG 0840b8ef
// `add $0x1102d,%eax`（= 0xeed1+0x8c×0x3d = m_invenType3[0x8c] 首槽，
// 0x8c 正是 dispatch reset 循环 0..0x8b 之后第一个 creature 槽）；旧译
// `&m_invenType3[72].m_reserved` 编译成 +0x1002d，与 ORIG 差 0x1000 字节
// ⇒ blob 落到错误槽区。已改为 `&m_invenType3[0x8c]`，.o 立即数 0x1102d
// 与 ORIG 逐字节一致（四数不变、p167 81/81 CLEAR）。
// [SweepDBLoadEtcMain 本波] **gap 4→0（725/725）、p167 CLEAR、jcc 49/49 逐位核过**，
// 修 4 处真 bug + 2 处源形：
//   ① 三处「incorrect inventory size」日志第 3 参误用 len（解压后容量）——ORIG
//      0840b9a1/0840ba2b/0840baba 读的是 `-0x2c/-0x30/-0x28`（binLen，DB 压缩尺寸）
//      ⇒ 改 equip/inven/creatureBinLen（日志契约 original_size,db_size 才自洽）；
//   ② `char* m = NumberToString(...)` 具名局部：ORIG 0840bb7a `mov %eax,0x8(%esp)`
//      直压实参、无 m 槽（P0 槽-局部对应律）⇒ 内联进 set_query 实参位；
//   ③ col 具名局部（ORIG -0x20，`movl $0x6` + 后三调用 `col++`）：补后
//      get_int/get_uint 取址回到 ORIG 的 `lea 0x1287c(%eax),%edx` 形（旧字面量形
//      发 add 且少 col 的 8 条装载/自增）；
//   ④ 三个 binLen 与 len 同型化 unsigned（P211 档③/P209）：int 型时 4.4 走
//      `mov binLen,%edx; mov L,%eax; cmp %eax,%edx`（+1 条×3），unsigned 后复现
//      ORIG `mov L,%eax; cmp %eax,binLen; jbe` 两段 mem-dst 形
//      （探针 /tmp/p209p/q.cpp p7↔p9）。
// 残余 = N104（catch/unwind 块 29 条内容逐条全等、仅相对 % 检查块位置互换）+
// N82（ok -0x25 vs -0x31、binLen 组整体深 4）+ 3 处 `jbe↔jae`（cmp 操作数方向
// fold：ORIG `cmp %eax,B`=B−L 配 jbe，我方 `cmp B,%eax`=L−B 配 jae，落点同体）。
// ============================================================================
#include "DB_LoadInventory/GetUserInven.inc"
}

// ============================================================================
// DB_LoadInventory::GetUserAvatar @0x0840bd3a
// 主 charac（+4）与 tag charac（+8，非零才做）各调 _GetUserAvatar：
//   主：sig+0xcd20 / sig+0x4044 / sig+0xa3a1 / sig+0xcd1c
//   tag：sig+0x1e53 / sig+0x54a / sig+0xc / sig+0x1e4f
// 错误行号 0x25cb / 0x25d7。
// b28d-solo-DB_LoadInventory：AE（87=87 仅重定位差）。修法 = P59 guard 形
// （if(!ok) 早返，ORIG `movzbl; xor $1; test; je` fail 直落）+ P227 全表达式
// 临时链（实参入 %ebx 跨 ctor 保活）。
// ============================================================================
#include "DB_LoadInventory/GetUserAvatar.inc"
}

// ============================================================================
// DB_LoadInventory::_GetUserAvatar @0x0840be8a（static bool）
// §6.2 收口注记（b28d 复攻，gap 76→1）：语义/列序/行号与 ORIG 全对齐。
// 本轮修复（逐段对照 ORIG 汇编 + LCS 段级归一化多重集）：
//   ①【真语义 bug】slot<10 与 slot>=10 的目标数组写反：ORIG 0840c971 的
//     slot<10 分支基址是 `add 0x14(%ebp),%eax`（第 4 形参 = 调用点 sig+0xa3a1），
//     0840c895 的 idx 分支是 `add 0x10(%ebp),%eax`（第 3 形参 = sig+0x4044）；
//     旧码 slot<10 写 inven（第 3 参）、slot>=10 写 mirror（第 4 参）——反了。
//   ② P183 去缓存：删 `AvatarDetailInfo& e = out[row]` 与 `Inven_Item* item`，
//     每写点重发 `mov row,%eax; imul $0x4b; add 0xc(%ebp),%eax`（14 写点 ×3 条）。
//   ③ 列索引具名局部 `int col = 7;` + `get_*(col++)`（8 处 ×2 条）+ col10/col11
//     错误分支 P227 全表达式临时链用 `col - 1`。
//   ④ P312：ipgAgencyNo 声明 0x19 字节 ⇒ 落非 4 对齐槽 -0x5d，4.4 发与 ORIG
//     同形的「对齐剥离 + rep stosl」慢路（0840bf59..0840bfbd）。
//   ⑤ 声明序照 ORIG 清零序 uiId→clearAvatarId→count→abilityNo；slot/itId 去 `=0`。
//   ⑥ color1/color2 写点前移到 get_byte(14) 之前（ORIG 0840c6fe/712 < 0840c726）。
//   ⑦ P223 三失败出口 goto 共享尾标签 ⇒ 复现 0840ca41/44/47 三个 `nop;jmp` 岛。
//   ⑧ trade_restrict 位：常量位域赋值 `m_bits.m_flag = 1/0` 复现 ORIG 单次基址
//     `movzbl 0x33(%eax); or $0x20/and $0xdf; mov %dl,0x33(%eax)`（m_field 复合
//     赋值 4.4 恒两次基址 +3 条/处；探针 /tmp/p_sep.cpp f1 vs f4）。
// 剩余真缺口 = 0；唯一净差 1 条 = count 赋值 `mov %eax,%edx; mov 0x18(%ebp),%eax;
//   mov %edx,(%eax)`（4.1 RHS-first+中转 vs 4.4 LHS-first 直存）⇒ N76/P211 封顶：
//   异型化旋钮在共享头 MySQL::get_n_rows 返回类型（改它牵动全仓符号性比较），
//   按 S2 不在本泳道改；unsigned 视图/cast/字节视图三形实测均劣
//   （698/710/712 vs 695，且视图形占用 %ebx/%edx 破坏 callee-saved 布局）。
// 残差余项 = 槽号分区（db -0x1c vs -0x18、trace -0x7c vs -0x12c）⇒ N82/N95 豁免。
// p167 调用点差集 = C1/C2 同址别名伪差（59/59，P93 豁免）。
// user_items 15 列（limit 0x73）。AvatarDetailInfo 条目步长 0x4b：
//   +0 ui_id / +4 expire / +8 reg / +0xc ipgAgencyNo(0x18) / +0x25 clearAvatarId
//   / +0x29 jewelSocket(0x1e) / +0x47 color1 / +0x49 color2。
// slot<10 → 第 4 形参（mirror 名，调用点 sig+0xa3a1）；10<=slot-10<0x69 →
// 第 3 形参（inven 名，调用点 sig+0x4044）。Inven_Item 写：m_field1=8、
// m_fieldb=ability_no、m_addInfo=it_id、m_addInfo2=ui_id、m_amp lock、
// m_fielddLo=hidden、m_avatarEndur=endurance、+0x33 trade_restrict 位 0x20。
// 错误行号 0x26dc..0x277d（col12 失败仅记日志）。
// b28d-solo-DB_LoadInventory：17 处 trace 改 __PRETTY_FUNCTION__（ORIG 有
// _ZZ...E19__PRETTY_FUNCTION__ 符号）+ 前 6 处 db 检查改共用 bool ok（ORIG
// `mov %al,-0x19`）。
// ============================================================================
#include "DB_LoadInventory/_GetUserAvatar.inc"
}

// ============================================================================
// DB_LoadInventory::GetUserCreature @0x0840ca74
// creature_items 12 列；行数入 +0x1287c（>=0x8e 拒绝）。slot==0xee 映射到
// 0x8c 槽；槽位占用标记 used[0x8d]；重复槽 → Inven_Item 进 overflow vector，
// 遍历后填首个空槽并调 DB_SwapCreatureItem::makeRequest(ui_id, slot)。
// 主槽位（m_invenType3，步长 0x3d）：m_field1=5、m_fieldb=endurance、
// m_addInfo(+0x02) 单条 movl 4B ← it_id（Ghidra 的 8B ulong 表达为伪影，
// GetUserCreature.md 实证 +0x02/+0x07 两条独立 movl）、m_addInfo2=ui_id、
// m_amp lock。详情条目
// （+0x12880，步长 0x38）按 SIG_CREATURE_ITEM_DETAIL_INFO 写点填充。
// 错误行号 0x27f0..0x28e9。
// b28d-solo-DB_LoadInventory：exec/fetch/get_int(0) 共用 bool ok（ORIG -0x1d）
// + 13 处 trace 全表达式临时链（ebx 保活）已归位；残差 = 4.4 标量/临时槽分区
// （帧 0x280 vs 0x2c0、db -0x34 vs -0x1c）+ 循环体局部块（ORIG 单次 memset
// 0x74 合并清零 vs 4.4 逐条初始化）⇒ N82 封顶；p167 差集 MISSING=0。
// ============================================================================
// [L13-group6 判据⑫ 复核] widthscan：clr_l orig=3 / our=12（我方多 9 条
//   `movl $0x0,-N`）+ clr_w 0/1 + cmp_b 1/0 + movzbl 13/14。逐条回 ORIG 同下标
//   核对：ORIG 用**单次 `memset(local_220,0,0x74)`（116B 栈缓冲）合并清零**，
//   我方把同一缓冲拆成 12 个独立具名局部（name[16]/itId/endurance/slot/uiId/
//   stomach/exp/creatureType/noCharge/stat/expire/lockKey）各自 `= 0`。
//   ⇒ 属**结构建模差异**（P250 packed 栈缓冲），非类型宽度问题；修法需按 ORIG
//   0x74 布局建具名缓冲结构 + 整块 memset（改面较大，本波未动，留作后续首选）。
// [IG1 2026-09-10 语义轴落地·used 判定形] ORIG 0x0840cd16/0x0840d556 为
//   `movzbl used[i]; xor $1,%eax; test %al,%al; je` ⇒ 原码是 `if (!used[i])`
//   且元素按布尔语义（P248 档②）；我方原写 `used[i]==1`（多出 `cmp $0x1`，
//   清单 A 组候选）。已改 `char used[]`→`bool used[]` + 两处 `!used[...]`，
//   两处逐条同 ORIG。our_insns 733→735（补 2 条 ORIG 已有的 xor）、
//   ndiff 644→636（严格改善）、gap -2→-4、TU 四数不变 23/62/0/4。
//   余量：dupSlot 判定 ORIG `cmpb $0x0,-0x11` vs 4.4 `xor $1;test;je`（N122 族）。
#include "DB_LoadInventory/GetUserCreature.inc"
}

// ============================================================================
// DB_LoadInventory::_getCharacInvenExpand @0x0840ad2a（const）
// charac_inven_expand 4 列：cargo_capacity(0)→+0xa8e0，cargo(1)→+0xa8e4，
// jewel(2)→+0x949e，expand_equipslot(3)→+0x14aa4。容量 0x2b82/0x120f/0xa5c；
// 解压后 cargo/jewel 按 0x3d 对齐校验，expand_equipslot 按 (size-4)%0x3d。
// 返回 LineFunc 风格错误行号 0x243b..0x2463，成功 0。
// b28d-solo-DB_LoadInventory：len/binLen 副本（P171）+ 清零三形（memset call /
// 对齐剥离慢路 / 简单 rep stos）已归位（277 vs 278，残差 9 行）
// ＝N107（`&sig->m_cargoCapacity` 4.4 恒 add $0xa8e0 两步 vs ORIG lea；字段须
// unsigned 不可改）+ P75（3 处 uncompress_zip arg0/arg4 装载序跨版本）；
// p167 差集 MISSING=0。
// SweepCInventoryTail（2026-09-09）：补回 ORIG 实有静态符号
// `_ZZNK16DB_LoadInventory21_getCharacInvenExpand...E9handleIdx`（.rodata size 4，
// nm 实证；旧注释误判「本函数无 handleIdx」系漏读 K 前缀）⇒ 恢复
// `static const int handleIdx = 3;`，4.4 使用点仍折叠为立即数 3（指令数不变）。
// ============================================================================
#include "DB_LoadInventory/_getCharacInvenExpand.inc"
}

// ============================================================================
// DB_LoadInventory::_GetTagCharacInven @0x0840aba2（static bool）
// inventory 表 equipslot 列 → get_compressed_blob_data 解压进
// TAG_CHARAC_INVEN_DATA+0x262（cap 0x2dc，记录 0x3d）。错误行号 0x23e2/0x23e8/
// 0x23f1。
// ============================================================================
    // N(新候选)封顶：三个兄弟 if 块内 cMyTrace 槽深分配方向跨版本相反
    // （4.1 先声明者深：ORIG tt1 -0x44/tt2 -0x34/tt3 -0x24；4.4 反向：
    // 我方 tt1 -0x24/tt3 -0x44）。探针 /tmp/l22p/scope_probe.cpp 两形不可翻。
#include "DB_LoadInventory/_GetTagCharacInven.inc"
}

// ============================================================================
// DB_LoadInventory::_GetEquipMaxItemLevel @0x0840aac8
// charac_manage_info.max_equip_level → sig+0x15448（m_maxEquipLevel）。
// exec 失败/fetch 失败/get_int 失败 → 0；无行（getAffectedRowCount==0）→ 1。
// ============================================================================
#include "DB_LoadInventory/_GetEquipMaxItemLevel.inc"
}

// ============================================================================
// DB_LoadInventory::GetItemLock @0x0840d766（static unsigned int）
// charac_item_lock_info.item_lock_info blob → get_binary(0, 0x3cc) →
// uncompress_zip 进 out（stItemLockInfo）；解压后尺寸 mod 8 != 0 且非 0
// → 0x2931。返回 0=OK，其余为 DBThread.cpp 行号（0x291a/0x2924/0x292b/
// 0x292f/0x2931）。
// b28d-solo-DB_LoadInventory：AE（137=137 仅重定位差）。修法 = 独立
// `char buf[0x3cc]={0}`（rep stos 0xf3 精确）+ 独立 outLen（取址折叠
// lea -0x48，消旧结构体形的 N78 计数差与 add 两步）+ P221 全表达式临时链
// （3 处 trace 槽 -0x44/-0x34/-0x24）+ get_binary 判空内联（ORIG 无 ok 槽）。
// ============================================================================
#include "DB_LoadInventory/GetItemLock.inc"
}