// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 QuickParty/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— QuickParty 独立实现（batch-5 迁移自 GameStubs.cpp）。
// 本批（S4）：CQuickParty ctor/init、CQuickPartySystemManager ctor/
// init_quick_party_pool/cancel_quick_party 按权威报告实现。
// remove_quick_party_in_pool / push_quick_party_in_pool / destroy 等仍由后续批次实现。

#include "QuickParty.h"
#include "CGameManager.h"

#include <vector>

#include "LogManager.h"

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CDungeon.h"
#include "GameWorld.h"   // send_party_info_to_all / G_GameWorld


// CGameManager.h 目前与 CBattle_Field.h 组合存在重定义冲突（其它批次修复中），
// 此处按 ORIG 符号做最小声明（_ZN12CGameManager13GetQuickPartyEi /
// _ZN12CGameManager26GetQuickPartyRewardManagerEv，非虚函数，链接解析到真实定义）。
#include "CQuickPartyRewardManager.h"
// [dedup] CGameManager 权威定义见 CGameManager.h（namespace WongWork），本 TU 本地桩已删。
CGameManager* G_CGameManager();   // CDungeon.h 同声明

// CParty +0xb24 内嵌 CBattle_Field 子对象局部视图（CParty.h 以
// m_battleField[0x158] 等 pad 占位、未引入 CBattle_Field 类型；Main 裁定
// 本组 TU 局部视图，与 CParty.cpp 口径一致）。布局：pad 0xb24 + BF 0xcd0。
struct CPartyBattleFieldView
{
    char m_pad[0xb24];
    CBattle_Field m_battleField;   // +0xb24
};

namespace QuickParty
{

#include "QuickParty/CQuickParty.inc"

#include "QuickParty/STQuickPartyPoolMap_Data.inc"
}

#include "QuickParty/dtor_STQuickPartyPoolMap_Data.inc"

#include "QuickParty/dtor_CQuickParty.inc"

#include "QuickParty/get_quick_party_index.inc"

#include "QuickParty/init.inc"
}

#include "QuickParty/set_quick_party_index.inc"

#include "QuickParty/get_dungeon_index.inc"
#include "QuickParty/get_dungeon_diff.inc"

#include "QuickParty/set_change_quick_party_count.inc"
}

// delete_party_index @0x0826980e：遍历 m_partyIndex[0..3]，命中则置 -1 并返回。
#include "QuickParty/delete_party_index.inc"
}

// get_quick_party_count @0x082698be：累加各登记 party 的当前成员数。
// [b28d-L1] IDENTICAL/AE（routed 口径 check_tu_routed）：ORIG 0x082698be 逐条对齐（40/40，P167 CLEAR）。
// [b28d-solo-QuickParty] verdict=AE（TU 口径 39=39）；P167 CLEAR（0 missing/0 extra）。
#include "QuickParty/get_quick_party_count.inc"
}

#include "QuickParty/destroy.inc"
}

// [moved] GetActiveQuickPartyCnt/GetOccupiedQuickPartyPoolCnt 定义移至 CGameManager.cpp（避免陷入 namespace QuickParty）。

// ===================== CQuickPartySystemManager =====================

#include "QuickParty/CQuickPartySystemManager.inc"
}

#include "QuickParty/dtor_CQuickPartySystemManager.inc"
#include "QuickParty/init_quick_party_pool.inc"
}

