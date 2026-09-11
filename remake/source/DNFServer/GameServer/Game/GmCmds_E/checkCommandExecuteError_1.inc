// ORIG checkCommandExecuteError() 0x084b02be：count ∉ [0,100000] 报错。
const char* CRandomObjectTest::checkCommandExecuteError()
{
    if (m_count > 0x186a0 || m_count < 0)
        return "0 ~ 100000\xb9\xf8 \xc5\xeb\xb0\xe8\xb1\xee\xc1\xf6\xb8\xb8 \xc1\xf6\xbf\xf8\xb5\xcb\xb4\xcf\xb4\xd9";
    return 0;
}