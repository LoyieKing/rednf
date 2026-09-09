#include "CNNpcNameVector.h"
#include "CNRepeatQuestList.h"

// [b28d-Main·检查点18] 原自造 `OrigNpcVec_Touch()` 已删（ORIG 无该符号）。
// ORIG 本 TU 的真实内容是**两个全局对象**（同址 BSS 实证）：
//   g_npcNameVector   B 0x094f7be0 size 0xc
//   g_repeatQuestList B 0x094f7bec size 0xc（紧邻）
// 它们的构造/析构自然发射 CNNpcNameVector/CNRepeatQuestList 的 C1/C2/D1/D2/clear，
// 并由本 TU 的静态初始化函数发射 `_GLOBAL__I_g_npcNameVector`（0x089c2c25，按 TU 首符号命名）。
CNNpcNameVector g_npcNameVector;
CNRepeatQuestList g_repeatQuestList;