// cancel_quick_party @0x0826a386：把 party 从其所属 CQuickParty 中注销，
// 剩余成员>1 且非解散(flag=true)时重新入池；EPLP 等待态且 flag 时重掷等待 timer。
// [b28d-L1] 修法（P167）：ORIG 0x0826a386 三处 get_quick_party_index（无 qpIndex 具名局部）——①短路条件 ②日志三元 ③GetQuickParty(内联实参)。原 qpIndex 缓存使 P167 报 MISSING×1；改为表达式直读后差集归零。残余为帧/槽形态差（N82）。
// [b28d-solo-QuickParty] verdict=AE（243=243，TU routed 口径；P167 CLEAR）：修法=
// ①`int count = 0;` 声明预清（ORIG 0x0826a586 在条件前 `movl $0,-0xc`）；
// ②删日志三元 party 判空（P171 照抄 ORIG 直调 get_quick_party_index）；
// ③EPLP 段去 bf/buffType 缓存（P48/P183：ORIG 两次独立 `mov party,%edx; add $0xb24`，
// getRandomBuffDungeonType 结果直传 setEplpWaitingTimer）。旧 N82 判词作废。
#include "QuickParty/cancel_quick_party.inc"
}

}  // namespace QuickParty

// ===================== push/remove_quick_party_in_pool 批次 =====================
// 依赖头：TimerQueue / PacketGuard（ORIG 弱访问器均按反汇编直读字段）。
#include "TimerQueue.h"
#include "PacketGuard.h"

namespace QuickParty
{

// ---- 简单弱访问器（ORIG W 符号，objdump 直读实证） ----

#include "QuickParty/get_party_index.inc"
}

#include "QuickParty/set_dungeon_index.inc"
#include "QuickParty/set_dungeon_diff.inc"

#include "QuickParty/get_pool_index.inc"
}

#include "QuickParty/set_pool_index.inc"

#include "QuickParty/get_first_matching_cnt.inc"
}

#include "QuickParty/is_change_quick_party_count.inc"
}

// get_party_object @0x082696d0：idx∈[0,3] 且槽非 -1 时经 GameManager 取 party。
#include "QuickParty/get_party_object.inc"
}

// gen_timer_key @0x08269680：计数数组位于 +0x0c 起、槽下标 (msg-0x9d)+4；
// 先自增再返回新值。msg=0x9e 对应 +0x20 槽。
#include "QuickParty/gen_timer_key.inc"
}

// get_matching_quick_party_master_unique_id @0x08269936：
// 首选 EPLP 状态 >1 的成员 party；否则任一 quick party；否则首个存在的 party；
// 返回其 manager 的 unique_id（无则 0xffff）。
// [b28d-L1] N82 封顶：ORIG 0x08269936；P167 CLEAR（13/13）⇒ 语义完成，残余为帧/槽位形态差（ORIG sub 0x48 vs 4.4 push ebx+sub 0x34）。
// [b28d-solo-QuickParty] verdict=AE（104=104）：修法=P0 第二循环/第三段具名 partyIdx 落槽
// （内联实参使 4.4 升格 %ebx 跨 G_CGameManager()）+ party 循环外声明 `= 0`（ORIG -0x28
// 槽 + movl $0 初始化）+ P171 删 mgr 判空（ORIG 0x08269a75 无 test）。旧 N82 判词作废。
#include "QuickParty/get_matching_quick_party_master_unique_id.inc"
}

// send_matching_update_quick_party @0x08269a8e：对每个存在的成员 party 发包
// 0x180：byte(poolIdx)、short(masterUid)、byte(changed)。
#include "QuickParty/send_matching_update_quick_party.inc"
}

// insert_timer_waiting_party_matching @0x0826acea：
// InsertTimer(objType=1, qpIndex, msg=0x9e, sec=0x14, key, 0x9e)。
// [b28d-L1] N82附 封顶：ORIG 0x0826acea 首值 gen_timer_key→%ebx、get_quick_party_index→%esi；4.4 恒 esi/ebx 对调（裸内联实参/P226 语句表达式两形实测均不可翻），P167 CLEAR（4/4）语义等价。
// [b28d-solo-QuickParty] verdict=NEAR 封顶（28=28，仅 gen_timer_key→%esi /
// get_quick_party_index→%ebx 与 ORIG 的 %ebx/%esi 角色互换；P167 CLEAR）。本轮实测：
// ①裸内联/双 register/具名 uint key 恒互换（具名 uint 落槽 -0xc 反劣 29 条）；
// ②`register unsigned int key` + 内联 index 令 gen→%ebx 但 4.4 多发 `mov %ebx,%esi`
// 中转（29 条，N18a 反向）；③4.4.6 后端（-B cc1plus446bin）复测同样互换 ⇒ 非 N7
// 后端敏感族，属 N82附「多实参 call 首值 callee-saved 对调」封顶。
#include "QuickParty/insert_timer_waiting_party_matching.inc"
}

