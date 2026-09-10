void createLotteryInfo(LotteryInfo* lottery, std::vector<int>* counts);

// ---------------------------------------------------------------------------
// Area 依赖结构（字段布局按 ORIG 反汇编偏移）
// ---------------------------------------------------------------------------
// MapArea / STAttackedMonster：全局作用域定义见 CVillageMonsterMgr.h
// （InsertVillageMonster 的 mangled 参数需非限定类型名）。
// TownAreaScript：共享定义见 TownAreaScript.h（0x30）。

// LotteryInfo：0x20 字节（Area 内嵌于 +0x24；ORIG C1 083706d4）
// LotteryInfo（0x20）：权威定义见 CStackableItem.h（经 CUser.h 链引入；
// Area 内嵌于 +0x24；ORIG C1 083706d4）。
// Mutex（0x18）：权威定义见 GameTypes.h（Area 内嵌于 +0x0c；内联
// pthread_mutex_init/destroy，ORIG 亦为内联弱符号族）。

// set_area 输入结构
