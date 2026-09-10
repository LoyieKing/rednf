WarAreaCounter::~WarAreaCounter() {}

// ---- stEquipmentGradeSample（ORIG C1 0x837c810 / D1 0x837c884：
//      5 个 vector<unsigned int> 依 +0x00→+0x30 构造、逆序析构）----
// ORIG C1 0x837c810：体为空，5 个 vector 由编译器隐式数组构造（含 EH 逆序析构）。
