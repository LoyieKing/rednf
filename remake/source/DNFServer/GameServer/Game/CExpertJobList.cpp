// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CExpertJobList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
    std::multimap<int, stItemSelectRate>& m, int key,
    std::vector<stItemSelectRate>& out)
{
    // b28d-solo [P222 正向包裹；P13：ORIG 循环初始化区一次性求值 lower/upper_bound
    // （-0x14/-0x18 两具名迭代器），条件仅比较不重复调用]。
    std::multimap<int, stItemSelectRate>::iterator found = m.find(key);
    if (found != m.end())
    {
        for (std::multimap<int, stItemSelectRate>::iterator it = m.lower_bound(key),
                 e = m.upper_bound(key); it != e; ++it)
        {
            out.push_back(it->second);
        }
        return true;
    }
    return false;
}
}

// ORIG C1/C2 @0x0896e5cc：全部成员容器构造后尾调 Clear()（与 ORIG 构造序一致）。
#include "CExpertJobList/STExpertJobScript.inc"
{
    Clear();
}

// ============================================================================
// STExpertJobScript::Clear —— 清空全部容器并清零标量。
// 由 ImportExpertJobScript 在加载前调用（脚本为 new 分配，容器已默认构造，
// 此处保证重复加载语义一致）。
// ============================================================================
#include "CExpertJobList/Clear.inc"
{
    // b28d-61 [按 ORIG 0x0896dfec 调用序重排，120/120 逐条对齐]：
    // ORIG 顺序非声明序（0x248 在 0x230 前、0x18/0x30 等在标量清零之后）。
    m_expVec.clear();
    m_growSkillVec.clear();
    m_disjointResultMap.clear();     // 0x1bc
    m_baseConst = 0;                 // 0x1d4
    m_additionalConst = 0;           // 0x1d8
    m_clearQuests.clear();           // 0x1dc
    m_giveupCost.clear();            // 0x1e8
    m_compoundRateVec.clear();       // 0x1f4
    m_recipeInfoMap.clear();         // 0x200
    m_recipeIndexMap.clear();        // 0x218
    m_recipeItemMap.clear();         // 0x248
    m_recipeLevelMap.clear();        // 0x230
    m_enduranceRepairCost.clear();   // 0x260
    m_enduranceReduce1 = 0;          // 0x26c
    m_enduranceReduce2 = 0;          // 0x270
    m_enduranceInit = 0;             // 0x274
    m_upgradeCost.clear();           // 0x278
    m_gainExp1 = 0;                  // 0x284
    m_gainExp2 = 0;                  // 0x288
    m_limitDisjointGold = 0;         // 0x28c
    m_portableDisjointMap.clear();
    m_selfDisjoin.clear();   // ORIG：stSelfDisjoinMaterialInfo::clear @0x0896dbe6
    m_shopDollMap.clear();
    m_unsealScrollMap.clear();
    m_monsterCardBindRateMap.clear();
    m_commonBindInfo.clear();    // ORIG：this+0x84 BindInfo clear
    m_specialBindInfo.clear();   // ORIG：this+0xc4 BindInfo clear
    m_alchemistExtractInfo.clear();
    m_enchanterExtractInfo.clear();
    m_dollExtractInfo.clear();
    m_alchemistResultInfo.clear();
    m_enchanterResultInfo.clear();
    m_dollResultInfo.clear();
}

