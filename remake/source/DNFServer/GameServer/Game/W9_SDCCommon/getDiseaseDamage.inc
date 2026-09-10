int SDCActiveStatusSyncCommon::getDiseaseDamage(
    STSDCActiveStatusSyncDataSend* d, int t)
{
    if (!(d->m_hpPercent < 100))
        return 0;

    // [b28d-solo-W9_SDCCommon] ORIG 0x0808dd5c 槽位复现（探针 /tmp/sdc_p4.cpp
    // vc 55/55 逐条全等）：dmg 具名 int -0xc；(float)(int) 双 cast 把
    // baseDamage 物化到 -0x20；PvP 分支 (float)dmg 作乘法**左**操作数先物化
    // 到临时 -0x24（fildl/fstps 先于 callee call），fistpl 复用 dmg 槽。
    int dmg = static_cast<int>(GetUniformVelocityFloatSDC(
        0.0f, static_cast<float>((int)d->m_baseDamage), t, d->m_durationTick));
    if (d->m_isPvp != 0)
    {
        dmg = static_cast<int>(static_cast<float>(dmg) *
            getUserToUserDamageRate(d->m_attackerType == 10));
    }
    return dmg;
