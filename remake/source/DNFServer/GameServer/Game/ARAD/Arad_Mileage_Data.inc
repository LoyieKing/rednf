// ============================================================================
// Arad_Mileage_Data（+0x70）—— ORIG ctor 0x8188cc0 / dtor 0x8188cea / parsing 0x81a3416 /
// find 0x81a3668 / default_value 0x81a36f0。解析 "default"（percent）与 "item"（ipg_no/percent）。
// ============================================================================
// [b22] ORIG ctor 0x08188cc0 不写 +4（m_defaultPercent）：仅基类 ctor+vptr+vector ctor，
// 16→14 条。默认值清零发生在 parsing()（`m_defaultPercent = 0` 行），ctor 侧删初始化。
Arad_Mileage_Data::Arad_Mileage_Data() {}
