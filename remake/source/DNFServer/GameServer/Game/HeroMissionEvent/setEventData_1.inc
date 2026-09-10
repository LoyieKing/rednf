void HeroMissionEvent::setEventData(CUser* user, const std::string& data)
{
    setEventData(user, data.c_str());
