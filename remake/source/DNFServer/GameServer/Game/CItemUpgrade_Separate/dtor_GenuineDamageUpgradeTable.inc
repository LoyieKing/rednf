GenuineDamageUpgradeTable::~GenuineDamageUpgradeTable() {}    // ORIG W 0x0811eeda：三 map 析构

// ORIG T 0x08912a54。浮点读取：statWeightsByRarity[rarity]（this+(rarity+8)*4）
// 与 weaponWeights[a][b]（this+((6a+b)+12)*4+8）；分母 *(int*)this；useAlt 选择
// info.m_b / info.m_a；尾 +0.5f（DAT_08e122e8）后截断。
// [b28d-solo-CItemUpgrade_Separate] 重写：ORIG 为 result 具名 float（P232 扩展二段
// 零初始化）、altBias 运行时 flds/fmulp/fstps（pointFive/two 必须非 const 静态量，
// 否则 4.4 折 fld1）、if/else 两臂各自完整计算（非三目共享 multiplier）、
// 乘法书写序 m_b*(count/divisor+altBias)*weapon*stat 使 4.4 浮点栈序与 ORIG 逐条一致。
