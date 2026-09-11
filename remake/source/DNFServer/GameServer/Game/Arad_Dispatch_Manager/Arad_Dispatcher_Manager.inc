// ============================================================================
// Arad_Dispatcher_Manager —— ORIG C1 @0x0818fdd4：两个 0x354 项指针数组清零。
// ============================================================================
Arad_Dispatcher_Manager::Arad_Dispatcher_Manager()
{
    for (int i = 0; i < 0x354; ++i)
        m_interDispatchers[i] = 0;
    for (int i = 0; i < 0x354; ++i)
        m_dbDispatchers[i] = 0;
}