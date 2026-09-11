// ORIG 0x084330d4 sz 0x449：GetDBHandle(6,0)；localtime(&g_IPMonitorLastDBAccessTime)
// sprintf "%d-%d-%d %d:%d:%d" → seLect ip,type,m_id_cnt,unix_timestamp(start_time),
// unix_timestamp(end_time) from ip_monitor_punish where start_time > '%s'；
// exec 失败或 n_rows==0 返 0；trace(0x7ac4,0)；逐行读入 0x20 行缓存（0xc80），
// 满 100 行或末行 → Acquire(0x7ae9) → <<0x156 → <<-1 → GetInBuffer<SIG_IP_
// MONITOR_PUNISH> → +0=rowCount+1 → memcpy +4..0xc83 → +0xc84=0 → put(1)；返 1。
// [SweepW8LT b28d] DIFF：gap 10→6（our 271 / ORIG 277）。本波修两处：
// ① P227 trace 改全表达式临时链（ORIG ctor 前 `call get_n_rows; mov %eax,%ebx`
//    跨 ctor 保活；具名对象使 4.4 在 ctor 后重调 get_n_rows，多一次 DB 调用）；
// ② P183 删 items 指针缓存，全表达式 `((DB_IpMonitorPunishItem*)buf)[rowCount]`
//    重算（ORIG 每处 `lea buf; shl $5; add`）。p167 31/31 [0缺0多]（C1/C2 别名）。
// 余 -6：localtime 地址装载 +1、timeBuf 清零 5 vs 6 条、N82 槽差。ORIG 084330d4
bool DB_BlackIPMonitorPartLoad::dispatch(int userIdx, int slot, Stream* stream)
