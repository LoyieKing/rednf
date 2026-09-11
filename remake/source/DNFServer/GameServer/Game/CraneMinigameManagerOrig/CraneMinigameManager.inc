CraneMinigameManager::CraneMinigameManager()
    : m_selectIdx(0)
{
    // ORIG 0x080ebc32：仅调 CraneMinigameScriptC1（构造 m_items/m_sendMail/
    // m_needMaterials）+ m_selectIdx=0；m_material(+0x10) 不初始化。
}