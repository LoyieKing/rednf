void HeroMissionValue::getStringToken(std::string& out)
{
    std::stringstream ss;
    if (out.length() != 0)
    {
        ss << ',';
    }
    // [b28d-solo-HeroMissionEvent] ORIG 0x08164c47 输出 m_mission 走
    // `_ZNSolsEi`（int 重载）⇒ 用枚举视图（枚举提升 int），unsigned 视图会选
    // `_ZNSolsEj` 造成符号级错配。
    ss << m_missionType << ' ' << m_field4 << ' ';
    ss << m_field6 << ' ' << m_fieldA << ' ' << m_field8;
    out += ss.str();
