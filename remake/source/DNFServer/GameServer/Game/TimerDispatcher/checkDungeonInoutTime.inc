// [b28d-solo-TimerDispatcher] P157：ORIG 每个字段都重新 `it->ptEv()`（无
// `it->second` 引用缓存），旧码 `const STDungeonInoutTime& w = it->second;`
// 引入指针槽 -0xc 且只调一次 ptEv ⇒ 改回逐字段 `it->second.m_xxx`。
// [b28d-solo-TimerDispatcher2] P167 差集（MISSING G_CDataManager/ne/ptEv、EXTRA eq）
// 判明原码形：①时间读取（getCurSec/localtime_r/hour/min）在 find **之前**；
// ②find 与 end 各自重发 `G_CDataManager()`+`lea 0x130`（无 map 引用局部缓存，P157/P183）；
// ③正极性 `if (it != end) { if (cond) return true; } return false;`（P220，ORIG ne）。
bool Timer_DungeonInoutOpenTime::checkDungeonInoutTime(int dungeonIdx)
