bool CEventScriptMng::initScriptlist(InGameEventInfoList& list)
{
    // [b28d-solo-CEventScriptMng] initScriptlist：AE。
    // ORIG 0x0810ba7a：每个字段访问重发 `it->second.xxx` / `m_entities[...]`
    // （P157/P183 禁缓存引用/指针）。
    // ORIG 结构：end() 循环头缓存一次（P13，it 槽 -0x18 / endIt 槽 -0x1c）；
    // m_isEventing 正极性包裹（P214①：ORIG `setne;test;je <自增>`，负写
    // `==0 + continue` 反产 sete/jne）；name 为 getEventScript 实参内的**匿名
    // string 临时**（P0：ORIG 在 null 检查前即 ~string/~allocator，具名局部
    // 会延后到语句尾并多一组 EH 记账）。三处修毕 compare AE；
    // p167 CLEAR（35/35）。
    for (std::map<int, InGameEventInfo>::iterator it = list.m_info.begin(),
             endIt = list.m_info.end();
         it != endIt; ++it)
    {
        if (it->second.m_isEventing != 0)
        {
            InGameEventScript* script =
                InGameEventScriptLoader::getEventScript(
                    it->second.m_eventId, toTString(it->second.m_name));
            if (script == 0)
                return false;

            m_entities[it->second.m_eventId] =
                create_event_entity(it->second.m_eventId, false);
            m_entities[it->second.m_eventId]->set_event_flag(
                it->second.m_isEventing != 0);
            m_entities[it->second.m_eventId]->set_event_data(it->first, script);
            m_entities[it->second.m_eventId]->set_event_start_time(
                it->second.m_startTime);
            m_entities[it->second.m_eventId]->set_event_end_time(
                it->second.m_endTime);
        }
    }

    init_evnet_entity();
    return true;
}