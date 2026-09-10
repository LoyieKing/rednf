int SDCActiveStatusSyncCommon::getBleedingDamageClient(
    STSDCActiveStatusSyncDataSend* d, bool, void*)
{
    int tickB;
    int tickA = d->m_durationTick / 500;
    // ORIG 执行序：tickA 计算/存储先行，tickB 常量写入在后；槽 tickA=-0x18 深、
    // tickB=-0x14 ⇒ 4.4 先声明占高地址，tickB 仅声明、赋值延后至 tickA 后。
    tickB = 1;
    int tick = std::max(tickB, tickA);          // ORIG (%esp)=tickB、0x4(%esp)=tickA
    // P0：ORIG float 具名局部槽 -0xc（fdivrp → fstps → flds 两段物化）。
    float dmg = static_cast<float>(d->m_baseDamage) / static_cast<float>(tick);
    return static_cast<int>(dmg);
