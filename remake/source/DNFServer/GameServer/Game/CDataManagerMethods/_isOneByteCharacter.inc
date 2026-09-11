// ===================== _isOneByteCharacter =====================
// ORIG 0x08362c0e（_ZN12CDataManager19_isOneByteCharacterEh）：
// sub $0x4,%esp 后直接 mov $0x1,%eax 返回 —— 恒真（ORIG 为单字节恒定实现，
// 双字节分支由 hasSpecialCharacter 的步进 2 路径承担）。
bool CDataManager::_isOneByteCharacter(unsigned char /*c*/)
{
    return true;
}