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
