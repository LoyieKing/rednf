void Arad_ServerStateManager::insertStateNoSync(
    ENUM_GLOBAL_EFFECT_ARAD code, const AradServerStateMessage& msg)
{
    AradServerStateMessage found;
    if (findState(code, found))
        return;

    m_stateMap[code] = msg;
    saveServerState(K_NOTIFY_EVENT_CATEGORY_SERVER_STAT, code, msg, true);
    sendEffectPacket(code, msg, 0);
    cMyTrace(__PRETTY_FUNCTION__, 0x88, 0)(
        "[SERVER_STAT] apply server stat. (category:%d, code:%d, end:%u)", 1,
        (int)code, msg.m_endSec);
