DimensionActivationData::DimensionActivationData()
    : m_field0(0), m_coinMap()
{
	// ORIG C1：+0x00 = 0，构造 map/rewardData 后清 coinMap（m_rewardData 默认初始化，无值初始化 memset）
	m_coinMap.clear();
