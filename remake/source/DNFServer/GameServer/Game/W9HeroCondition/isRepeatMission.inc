// ORIG isRepeatMission 0x08164fc2：movzbl *(this+0x10) = condition 字节 +0x08。
// [b28c-L26 档①] 定义返回类型随头改 bool（P159：mangled 不含返回类型，ABI 不变）。
bool BaseHeroMissionCondition::isRepeatMission()
{
    return m_condition.m_isRepeatMission;
}