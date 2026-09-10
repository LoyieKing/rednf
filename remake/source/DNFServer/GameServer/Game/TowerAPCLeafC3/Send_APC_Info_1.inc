void TowerOfDespair_APC_Mgr::Send_APC_Info(TOD_Layer const& layer, CUser* user)
{
    // ORIG 形态：默认构造 it → end() 先求值 → find 赋值 → operator!= 判断；
    // 日志三参为内联 getter 表达式（右到左求值，寄存器临时跨 noop 构造存活）。
    std::map<TOD_Layer, SendingAPC_Info>::iterator it;
    std::map<TOD_Layer, SendingAPC_Info>::iterator endIt = m_APCInfoMap.end();
    it = m_APCInfoMap.find(layer);
    if (it != endIt)
    {
        it->second.Send_APC_Info(layer, user);
        cMyTraceNoop noop;
        noop("TOD:sending [%d] toUser, layer [%d] : clone : [%d]",
             user->getCurCharacNo(),
             layer.GetLayer(),
             it->second.get_user_charac_no());
    }
    else
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0xa1, 5);
        trace("TOD:invalid layer %d", (unsigned int)(layer.GetLayer() & 0xffff));
    }