// remove_quick_party_in_pool —— ORIG 0x0826aa9a。
// 报告：docs/class_func_reports/QuickParty__CQuickPartySystemManager/remove_quick_party_in_pool.md
// pool index ∈ [0,3)；map 未命中 → destroy + trace 并返回 false；
// 命中 → 从对应档位 list 移除 quick party index 并返回 true。
// [b28d-L1] N93/N98 封顶：ORIG 0x0826aa9a 槽序 it=-0x28/end 临时=-0x24/qpIndex=-0x20；块内声明使 4.4 把 qpIndex 排最深。P212 全头部声明已把 it 翻正 -0x28（ndiff 52→46），残余 end 临时槽与 qpIndex 互换——声明序/具名 e 两形实测（r19c/r19d）只交换槽位，4.4 按首次使用序分配不可翻。
// [b28d-solo-QuickParty] verdict=AE（71=71，TU routed 口径；P167 CLEAR）：修法=
// qpIndex 具名局部改内联实参 `m_lists[poolIdx].remove(qp->get_quick_party_index())`
// ——ORIG 0x0826ab40 的 qpIndex 槽实为 list::remove(const int&) 绑定的匿名临时
// （-0x20），end() sret 临时落 -0x24；原具名局部使 4.4 把两者槽号互换。旧
// N93/N98 封顶判词作废。
#include "QuickParty/remove_quick_party_in_pool.inc"
}

// push_quick_party_in_pool —— ORIG 0x0826afb4。
// 报告：docs/class_func_reports/QuickParty__CQuickPartySystemManager/push_quick_party_in_pool.md
// 正常路径：写 qp 的 dungeon/pool 字段并挂入对应档位 list、按需插等待 timer、
// 广播 0x180 匹配更新包；异常路径（qp 空/档位越界）：逐 party 清数据并发 0x17f
// 解散包后 destroy，记日志返回 false。
// [b23判词·N-XVAL] P167 调用点差集 CLEAR（25/25）：ORIG `list<int>::push_back(int&&)`
// （EOi）——源码以 static_cast<int&&>(qpIndex) 生成 xvalue 精确命中该重载（原 const& 形选错）。
// 残余 gap+2 = gcc 4.4 对 xvalue 实参多一次 `mov %eax,-N(%ebp); mov -N(%ebp),%eax` 存回读
// 往返 + 帧槽 -0x2c→-0x30 位移（N82），跨版本求值/帧结构不可源码复现，语义等价。
// [b28d-L1] N82/N-XVAL 封顶：ORIG 0x0826afb4；P167 CLEAR（25/25）。残余 = xvalue 实参存回读往返 + 帧槽 -0x2c→-0x30 位移（b23 判词复核仍成立）。
// [b28d-solo-QuickParty] verdict=AE（171=171）：修法=内联 xvalue 实参
// `push_back(static_cast<int&&>(qp->get_quick_party_index()))`，消 4.4 的
// `mov %eax,-0x24; mov -0x24,%eax; mov %eax,-0x20` 存回读往返与 PacketGuard
// -0x2c→-0x30 位移（旧 N82/N-XVAL 判词作废）。P167 CLEAR（25/25）。
#include "QuickParty/push_quick_party_in_pool.inc"
}

}  // namespace QuickParty

// ===================== TailB 批：CQuickParty 访问器 + 系统管理器匹配/开局链 =====================
// 报告：docs/class_func_reports/QuickParty__CQuickParty{,SystemManager}/ 各同名 md。

namespace QuickParty
{

// ---- get_timer_key @0x08269668：读 timer key 计数数组槽（与 gen_timer_key 同址）----
#include "QuickParty/get_timer_key.inc"
}

