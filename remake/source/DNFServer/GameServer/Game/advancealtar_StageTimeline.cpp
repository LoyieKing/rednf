// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 advancealtar_StageTimeline/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ advancealtar::StageTimeLineParameter 实现（EventStageDeps）。
// 覆盖 ORIG 符号（均经 nm 确认 T/W）：
//   - reset()                        0x08899ef6 T
//   - importStageTimeLineParameter() 0x08899f26 T
//   - debugLog()                     0x0889a02c T（ORIG 纯 ret）
//   - importSummonObject()           0x0889a21e T
//   - importActionDefine()           0x0889a56e T
// 以及仅被本域使用的元素类型生命周期：
//   - _SummonObject::_SummonObject   0x08134650 W（memset 0x24）
//   - _Action::_Action               0x0889b978 W（memset 0x14）
//   - ActionDefine C1/D1/reset       0x08899c62/0x08899c82/0x08899c98 T
// 逐函数对照 docs/class_func_reports/advancealtar__StageTimeLineParameter/
// [G5 2026-09-10 · L12 round3 复核] 2 件全核（p167 + widthscan + 判据⑪）：
//   · importStageTimeLineParameter：判据⑪ = (b) 整帧平移（79/79）⇒ N126 封顶，
//     p167 MISSING=EXTRA=2（别名）。
//   · importActionDefine（gap +11）：widthscan 报 `clr_l orig=0 our=10`
//     （我方 9×`movl $0x0,-0x10`=state、1×`-0x14`=err）⇒ ORIG 的 err/state 由
//     esi/ebx 寄存器承载（无槽）；本波 P231 纯 register（非 asm 绑定，P0 合规）
//     实测 286→288 insns 反劣 ⇒ 回滚，判 N（槽/寄存器分配差）；p167 MISSING=EXTRA=8
//     （别名）⇒ 无语义缺口。
//   · [C05③ 2026-09-10 F3c 复核] ORIG 同目标 3 条（0889a5dd/0889a5f4/0889a60b
//     `jne 0x0889a64c` = body 入口）在我方是 `||` 短路 bool 物化形
//     （`mov $1/$0,%eax; test %al,%al; je`，非 `jmp+nop` 岛）；两侧 `jmp+nop` 岛
//     计数 2:2（函数头 + 内层回边）⇒ 闸门「我方岛 − ORIG 岛 ≥ 2」不成立，
//     守卫亦非源码相邻独立 `if(cond) goto L;`（是 `||` 条件表达式）⇒ 维持 N。
# include "CDataManager.h"    // advancealtar::StageTimeLineParameter/_SummonObject/
                              // _Action/ActionDefine 权威声明
#include "DNFLexWrapper.h"    // loadRDARScriptFile/ScanType/ScanInt(bool*)/ScanStr

#include <cstdio>
#include <cstring>

