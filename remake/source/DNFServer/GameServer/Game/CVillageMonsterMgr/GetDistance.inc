int GetDistance(pvp_assault::Point2d a, pvp_assault::Point2d b);

// ============================================================================
// pvp_assault::IsIncludeVirtualMoveAreas —— ORIG T 0x82e5a0b（村战 OnRun 依赖）。
//   遍历虚拟移动区：点落在某区 margin 内 → 返回 1（outX/outY 维持原坐标）；
//   否则取各区域近边缘中距原点的最小值（margin!=0 写绝对坐标，margin==0 写
//   增量），全程未命中返回 0。
// ============================================================================
