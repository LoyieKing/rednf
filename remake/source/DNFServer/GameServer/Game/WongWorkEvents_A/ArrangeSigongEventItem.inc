// ----------------------------------------------------------------------------
// EventSigong::ArrangeSigongEventItem —— ORIG T 0x083fa855 (size 0x3a9)
// 双栏结构：row[+8]=itemN_no、row[+6]=serverN、row[+2]=checkN；
// itemN 数量为常量：item1=1、item2=200（ORIG movl $0x1,-0x5c/$0xc8,-0x58，
// SQL 未取 cnt 列）。event_code=3。
// [b28d-solo-WongWorkEvents_A] AE：P227/P139 错误路径全表达式临时链 +
// __PRETTY_FUNCTION__；P140 删多余守卫；P225 尾测 while/break；P311 反向
// 「每栏独立 2 元素数组 + 独立 id」复现 disp 折叠（局部 struct 视图无效）。
// ----------------------------------------------------------------------------
void EventSigong::ArrangeSigongEventItem(memberIdentificationNumber_t mid, unsigned int serverId)
