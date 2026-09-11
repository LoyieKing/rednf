// ---- CharacterCreateCreatureGiftScript（ORIG C1 0x83730ee：构造 map@0/
//      +0x18=0/map@+0x1c/map@+0x34 后三容器各自 clear）----
CharacterCreateCreatureGiftScript::CharacterCreateCreatureGiftScript()
    : m_eventJobMap(), m_changeAbleCreatureLev(0), m_clientChangeMap(),
      m_infinityChangeMap()
{
    m_eventJobMap.clear();
    m_clientChangeMap.clear();
    m_infinityChangeMap.clear();
}