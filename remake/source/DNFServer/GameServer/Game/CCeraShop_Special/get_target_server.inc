// ---- get_target_server —— ORIG 0x0827d0f5 ----
// g_SPremiumInfo[type].m_useTargetServer(+0x5c)==1 时返回
// G_CEnvironment()->m_serverEnvir.m_serverGroup（CEnvironment+0x378），否则 0。
int premium_helper_function::get_target_server(int type)
{
    if (g_SPremiumInfo[type].m_useTargetServer == 1)
        return G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x198+0x1e0
    return 0;
}

