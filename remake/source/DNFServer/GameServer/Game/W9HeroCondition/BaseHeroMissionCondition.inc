// ---------------------------------------------------------------------------
// BaseHeroMissionCondition
// ORIG ctor 0x08164f12：写 vptr；this+0x4 = missionNo；构造 this+0x8 处的
// HeroMissionCondition（默认构造）。
BaseHeroMissionCondition::BaseHeroMissionCondition(
    HeroMissionCondition::MissionNo::T no)
    : m_condition(), m_missionCode(no)
{
}