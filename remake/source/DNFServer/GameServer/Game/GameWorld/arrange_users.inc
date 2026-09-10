void GameWorld::arrange_users()
{
    // [b28d-solo-GameWorld] P283 已试（goto 尾条件复现 ORIG `jmp COND` 形）：
    // NDIFF 8→9 反劣，槽集合仍不可对齐（ORIG it=-0x2c/end=-0x28/ppEi=-0x14；
    // 4.4 聚合对象 trace 恒占最深 -0x2c、迭代器落 -0x1c/-0x18/-0x14）⇒ 回 for 形。
    // N82附 封顶：4.4 聚合对象优先占低槽 vs 4.1 按声明序，槽组整体平移。
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user != NULL)
        {
            // ORIG +0x03f~+0x05a：NumberToString 先于 cMyTrace ctor，结果跨 ctor 缓存 %ebx（P87）
            // b28d-solo：P227/P221 全表达式临时链（实参在 ctor 前求值、%ebx 跨 ctor 保活，
            // 且 trace 不占具名槽）试翻 ORIG it=-0x2c/trace=-0x24 的槽方向。
            cMyTrace(__PRETTY_FUNCTION__, 0x10a6, 0)("GameWorld::arrange_users ACCID: %s",
                NumberToString(user->get_acc_id(), 0));
            user->DisConnSig((DISCONN_SIG)7, 1, 0);
        }
    }

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        int index = 0;
        int gcNo = GlobalData::s_statistic_proxy_mgr->GetStartIndex();
        while (index < GlobalData::s_statistic_proxy_mgr->GetEndIndex())
        {
            // [b28d-solo-GameWorld] P0：ORIG env 无槽往返（call G_CEnvironment 结果直压实参）
            // ⇒ 内联表达式，勿具名局部。
            DeleteChannel(G_CEnvironment()->get_gc_no_hardcode((ENUM_SERVER_GROUP)gcNo),
                          (ENUM_SERVER_GROUP)gcNo);
            gcNo = GlobalData::s_statistic_proxy_mgr->GetNextIndex(index);
        }
    }
    else
    {
        // ORIG +0x191~+0x196：mov 0x198(%eax) 字段直读（非 get_gc_no 调用）
        DeleteChannel(G_CEnvironment()->m_serverEnvir.m_gcNo, E_SERVER_GROUP_NONE);
    }
    G_CEnvironment()->set_suspend_state(true);
