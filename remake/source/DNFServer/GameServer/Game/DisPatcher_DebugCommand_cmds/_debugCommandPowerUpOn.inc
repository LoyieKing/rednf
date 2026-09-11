// ===========================================================================
// 24. _debugCommandPowerUpOn —— 0x0858ffee
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandPowerUpOn(CUser* user,
                                                     MSG_DEBUG_COMMAND& /*msg*/)
{
    // P168a①：callee 返 bool ⇒ 一律裸形直产 `call; xor$1; testb; je`；
    // 具名 bool 局部反产槽物化两段（mov %al,-9 + movzbl 回读，b28c-lane12 实证）。
    if (!user->isGMUser())
        return false;

    user->SetPowerUp(true);                                     // +0x2f SetPowerUp(1)
    std::vector<CUser*> targets;                                // -0x14 局部 vector
    targets.push_back(user);                                    // +0x4c push_back(user)
    G_GameWorld()->SendPowerUpList((ENUM_POWER_UP_ACTIVATE_TYPE)1, targets);
    return true;
}