// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CExpertJobList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CExpertJobList（专家职业脚本列表，G5 批次）。
// ---- b28d-61 判词汇总（本 TU 6 件非 identical）----
// 已修 AE：get（P222 正向 if 翻块序）、STExpertJobScript::Clear（ORIG 0x0896dfec
//   调用序重排：0x248 在 0x230 前、0x18/0x30 等落标量清零之后，120/120 对齐）。
// N 封顶（P167 缺口=STL 迭代器/字符串符号，N89）：
//   destroy 0x0822b5f2（MISSING Rb_tree_iterator 符号）、
//   get_disjoint_additional_items 0x085d3cxx（MISSING Rb_tree_iterator）、
//   init 0x0822b3xx（MISSING iterator C1Ev+EXTRA）、
//   ImportExpertJobEtcScript 0x0897xxxx（MISSING string C1/D1）、
//   ImportExpertJobScript 0x089xxxxx（MISSING make_pair+EXTRA 24，N89 大函数）。
// 语义依据 docs/class_func_reports/CExpertJobList/{init,destroy}.md 反汇编。
// init（0x896d8ec）：遍历 g_expertJobScriptFileList，逐条 new STExpertJobScript
//   + ImportExpertJobScript，成功则 FindIndexByFullScan 取 job key 插入 m_jobMap；
//   失败 trace + delete + 返回 false。最后 ImportExpertJobEtcScript(m_etc,"expertjob.etc")。
// destroy（0x896dac6）：遍历 m_jobMap delete 各脚本。
//
// ImportExpertJobScript（0x896b0e4）/ ImportExpertJobEtcScript（0x896ae21）
// 按 docs/class_func_reports/_lt_global_gt_/{ImportExpertJobScript,
// ImportExpertJobEtcScript}.md 反编译忠实转写：函数内访问均形如
// `param_1 + local_58*0x290 + X` 且 local_58 恒为 0，故有效偏移即 X，
// 与 STExpertJobScript.h 权威布局一一对应。
// ============================================================================

#include "CExpertJobList.h"

#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt / ScanStr / ScanFloat
#include "LogManager.h"      // LogManager::logFormat
#include <stdio.h>
#include "NpcScript.h"

// ---- 本地 STScriptFileList（与 CItemShop.cpp / STScriptFileList.cpp 同布局：
//      +0x00 std::map<int,std::string>，大小 0x18）----


// ORIG 全局（B g_expertJobScriptBaseDirectory 0x094f7660[0x100] /
// g_expertJobScriptFileList 0x094f7760 = 0x18 STScriptFileList 对象）。
// 由 initExpertJobScript（InitScriptProducers_5.cpp）填充；init() 遍历使用。
// b31-SweepCExpertJobList [P327]：定义序照 ORIG —— ORIG GI 键名
// _GLOBAL__I_g_expertJobScriptBaseDirectory（0x0896dbb0，size 0x1c）取「本 TU 内
// 第一个强符号·外部链接·定义」实体；我方原序（FileList 在前）产
// _GLOBAL__I_g_expertJobScriptFileList（仅 NEW，ORIG 无此名）。BaseDirectory 是
// POD 数组（零初始化、无动态构造）⇒ 前移不改变本 TU 构造序，其余符号集不变。
char g_expertJobScriptBaseDirectory[0x100];
STScriptFileList g_expertJobScriptFileList;

// [potion info] 段读取并写出的炼金术值表。
// ORIG 为单一全局 g_alchemistInfo（B 0x094f7778，nm 尺寸 8）；[0] 即低半区
// g_alchemistInfo，[1] 即未命名全局 _DAT_094f777c（见 ImportExpertJobScript.md
// [potion info] 反编译）。本 TU 为唯一定义/写点。
int g_alchemistInfo[2];

// ============================================================================
// expert_job::get_disjoint_additional_items（ORIG 0x896d75d）。
// 在 multimap 中按 key 取 equal_range，将每个 mapped stItemSelectRate 压入 out；
// 未找到返回 false，否则返回 true。供 [disjoint result] 校验/填充附加/大奖物品。
// b28d-solo-CExpertJobList 判词（AE）：P222 正向包裹（命中块 fall-through、false 归
// 共享尾 0x0896d829）+ P13 缓存 upper_bound（ORIG 0x0896d7e2 循环初始化区一次求值、
// 条件只比较）；64=64。
// ============================================================================
namespace expert_job
{
#include "CExpertJobList/get_disjoint_additional_items.inc"
}
}

