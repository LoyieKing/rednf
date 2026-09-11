// OnReady ORIG T 0x08565018 报告 OnReady.md
bool CFairMatch::OnReady(CUser* /*user*/, bool ready)
{
    if (!ready && m_field04 == 4)
        return false;
    return true;
}