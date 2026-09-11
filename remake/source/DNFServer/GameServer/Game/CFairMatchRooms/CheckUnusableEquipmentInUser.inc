// CheckUnusableEquipmentInUser ORIG T 0x08562cfe 报告同名 md —— 恒返 1（bool 返回：
// CheckJoinable +0x12/OnCreateRoom +0x11 均以 xor $1 取反，P22 实证）
bool CFairMatch::CheckUnusableEquipmentInUser(CUser* /*user*/)  // static（声明侧 CFairMatch.h）
{
    return true;
}