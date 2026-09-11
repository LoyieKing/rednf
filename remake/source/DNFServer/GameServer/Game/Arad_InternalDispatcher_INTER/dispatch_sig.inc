// ============================================================================
// Arad_INTER_kLoadServerState —— ORIG 0x081992fe
// ============================================================================
int Arad_INTER_kLoadServerState::dispatch_sig(CUser* user, char* body, int len)
{
    // param_3 即 (arad::SigServerState*) 包指针复用位。
    // 【b26 NL10】局部即目标指针形（cast 收敛到声明处）：ORIG 0x081992fe
    // insertState 调用为先载 receiver(全局)→eax、后载槽→edx；cast 延到调用点会
    // 使 4.4 反转求值序（探针 p_s2e probe4 实证）。
    arad::SigServerState* packet = (arad::SigServerState*)body;
    if (packet == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x533, "SigServerState is null.");
        return 0x534;
    }
    GlobalData::s_serverStateManager_->insertState(packet);
    return 0;
}