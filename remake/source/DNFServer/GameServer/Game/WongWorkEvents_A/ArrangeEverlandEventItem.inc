// ----------------------------------------------------------------------------
// EventEverland::ArrangeEverlandEventItem —— ORIG T 0x083fad60 (size 0x32e)
// SQL 无 cnt 列；item_no 恒为常量 1（ORIG movl $0x1,(%eax)）。event_code=3。
// [b28d-solo-WongWorkEvents_A] AE：P227/P139 错误路径全表达式临时链 +
// __PRETTY_FUNCTION__；P140 删 outCount<MAX 多余守卫；P311 反向 struct 行
// 数组复现两步寻址；P225 尾测 while/break。
// ----------------------------------------------------------------------------
void EventEverland::ArrangeEverlandEventItem(memberIdentificationNumber_t mid, unsigned int serverId)
