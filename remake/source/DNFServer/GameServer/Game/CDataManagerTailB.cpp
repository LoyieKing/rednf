// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDataManagerTailB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CDataManagerTailB.cpp —— W6_TailB 批次：CDataManager 域 ORIG-only 符号收口
// 转写依据 docs/class_func_reports/CDataManager/ 各报告（## 汇编 + ## 反编译 C
// 已逐份读完）；对象字段以 CDataManager.h 权威布局（偏移注释）具名访问。
// 仅承载本批符号的定义；不修改其它实现。
// ============================================================================

#include <algorithm>
#include <cstring>
#include <cstddef>
#include <vector>

#include "GameTypes.h"      // cMyTrace
#include "CDataManager.h"
#include "CStackableItem.h"   // booster/package 检查的完整类型
#include "STStackableScript.h" // STStackablePackageData（pkg reinterpret 目标）
#include "CMap.h"           // VerifyMap → CMap 完整类型
#include "CDungeon.h"       // VerifyDungeon → CDungeon::VerifyMaze / m_clearItemMap
#include "CSkill.h"         // Verify → CSkill 技能遍历
#include "CMonster.h"       // Verify → CMonster 等级检查 / get_index
#include "Quest.h"          // Verify → RewardItemInfo / Quest 遍历
#include "CDungeonClear.h"  // WongWork::CDungeonClear

// ---------------------------------------------------------------------------
// 原 5 个 ORIG-W 访问器已按 P233 迁回 CDataManager.h 类内 inline 定义
// （IsAvailablePvPChannel 0x0819ede0 / GetMinLevelPvPChannel 0x0819ee2e /
// GetMaxLevelPvPChannel 0x0819ee3c / GetLimitItemUsageInfo 0x8693f16 /
// is_AuctionRegFreeCeraItem 0x84e92a4：ORIG 绑定全 W、地址分散于消费 TU 段
// ARAD_LoginSecurity/InterDispatcher/Inter_LoadEtc_impl/CUserOnlyOrig）；
// 本 TU 不再定义这 5 个符号，由各使用点发射 W 副本。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// VerifyDungeon(CDungeon const&) const —— ORIG T 0x08362b44
//   遍历 dungeon.m_clearItemMap(+0x1c) 的 value（clear 奖励 itemIdx）逐一调
//   find_item（结果仅用于数据校验，不使用返回值），随后 dungeon.VerifyMaze()；
//   固定返回 1（调用方 Verify 按返回值恒真处理，与 ORIG 一致）。
// ---------------------------------------------------------------------------
#include "CDataManagerTailB/VerifyDungeon.inc"
}

// ---------------------------------------------------------------------------
// Verify() const —— ORIG T 0x08360d94（0x1359 字节）
// 结构（报告 ## 反编译 C 全文对照）：
//   1) mob reward 单调性检查（m_mobReward[i+1] <= m_mobReward[i] → trace 0x2723）；
//   2) 11 个职业技能表 CSkill+0xc8（required level range tag）==0 → 报错任一命中
//      则 trace 0x2735 并返回 false；
//   3) 全部主地图 VerifyMap(map, -1)，失败返回 false；
//   4) 全部副本 VerifyDungeon(dungeon)；
//   5) 道具表遍历：stackable 的 booster/package 引用完整性检查（缺失即置错）；
//   6) Quest 表奖励引用检查（reward/collision item & collision quest）；
//   7) 怪物等级区间合法性检查；
//   8) WongWork::CDungeonClear 全副本模拟（diff=4）输出二进制串，失败打
//      "Simulate dungeon clear data fail!!!!"；全部通过返回 true。
// 注：CSkill+0xc8、quest+0xb8/+0xc4/+0xd0/+0xdc/+0xec、CMonster+0x58/+0x5c 在
// 当前权威头尚未具名化，按 ORIG 偏移访问并标注【ORIG asm 证据】；待对应头批次
// 收口后替换为具名字段。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] N：N82 —— §6.2 三件套：p167 166=166 CLEAR
//   （MISSING/EXTRA 归零）、gap -49 条（4.4 多具名槽往返）、帧 0x1ac0 vs
//   0x1abc（-4）。语义完成项：空循环照抄（ORIG i∈[1,0xc6) 死体）、
//   __PRETTY_FUNCTION__（12 处）、booster/package 经 G_CDataManager()->find_item、
//   选择列表 vector 拷贝构造+析构、getSkillList 双调用、getQuestList 具名、
//   collision item 用 (*it)、toString 返回值判失败（删 memset 近似）。
//   残余为 4.1/4.4 槽复用策略（N82）与 STL 内联/转换构造发射差（N89 族）。
//   ORIG 0x08360d94。
// [SweepCDataManagerTailB 2026-09-10] 修真缺口（判据㉑ 缺调用 + 判据② call * 虚槽）：
//   ① ORIG 道具循环对 `item->GetItemType()`（vtable+0xc，_ZTV14CStackableItem
//      vptr+0xc=CStackableItem::GetItemType）共 **4 次虚调用**（08361240/08361257/
//      08361467/083617d0，Ghidra C 亦示 || 两臂各一次 + else 一次 + 死比较一次）；
//      旧码把结果缓存进 `const int type` 只调 1 次 ⇒ 缺 2 次 call *。已改逐点重读。
//   ② ORIG is_stackable 守卫为 08361222 `test %al,%al; je 083617d5`（正极性，
//      无 xor $1）；旧码 `if(!is_stackable()) continue;` 4.4 恒发 xor $1（P168a），
//      已改 `if (is_stackable()) { body }` 包裹。
//   读数：raw insns 1154→1172→1167（ORIG 1174）。二轮按 P335/P0 对齐：删 4 个
//   ORIG 无的具名局部（skill/sp、qp、mp；P0 无槽往返 ⇒ 表达式直读），补 ORIG
//   有的 dungeon 局部（08361fab mov %eax,-0x1c; mov -0x1c,%eax）⇒ fn_diff_tu
//   bad 1068→989、store-reload 成对 P 7→3；call * 2→4=ORIG 4；
//   p167 166=166 CLEAR（MISSING 全为 C1/C2、D1/D2 同址别名，P93 豁免）。
//   残余：4.1 多路径 jmp+nop 汇合块（ORIG jcc 103 vs 我方 95）、P197④ setcc 物化、
//   N18a lea/add、N122 bool 取反 xor（我方 2 处 vs ORIG 1 处）、else 分支
//   `cmp $0x22` 被 4.4 DCE（P171 死比较跨版本前端差）。
#include "CDataManagerTailB/Verify.inc"
}

