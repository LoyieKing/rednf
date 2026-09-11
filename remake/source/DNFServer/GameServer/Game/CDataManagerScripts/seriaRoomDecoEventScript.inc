// ---- seriaRoomDecoEventScript（ORIG C1 0x8372ef4：构造 vector@0、+0xc=0、
//      map@+0x10、vector@+0x28、+0x34=0、+0x38=0 后三容器各自 clear）----
seriaRoomDecoEventScript::seriaRoomDecoEventScript()
    : m_eventItemIdx(), m_maxEventItemCount(0), m_matchEventMap(),
      m_soundList(), m_fatigueCondition(0), m_rewardMailExpireDate(0)
{
    m_matchEventMap.clear();   // ORIG：先 clear +0x10 map
    m_eventItemIdx.clear();    // 再 clear +0x00 vector
    m_soundList.clear();       // 最后 clear +0x28 vector
}