// ============================================================================
// STExpertJobEtcScript::Clear —— 清空 etc 脚本容器。
// ============================================================================
#include "CExpertJobList/Clear_1.inc"
{
    m_levelMap.clear();
    m_disjointerBigWinRate.clear();
}
// ORIG C1 0x0849f438：构造两个 map 后调 Clear()。
#include "CExpertJobList/STExpertJobEtcScript.inc"
{
    Clear();
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
// [P6AuditOldN.P6S2 2026-09-11 · 本波复核] n1_v DIFF 2235/2231 gap=+4（blocks=356）。
//   ORIG 0x0896b0e4：`sub $0x3c0,%esp` vs 我方 `sub $0x3d0,%esp`（帧差 0x10）；
//   首块 ORIG 多 `lea -0x228(%ebp),%eax; mov %eax,(%esp); call <T>`，其后全为槽轮换
//   （-0x228/-0x22d/-0x248/-0x260 组 ↔ 我方 -0x22c/-0x231/-0x24c/-0x264 组）。
//   p167fix 修正口径后残差为 std 模板族（__copy_move_backward/vector<stMonsterCardBindResult>），
//   非源码缺口。新钥匙判定：P339（356 块级轮换非单槽可定序）/P0/P183（无具名剩余）/
//   类型轴（无 callee 类型分歧）均不适用；未落地，维持 N（未决）。
#include "CExpertJobList/ImportExpertJobScript.inc"
{
    // b28d-solo [P16/P292：ORIG 0x0896b102 `xor $1; test %al,%al; je` bool 形早返；局部集合照抄
    // ORIG：-0x54 scriptIdx（恒 0，成员访问索引化）/ -0x50 死 int / -0x22c 死 string /
    // -0x22d ok 预清]。
    if (!loadRDARScriptFile(g_expertJobScriptBaseDirectory, path))
        return false;
    {
        // [b2c 试形 B·采用] `int readOnly`（[disjoint result] 第三次 ScanInt 的写后未读
        // 局部，ORIG 槽 -0x388）声明位置决定 4.4 栈槽分组：
        //   形①（原）声明在该赋值处 ⇒ 落浅端 -0x4c，key/unusedCount/scriptIdx 及以下
        //        整组槽 +4（raw diff groups 411）；
        //   形② 顶层最先声明 ⇒ readOnly 落 -0x58，浅端标量组（-0x4c/-0x50/-0x54）全部
        //        回归 ORIG **0 偏移**，只剩对象组 +4（raw diff groups **356**，delta=0
        //        行数 60→119）；
        //   形③ 本分支循环体首（与 grade/res 并列）⇒ 又落回 -0x4c（等同形①，407 组）。
        // 采用形②（四数不变、ndiff 严格改善）。
        int readOnly;
        int scriptIdx = 0;
        (*script).Clear();
        std::string section;
        std::string unusedSection;   // ORIG -0x22c（构造/析构但未使用，P171 照抄）
        int unusedCount = 0;         // ORIG -0x50（死 int，P171 照抄）
        bool ok = false;             // ORIG -0x22d（预清）
        std::multimap<int, stItemSelectRate> additional;  // [additional result]
        std::multimap<int, stItemSelectRate> bigwin;      // [big win result]

        // b290-BigExpert [P164：ORIG 循环头在 body 之前（0x0896b189 = `movl $1,0x4(%esp)`
        // + ScanType + `xor $1; test; jne <body>`，且 body 尾 `jmp 896b189`），4.4 对
        // `while` 恒旋转到尾部并在末尾复制一份 cond（+6 行）⇒ 改 for(;;) + 头部早返，
        // 同 TU 的 ImportExpertJobEtcScript 已用此形拿到 189=189]。
        for (;;)
        {
            if (!ScanType(section, true))
                break;
            if (section == "[expertness exp]")
            {
                while (true)
                {
                    // b28d-solo [ORIG 0x0896b1df：先构造 ExpertJobExp，三个字段直写成员
                    // （ScanStr 直接写 e.m_name，无 string 临时+operator=）]。
                    ExpertJobExp e;
                    e.m_exp = ScanInt(&ok);
                    if (!ok) break;
                    e.m_level = ScanInt(&ok);
                    if (!ok) return false;
                    ScanStr(&e.m_name);
                    script[scriptIdx].m_expVec.push_back(e);
                }
            }
            else if (section == "[skill]")
            {
                script[scriptIdx].m_growSkillVec.clear();
                std::pair<int, int> p;
                for (;;)
                {
                    p.first = ScanInt(&ok);
                    if (!ok) break;
                    p.second = ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_growSkillVec.push_back(p);
                }
            }
            else if (section == "[additional result]")
            {
                additional.clear();
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    stItemSelectRate rate;
                    rate.m_rateMin   = ScanInt(&ok); if (!ok) return false;
                    rate.m_rateMax   = ScanInt(&ok); if (!ok) return false;
                    rate.m_itemIndex = ScanInt(&ok); if (!ok) return false;
                    rate.m_rate      = ScanInt(&ok); if (!ok) return false;
                    rate.m_float8    = ScanFloat(&ok); if (!ok) return false;
                    additional.insert(std::make_pair(key, rate));
                }
            }
            else if (section == "[big win result]")
            {
                bigwin.clear();
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    stItemSelectRate rate;
                    rate.m_rateMin   = ScanInt(&ok); if (!ok) return false;
                    rate.m_rateMax   = ScanInt(&ok); if (!ok) return false;
                    rate.m_itemIndex = ScanInt(&ok); if (!ok) return false;
                    rate.m_rate      = ScanInt(&ok); if (!ok) return false;
                    rate.m_float8    = ScanFloat(&ok); if (!ok) return false;
                    bigwin.insert(std::make_pair(key, rate));
                }
            }
            else if (section == "[base const]")
            {
                script[scriptIdx].m_baseConst = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[additional const]")
            {
                script[scriptIdx].m_additionalConst = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[disjoint result]")
            {
                script[scriptIdx].m_disjointResultMap.clear();
                while (true)
                {
                    // b290-BigExpert [P332/构造序：ORIG 0x0896b751 grade C1 之后**紧接**
                    // 0x0896b764 res C1（`lea -0x3a8(%ebp); call stDisjointResultC1Ev`），
                    // 两者均在首个 ScanInt 之前 ⇒ 原码 res 与 grade 同处循环体首并列声明。
                    // 原写法把 res 放在 grade 三字段之后 ⇒ 早退路径的析构状态机（%esi
                    // 0/1/2 三态，ORIG `mov $0x1,%esi` 12 处 vs 我方 3 处）编码不一致。
                    DisjointMachineGrade grade;
                    stDisjointResult res;
                    grade.m_grade = (unsigned char)ScanInt(&ok);
                    if (!ok) break;
                    grade.m_field1 = (unsigned char)ScanInt(&ok);
                    if (!ok) return false;
                    grade.m_field2 = (unsigned char)ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field4 = ScanFloat(&ok);
                    if (!ok) return false;
                    // b290-BigExpert [P331 槽数：ORIG -0x4c(%ebp) 只有 2 次写
                    // （0x0896b8a7 / 0x0896b923）⇒ 两次 key 读复用同一具名局部，
                    // 原码是两个 int（-0x4c/-0x48）会多占一槽并整体平移其后槽位]。
                    int key;
                    key = ScanInt(&ok);
                    if (!ok) return false;
                    if (key != 0 &&
                        !expert_job::get_disjoint_additional_items(
                            additional, key, res.m_additionalItems))
                        return false;
                    key = ScanInt(&ok);
                    if (!ok) return false;
                    if (key != 0 &&
                        !expert_job::get_disjoint_additional_items(
                            bigwin, key, res.m_bigwinItems))
                        return false;
                    // ORIG 0x0896b99f `mov %eax,-0x388(%ebp)`：第三次 ScanInt 结果写入
                    // 具名局部（写后未读，P171 照抄）⇒ 必须保留该 store。
                    // [b2c 试形 B] 该具名局部改在**函数顶层**声明（`int readOnly;` 见上），
                    // 此处只赋值 —— 见顶层注释（槽位对齐实验）。
                    readOnly = ScanInt(&ok);
                    if (!ok) return false;
                    // b290-BigExpert [REAL-BUG 补：ORIG 0x0896ba7e..0x0896ba9d 在 insert
                    // 之后 `movzbl -0x2ac(%ebp); xor $1; test; je`（= 检查
                    // pair<iterator,bool>::second）⇒ 重复键走 `mov $0,%ebx; mov $1,%esi;
                    // jmp <循环尾>` = return false。原码丢弃 insert 返回值 ⇒ 重复项被静默
                    // 接受、后续 map 覆盖语义与 ORIG 不同。]
                    // ORIG 0x0896ba28 insert 的 sret 目标即具名局部 -0x2b0，
                    // 两个临时 pair 析构后（0x0896ba7e）才读 -0x2ac（= 该局部 +4 的
                    // second）⇒ 原码是「具名 pair 接收 insert 返回值，下一语句判 second」，
                    // 直接写 `if (!insert(...).second)` 会在 insert 后立即读临时。
                    std::pair<std::map<DisjointMachineGrade, stDisjointResult>::iterator,
                              bool> inserted =
                        script[scriptIdx].m_disjointResultMap.insert(
                            std::make_pair(grade, res));
                    if (!inserted.second)
                        return false;
                }
            }
            else if (section == "[clear quests]")
            {
                script[scriptIdx].m_clearQuests.clear();
                int v;
                for (;;)
                {
                    v = ScanInt(&ok);
                    if (!ok) break;
                    script[scriptIdx].m_clearQuests.push_back(v);
                }
            }
            else if (section == "[giveup cost]")
            {
                script[scriptIdx].m_giveupCost.clear();
                int v;
                for (;;)
                {
                    v = ScanInt(&ok);
                    if (!ok) break;
                    script[scriptIdx].m_giveupCost.push_back(v);
                    if (script[scriptIdx].m_giveupCost.size() > 9)
                    {
                        printf("SCRIPT ERROR over count [%s] %s\n",
                               path, section.c_str());
                        return false;
                    }
                }
            }
            else if (section == "[compound rate]")
            {
                script[scriptIdx].m_compoundRateVec.clear();
                stCompoundRate r;
                for (;;)
                {
                    r.m_level = ScanInt(&ok);
                    if (!ok) break;
                    r.m_successRate = ScanInt(&ok); if (!ok) return false;
                    r.m_expMin      = ScanInt(&ok); if (!ok) return false;
                    r.m_expMax      = ScanInt(&ok); if (!ok) return false;
                    script[scriptIdx].m_compoundRateVec.push_back(r);
                }
            }
            else if (section == "[items]")
            {
                script[scriptIdx].m_recipeInfoMap.clear();
                script[scriptIdx].m_recipeIndexMap.clear();
                script[scriptIdx].m_recipeLevelMap.clear();
                script[scriptIdx].m_recipeItemMap.clear();
                int idx;
                for (;;)
                {
                    idx = ScanInt(&ok);
                    if (!ok) break;
                    // b28d-solo [ORIG 0x0896beae..0x0896c01b：三个字段直写 info(+0/+4/+8)，
                    // 三处 make_pair 实参 = (m_field0, idx)/(m_field0, m_field8)/
                    // (m_field4, m_field0)（p167 MISSING IRmRiE/IRmRtE/IRmS0_E 实证）]。
                    stRecipeInfo info;
                    info.m_field0 = ScanInt(&ok);
                    if (!ok) return false;
                    info.m_field4 = ScanInt(&ok);
                    if (!ok) return false;
                    info.m_field8 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (0xef < idx)
                    {
                        printf("SCRIPT ERROR over count [%s] %s\n",
                               path, section.c_str());
                        return false;
                    }
                    script[scriptIdx].m_recipeInfoMap.insert(std::make_pair(idx, info));
                    script[scriptIdx].m_recipeIndexMap.insert(
                        std::make_pair(info.m_field0, idx));
                    script[scriptIdx].m_recipeLevelMap.insert(
                        std::make_pair(info.m_field0, info.m_field8));
                    script[scriptIdx].m_recipeItemMap.insert(
                        std::make_pair(info.m_field4, info.m_field0));
                }
            }
            else if (section == "[endurance repair cost]")
            {
                script[scriptIdx].m_enduranceRepairCost.clear();
                stEnduranceRepairCost r;
                for (;;)
                {
                    r.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    r.m_field4 = ScanInt(&ok); if (!ok) return false;
                    script[scriptIdx].m_enduranceRepairCost.push_back(r);
                }
            }
            else if (section == "[endurance reduce]")
            {
                script[scriptIdx].m_enduranceReduce1 = ScanInt(&ok);
                if (!ok) return false;
                script[scriptIdx].m_enduranceReduce2 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[endurance initial value]")
            {
                script[scriptIdx].m_enduranceInit = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[upgrade cost]")
            {
                script[scriptIdx].m_upgradeCost.clear();
                int idx;
                for (;;)
                {
                    idx = ScanInt(&ok);
                    if (!ok) break;
                    // N（P321-T5）：ORIG 0x0896c268..0x0896c26e 为
                    // `mov -0x48(%ebp),%eax; sub $0x1,%eax; mov %eax,%esi`（idx 槽 +
                    // 寄存器中转、无 expect 槽，P0 判据禁止引入具名 expect）；4.4 恒折
                    // `lea -0x1(%eax),%esi`。试过具名局部形（段长对齐但新增槽、违反 P0）
                    // 后回退内联表达式，记后端指令选择差。
                    if (idx - 1 != (int)script[scriptIdx].m_upgradeCost.size())
                        return false;
                    int v = ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_upgradeCost.push_back(v);
                }
            }
            else if (section == "[potion info]")
            {
                // 读取 (itemIndex, value) 对，按 itemIndex 写入两个全局炼金术值；
                // 非法 itemIndex 返回 false。
                // b290-BigExpert [REAL-BUG 修正 + P329/switch 形态]：ORIG
                // 0x0896c35d..0x0896c398 的 switch 判定树实证 0x27ac52 落
                // `mov %eax,0x94f777c`（= g_alchemistInfo[1]），只有 0x27ac59 落
                // `mov %eax,0x94f7778`（=[0]，0x0896c3a4 独此一路）——原码把
                // 0x27ac52 误并入 [0] 组 ⇒ 该药水索引写错全局。改为 switch 形，
                // 同时复现 ORIG「条件一次取入 %eax + 二分判定树」（4.1 对 || 链
                // 发数值二分树、4.4 对 if/else-if 发线性 cmpl 链 ⇒ 24 行 struct 差）。
                for (;;)
                {
                    int idx = ScanInt(&ok);
                    if (!ok) break;
                    int val = ScanInt(&ok);
                    if (!ok) return false;
                    // ORIG 0x0896c36c 判定树用 `ja`（无符号序）⇒ switch 表达式为
                    // unsigned（P321-T2：消费者指令定符号性）。
                    // P324：叶块物理序 = 书写序。ORIG 叶块序为 [1] 组（0x0896c39a）
                    // 在前、[0]（0x0896c3a4）在后 ⇒ [1] 组必须写在前面。
                    switch ((unsigned int)idx)
                    {
                    case 0x27ac52:
                    case 0x1f8e:
                    case 0x98981a:
                    case 0x98988d:
                    case 0x989885:
                    case 0x2920913e:
                        g_alchemistInfo[1] = val;
                        break;
                    case 0x27ac59:
                        g_alchemistInfo[0] = val;
                        break;
                    default:
                        return false;
                    }
                }
            }
            else if (section == "[gain exp]")
            {
                script[scriptIdx].m_gainExp1 = ScanInt(&ok);
                if (!ok) return false;
                script[scriptIdx].m_gainExp2 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[limit disjoint gold]")
            {
                script[scriptIdx].m_limitDisjointGold = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[portable disjoint info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_portableDisjointMap.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x211,
                            "import expertjob script - [portable disjoint info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[disjoint self service]")
            {
                // b290-BigExpert [P332 具名指针形：ORIG 0x0896c5ee..0x0896c5fd 于块首把
                // `&script[scriptIdx].m_selfDisjoin` 溢出到 -0x3c(%ebp)，随后 3 处
                // `mov -0x3c(%ebp),%eax` 重载 + `mov %eax,%edx; mov %edx,N(%eax)` 落值
                // （0x0896c610/0x0896c63d/0x0896c66b）。原逐点重算 imul/add 每点多 2 条。]
                STExpertJobScript::stSelfDisjoinMaterialInfo* selfDisjoin =
                    &script[scriptIdx].m_selfDisjoin;
                // N76 封顶：ORIG 0x0896c60e..0x0896c613 为 RHS-first + edx 中转
                // （`mov %eax,%edx; mov -0x3c(%ebp),%eax; mov %edx,(%eax)`），4.4 对
                // int 字段恒 LHS-first；按 P211/P248 档③试 (unsigned int)/
                // (unsigned long) 两种 RHS 异型化均被前端折叠（实测仍 LHS-first）
                // ⇒ 装载序旋钮不可用，记 N76（P321-T5：禁为装载序改类型）。
                selfDisjoin->m_field0 = ScanInt(&ok);
                if (!ok) return false;
                selfDisjoin->m_field4 = ScanInt(&ok);
                if (!ok) return false;
                selfDisjoin->m_field8 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[shop doll info]")
            {
                // b290-BigExpert [P332 具名指针形：ORIG 0x0896c6ab..0x0896c6ba 块首
                // `&script[scriptIdx].m_shopDollMap` → -0x38(%ebp)，循环内 insert 只
                // `mov -0x38(%ebp),%edx` 重载（0x0896c761）]。
                std::map<int, unsigned char>* shopDollMap =
                    &script[scriptIdx].m_shopDollMap;
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned char val = (unsigned char)ScanInt(&ok);
                    if (!ok) return false;
                    if (!shopDollMap->insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x236,
                            "import expertjob script - [shop doll info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[unseal scroll level limit info]")
            {
                // b290-BigExpert [P332 具名指针形：ORIG 0x0896c7d9..0x0896c7e8
                // `&script[scriptIdx].m_unsealScrollMap` → -0x34(%ebp)]。
                std::map<int, unsigned short>* unsealScrollMap =
                    &script[scriptIdx].m_unsealScrollMap;
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!unsealScrollMap->insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x24b,
                            "import expertjob script - [unseal scroll level limit info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[monstercard bind info]")
            {
                // b290-BigExpert [P332 具名指针形：ORIG 0x0896c908..0x0896c917
                // `&script[scriptIdx].m_monsterCardBindRateMap` → -0x30(%ebp)]。
                std::map<int, stMonsterCardBindRate>* monsterCardBindRateMap =
                    &script[scriptIdx].m_monsterCardBindRateMap;
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    stMonsterCardBindRate rate;
                    rate.m_field0 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    rate.m_field2 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!monsterCardBindRateMap->insert(
                            std::make_pair(key, rate)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x266,
                            "import expertjob script - [monstercard bind info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[monstercard bind only]")
            {
                int cnt[4] = { 0, 0, 0, 0 };
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    int idx = ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_specialBindInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field4;
                }
                for (int i = 0; i < 4; ++i)
                    script[scriptIdx].m_specialBindInfo.m_count[i] = cnt[i];
            }
            else if (section == "[monstercard bind list]")
            {
                int cnt[4] = { 0, 0, 0, 0 };
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    int idx = ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_commonBindInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field4;
                }
                for (int i = 0; i < 4; ++i)
                    script[scriptIdx].m_commonBindInfo.m_count[i] = cnt[i];
            }
            else if (section == "[alchemist extraction info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_alchemistExtractInfo.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2b1,
                            "import expertjob script - [alchemist extraction info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[enchanter extraction info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_enchanterExtractInfo.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2c4,
                            "import expertjob script - [enchanter extraction info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[doll_controller extraction info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_dollExtractInfo.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2d7,
                            "import expertjob script - [doll_controller extraction info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[alchemist extraction result item]")
            {
                int cnt[3] = { 0, 0, 0 };
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    std::string type;
                    // b290-BigExpert [P198②：ORIG 0x0896d048 call ScanStr 之后
                    // `mov %al,-0x22d(%ebp)`（0x0896d04c）+ `movzbl; xor $1; test; je`
                    // 三句形 ⇒ 原码是「先存具名 ok 再判」，非 `if (!ScanStr(..))`；
                    // ok 此后必被下一轮 ScanInt 覆写 ⇒ 无行为差]。
                    ok = ScanStr(&type);
                    if (!ok)
                        return false;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    // ORIG 0x0896d16e `movl $0x3,-0x1c(%ebp)`：idx 具名局部初值 3
                    // （三分类各自覆盖、else 直接 return ⇒ 值 3 从不被读，P171 照抄）。
                    int idx = 3;
                    if (type == "food")
                        idx = 0;
                    else if (type == "life")
                        idx = 1;
                    else if (type == "jewel")
                        idx = 2;
                    else
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2f9,
                            "import expertjob script - [alchemist extraction result item] result_type is invalid");
                        return false;
                    }
                    script[scriptIdx].m_alchemistResultInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field4;
                }
                for (int i = 0; i < 3; ++i)
                    script[scriptIdx].m_alchemistResultInfo.m_count[i] = cnt[i];
            }
            else if (section == "[enchanter extraction result item]")
            {
                int total = 0;
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    total += (int)res.m_field4;
                    script[scriptIdx].m_enchanterResultInfo.m_vec.push_back(res);
                }
                script[scriptIdx].m_enchanterResultInfo.m_count = total;
            }
            else if (section == "[doll_controller extraction result item]")
            {
                int cnt[3] = { 0, 0, 0 };
                while (true)
                {
                    stDollControllerResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field6 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    // b290-BigExpert [REAL-BUG 修正]：ORIG 0x0896d48f 第 4 次 ScanInt
                    // 落 `mov %ax,-0x37c(%ebp)` = res+0x8（m_field8），非 res+0x4；
                    // 收尾 0x0896d59c `movzwl -0x37c(%ebp)` 亦取 m_field8。原码写回
                    // m_field4 ⇒ ①覆盖第 2 个值（分类条件 0x0896d4ba 读的正是 res+4）
                    // ⇒ 分派用错操作数；②入表元素的 +8 字段恒为 ctor 链式残值。
                    res.m_field8 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    int idx = 3;
                    // b290-BigExpert [BigAuction 翻法三：ORIG 0x0896d4b3..0x0896d558 为
                    // 平铺 else-if 链 ⇒ 叶块物理序 = idx=0 / 1 / 2 / 报错，且每段条件
                    // 展开为「双 cmp + 反向跳转」的区间形（0x0896d4e3 cmp $0x1e jbe /
                    // 0x0896d506 cmp $0x32 jbe）。原嵌套 else 形落物理序反成
                    // 报错/2/1/0，且把 `> 0x1e` 写成 `< 0x1f` 变 `cmp $0x1f`。]
                    if (res.m_field4 != 0 && res.m_field6 <= 0x1e)
                    {
                        idx = 0;
                    }
                    else if (res.m_field4 > 0x1e && res.m_field6 <= 0x32)
                    {
                        idx = 1;
                    }
                    else if (res.m_field4 > 0x32 && res.m_field6 <= 0x50)
                    {
                        idx = 2;
                    }
                    else
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x33b,
                            "import expertjob script - [doll_controller extraction result item] result_type is invalid");
                        return false;
                    }
                    script[scriptIdx].m_dollResultInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field8;
                }
                for (int i = 0; i < 3; ++i)
                    script[scriptIdx].m_dollResultInfo.m_count[i] = cnt[i];
            }
        }
        return true;
    }
    return false;
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
{
    // b28d-solo [P16/P292：ORIG 0x0896ae3c `xor $1; test; je <主体>` 为「bool 形早返」]。
    if (!loadRDARScriptFile(g_expertJobScriptBaseDirectory, path))
        return false;
    script.Clear();
    std::string section;
    // b28d-solo [P171 照抄 ORIG 死局部：-0x30 std::string（0x0896ae69 构造、
    // 仅 EH/尾段 0x08707be0 析构，全程未使用）与 -0xc int（0x0896ae6e 清零后未读）]。
    std::string unusedSection;
    int unusedLine = 0;
    // b28d-solo [P198①：ORIG 0x0896ae75 `movb $0x0,-0x31` 为 bool 局部预清 ⇒ 两句式]。
    bool ok = false;
    // b28d-solo [P164：ORIG 0x0896ae7c cond 块在 body 之前（4.4 对 while 恒旋转到尾部）
    // ⇒ for(;;) + 头部 `if (!ScanType) break;`，循环外统一 return true]。
    for (;;)
    {
        if (!ScanType(section, true))
            break;
        if (section == "[expertjob level limit]")
        {
            while (true)
            {
                // ORIG 0x0896aeb8/0x0896aebc：循环体首 `movb $0`+`movw $0` 预清 ⇒ 声明=0+赋值。
                unsigned char level = 0;
                short needLevel = 0;
                level = (unsigned char)ScanInt(&ok);
                if (!ok) break;
                needLevel = (short)ScanInt(&ok);
                if (!ok) return false;
                if (!script.m_levelMap.insert(
                        std::make_pair(level, needLevel)).second)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptExpertJob.cpp",
                        __FUNCTION__, 0x4a,
                        "import expertjob etc script - [expertjob level limit] index duplicated");
                    return false;
                }
            }
        }
        else if (section == "[disjointer big win rate]")
        {
            while (true)
            {
                unsigned char key = 0;
                unsigned char val = 0;
                key = (unsigned char)ScanInt(&ok);
                if (!ok) break;
                val = (unsigned char)ScanInt(&ok);
                if (!ok) return false;
                if (!script.m_disjointerBigWinRate.insert(
                        std::make_pair(key, val)).second)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptExpertJob.cpp",
                        __FUNCTION__, 0x60,
                        "import expertjob etc script - [disjointer big win rate] index duplicated");
                    return false;
                }
            }
        }
    }
    return true;
}

