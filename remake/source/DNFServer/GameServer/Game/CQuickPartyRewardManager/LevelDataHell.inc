LevelDataHell::LevelDataHell() : m_field0(-1), m_value4(-1) {}

// ============================ QuickPartyData ============================

// ORIG 0x826dad4：两 vector<int> C1、BasicReward C1、map C1、三容器 clear、
// +0x1a0[2][2] 全 -1。容器构造由成员默认构造承担；clear 为 ORIG 显式调用序列。
