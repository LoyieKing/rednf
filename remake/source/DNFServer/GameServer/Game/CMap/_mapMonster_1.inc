// _mapMonster 拷贝构造 —— ORIG 0x0813d708：逐成员拷贝 +0x00..+0x30
// （不含 +0x31 pad），再拷贝构造 +0x34 vector。
// [b28d-24] DIFF 残7: ORIG 连续成员 store 用 %ecx 缓存 this（P29 初始化列表），4.4 逐条重载 0x8(%ebp)。ORIG 0x0813d708
_mapMonster::_mapMonster(const _mapMonster& other)
    : m_mobId(other.m_mobId),
      m_flag04(other.m_flag04),
      m_field08(other.m_field08),
      m_spawnType(other.m_spawnType),
      m_pair10(other.m_pair10),
      m_roleType(other.m_roleType),
      m_champion(other.m_champion),
      m_posX(other.m_posX),
      m_level(other.m_level),
      m_hpPct(other.m_hpPct),
      m_orderIdx(other.m_orderIdx),
      m_flag30(other.m_flag30),
      m_mobList(other.m_mobList)
{
}