namespace advancealtar
{

// ---- 元素类型生命周期 ------------------------------------------------------

// ORIG 0x08134650 W：memset(this, 0, 0x24)。
#include "advancealtar_StageTimeline/_SummonObject.inc"
{
    std::memset(this, 0, sizeof(*this));
}

// ORIG 0x0889b978 W：memset(this, 0, 0x14)。
#include "advancealtar_StageTimeline/_Action.inc"
{
    std::memset(this, 0, sizeof(*this));
}

// ORIG 0x08899c62 T：+0x00 清零 + vector<_Action>@+0x04 默认构造。
#include "advancealtar_StageTimeline/ActionDefine.inc"
    : m_id(0), m_actions()
{
}

// ORIG 0x08899c82 T：析构 vector<_Action>（成员自动析构即一致）。
#include "advancealtar_StageTimeline/dtor_ActionDefine.inc"
{
}

// ORIG 0x08899c98 T：id=0 + m_actions.clear()。
#include "advancealtar_StageTimeline/reset.inc"
{
    m_id = 0;
    m_actions.clear();
}

// 布局断言（报告/nm 实证尺寸）。
typedef char assert_summon_0x24[(sizeof(_SummonObject) == 0x24) ? 1 : -1];
typedef char assert_action_0x14[(sizeof(_Action) == 0x14) ? 1 : -1];
typedef char assert_actdef_0x10[(sizeof(ActionDefine) == 0x10) ? 1 : -1];

// ---- StageTimeLineParameter ------------------------------------------------

// ============================================================================
// reset @ 0x08899ef6：清两个 map + TimeLine::reset（0x08899dfa，位于
// advancealtar_StageControl.cpp）。无其它副作用。
// ============================================================================
#include "advancealtar_StageTimeline/reset_1.inc"
{
    m_summonMap.clear();
    m_actionMap.clear();
    m_timeLine.reset();
}

// ============================================================================
// debugLog @ 0x889a02c：push %ebp / mov %esp,%ebp / pop %ebp / ret —— 纯空函数。
// ============================================================================
#include "advancealtar_StageTimeline/debugLog.inc"
{
}

// ============================================================================
// importStageTimeLineParameter @ 0x8899f26：
//   reset() → loadRDARScriptFile("", path) 失败返 0；
//   循环 ScanType(token,true)：
//     - 失败（EOF）→ 返 0；
//     - "[summon object]" → importSummonObject()，非 0 即作为错误码返回；
//     - "[action define]" → importActionDefine()，非 0 即作为错误码返回；
//     - 其它 token 跳过。
// 返回 0 = 成功（init.md 口径非零=Fail），非 0 = 子段错误码。
// ============================================================================
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N78/N82）→ 试=两 token 分支倒置（E2 实测：83/82 gap=+1 与四数 71/115/0/3/42 逐项不变 ⇒ 回滚）；前任显式 `continue;` 补 ORIG 第 3 回边块无效（82→82，4.4 合并 fall-through）⇒ 维持。
#include "advancealtar_StageTimeline/importStageTimeLineParameter.inc"
{
    // b28d-35: DIFF→N 封顶（N82 帧/槽 + N78 内联清零族）。ORIG @0x08899f26。
    std::string token;
    int unused18 = 0;                   // ORIG -0x18 死存储（保留以对齐栈帧）
    bool unused11 = false;              // ORIG -0x11 死存储（同上）

    reset();
    if (!loadRDARScriptFile("", path))
        return 0;                       // ORIG 08899f69: ebx=0

    for (;;)
    {
        if (!ScanType(token, true))     // ORIG 08899f88
            return 0;                   // 08899ff4..ffa: ebx=0

        if (token == "[summon object]")
        {
            int r = importSummonObject();
            if (r != 0)
                return r;
            continue;                   // ORIG 08899f73 → 循环头
        }
        if (token == "[action define]")
        {
            int r = importActionDefine();
            if (r != 0)
                return r;
            continue;                   // ORIG 08899f79 → 循环头
        }
        // 未知 token：直接回循环头（ORIG 无额外分支）。
        // [S3Stage 2026-09-11 实测] 显式 `continue;` 试补 ORIG 的第 3 个回边块无效
        //   （82→82，块被 GCC 合并到 fall-through）⇒ 该 1 条差属 4.1 vs 4.4.6 布局，
        //   判 N 封顶（本件维持 83/82）。
    }
}

// ============================================================================
// importSummonObject @ 0x889a21e：
//   段内逐行：key isEnemy("enemy"/"friend") type("normal"/"named"/"boss")
//   i1 i2(存低16位) i3 i4 i5 i6 → 插入 m_summonMap（insert，key=m_id）。
//   行首 ScanInt 失败 = 段结束（[/summon object]/EOF）→ 返 0；
//   任一子项失败 → 返对应错误码。
// ============================================================================
#include "advancealtar_StageTimeline/importSummonObject.inc"
{
    bool ok = false;                                // ORIG -0x61，入口置 0
    for (;;)                                        // ORIG 0889a230
    {
        _SummonObject obj;                          // ctor memset 0x24

        obj.m_id = ScanInt(&ok);                    // ORIG 0889a244
        if (!ok)
            return 0;                               // 段尾/EOF（0889a25a: ebx=0）

        std::string enemyTok;
        if (!ScanStr(&enemyTok))                    // ORIG 0889a275
            return 0x20b;
        if (enemyTok == "enemy")                    // ORIG 0889a290
            obj.m_isEnemy = 1;
        else if (enemyTok == "friend")              // ORIG 0889a2b0
            obj.m_isEnemy = 0;
        else
        {
            printf("advancealtar : SummonObject.isEnemy undefined='%s'\n",
                   enemyTok.c_str());
            return 0x218;
        }

        std::string typeTok;
        if (!ScanStr(&typeTok))                     // ORIG 0889a30b
            return 0x21d;
        if (typeTok == "normal")                    // ORIG 0889a326
            obj.m_type = 0;
        else if (typeTok == "named")                // ORIG 0889a349
            obj.m_type = 1;
        else if (typeTok == "boss")                 // ORIG 0889a36c
            obj.m_type = 2;
        else
        {
            printf("advancealtar : SummonObject.Type undefined='%s'\n",
                   typeTok.c_str());
            return 0x22e;
        }

        obj.m_field0c = ScanInt(&ok);               // ORIG 0889a3bf
        if (!ok) return 0x232;
        obj.m_field10 =
            static_cast<unsigned short>(ScanInt(&ok));  // ORIG 0889a3ea（mov %ax）
        if (!ok) return 0x235;
        obj.m_field14 = ScanInt(&ok);               // ORIG 0889a413
        if (!ok) return 0x238;
        obj.m_field18 = ScanInt(&ok);               // ORIG 0889a43b
        if (!ok) return 0x23b;
        obj.m_field1c = ScanInt(&ok);               // ORIG 0889a463
        if (!ok) return 0x23e;
        obj.m_field20 = ScanInt(&ok);               // ORIG 0889a48b
        if (!ok) return 0x241;

        // ORIG 0889a4aa..0x2d4：make_pair(obj.m_id, obj) → map insert。
        m_summonMap.insert(std::make_pair(obj.m_id, obj));
    }
}

// ============================================================================
// importActionDefine @ 0x889a56e：
//   外层循环每轮重建 ActionDefine；ScanType(token,true)：
//     - 失败（段内 EOF）→ 错误 0x253；
//     - "[/action define]" → 返 0（正常收段，调用方按 0 继续）；
//     - "[normal]"/"[named]"/"[boss]" → ad.id=ScanInt(ok)（失败 0x26b），随后
//       内层循环逐行 _Action：summonId=ScanInt(ok)（失败=行耗尽→插入并续外层），
//       find(summonId) 未命中 → printf + 0x27a；m_pSummon=&命中值；
//       firstDelayMs<0 → 0x285；nextDelayMs<500 → 0x290；
//       第 4 列 !=0 → flag=1 且 flagValue=该列低 16 位。push_back 后循环。
//       插入 make_pair(ad.id, ad) 后外层继续。
//     - 其它 token → printf "Type undefined" + 0x267。
// ============================================================================
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=新口径 n1_v 275/274 gap=+1 blocks=8；`||` 链 bool 物化（我方 `mov $0x1,%eax; jmp; mov $0x0,%eax; test %al,%al` 岛）vs ORIG 三独立 cmp + 块序；P231 register×2（286→288）、S3Stage 去 err/state 具名（287 反劣 1）均回滚 ⇒ 维持（ORIG esi/ebx 为多 return 点返回临时，非源码变量）。
#include "advancealtar_StageTimeline/importActionDefine.inc"
{
    // b28d-35: DIFF→N82（帧/槽 + 表遍历形状）。ORIG @0x0889a56e。
    bool ok = false;                                // ORIG -0x41，入口置 0
    for (;;)                                        // ORIG 0889a57d
    {
        ActionDefine ad;                            // ctor：id=0 + vector 构造
        std::string tok;
        // [G5 2026-09-10 widthscan 线索·实测反劣已回滚] `clr_l orig=0 our=10`
        //   （我方 9×`movl $0x0,-0x10` = state、1×`-0x14` = err）；ORIG 注释的
        //   esi/ebx 寄存器承载成立，本波 P231 纯 register（非 asm 绑定，P0 合规）
        //   实测 286→288 insns（反劣）⇒ 回滚，判 N（err/state 槽形为 4.1/4.4 分配差）。
        // [S3Stage 2026-09-11 重攻] 实测 register×2（去 err 初值）= 287 反劣 1 条
        //   （多 push %edi、帧 0x80→0x9c），已回滚。⇒ ORIG 的 esi/ebx 非源码变量，
        //   而是 **GCC 为多 return 点生成的返回临时 + 状态标记**（0x889a8d6/dd/e4 的
        //   `mov $N,%ebx` 常数重物化只在「临时」上出现；具名变量必落槽）。
        //   本轮改法 = 直接 `return <码>;`（去掉 err/state 具名变量），让 4.4 自产该结构。
        if (!ScanType(tok, true))                   // ORIG 0889a5a1
            return 0x253;                           // ORIG 0889a5ad：esi=0x253/ebx=0
        else if (tok == "[/action define]")         // ORIG 0889a5bc
            return 0;                               // ORIG 0889a5d3：ebx=1
        else if (tok == "[normal]" || tok == "[named]" ||
                 tok == "[boss]")                   // ORIG 0889a5dd..64c
        {
            ad.m_id = ScanInt(&ok);                 // ORIG 0889a652
            if (!ok)
                return 0x26b;                       // ORIG 0889a665
            else
            {
                for (;;)                            // ORIG 0889a674（动作行循环）
                {
                    _Action act;                    // ctor memset 0x14

                    act.m_summonId = ScanInt(&ok);  // ORIG 0889a685
                    if (!ok)
                        break;                      // 行耗尽 → 插入本 ActionDefine

                    std::map<int, _SummonObject>::iterator it =
                        m_summonMap.find(act.m_summonId);   // ORIG 0889a6b0
                    if (it == m_summonMap.end())
                    {
                        printf("advancealtar : not found SummonObject Id = %d\n",
                               act.m_summonId);
                        return 0x27a;               // ORIG 0889a6f6
                    }
                    act.m_pSummon = &it->second;    // ORIG 0889a70b：iter 值 +0x04

                    act.m_firstDelayMs = ScanInt(&ok);  // ORIG 0889a71c
                    if (!ok)
                        return 0x27f;               // ORIG 0889a72f
                    if (act.m_firstDelayMs < 0)
                    {
                        printf("advancealtar :  action.firstDelayMs %d < 0\n",
                               act.m_firstDelayMs);
                        return 0x285;               // ORIG 0889a758
                    }

                    act.m_nextDelayMs = ScanInt(&ok);   // ORIG 0889a76d
                    if (!ok)
                        return 0x289;               // ORIG 0889a780
                    if (act.m_nextDelayMs < 500)
                    {
                        printf("advancealtar :  action.nextDelayMs %d < 500\n",
                               act.m_nextDelayMs);
                        return 0x290;               // ORIG 0889a7ac
                    }

                    int flagVal = ScanInt(&ok);     // ORIG 0889a7c1（-0xc）
                    if (!ok)
                        return 0x294;               // ORIG 0889a7d4
                    // [S3Stage 2026-09-11] ORIG 0889a7e7 `jne 0889a7ef` ⇒ 假路径
                    //   （flag=0）在**前**、真路径在后 = 源码条件为 `== 0`（否定式），
                    //   我方旧形 `!= 0` 产 then-first 多 1 条 jmp。
                    if (flagVal == 0)
                    {
                        act.m_flag = 0;             // ORIG 0889a7e9 显式写 0
                    }
                    else
                    {
                        act.m_flag = 1;
                        act.m_flagValue =
                            static_cast<unsigned short>(flagVal);   // mov %ax
                    }
                    ad.m_actions.push_back(act);    // ORIG 0889a80a
                }

                // ORIG 0889a815..85f：make_pair(ad.m_id, ad) → map insert。
                // （行耗尽/插入成功都落到这里；ORIG 的 `state = 2` 由 4.4 自产临时承载）
                m_actionMap.insert(std::make_pair(ad.m_id, ad));
            }
        }
        else
        {
            printf("advancealtar : ActionDefine.Type undefined='%s'\n",
                   tok.c_str());
            return 0x267;                           // ORIG 0889a63d
        }
        // 成功插入 → 继续外层循环（ORIG LAB_0889a8c0 之后的 continue 路径）。
    }
}

}  // namespace advancealtar