// ORIG C1/C2 @0x0896e5cc：全部成员容器构造后尾调 Clear()（与 ORIG 构造序一致）。
#include "CExpertJobList/STExpertJobScript.inc"
}

// ============================================================================
// STExpertJobScript::Clear —— 清空全部容器并清零标量。
// 由 ImportExpertJobScript 在加载前调用（脚本为 new 分配，容器已默认构造，
// 此处保证重复加载语义一致）。
// ============================================================================
#include "CExpertJobList/Clear.inc"
}

// ============================================================================
// STExpertJobEtcScript::Clear —— 清空 etc 脚本容器。
// ============================================================================
#include "CExpertJobList/Clear_1.inc"
}
// ORIG C1 0x0849f438：构造两个 map 后调 Clear()。
#include "CExpertJobList/STExpertJobEtcScript.inc"
}

// ============================================================================
// ImportExpertJobScript（ORIG 0x896b0e4）。
// 加载指定专家职业脚本，逐节填充 STExpertJobScript 各容器/标量。
//
// b29-CallAnchor 复核（2026-09-09；方法=call 锚点间隔 + 段级归一化多重集 + 与 ORIG
// 逐段语义核对，专治「p167 CLEAR ≠ 无真缺口」）。本段为最新判词：
//   基线：p167 239 = 239（MISSING/EXTRA 全为 C1↔C2 / D1↔D2 同址别名，P93 豁免）；
//   gap（clean 口径）2235 − 2231 = 4（复核前 12）。
//   **真语义缺口 1 处（已修）**：[disjoint result] 的 `m_disjointResultMap.insert(...)`
//     丢弃返回值。ORIG 0x0896ba28 insert 之后 0x0896ba7e..0x0896ba9d 有
//     `movzbl -0x2ac(%ebp),%eax; xor $1; test %al,%al; je +0x9b4`（读 insert 返回
//     pair<iterator,bool>::second）⇒ 重复键走 `mov $0,%ebx; mov $1,%esi; jmp <循环尾>`
//     = return false。旧判词把这段 7 条误判为「N128 EH 状态机寄存器化差异」——
//     实为**缺 duplicate 检查**（重复项原被静默接受，map 覆盖语义与 ORIG 不同）。
//     修法：具名 `std::pair<...::iterator, bool> inserted = ...insert(...)` 再判
//     `if (!inserted.second) return false;`（ORIG 的 sret 目标即具名局部 -0x2b0，
//     两个临时 pair 析构后才读 -0x2ac）⇒ 该段逐条全等（含槽位）。
//   形态修 3 处（每处 +1 条，语义等价）：
//     k62  ORIG 0x0896b99f `mov %eax,-0x388(%ebp)`（第三次 ScanInt 结果写具名局部、
//          写后未读，P171）⇒ 源改 `int readOnly = ScanInt(&ok);` 保留 store。
//     k209 ORIG 0x0896d16e `movl $0x3,-0x1c(%ebp)`（[alchemist extraction result item]
//          的 idx 初值 3，三分支全覆盖 ⇒ 死初始化，P171）⇒ 源补 `int idx = 3;`。
//     k131 [potion info] 判定树：ORIG 用 `ja`（无符号序）⇒ switch 表达式 unsigned
//          （P321-T2）；叶块物理序 ORIG 为 [1] 组在前、[0] 在后 ⇒ 按 P324 调整 case
//          书写序（case 0x27ac59 移到 [1] 组之后）。两项合计该段归零。
//   剩余 4 处形态残差（均非语义缺口，封 N）：
//     k125 [upgrade cost] ORIG `mov -0x48,%eax; sub $1,%eax; mov %eax,%esi` vs 4.4
//          `lea -0x1(%eax),%esi`。试过具名 expect 局部（段长对齐但新增槽、违 P0）后
//          回退内联表达式 ⇒ 后端指令选择差（P321-T5）。
//     k145/146/147 [disjoint self service] 三次落值：ORIG 为 RHS-first + edx 中转
//          （`mov %eax,%edx; mov -0x3c(%ebp),%eax; mov %edx,N(%eax)`），4.4 对 int
//          字段恒 LHS-first。按 P211/P248 档③试 (unsigned int)/(unsigned long) 两种
//          RHS 异型化均被前端折叠 ⇒ N76（禁为装载序改共享头字段类型）。
//
// b290-BigExpert 复核（双判据 + 4 条已验证修法；本段为最终判词，覆盖下方 b28d 旧判词的
// ②③④ 三项「可攻」判断）：
//   双判据：① 指令数缺口 origN 2194 / ourN 2182 ⇒ gap 12（原 38）；② p167 调用点差集
//   CLEAR（ORIG 239 = 本 TU 239，0 missing / 0 extra）⇒ 无未翻译段、无缺失步骤。
//   structdiff（槽号/寄存器/立即数掩蔽，strict autojunk=False）：**154 ⇒ 17**。
//   本波修成的 4 族（每族均实测）：
//     A. REAL-BUG×2（优先于形态）：
//        A1 [potion info] 0x0896c35d..0x0896c398 判定树实证 **0x27ac52 → g_alchemistInfo[1]
//           （`mov %eax,0x94f777c`，与 0x98981a/0x1f8e/0x98988d/0x2920913e/0x989885 同列），
//           只有 **0x27ac59 → g_alchemistInfo[0]**（0x0896c3a4 `mov %eax,0x94f7778`）；
//           原码把 0x27ac52 并入 [0] 组 ⇒ 该药水索引写错全局（后果：炼金药水上限落到
//           错误的全局半区，0x094f7778/0x094f777c 均为运行时可读全局）。
//        A2 [doll_controller extraction result item] 0x0896d48f 第 4 次 ScanInt 落
//           `mov %ax,-0x37c(%ebp)` = res+0x8（**m_field8**），且分类条件读 res+4/res+6、
//           收尾 `movzwl -0x37c(%ebp)`（0x0896d59c）累加 res+8；原码第 4 次写回 m_field4
//           ⇒ ①覆盖第 2 个值使分类条件取错操作数（分派结果可错）②入表元素 +8 恒为 ctor
//           链式残值（0x0896deac：w+8=0→w+6=w+8→w+4=w+6）。
//     B. P332 具名指针形（4 处）：ORIG 块首把 `&script[scriptIdx].<成员>` 溢出到
//        -0x3c/-0x38/-0x34/-0x30（0x0896c5fd / 0x0896c6ba / 0x0896c7e8 / 0x0896c917），
//        其后 insert/落值只 `mov -N(%ebp)` 重载 ⇒ struct 154→84。
//     C. 构造序照抄：[disjoint result] res 与 grade 并列于循环体首（ORIG grade C1
//        0x0896b751 之后**紧接** res C1 0x0896b764，均在首个 ScanInt 之前）⇒ 早退析构
//        状态机（%esi 三态）编码对齐；struct 84→53。
//     D. P331 槽数：ORIG -0x4c(%ebp) 仅 2 次写（0x0896b8a7/0x0896b923）⇒ 两段 key 读复用
//        同一具名局部；我方拆成两个 int 会多占一槽并整帧平移 ⇒ 单这一处 ndiff 319→**575→1293**
//        （autojunk 口径，对齐坍缩解除）。
//     E. P164 循环头形：主 while 改 `for(;;) + 头部 if(!ScanType) break;`，复现 ORIG
//        循环头在 body 之前（0x0896b189）并去掉 4.4 的尾部 cond 复制；struct 41→19。
//     F. P198② ScanStr 具名 ok 两句形（0x0896d04c `mov %al,-0x22d`）。
//   封 N（余 17 行，逐条给据）：
//     N-P223  ORIG 0x0896b16b..0x0896b188 的 8×`nop;jmp` 循环头中继岛（16 条，4.1 对
//             多源同标签尾岛；4.4 不发，实测无源码形可诱导）；
//             实测：改 for(;;) 形后岛仍只在 ORIG 侧。
//     N128/N223 [disjoint result] 的 %esi 早退状态写在 ORIG 为 `mov $0x1,%esi`（12 处）、
//             我方 3 处（其余以 `mov $0x0,%esi`/直落尾岛表达）——EH 状态机寄存器化差异，
//             N128 单向：ORIG 更省时禁止反向补写。残差 7 行（0x0896ba54..0x0896ba65）。
//     N82/P212 帧内槽位分组：4.4 深槽区在本件按对象生命周期穿插，ORIG 4.1 分区；
//             实测三形（含 volatile 溢出副本）后 section/ok 组恒差 8（我方 -0x230/-0x235
//             vs ORIG -0x228/-0x22d），structdiff 已掩蔽 ⇒ 残差 2 行（0x0896c21d `sub $1`
//             + `mov %eax,%esi` vs 我方 `lea <A>(%eax)`、0x0896c34f 尾 cmp 极性）。
//
// b28d-solo-CExpertJobList 判词（N 封顶；§6.2 三件套）：
//   语义完成：p167 调用点差集 CLEAR（ORIG 239 = 本 TU 239，0 missing / 0 extra）；
//   真语义 bug 修 3 族：[items] 三处 make_pair 实参取错字段（ORIG 0x0896bf39/
//   0x0896bfaa/0x0896c01b 实证 (m_field0,idx)/(m_field0,m_field8)/(m_field4,m_field0)
//   + stRecipeInfo +0/+4 字段化）、[expertness exp] 去 string 临时+operator=（改直写
//   e.m_name）、load 判定改 bool 早返形。
//   byte 层残差（ndiff 702 / gap 38 @ ORIG 2194 条）：
//   ① ORIG 0x0896b16b..0x0896b188 的 8 个 `nop;jmp` 循环头中继岛（4.1 if-else 链
//      占位产物，16 条；P223 族多源同标签尾岛）；
//   ② 块内局部槽基差 4（string/multimap 组 -0x224/-0x244/-0x25c vs ORIG
//      -0x228/-0x248/-0x260；int 组 -0x48/-0x44 vs ORIG -0x54/-0x50）——4.4 与 4.1
//      块内槽分配策略差（N82/P212 族；声明序已逐条照 ORIG）。
//      [b290 更正] ② 的根因是「槽数」而非「声明序」：见上 D 项（并少 1 个 int 即归零）。
//   ③ [potion info] 的 `||` 链：4.1 发数值二分比较树、4.4 发线性 cmpl 链（14 条）；
//      [b290 更正] 可攻：ORIG 该处本是 **switch**（判定树 + 条件一次取入 %eax），
//      改 switch 后归零；同时暴露 A1 的 REAL-BUG。
//   ④ 索引化地址计算书写序（imul/add）与 EH 状态机编码差。
//      [b290 更正] 前半可攻（B 项具名指针形，已修）；后半（%esi 状态机）保留为 N128。
// ============================================================================
#include "CExpertJobList/ImportExpertJobScript.inc"
}

