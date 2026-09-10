int getCharacterJobToEnumValue(std::string& job);

// ============================================================================
// A. 事件脚本 ctor（ctor 内显式语句顺序严格对照各自反汇编）
// ============================================================================

// 报告 LevelupSupportSciprt.md @0x08ab5124：仅构造 +0x00 的
// map<int, vector<ItemInfo>*>。