// ============================================================================
// 构造（ORIG 0x896e312，键到 g_expertJobScriptBaseDirectory）
// ============================================================================
#include "CExpertJobList/CExpertJobList.inc"
    : m_jobMap(), m_etc() {}

// [b28d-Main·检查点21] CExpertJobList/STExpertJobEtcScript 的 dtor 已删（ORIG 无符号）。

// ============================================================================
// init（ORIG 0x896d8ec）
// [b2c-P167/P61] ORIG 0x0896d9f9 调 `_ZSt9make_pairIiRP17STExpertJobScriptE...`
// （第一模板参 **int**）；旧写法 `int job = ...; make_pair(job, script)` 传具名左值 ⇒
// 4.4 推导 int&（`_ZSt9make_pairIRiRP17...`）。ORIG 调用点 `mov %eax,-0x24(%ebp)`（函数
// 返回值落临时对象）+ `lea -0x24(%ebp),%edx; mov %edx,0x4(%esp)`（传该临时地址给
// `T1&&`）⇒ 第一实参是 **FindIndexByFullScan(...) 的 prvalue**（P61）。故把调用内联进
// make_pair 实参位、删具名 `int job`。
// b28d-solo-CExpertJobList 判词（NEAR / N82附 封顶）：133=133 助记符序列全等；
// 残差仅 job 槽位（ORIG -0x24 vs 本 TU -0x40）与其后 make_pair/pair/insert/end
// 四个临时槽整体差 4——4.4 标量分区排布 vs 4.1 按生命周期穿插（N82附；job 在
// 函数头/循环体首/循环尾三形实测均落 -0x40..-0x48，it/script/begin 临时槽已对齐）。
// ============================================================================
#include "CExpertJobList/init.inc"
{
    // ORIG 0x896d8ec：无空指针检查，直接遍历 g_expertJobScriptFileList 对象。
    const char* path = 0;   // ORIG -0x1c(%ebp)：循环内最后 path，ETC 失败时作 %s 参数
    // b28d-solo [P96/P50：ORIG 用非 const iterator + 独立 C1Ev 默认构造（-0x44）
    // + begin() 临时槽 -0x4c 后内联拷贝赋值；P28：增量段为 ppEi（后置 it++）]。
    std::map<int, std::string>::iterator it;
    for (it = g_expertJobScriptFileList.m_fileMap.begin();
         it != g_expertJobScriptFileList.m_fileMap.end(); it++)
    {
        path = it->second.c_str();
        if (path == 0)
            break;
        STExpertJobScript* script = new STExpertJobScript();
        if (!ImportExpertJobScript(script, path))
        {
            printf("expert job Script Error: %s\n", path);
            delete script;
            return false;
        }
        // ORIG 0x896d8ec：find 后 insert（NEW 二进制/源码曾丢失）
        m_jobMap.insert(std::make_pair(
            g_expertJobScriptFileList.FindIndexByFullScan(path), script));
    }
    if (!ImportExpertJobEtcScript(m_etc, "expertjob.etc"))
    {
        printf("ExpertJob ETC Script Error : %s\n", path);
        return false;
    }
    return true;
}

