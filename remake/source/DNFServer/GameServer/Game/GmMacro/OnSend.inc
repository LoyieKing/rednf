void CMacro::OnSend(int type, int sub)                  // 0x084a88aa
{
    switch (m_state)
    {
    case 1:
        Recording(type, sub, 0, 0);
        break;
    case 2:
    {
        stMacroData* cur = GetCurCmd();
        if (static_cast<int>(cur->m_cmd) == type &&
            static_cast<int>(cur->m_sub) == sub)
        {
            NextCmd();
        }
        break;
    }
    }
}