Arad_EventPeriodDataManager::~Arad_EventPeriodDataManager()
{
    clearScriptData();    // ORIG 0x0819554c：D1 写 vptr 后调 clearScriptData()
}