// ============================================================================
// ImportExpertJobEtcScript（ORIG 0x896ae21）。
// 加载专家职业「.etc」脚本：填充等级上限 map 与分解机大奖概率 map。
// b28d-solo-CExpertJobList 判词（AE）：P164 for(;;)+头部 if(!ScanType) break（复现
// ORIG cond 块在 body 之前）+ P16/P292 bool 形早返 + P198① ok 预清两句式 + P171 照抄
// ORIG 死局部（-0x30 std::string 仅构造/析构、-0xc int 清零未读）+ 两处预清声明分组
// 在循环体首；189=189。
// ============================================================================
#include "CExpertJobList/ImportExpertJobEtcScript.inc"
}

// ============================================================================
// 构造（ORIG 0x896e312，键到 g_expertJobScriptBaseDirectory）
// ============================================================================
#include "CExpertJobList/CExpertJobList.inc"
#include "CExpertJobList/init.inc"
}

// ============================================================================
// destroy（ORIG 0x896dac6）
// b28d-solo-CExpertJobList 判词（AE）：P13 缓存 end()（ORIG 0x0896dada 前一次性求值、
// 条件只比较）+ P141 删手写 delete 判空（ORIG 仅 test/je 自带守卫）；42=42。
// ============================================================================
#include "CExpertJobList/destroy.inc"
}

// ============================================================================
// get（ORIG W 0x0822b5f2 调用链）
// ============================================================================
#include "CExpertJobList/get.inc"
}

// ============================================================================
// STExpertJobEtcScript::isExpertJobLimitLevel（ORIG W 0x0849f47c）
// 遍历 m_levelMap（map<uchar,short>），任一 mapped short == level 返回 true。
// 供 CExpertJob::IncreaseExpertJobExp 判断当前等级是否为边界上限。
// ============================================================================
#include "CExpertJobList/isExpertJobLimitLevel.inc"
}
