CQuickPartySystemManager::~CQuickPartySystemManager() {}

// init_quick_party_pool @0x08269c0c：
// 该 dungeon 已登记于快速派对奖励副本表 → 直接返回；
// 非绝望之塔副本时，按 4 个难度档位向池 map 插入空数据
// （血/任务 NPC 副本仅插入档位 0）。
// [b28d-L1] 修法（P167/P96）：ORIG 0x08269c0c 只调 `__gnu_cxx::ne<const int*>`（两侧 const_iterator）；非 const 引用使 4.4 多发 begin/end+转换构造+混合 ne(PKi,Pi)（EXTRA×4/MISSING×1）。改 const 引用后迭代器差集清零；残余帧差 0x90 vs 0xa0（N82/P232）。
// [b28d-solo-QuickParty] verdict=AE（163=163，TU routed 口径；P167 CLEAR）：修法=
// P214① 正极性早返 `if (dungeon->isTowerOfDespairDungeon()) return;`（复现 ORIG
// 0x08269cc6 `test %al,%al; jne <nop 岛>`，消 `xor $0x1` 反形）+ 三条件正向 if 包裹
// （ORIG 0x08269d3a or 链物化后 `test %al,%al; je <插入块>` 直落）+ blood 条件
// `(signed char)` 化复现 `test %al,%al; jg`。旧「N96/P292 xor 不可翻」判词作废。
