NeedMaterialDataManager::NeedMaterialDataManager()
    // [b28c-31] ORIG 0x081b5a4f：map 构造后先 `lea 0x18(%eax),%edx` 再物化实参
    // （P211 LHS 地址先载形）⇒ 显式列全成员试翻装载序。
    : m_map(), m_default(-1, 0)
{
