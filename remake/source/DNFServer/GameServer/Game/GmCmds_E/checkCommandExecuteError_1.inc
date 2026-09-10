const char* CRandomObjectTest::checkCommandExecuteError()
{
    if (m_count > 0x186a0 || m_count < 0)
        return "0 ~ 100000번 통계까지만 지원됩니다";
    return 0;
