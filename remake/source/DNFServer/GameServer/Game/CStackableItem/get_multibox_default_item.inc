// [b28d-solo-CStackableItem] **AE-EQUAL 153=153**（本波修复，原 N18b 判词作废）。
// 修法 = 字段侧异型化（P274/P321-S1 一次旋钮）：stDefaultItem::m_prob int→unsigned int
// （MultiBoxLotteryInfo.h:33，Main 代落 + R2 广播；同宽 4B、偏移 +0x08/sizeof 0x10/
// mangled 零变化）。ORIG 0x0850f1bd 累加段 = `mov 0x8(%eax),%edx; mov -0x1c(%ebp),%eax;
// lea (%edx,%eax,1),%eax; mov %eax,-0x1c(%ebp)`（读-加-写四段）：4.4 对「int 字段 +
// int 累加器」恒折 `mov mem,%eax; add %eax,sum`（N18b），对「unsigned 字段 + int 累加器」
// 逐条复现（探针 /tmp/probe_sum2.cpp a1；int 侧 a5/a6 均折）。sum 保持 int ⇒ 后续
// `sum > rand` 仍为 ORIG 有符号 `jle`。volatile 化实测槽漂移(-0x1c→-0x28)+比较物化，已回退。
// 本件 `flag = stDefaultItemInfo::m_defaultFlag != 0` 的 test/setne 物化已匹配
// （该字段保持 char，见 get_multibox_lottery_item 判词的字段分型说明）。
// ORIG 0x850f136：rand/sum/itemIdx/count/flag 声明在函数头（-0x20/-0x1c/-0x18/-0x14/-0xd），
// 循环体内先 `it = begin()`（-0x2c）再 get_rand_int(1000000)；内层 while 每次调 end()、
// 增量 `it++`（ppEi）。
void CStackableItem::get_multibox_default_item(
    std::vector<std::pair<Inven_Item, bool> >* out)
{
    if (out == 0)
    {
        return;
    }
    int rand = 0;
    // [b28d-solo-CStackableItem] ORIG 0x0850f1bd 累加段 = 读-加-写四段
    // `mov 0x8(%eax),%edx; mov -0x1c(%ebp),%eax; lea (%edx,%eax,1),%eax;
    // mov %eax,-0x1c(%ebp)`；4.4 对「int 字段 + int 累加器」恒折
    // `mov mem,%eax; add %eax,sum`（N18b）。volatile 化实测槽漂移(-0x1c→-0x28)+
    // 比较物化(setg/test/je)，已回退。唯一复现形 = 字段侧 unsigned 化（异型 ⇒ 4.4
    // 不折叠；探针 /tmp/probe_sum2.cpp a1 逐条同形）⇒ 走 R2 报 Main 代落
    // MultiBoxLotteryInfo.h:33 stDefaultItem::m_prob int→unsigned int。
    int sum = 0;
    int itemIdx = 0;
    int count = 0;
    bool flag = false;
    for (unsigned int i = 0;
         i < (unsigned int)m_multiBox.m_default.m_defaultCount; i++)
    {
        std::vector<MultiBoxLotteryInfo::stDefaultItem>::iterator it =
            m_multiBox.m_default.m_items.begin();
        rand = get_rand_int(1000000);
        sum = 0;
        itemIdx = 0;
        while (it != m_multiBox.m_default.m_items.end())
        {
            sum += (*it).m_prob;
            if (sum > rand)
            {
                itemIdx = (*it).m_itemIdx;
                count = (*it).m_count;
                flag = (*it).m_flag != 0;
                break;
            }
            it++;
        }
        if (itemIdx == 0)
        {
            itemIdx = m_multiBox.m_default.m_defaultIdx;
            count = m_multiBox.m_default.m_defaultNum;
            flag = m_multiBox.m_default.m_defaultFlag != 0;
        }
        Inven_Item item;
        item.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, item, count);
        std::pair<Inven_Item, bool> p;
        p.first = item;
        p.second = flag;
        out->push_back(p);
    }
}