CBurningFatigueEvent::CBurningFatigueEvent()
{
    // ORIG：CEventBase ctor 置基类 vptr 与 stADDINFO/+0x08 flag；
    // m_params 指向全局 CDataManager 内 +0x4f4 的 burning-fatigue 参数 vector
    // （= CDataManager::m_serverParameter(+0x68) 内 ServerParameterScript+0x48c，
    //   与 importServerParameterScript 的 push_back 目标互证）。
    m_params = reinterpret_cast<std::vector<stBurningFatigueParam_t>*>(
        reinterpret_cast<char*>(G_CDataManager()) + 0x4f4);
}