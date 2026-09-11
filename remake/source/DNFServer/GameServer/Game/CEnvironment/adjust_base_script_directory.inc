void CEnvironment::adjust_base_script_directory()
{
    if (m_serverEnvir.m_baseScriptDirectory.empty())
        m_serverEnvir.m_baseScriptDirectory.assign("./Script/");
}