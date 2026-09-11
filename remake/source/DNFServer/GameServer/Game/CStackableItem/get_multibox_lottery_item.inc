// [b28d-solo-CStackableItem] AE 214=214。修法（P300/P248 档②）：本函数 `flag` 局部
// char→bool + **LotteryUnit::m_defaultFlag** 字段 char→bool（MultiBoxLotteryInfo.h，
// 已 R2 广播）⇒ `flag = unit.m_defaultFlag;` 与 `p.second = flag;` 均产 ORIG 直存
// `movzbl -0x5c; mov %al`（此前 char 版 4.4 归一化 `cmpb/setne` 多 1 条）。
// 注意**同名字段分属两类、真型相反**：stDefaultItemInfo::m_defaultFlag 保持 char
// （其消费点 get_multibox_default_item / createStackableMultiBoxLotteryInfo 要求
// char 形）——字段级分型后三件同时满足（见各自判词）。
// ORIG 0x850edda：rand/sum/itemIdx/count/flag 声明在函数头（-0x24/-0x20/-0x1c/-0x18/-0xd）；
// 外层 while 每次调 end()、增量 unitIt++（ppEi）；unit 拷贝构造落 -0x68；内层双迭代器
// itemIt(-0x40)/bonusIt(-0x44) 均 ppEi，字段访问走 operator*（deEv）。
void CStackableItem::get_multibox_lottery_item(
    std::vector<std::pair<Inven_Item, bool> >* out)
{
    if (out == 0)
    {
        return;
    }
    int rand = 0;
    int sum = 0;
    int itemIdx = 0;
    int count = 0;
    // b28d-solo 修法：flag char→bool（P300/P248 档②，配 m_defaultFlag bool 化 ⇒ 直存）。
    bool flag = 0;
    std::vector<MultiBoxLotteryInfo::LotteryUnit>::iterator unitIt =
        m_multiBox.m_units.begin();
    while (unitIt != m_multiBox.m_units.end())
    {
        MultiBoxLotteryInfo::LotteryUnit unit(*unitIt);
        for (int i = 0; i < unit.m_count; i++)
        {
            std::vector<std::pair<int, int> >::iterator it =
                unit.m_items.begin();
            std::vector<std::pair<int, int> >::iterator it2 =
                unit.m_bonus.begin();
            rand = get_rand_int(1000000);
            sum = 0;
            itemIdx = 0;
            while (it != unit.m_items.end())
            {
                sum += (*it).second;
                if (sum > rand)
                {
                    itemIdx = (*it).first;
                    count = (*it2).first;
                    flag = (*it2).second != 0;
                    break;
                }
                it++;
                it2++;
            }
            if (itemIdx == 0)
            {
                itemIdx = unit.m_defaultIdx;
                count = unit.m_defaultNum;
                flag = unit.m_defaultFlag;
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
        unitIt++;
    }
}