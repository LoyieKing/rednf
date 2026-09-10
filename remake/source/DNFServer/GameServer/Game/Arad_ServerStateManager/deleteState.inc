void Arad_ServerStateManager::deleteState(ENUM_GLOBAL_EFFECT_ARAD code)
{
    Guard<Mutex> guard(&m_lock);
    AradServerStateMessage cleared;
    if (findState(code, cleared))
    {
        // ORIG：movw $0,(local)——仅将起始 word 清零；局部消息其余字段为 ctor 全零。
        cleared.m_active = 0;

        sendEffectPacket(code, cleared, 0);
        saveServerState(K_NOTIFY_EVENT_CATEGORY_SERVER_STAT, code, cleared, false);
        cMyTrace(__PRETTY_FUNCTION__, 0x38, 0)(
            "[SERVER_STAT] delete server stat. (category:%d, code:%d, end:%u)", 1,
            (int)code, cleared.m_endSec);
        m_stateMap.erase(code);
    }
