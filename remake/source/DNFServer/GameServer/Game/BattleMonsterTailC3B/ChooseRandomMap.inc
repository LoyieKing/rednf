// ---------------------------------------------------------------------------
// CDungeon::ChooseRandomMap — ORIG 0x0834bf56（const）
// 按 map_type 选容器：1 → boss_maplist_[dir](+0x324)、2 → dummy_maplist_[dir]
// （+0x4a4）、其余 → maplist_[dir](+0x1a4)；entrance==0 全表随机游走取
// second->get_index()，否则 equal_range(entrance)/count(entrance) 后同型游走。
// 各失败路径按 ORIG 行号/文案记日志（map_type 实参始终传形参本尊）。
// 容器映射与 InsertMap（BattleDungeonMapA2）的 get_map_type 分支一致。
// ---------------------------------------------------------------------------
int CDungeon::ChooseRandomMap(int mapType, int direction, int entrance) const
