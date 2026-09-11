void StrikerScript::clear()
{
    // ORIG 0x08a9dbb4：标量复位 + 容器 clear。
    m_tagCostTime = 0;
    m_tagCharacterSetting = 0;
    m_strikerCombo = 0;
    m_strikerStartTime = 0;
    m_strikerEndTime = 0x1f4;                    // 500
    m_moduleImpossible.clear();
    m_dungeonImpossible.clear();
    m_field38 = 0;
    m_field3c = 0;
    m_fileList = 0;                              // +0x90
    m_loaded = 0;                                // +0xa0 byte
    m_field64 = 0;                               // +0x64
    m_field68 = 0;                               // +0x68
    m_commonCoolTime = 0;                        // +0xa4
    m_positionType = 0;                          // +0xac
    m_upgradeRevision = 0x63;                    // +0xb0 = 99
}