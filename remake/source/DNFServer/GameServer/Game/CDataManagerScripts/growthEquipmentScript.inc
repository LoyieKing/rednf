growthEquipmentScript::growthEquipmentScript()
    : m_rewardMap(), m_giftBox(-1), m_materialItem(-1),
      m_materialMax(0), m_eventBubbleMax(0), m_eventSandMax(0)
{
    m_rewardMap.clear();  // ORIG C1 构造 map 后 clear
