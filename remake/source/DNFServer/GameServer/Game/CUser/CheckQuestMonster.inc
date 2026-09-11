// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0866cb04 | _ZN5CUser17CheckQuestMonsterEii21ENUM_QUEST_ENEMY_TYPE
// [b28d-GrpD 判词·结构化 N] ORIG 0x0866cb04：残差为**我方具名临时整体槽化**导致的
//   块序重排（帧 0x1c0 vs ORIG 0x1d0；-0x78/-0x74 累加器槽 vs ORIG 寄存器保活）。
//   逐块核对：字段偏移（+0x8(%eax,%edx,4) 索引、imul $0x94、0x700 跳表）、
//   极性（jle/jg、je/jne 均落点体互换 = 等价，判据⑦）、算术步长全部同址同值
//   ⇒ 无语义缺口；后续可按 P183 逐点删具名量收敛。
void CUser::CheckQuestMonster(int a, int b, ENUM_QUEST_ENEMY_TYPE type)
