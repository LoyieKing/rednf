// ORIG 0x89bafd1：解析 multibox 彩票脚本。
//   a (vector<vector<int>>)：每元素是一个 LotteryUnit 的原始 int 流
//     [count, defaultIdx, defaultNum, defaultFlag, (itemIdx,weight,count,flag)xN]
//     —— m_items.push_back({itemIdx,weight})，m_bonus.push_back({count,flag})
//   b (vector<int>)：field8=b[0]，fieldc=b[1]
//   c (vector<int>)：m_defaultCount/Idx/Num/Flag + m_items(stDefaultItem) 流
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// [b28d-solo-CStackableItem] p167 收口（本波核心修法，ORIG 调用集 106 逐符号对齐）：
// ①c 部分 ORIG 是 5 个**裸早返** `if (it == c->end()) return;`（0x089bb694/0x089bb6fe/
//   0x089bb768/0x089bb7d2/0x089bb83c 各落独立 nop 中继岛 0xa51/0xa54/0xa57/0xa5a/0xa64，
//   P223 岛形），原「嵌套 if (it != c->end())」写法恒产 ne×5（p167 EXTRA ne×5 /
//   MISSING eq×6）⇒ 改写后 ne/eq 全部归位。
// ②a 内层循环与 c 循环均为**底部旋转**（a：0x089bb45a eq 检查 + 0x089bb493 ne 条件同在
//   循环尾；c：body 0x089bb87d / 条件 0x089bb9e2）⇒ 源码 do-while 形。
// ③真语义 bug 修正：a 内层第 4 个边界检查在 **push_back 之后**（ORIG 0x089bb45a 的
//   `eq; jne 0x4ab` 落在 m_units.push_back 之前）——原写法在 push 前 break 会丢弃
//   「恰好用完最后一个元素」的那一组；c 循环体内同样只有 3 个中间检查（m_flag 后无检查）。
// ④p167 终态（b28d-poolcera 更新）：**106/106 全 CLEAR**。原 EXTRA（D1×2 +
//   _Unwind_Resume×1）根因 = 我方 MultiBoxLotteryInfo.h 给用户空析构（非平凡）；
//   ORIG `_Destroy_aux<true>` 实证 stDefaultItem 析构平凡 ⇒ 删声明+定义后 D1 岛与
//   landing pad 自然消失（非 N91 codegen 差，判词翻案）。行数 668→649（ORIG 652，
//   gap 3）；⑤framePad 删除（P232 回推形随 D1 岛消失而反向，保留=整体深 4；删后
//   ndiff 1187→15）+ 第5检查翻形 B `if (!(it == c->end())) { do-while }`（je 极性
//   归位，探针 F/G/cmp 形不可再翻）。终残差 = 3 段 `mov (%eax),%eax; mov %eax,%edx`
//   两段装载被 4.4 折单段（N18a，P248 三档类型旋钮已封）+ `!eq` 的 xor $1 物化
//   vs ORIG je+jmp 双跳（N18a 折叠族，cargo_abs 先例）。ndiff 359→15。
// [b28d-solo-CStackableItem 2026-09-09 二轮] **gap 3→0（fresh 652=652）**，旧判词
// 「3 段两段装载 N18a 封顶」**作废**：真因 = stDefaultItemInfo 三字段
// m_defaultCount/m_defaultIdx/m_defaultNum 建模成 int（应为 unsigned int，P254/
// P211 档③同异型侧）。直证（同函数、同一 RHS `*it` int、唯字段类型不同）：
//   · 本类字段 store = `mov (%eax),%eax; mov %eax,%edx; mov 0x8(%ebp),%eax;
//     mov %edx,0x1c(%eax)`（0x089bb6af / 0x089bb719 / 0x089bb783，+0x1c/+0x20/+0x24）
//     —— 带 eax→edx 中转 = 与 int 异型；
//   · 同函数 MultiBoxLotteryInfo::m_field8/m_fieldc（源码 int）store =
//     `mov (%eax),%edx; mov 0x8(%ebp),%eax; mov %edx,0x8(%eax)`（0x089bb5c7 /
//     0x089bb62f）—— 3 条无中转 = 同型。
// 旁证：get_multibox_default_item 0x0850f371 读 m_defaultCount 后 `cmp i; seta`
// （无符号 above，循环变量 unsigned int）且读取单段（unsigned 字段读形）。
// 探针（446 后端与路由一致）：/tmp/probe_mb.cpp（int 字段=3 条 / unsigned=4 条带中转）、
// /tmp/probe_mb2.cpp（unsigned 字段读 + unsigned 循环上界与 ORIG 同形零回退）。
// 头改动 MultiBoxLotteryInfo.h 三字段 int→unsigned int（同宽 4B，sizeof/偏移/mangled
// 零变化），Main 批准 + all 广播；消费 TU 复编：CStackableItem 1331/3/6（零回退）、
// MultiBoxLotteryInfo 113/0/1（零回退）、GmCmds_F 5/18/1/1（零回退）。
// 终残差 = 1 处 `!eq` 的 xor $1 物化 vs ORIG `test; je body; jmp tail`（等长替换，
// 652=652）：跨版本 `if(!bool)` lowering 差（4.1 块交换 / 4.4 xor 物化），探针
// /tmp/probe_mb3.cpp f_sw/f_goto/f_else/f_while/f_neg 五形均不可造 ⇒ 判 N（P292/N18a 族）。
void createStackableMultiBoxLotteryInfo(
    MultiBoxLotteryInfo* info,
    std::vector<std::vector<int> >* a, std::vector<int>* b,
    std::vector<int>* c)
{
    // b28d-poolcera：stDefaultItem 平凡化后删 P232 framePad（原判词回推的槽基
    // 已随 D1 岛/EH 帧变化，保留反而整体深 4；实测删除后槽与 ORIG 逐一对齐）。
    if (info == 0 || a == 0 || b == 0)
    {
        return;
    }
    info->clear();
    // ---- 解析 a：构建 m_units ----
    // [b28d-solo-CStackableItem] ORIG 0x89bafd1：`vec(*itA)` → `it = (*itA).begin()`
    // → `unit` 构造/clear（it 先于 unit）；所有边界比较用 **(*itA).end()**（原码如此，
    // 非 vec.end()）；每次 `*it` 与 `it++` 为两条独立语句。
    std::vector<std::vector<int> >::iterator itA = a->begin();
    while (itA != a->end())
    {
        std::vector<int> vec(*itA);
        std::vector<int>::iterator it = (*itA).begin();
        MultiBoxLotteryInfo::LotteryUnit unit;
        unit.clear();
        unit.m_count = *it;
        it++;
        unit.m_defaultIdx = *it;
        it++;
        if (it == (*itA).end())
        {
            break;
        }
        unit.m_defaultNum = *it;
        it++;
        if (it == (*itA).end())
        {
            break;
        }
        unit.m_defaultFlag = (*it != 0);
        it++;
        if (it == (*itA).end())
        {
            break;
        }
        // [b28d-solo-CStackableItem] ORIG 内层循环为底部旋转（0x089bb45a eq 检查
        // 与 0x089bb493 ne 条件都在内层循环尾）⇒ 源码 do-while 形。
        do
        {
            std::pair<int, int> p_item, p_bonus;
            p_item.first = *it;
            it++;
            if (it == (*itA).end()) break;
            p_item.second = *it;
            it++;
            if (it == (*itA).end()) break;
            p_bonus.first = *it;
            it++;
            if (it == (*itA).end()) break;
            p_bonus.second = *it;
            it++;
            unit.m_items.push_back(p_item);
            unit.m_bonus.push_back(p_bonus);
            // [b28d-solo-CStackableItem] ORIG 0x089bb45a：第 4 个边界检查在
            // **push_back 之后**（`eq; jne 0x4ab` 落在 m_units.push_back 之前），
            // 即最后一组仍被提交后再退出（原写法在 push 前 break = 真语义 bug）。
            if (it == (*itA).end()) break;
        } while (it != (*itA).end());
        info->m_units.push_back(unit);
        itA++;
    }
    // ---- 解析 b：field8 / fieldc ----
    if (!b->empty())
    {
        std::vector<int>::iterator it = b->begin();
        info->m_field8 = *it;
        it++;
        if (it == b->end())
        {
            return;
        }
        info->m_fieldc = *it;
        it++;
    }
    // ---- 解析 c：默认项 + m_items ----
    // [b28d-solo-CStackableItem] P167/P223：ORIG 0x089bb694/0x089bb6fe/0x089bb768/
    // 0x089bb7d2/0x089bb83c 五处 `eq(it,end)` 各落独立 nop 中继岛（0xa51/0xa54/
    // 0xa57/0xa5a/0xa64）⇒ 源码为 5 个裸早返 `if (it == c->end()) return;`（不是
    // 嵌套 `if (it != c->end())`：后者恒产 ne ×5，p167 实测 EXTRA ne×5/MISSING eq×6）。
    std::vector<int>::iterator it = c->begin();
    if (it == c->end())
    {
        return;
    }
    info->m_default.m_defaultCount = *it;
    it++;
    if (it == c->end())
    {
        return;
    }
    info->m_default.m_defaultIdx = *it;
    it++;
    if (it == c->end())
    {
        return;
    }
    info->m_default.m_defaultNum = *it;
    it++;
    if (it == c->end())
    {
        return;
    }
    info->m_default.m_defaultFlag = (char)*it;
    it++;
    if (!(it == c->end()))
    {
    // [b28d-solo-CStackableItem] ORIG c 循环为底部旋转（body 0x089bb87d、条件
    // 0x089bb9e2 的 end+ne）⇒ do-while（首轮由上一早退检查保证）。
    do
    {
        MultiBoxLotteryInfo::stDefaultItem item;
        item.m_itemIdx = *it;
        it++;
        if (it == c->end()) break;
        item.m_prob = *it;
        it++;
        if (it == c->end()) break;
        item.m_count = *it;
        it++;
        if (it == c->end()) break;
        item.m_flag = (char)*it;
        it++;
        info->m_default.m_items.push_back(item);
    } while (it != c->end());
    }
}