// ---------------------------------------------------------------------------
// _subPunctuationMark(char const*, char*) —— ORIG T 0x08362c22
//   从 src 中剔除"跳过字符"后拷入 dst：
//     - 单字节：不在 m_slangName.m_charList(+0x86ac) 中 → 复制；
//     - 双字节：word = src[i]|src[i+1]<<8 不在 m_slangName.m_shortList(+0x86b8)
//       中 → 复制两字节。
//   （若命中跳过表则不复制——函数名与两分支对称性一致；见报告汇编逐条核对。）
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：删 strlen 缓存（ORIG 每轮调 strlen）、
//   out 用 int（unsigned 会令 dst[out] 折 lea）、w 用 `unsigned char buf[2]={0}`
//   字节写入形（ORIG movw $0 + 两次 movb + movzwl 回读）。ORIG 0x08362c22。
#include "CDataManagerTailB/_subPunctuationMark.inc"
}

// ---------------------------------------------------------------------------
// rewriteUsableEmoticon(CUser const&, char*) —— ORIG T 0x0835fba2
//   扫描文本中 "\x1e <emotcode>" 三字节模式，表情 code 以 (signed char) 取得
//   后查 m_emoticonList(+0x28)；未找到或 emoticon->isUsable(user)（vtbl+0x08,
//   asm 0835fc31..0x9c 虚调用）为假则把第 3 字节改写 0xff 作废。
//   循环条件 strlen(text)-2 > i（有符号）。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：emo 具名指针提前到函数头声明（ORIG 槽
//   emo(-0x18)/len(-0x14)/i(-0x10)/emoCode(-0xc) 按「先声明深」复现）；
//   循环下标用 int（size_t 会令 text[i] 折成 lea 三连）。ORIG 0x0835fba2。
#include "CDataManagerTailB/rewriteUsableEmoticon.inc"
}

// ---------------------------------------------------------------------------
// hasPreventPrefixCharacName(char const*, int) —— ORIG T 0x0836360c sz 0x10a
// （objdump 0x0836360c..0x08363716 逐指令转写；报告缺失标 [报告缺失]）。
//   count = m_serverParameter.m_preventPrefixCount（CDM+0x194）；
//   entry(i) = m_serverParameter.m_preventPrefixEntries + i*0x1e（CDM+0x190+8 起）。
//   对每条：entryLen=strlen(entry)；21 字节栈缓存清零（P4 gcc 内建展开形态）
//   → memcpy(prefix, name, entryLen) → strcmp(entry, prefix)；==0 → 返回 1。
//   循环条件 count > i（setg）；扫完返回 0。第二参 unused ORIG 亦未读。
//   依 §8.3 已将 ServerParameterScript.m_pad12c 头部拆具名（sizeof 0x49f4 不变）。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] N：P30/N18a —— 删 sps 引用/entry 局部后帧与
//   槽位全对齐（sub $0x30、prefix -0x25、i -0x10、entryLen -0xc），仅余 4.4.6
//   对 i*0x1e 索引展开的寄存器选择（`mov %edx,%ecx; sub; mov` vs `sub %eax,%edx`）
//   与 `lea 0x190(%edx)` 折成 `mov+add` 共 2 处 4 条；p167 3=3 CLEAR。
//   ORIG 0x0836360c。
#include "CDataManagerTailB/hasPreventPrefixCharacName.inc"
}