// ============================================================================
// destroy（ORIG 0x896dac6）
// [b2c-P167] ORIG 0x0896daff 调用 **_deEv**（`_ZNKSt17_Rb_tree_iteratorISt4pairIKi
// P17STExpertJobScriptEEdeEv` = operator*，返回 pair& 后 `mov 0x4(%eax),%eax` 取
// second）；旧写法 `it->second` 发 **ptEv** + `add $0x4` ⇒ 按 P167 改 `(*it).second`。
// b28d-solo-CExpertJobList 判词（AE）：P13 缓存 end()（ORIG 0x0896dada 前一次性求值、
// 条件只比较）+ P141 删手写 delete 判空（ORIG 仅 test/je 自带守卫）；42=42。
// ============================================================================
#include "CExpertJobList/destroy.inc"
{
    // b28d-solo [P13：ORIG 0x0896dada 前一次性求值 end()（循环条件只比较不重调）]。
    for (std::map<int, STExpertJobScript*>::iterator it = m_jobMap.begin(),
             e = m_jobMap.end(); it != e; ++it)
    {
        STExpertJobScript* script = (*it).second;
        delete script;
    }
}

// ============================================================================
// get（ORIG W 0x0822b5f2 调用链）
// ============================================================================
#include "CExpertJobList/get.inc"
{
    std::map<int, STExpertJobScript*>::iterator it = m_jobMap.find(job);
    // b28d-61 [P222 翻正，ORIG 0x0822b3bf `je +0x5d`→mov$0 岛、命中块 fall-through]：
    // 旧「否定早返」形 `if (it == end()) return 0; return it->second;` 在 4.4 产
    // 反向块序（je→命中块、mov$0 落 fall-through，diff 8）；改正向形后 32/32 全等。
    if (it != m_jobMap.end())
        return it->second;
    return 0;
}

// ============================================================================
// STExpertJobEtcScript::isExpertJobLimitLevel（ORIG W 0x0849f47c）
// 遍历 m_levelMap（map<uchar,short>），任一 mapped short == level 返回 true。
// 供 CExpertJob::IncreaseExpertJobExp 判断当前等级是否为边界上限。
// ============================================================================
#include "CExpertJobList/isExpertJobLimitLevel.inc"
{
    // [b28c-L29] ORIG W 0x0849f47c：循环尾用后置 it++（ppEi 带 dummy int 实参
    // 0x0 + sret 旧值槽 -0xc），非前置 ++it。
    for (std::map<unsigned char, short>::iterator it = m_levelMap.begin();
         it != m_levelMap.end(); it++)
    {
        if ((int)it->second == level)
            return true;
    }
    return false;
}
