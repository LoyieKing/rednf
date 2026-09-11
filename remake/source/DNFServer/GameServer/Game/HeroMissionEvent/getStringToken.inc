// ============================================================================
// HeroMissionValue::getStringToken —— ORIG T 0x08164bca（getStringToken.md
// 全文转写；saveEventData 依赖，定义归本 TU）。
// 若 out 非空先补 ','，再按 "mission f4 f6 fA f8" 空格分隔追加（字段序与
// setStringToken 的解析偏移一一对应：tok0→m_mission、tok1→+0x04、tok2→+0x06、
// tok3→+0x0a、tok4→+0x08）。
// [b28d-solo-HeroMissionEvent] 判词 AE：fresh clean ndiff=0（orig=111 our=111，
// gap=0）；清单 NEAR1 系旧读数。修法=枚举视图选 `_ZNSolsEi` + 链拆两语句。
// ============================================================================
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
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
}