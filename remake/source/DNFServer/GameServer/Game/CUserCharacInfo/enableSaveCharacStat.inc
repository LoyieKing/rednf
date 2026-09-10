void CUserCharacInfo::enableSaveCharacStat() { m_saveCharacStat = 1; }

// ============================================================================
// GetTenThousandPercentage（ORIG 0x8550fc9 W）
// 万分比计算：value * percent / 10000（编译器以 0x68db8bad 魔数除法实现，
// 反汇编确认 = imul 后魔数除法，语义即 value*percent/10000）。
// 声明见 CUserCharacInfo.h（SetCurCharacLuckPoint 使用）。
// ============================================================================
