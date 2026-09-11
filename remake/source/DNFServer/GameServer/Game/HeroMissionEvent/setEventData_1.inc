// ============================================================================
// setEventData(std::string const&) —— ORIG T 0x08166d0e（setEventData_08166d0e.md
// 全文转写）：thunk——c_str() 后转调 char const* 版本。
// ============================================================================
void HeroMissionEvent::setEventData(CUser* user, const std::string& data)
{
    setEventData(user, data.c_str());
}