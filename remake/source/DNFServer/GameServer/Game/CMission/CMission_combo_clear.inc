CMission_combo_clear::CMission_combo_clear(
    const MissionScript& script,
    const std::map<combo_script_key, combo_script_data>& comboMap)
    : CMission(script), m_comboMap(comboMap)
{
}