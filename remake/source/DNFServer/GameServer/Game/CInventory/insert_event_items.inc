// ============================================================================
// CInventory::insert_event_items（ORIG 0x08506bb2，_ZN10CInventory18insert_event_itemsERKSt6vectorISt4pairIiiESaIS2_EERS0_IiSaIiEE15eMoneyAddReason14eItemAddReasonbPKcSD_）。
// items 元素 {itemIdx, count}；itemIdx==0 为金钱条目（>499999 走 cMyTrace 审计，
// gain_money 后 outSlots.push(0,0,gained)，差额邮件兜底）。
// 物品条目按 is_stackable/IsAvatarItem/造物蛋(type 0x16) 三路分发：
//   堆叠 → make_item+insertItemIntoInventory(reason,1,1)；
//   头像 → genIPGNo+AddAvatarItem(itemIdx,usablePeriod,...)+GetRemainDate；
//   造物蛋 → create_item+SetEggItem+insertItemIntoInventory(8,1,0)
//            +InsertCreatureItem；其余非堆叠逐份插入，失败邮件兜底。
// reason==8 时累计 cube 统计与 Store 卖价，函数尾 AddValueStatistic(10)。
// ORIG 怪癖（忠实保留）：头像/造物蛋在 itemReason!=8、SetEggItem 失败时
// 不推进迭代器直接回到循环条件；堆叠/头像插入失败且无邮件时跳到函数尾。
// 依赖：docs/class_func_reports/CInventory/insert_event_items.md。
// [N82 封顶] batch-10 续派：调用点差集 95=95 归零（it++ 后置×5/共用尾、cur=pair 临时
// && 赋值、Iterator 默认构造+赋值、push_back(int&&) 物化 5 处、GetRemainDate 返回值
// 直推、money 段 cnt 覆写单槽形）；残余 |gap|=4 与 jmp/nop/xor 布局为 N82 残差。
// [b28d-tu-CInventory] 本轮复核（P329/方法3 均不适用）：本件无具名中间局部可删
//   （buf 族不存在）；残差主体 = 浅槽全体 4-8B 平移 + 帧 0x2fc vs 0x2dc（我方多
//   8 槽 = 4.4 循环体内 pair/push_back 临时不塌槽，P331 探针1）+ 深槽 bLog/
//   临时组 0x20 平移（4.1 lifetime 复用）⇒ N82 封顶维持。struct 573/ndiff 917。
// ============================================================================
int CInventory::insert_event_items(const std::vector<std::pair<int, int> >& items,
