// ---------------------------------------------------------------------------
// ORIG _ZN16HeroMissionValue14setStringTokenESs T 0x08164d68 sz 0x1aa：
//   clear 后按空格分隔（char_separator(" ", NULL, drop)）逐 token atoi 写入：
//   tok0→m_mission(int)、tok1→+0x04、tok2→+0x06、tok3→+0x0a、tok4→+0x08。
//   注意第 4/5 个 token 写入偏移与字段声明序相反（ORIG 如此，忠实保留）。
// ---------------------------------------------------------------------------
void HeroMissionValue::setStringToken(std::string token)
