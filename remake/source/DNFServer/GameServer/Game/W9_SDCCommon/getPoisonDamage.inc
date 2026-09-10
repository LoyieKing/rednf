int SDCActiveStatusSyncCommon::getPoisonDamage(STSDCActiveStatusSyncDataSend* d,
                                               void*, int t)
{
    if (!(d->m_hpPercent < 100))
        return 0;

    float dmg = GetUniformVelocityFloatSDC(0.0f,
                                           static_cast<float>(d->m_baseDamage),
                                           t, d->m_durationTick);
    // [N82 封顶] 同 getBurnDamage：槽组整体平移 8 + 帧 0x28 vs 0x38，封顶。
    if (d->m_isPvp != 0)
        dmg *= getUserToUserDamageRate(d->m_attackerType == 10);
    // ORIG 0x0808dcb0：fistpl 截断（fnstcw/fldcw 设 RC=truncate），非四舍五入。
    return static_cast<int>(dmg);
