stVendingMachineInfo_t::~stVendingMachineInfo_t() {}

// ORIG clear 0x0891b3fe：+0=0、+4=-1、+8=0、+0xc=0、vecGive.clear()、
// +0x1c=0、vecBonus.clear()。注意：不清 m_lottery（+0x2c），忠实保留。
