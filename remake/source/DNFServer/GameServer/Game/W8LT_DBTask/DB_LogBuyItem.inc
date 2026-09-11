// ORIG 0x0812859a：time/localtime 取当前 tm；tm_hour 早于
// G_CEnvironment+0x37c（m_fatigueTime）则回退 86400 秒重取；置
// tm_hour=limit、min/sec=0 后 mktime（结果不再使用）。sprintf 日串
// "%04d-%02d-%02d"（tm_year+0x76c / tm_mon+1 / tm_mday）。*stream >>
// m_id >> charac_no >> item_id >> item_count >> item_type(int)。GetDBHandle
// (4,0)。先 update log_acquire_item 累计 item_count；exec 失败或无影响行
// 则 insert 新行（m_id 经 NumberToString）；insert exec 失败 cMyTrace
// (0x566,5) 后返 0；全成返 1。
// ORIG C1 0x844f66e（sz 0x4：base ctor+vptr only）——权威头仅声明，此处外联定义。
DB_LogBuyItem::DB_LogBuyItem() {}
