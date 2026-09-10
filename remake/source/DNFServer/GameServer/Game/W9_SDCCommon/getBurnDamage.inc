int SDCActiveStatusSyncCommon::getBurnDamage(bool& flag,
                                             STSDCActiveStatusSyncDataSend* d,
                                             void*, int t)
{
    float dmg =
        GetUniformVelocityFloatSDC(0.0f, static_cast<float>(d->m_baseDamage),
                                   t, d->m_durationTick);
    // [N82 封顶] fistpl 已复用 int 中转槽（与 ORIG 同构）；残差为整组槽平移 8
    // （dmg -0x4 vs ORIG -0xc、中转 -0x18 vs -0x20、cw -0x12/-0x14 vs -0x1a/-0x1c）
    // + 帧 0x28 vs 0x38；P190 double pad 实测 dmg 落 outgoing 区反劣化，封顶。
    if (0 < d->m_baseDamage)
        flag = false;
    if (d->m_isPvp != 0)
        dmg *= getUserToUserDamageRate(d->m_attackerType == 10);
    return static_cast<int>(dmg);
