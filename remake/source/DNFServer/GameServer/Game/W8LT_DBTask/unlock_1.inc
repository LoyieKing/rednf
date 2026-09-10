void StatisticsShusiaService::unlock() { m_mutex.unlock(); }
// StatisticsShusiaService::lock/unlock —— ORIG W 0x0844d4fc / 0x0844d510（Mutex 转发，
// StatisticsToObserveCrash::lock/unlock —— ORIG W 0x0844d4d0 / 0x0844d4e6（Mutex 转发，
// 反汇编 = call Mutex::lock/unlock；声明见 GameWorld.h，Main 批准共享头两声明；
// DB_CrashDownReport::dispatch 0x0841abc6 遍历前 lock / 遍历后 unlock）。
