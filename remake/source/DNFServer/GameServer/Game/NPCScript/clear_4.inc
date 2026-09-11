// ----------------------------------------------------------------------------
// NpcScript::clear —— ORIG 0x089fbf22 逐字段。
// ----------------------------------------------------------------------------
void NpcScript::clear()
{
    m_scriptFileIndex = 0;
    m_name = "";
    m_fieldName = "";
    m_smallFace = "";
    m_smallFaceId = 0;
    m_bigFace = "";
    m_bigFaceId = 0;
    m_popupFace = "";
    m_popupFaceId = 0;
    m_fieldAnimation = "";
    m_fieldWav = "";
    for (int i = 0; i < 2; ++i)
    {
        m_popupWav[i] = "";
    }
    m_lookAround = true;
    m_roleMap.clear();
    m_questSpeech = "";
    for (int i = 0; i < 12; ++i)
    {
        m_skills[i].clear();
    }
    m_directoryPath = "";
    m_unknown38 = -1;
    m_stringData.clear();
    m_intData.clear();
    m_balloonMessages.clear();
    m_balloonTicks.clear();
    m_dialogs.clear();
    m_presentResponces.clear();
    m_presentResponceExceptions.clear();
    m_presentResponceEvents.clear();
    m_favorableRelationShip.clear();
    m_firstFavorDialogs.clear();
    m_visibleOnDungeonClear = false;
    m_webPages.clear();
    m_clickMarks.clear();
    // b28c-L8 代落 C28L35（Main R1 裁决 NPCScript.cpp 归本 lane）：ORIG 0x089fbf22
    // 的两个 +0xf8/+0xfc assign 实际排在 +0x698（m_clickMarks）之后、+0x6b8（m_hide）
    // 之前（Ghidra C 段逐条同序）；旧形挂在 +0x110 之后 ⇒ 10 条指令整体错位（ndiff 20）。
    m_moveTownLimitStr = "";
    m_moveTownConfirmStr = "";
    m_hide = false;
    // ORIG clear 未显式清 +0x6a4..+0x6e4：randomMessageBalloonInfo / NpcMovieData
    // 由其自身 clear 保证空态，这里不重复清（与 ORIG 字节一致）。
}