// ============================================================================
// WorldMapScript 静态图案表（ORIG nm 真实静态成员；权威声明见 WorldMapScript.h，
// 本 TU 提供定义）。
//
// 0x28 = STWorldmapPattern（string + int + coord[3][2] 聚合@+0x08 + int + int）。
// 0x28 = STWorldmapPattern（string + 9 int；coord[3][2] 落在 +0x08..+0x24）。
// aWorldMapPatternTable2ND_E（B 0x095014e0）：[20] 条目，每条目 0x1c =
// STWorldmapPattern2ND（{ int base@+0 ; int coord[3][2]@+4 }）。
// ============================================================================
// ---- 本 TU 下方定义的依赖（前置声明）----
bool loadWorldMapCoordPattern(void);   // _Z24loadWorldMapCoordPatternv 0x08ab0234