// ---- check_timer_key @0x082696b0：槽值与给定 key 相等判断 ----
// [b28d-L1] N7/N18b 封顶：ORIG 0x082696b0 为 `mov 0xc(%eax,%edx,4),%edx; mov 0x10(%ebp),%eax; cmp %eax,%edx`（两寄存器）；4.4 恒折 `mov …,%eax; cmp 0x10(%ebp),%eax`（内存直比，11 vs 12 条）。unsigned/signed、key 置左/置右、&0xff 掩码四形实测同折。
// [b28d-solo-QuickParty] verdict=AE（12=12）：修法=P248 档②字段侧——m_timerKeyCnt
// int[2]→unsigned int[2]（QuickParty.h，同宽零布局影响）+ `elem == (unsigned int)key`；
// 探针 /tmp/b28d_solo/probe_ck.cpp g0/g4 逐条全等（ORIG 两寄存器 12 条）。旧 N7/N18b 判词作废。
#include "QuickParty/check_timer_key.inc"
}

// ---- set_party_index @0x0826978c：写入首个空槽（-1），全满则丢弃 ----
#include "QuickParty/set_party_index.inc"
}

// ---- set_party_index @0x082697c8：定位写，idx∈[0,3] 才生效 ----
#include "QuickParty/set_party_index_1.inc"
}

// ---- get_party_object() @0x0826972c：返回首个登记且仍存在的成员 party ----
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x082696d0；P167 CLEAR（2/2）。
// [b28d-solo-QuickParty] verdict=AE（TU 口径 33=33）；P167 CLEAR。
#include "QuickParty/get_party_object_1.inc"
}

// ---- delete_party_index() @0x08269856：全部槽位清 -1 ----
#include "QuickParty/delete_party_index_1.inc"
}

// ---- is_empty_quick_party @0x08269884：所有槽均为 -1 即为空 ----
#include "QuickParty/is_empty_quick_party.inc"
}

// ---- set_first_matching_cnt @0x0826b75a（W 内联）：直写 +0x14 字节 ----
#include "QuickParty/set_first_matching_cnt.inc"
}

// ---- find_quick_party_in_pool @0x0826a9fe：
// 从档位 list 逐个弹出 index 取 CQuickParty；取不到的记 trace 继续弹。----
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x0826a9fe；P167 CLEAR（7/7）。
// [b28d-solo-QuickParty] verdict=AE（TU 口径 41=41）；P167 CLEAR。
#include "QuickParty/find_quick_party_in_pool.inc"
}

// ---- pop_quick_party_in_pool @0x0826ae64：
// 档位 0 直接领新池对象；其余档位先从池 list 弹已有 quick party，
// 弹空则回退为领新池对象并按 firstMatchingCnt 复位计数。----
#include "QuickParty/pop_quick_party_in_pool.inc"
}

// ---- regist_quick_party @0x0826adb6：弹池→压池闭环 ----
#include "QuickParty/regist_quick_party.inc"
}

// ---- check_match_proper_party @0x08269e34：匹配前置校验，返回 ENUM_ERROR ----
// [b28d-L1] N82 封顶：ORIG 0x08269e34；P167 CLEAR（7/7 调用点差集归零）⇒ 语义完成，残余为 4.1/4.4 槽位与寄存器分配差（real diff 58 全为 disp/reg 形态）。
// [b28d-solo-QuickParty] verdict=AE（73=73）：修法=P0/P35 删 dungeonIdx 具名 short 局部
// （三处重读 key.m_dungeonIndex：test %ax / cwtl / movswl %ax,%ebx）+ P226 匿名语句
// 表达式复现 bloodPass 零槽物化（旧 N82 判词作废）。P167 CLEAR（7/7）。
#include "QuickParty/check_match_proper_party.inc"
}

// ---- check_quick_party_min_count @0x0826ab9e：开局最低人数校验 ----
#include "QuickParty/check_quick_party_min_count.inc"
}