// ---------------------------------------------------------------------------
// hasSpecialCharacterExceptSpace(char const*) —— ORIG T 0x08362e0e
//   对 name 每个位置：
//     单字节（_isOneByteCharacter）：先查 InvalidCharData.m_charRange(+0x0c →
//       CDM+0x86d0) 区间表（lo <= c 且 c < hi 命中），再查 m_charList(+0x00 →
//       CDM+0x86c4) 精确表；
//     双字节：word = c[i]|c[i+1]<<8，先查 m_shortRange(+0x24 → CDM+0x86e8)
//       （lo <= word 且 word <= hi 双闭），再查 m_shortList(+0x18 → CDM+0x86dc)
//       精确表；任一命中返回 1，否则继续，扫完返回 0。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：四处循环改「begin 先、end 后并缓存」
//   （for 初始化逗号声明 it, itEnd）；range 循环经 `&(*it)` 具名指针读
//   m_min/m_max（ORIG -0x14/-0xc 指针槽）；精确表查找由 std::find 改手写迭代器
//   循环；第二条件写 `r->m_max > *p`（P196 方向）。ORIG 0x08362e0e。
#include "CDataManagerTailB/hasSpecialCharacterExceptSpace.inc"
}

// ---------------------------------------------------------------------------
// get_createCharacterGift(int, createChracScript::rewardInfoList&) const
//   ORIG T 0x08365668：m_createChrac.m_map0(+0x4bd4) 先 find(paramIdx)、再
//   find(0xb)（默认角色创建礼包 key），各命中则将该条目 vector<rewardInfo>
//   追加到 out.m_list 尾部（insert(end, begin, end)）。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：默认礼包 key 用字面量直传（ORIG +0xce
//   `movl $0xb,-0x1c` 槽物化 = P244 字面量→槽→取址形；具名 int 局部会使
//   it/defaultKey 槽互换，DIFF）。ORIG 0x08365668。
#include "CDataManagerTailB/get_createCharacterGift.inc"
}

// ---------------------------------------------------------------------------
// get_useJobFatigueMatchGift(char, int, int) —— ORIG T 0x0836580e
//   gift = m_useJobFatigue.m_giftMap.find((int)job)，未找到返回 0；
//   u1 = gift.m_rewardMap.upper_bound(arg2 - arg3)
//   u2 = gift.m_rewardMap.upper_bound(arg2)（asm -0x20+arg3 即还原 arg2）
//   若 u1==end 且 u2==end → 0；u1==u2 → 0；否则返回 u1->second。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：声明序 jobType→diff→it 使
//   diff(-0x20)/jobType(-0x1c)/it(-0x24) 全对齐；第二次 upper_bound 的 key 是
//   匿名临时 `diff + b`（非具名局部，ORIG -0x14 为引用实参物化）；
//   单出口 `if (it != end) { ... } return 0;` 复现共享尾块。ORIG 0x0836580e。
#include "CDataManagerTailB/get_useJobFatigueMatchGift.inc"
}

// ---------------------------------------------------------------------------
// get_useJobFatigueMatchGiftMsg(int, std::string&, std::string&) ——
//   ORIG T 0x0836598e _ZN12CDataManager29get_useJobFatigueMatchGiftMsgEiRSsS0_
//   （报告缺失，按 nm 定址 + objdump 转写）。
//   反汇编逐条转写：
//     it = m_useJobFatigue.m_giftMap.find(job)（map::find 直构 -0x10）；
//     end = m_useJobFatigue.m_giftMap.end()（直构 -0xc，P13 缓存 end）；
//     it != end 时 msg1 = it->second.m_msg1（pair+0x20 = value+0x1c）、
//     msg2 = it->second.m_msg2（pair+0x24 = value+0x20）。
// ---------------------------------------------------------------------------
#include "CDataManagerTailB/get_useJobFatigueMatchGiftMsg.inc"
}

// ---------------------------------------------------------------------------
// get_server_name(int) const —— ORIG T 0x08365f00
//   边界外或未知 idx 返回 "ERROR_SERVER"；服务器名字面量取自 .rodata
//   （EUC-KR 字节序列，以 \x 转义保真，提取地址见 case 注释）。
// ---------------------------------------------------------------------------
#include "CDataManagerTailB/get_server_name.inc"
}
