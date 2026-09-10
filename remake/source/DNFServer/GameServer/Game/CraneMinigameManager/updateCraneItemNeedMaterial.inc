void CraneMinigameManager::updateCraneItemNeedMaterial(int count)
{
    unsigned int size = m_needMaterials.size();
    // b28c-L19 清绑（P0/N96 全域禁 register asm）：改 P0 表达式直传形。
    // ORIG +0x26..+0x2f：余数三段搬运后 1 字节存 +0x68；+0x49/+0x50：at() 结果
    // 指针与 m_material 双 callee-saved 保活（4.4 自然形可能落栈槽，残差记 N）。
    m_selectIdx = static_cast<unsigned char>(
        static_cast<unsigned int>(count) % size);
    NeedMaterialDataManager::getInstance()->setNeedMaterialInfo(
        m_material, m_needMaterials.at(m_selectIdx));