// ---- match_proper_party @0x08269f3c：为新 party 匹配同图快速队伍 ----
// [b28d-L1] 修法（P167/P220/P197④）：ORIG 0x08269f3c 调 ne×1 + list::size×1（无 eq/empty）⇒ 改正极性 `if (it != end) { 主体 }` + `size()==0`。差集仅余 N89（4.1 不内联 operator-> 发 ptEv×4）。残余槽位差 N82。
// [b28d-solo-QuickParty] verdict=AE（279=279，TU routed 口径；P167 CLEAR）：修法=
// ①去 `STQuickPartyPoolMap_Data& data = it->second` 引用缓存，每处重发 `it->second`
//   （P183；4.4 亦发 ptEv，N89 调用点差集消）；
// ②档位判空改正极性 `size() != 0` 包裹（P220 setne 形）；
// ③then/else 交换为 `if (need+memberCount == maxMembers)` 正极性（P222）；
// ④qp 两句式预清（ORIG 0x0826a128 `movl $0,-0x10`）；
// ⑤qpIndex 内联进 cMyTrace 实参（P227 全表达式临时链，%ebx 跨 ctor 保活）；
// ⑥`if (total != maxMembers) { trace+push } start_game(qp); return 0;` 块序（N93附）；
// ⑦traceMiss 改原位全表达式临时链。旧 N89/N82 判词作废。
#include "QuickParty/match_proper_party.inc"
}

// ---- make_quick_party @0x0826b222：把池内 quick party 合并成一个真实 party ----
// [b28d-L1] N82 封顶：ORIG 0x0826b222；P167 CLEAR（37/37）⇒ 语义完成，残余为槽位/帧形态差（real diff 264 全为 disp/reg）。
// [b28d-solo-QuickParty] verdict=AE（239=239，TU routed 口径；P167 CLEAR）：修法=
// ①第一循环 member 提至函数头、按 ORIG 槽序（member -0x30 深于 host -0x2c）声明
//   `CParty* member; CParty* host = 0;`；
// ②第二循环补具名 partyIdx（ORIG -0x1c）；第三循环 member 两句式预清
//   （ORIG 0x0826b430 `movl $0,-0x10`）+ user 内联实参（ORIG 0x0826b490 无 user 槽）；
// ③尾迭代器循环拆 `iterator it = begin(); iterator itEnd = end();` 后置比较
//   （复现 ORIG it=-0x3c/itEnd=-0x40/detached=-0x38 槽组）。旧 N82/N95 判词作废。
#include "QuickParty/make_quick_party.inc"
}

// ---- insert_timer_waiting_start_game @0x0826ad50：
// InsertTimer(objType=1, partyIdx, msg=0x2d, period=0x5a, key, msg)。 ----
#include "QuickParty/insert_timer_waiting_start_game.inc"
}

// ---- start_game @0x0826b58a：合并成团并广播、计时器等待后真正开局 ----
// [b28d-L1] N 封顶（P223 尾岛）：ORIG 0x0826b58a 尾段 `setle;test;je <nop 岛>; …; jmp <出口>; nop`；4.4 对 if(c){body} 恒 jump-thread 掉尾 jmp+nop（81 vs 80）。b28c-lane3 已试 P223 goto 岛/P225 while+break，本轮复核不翻。
// [b28d-solo-QuickParty] verdict=AE（82=82）：修法=P285——正向 if 体后补空 `else{return;}`，
// 4.4 即保留体尾 `jmp <leave>; nop` 岛（旧「P223 尾岛不可翻」判词作废）。P167 CLEAR（14/14）。
#include "QuickParty/start_game.inc"
}

// ---- start_game @0x0826a972：带 key 版（先校验人数再出池）。----
#include "QuickParty/start_game_1.inc"
}

// ---- direct_entrance_quick_party @0x0826a762：队长请求直接入场 ----
#include "QuickParty/direct_entrance_quick_party.inc"
}

}  // namespace QuickParty
