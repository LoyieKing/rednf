// ORIG WpBonusPointData/ExpBonusPointData ctor/clear 为独立符号（0x08513a6e/0x08513a82 等），
// 由 CEquipItem::C1/STEquipmentScript::C1 调用；故在此 out-of-line 定义以发射符号。
WpBonusPointData::WpBonusPointData() { clear(); }
