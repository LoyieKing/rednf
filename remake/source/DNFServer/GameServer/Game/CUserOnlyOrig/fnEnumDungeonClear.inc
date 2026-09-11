// ---------------------------------------------------------------------------
// CUser::SendDungeonClearList() T 0x086781b4 sz0xcd
// 报告：docs/class_func_reports/CUser/SendDungeonClearList.md
// put_header(0,5)→put_short(getDungeonClearSize)→enumDungeonClear(this,
// fnEnumDungeonClear,&packet)→finalize(true)→Send。回调自由函数 ORIG 名
// fnEnumDungeonClear（_Z18fnEnumDungeonClearscPc 0x08678166；CUser.h 现声明
// fnEnumDungeonClears 为 19 字符错拼，mangled 不匹配——已报 Main 修正）。
// ---------------------------------------------------------------------------
bool fnEnumDungeonClear(short dungeonIdx, char diff, char* userData);   // T 0x08678166
