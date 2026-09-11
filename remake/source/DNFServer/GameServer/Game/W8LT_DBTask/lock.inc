// StatisticsPvP::lock/unlock —— ORIG W 0x0844d524 / 0x0844d538（Mutex 转发，
// 反汇编 = call Mutex::lock/unlock；声明见 GameWorld.h，Main 批准共享头两声明）。
void StatisticsPvP::lock()   { m_mutex.lock(); }
