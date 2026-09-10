int get_rand_int(int range);  // ORIG T 0x86b1b87

// ============================================================================
// 本地支撑类型（名称/布局与 ORIG 一致；成员在各自批次 TU 定义）
// ============================================================================

// 掉落生成域类型（WongWork::stGenerateRefData_t/stGenerateResult_t/CItemGenerator）
// 对战地图数据 STRivalMapData 用 CMap.h 权威定义（本文件曾用局部重复定义已删，§4.4）。
// 权威声明见 CItemGenerator.h（唯一声明点，经 CDataManager.h 引入）。
// ORIG vtable 槽位（_ZTVN8WongWork14CItemGeneratorE @0x08c9a788）：
//   +0x08 generateRandomItem、+0x0c generateSpecificItem。
