// ORIG Load @0x0816451e：PRETTY 串 "virtual bool CreateDnfEventSciprtServer::Load()"
// （.rodata 0x8b7c9a0），行号 0xd0 / 计数 9 / true,false → cMyTrace C1(PKc,iibb)；
// 打点 "- Loading EventCreateDnf script - "；return loadScript()==0（b28c-32：
// m_script 已改为第一个基类，直调基类成员）。
bool CreateDnfEventSciprtServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0xd0, 9, true, false);
    trace("\t- Loading EventCreateDnf script - ");
    if (loadScript())
        return false;
    return true;
}