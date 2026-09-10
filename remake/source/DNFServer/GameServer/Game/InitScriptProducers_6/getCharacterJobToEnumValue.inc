int getCharacterJobToEnumValue(std::string& s);

// ----------------------------------------------------------------------------
// getGrowTypeIndex —— ORIG T 0x088be63d（0xc7b 字节，定义归本 TU）。
// ORIG 结构（/tmp/getGrowTypeIndex.md 汇编+Ghidra C 双段实证）：函数级静态
// __lookup = map<char const*, int, less_TCHAR>（__cxa_guard 惰性构造）+
// __init_lock(SpinLock) + __inited 双检；未建表时持锁以 idx=0 进入 switch，
// 自 case 0（与 case 0x8b5 合并）起逐 case **贯穿** insert(value_type(键,码))
// 共 25 条，表尾释放锁、置 __inited 并跳回 guard(088be650) 重新 find 派发；
// 建表后 find 命中按码组 return："none"→0；weaponmaster/nenmaster/ranger/
// elementalmaster/crusader/rogue/elementalbomber(0x8ba..0x8bf,0x8c1)→1；
// soulbringer/striker/launcher/summoner/infighter/necromancer/glacialmaster
// (0x8c7..0x8cc,0x8ce)→2；berserker/streetfighter/mechanic/battlemage/
// exorcist(0x8d4..0x8d8)→3；asura/grappler/spitfire/witch/avenger
// (0x8dd..0x8e1)→4；未命中/未知码 return 0xffff（local_24 入口初值）。
// 键比较为 less_TCHAR（大小写无关），与旧 strcmp 线性链语义不同。
// ----------------------------------------------------------------------------
