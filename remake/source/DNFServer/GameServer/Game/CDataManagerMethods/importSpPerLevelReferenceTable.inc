int importSpPerLevelReferenceTable(const char* path, stSpPerLevelTable& table);

namespace
{

// initRandomOptionItem 访问视图（与 CItemList.cpp 的 CDataManagerExt 同一
// 证据链：insert_item 写 +0x4e70；initRandomOptionItem 读 +0x4e70/+0x4e88）。
