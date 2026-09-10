CEnchanter::~CEnchanter() {}

// ============================================================================
// CExpertJob 静态工具（ORIG，见 expert_job__CExpertJob/*.md）
// ============================================================================

// GetSuccessRate @ 0x849b2e8：查 m_recipeLevelMap（itemIndex→所需等级），
// 所需等级 - 当前等级 = 等级差 → GetCompoundRate(差) 取率表项；variation =
// ROUND(CInventory::GetExpertJobCompoundRateVariation(type, level))。
// [b28d-solo-CExpertJob] AE（110=110）：P0 去 requiredLevel/level 具名局部（ORIG
//   0x0849b3a6/0x0849b416 均 %ebx 免槽跨 call 保活）+ P198① 函数头预清 +
//   P309 levelDiff 先于 script/variation 声明（复现 -0x1c/-0x18 槽序）；p167 CLEAR。
//   ORIG 0x0849b2e8
