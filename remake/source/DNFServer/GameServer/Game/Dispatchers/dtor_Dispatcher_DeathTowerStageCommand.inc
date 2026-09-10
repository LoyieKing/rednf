Dispatcher_DeathTowerStageCommand::~Dispatcher_DeathTowerStageCommand() {}
// §8.3 TU 视图：ORIG 08208cb2 `add $0xb58,%eax` → stMapMonsterKillChecker_t::init
// （dispatch_sig.md:150/262-264）。CDeathTower.h（共享）未具名该内嵌成员——其
// m_timerKeys[10]@+0xb3c 占 0xb3c..0xb64 与本区重叠，建模疑似缺陷已 hub 报 Main 核。
// 本 TU 以视图把 +0xb58 表达为具名成员，避免越界改共享头。
