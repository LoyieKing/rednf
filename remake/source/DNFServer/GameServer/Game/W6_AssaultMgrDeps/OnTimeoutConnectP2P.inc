// ===========================================================================
// 四个超时入口——模式均为取场次转派 + catch 吞 + 恒返 1。
// ORIG 地址随注释；place 具名局部（P0 槽往返：mov -0x10）后转派。
// ===========================================================================
int CAssaultMgr::OnTimeoutConnectP2P(unsigned int id, int authenValue)
{
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(id, authenValue);
        place->TimeoutConnectP2P();
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;                                            // ORIG T 0x082efa52
}