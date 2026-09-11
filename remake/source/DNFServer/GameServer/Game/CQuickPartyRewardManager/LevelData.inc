// ---- LevelData / LevelDataHell ctor（ORIG W 0x08a6ef72 / 0x08a6ef8a：
// LevelData：m_field0=0、m_value4=-1；LevelDataHell：两 int 全置 -1；
// dtor 空）。此处强定义供 EventRewardQuickPartyB2 等 TU 以外部引用解析
// （W 与 T 链接可共存，语义一致）。----
LevelData::LevelData() : m_field0(0), m_value4(-1) {}
