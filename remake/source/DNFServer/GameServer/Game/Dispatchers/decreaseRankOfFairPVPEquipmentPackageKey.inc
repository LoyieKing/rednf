// ORIG T 0x088945c6：包键槽高 16 位（段位，makeFairPVP 第二参）减 1 写回后返回整槽；
// 低 16 位（职业）保留；无下溢保护（wrap）。
int decreaseRankOfFairPVPEquipmentPackageKey(int key)
