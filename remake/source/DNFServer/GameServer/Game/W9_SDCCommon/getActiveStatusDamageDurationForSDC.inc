// [b28d-solo-W9_SDCCommon] ORIG 0x0808df82：守卫顺序 outPtr(0x14)→outB(0x18)
// →d(0x1c)（P133 独立守卫，禁合并短路）；params.size()!=0 产
// `test %eax,%eax; setne %al; test %al,%al; je`（P197④：empty() 恒产 xor 反向）；
// 赋值块读 **outPtr**（0x14）而非 outB（原重建误写 *outB，真语义 bug，已修）；
// switch 叶块物理序 = case 书写序（P46）：burn(0x107)→poison(0x13a)→
// bleeding(0x162)→disease(0x17b)。
void SDCActiveStatusSyncCommon::getActiveStatusDamageDurationForSDC(
    char attackType, int statusLevel, int* outPtr, int* outB,
    STSDCActiveStatusSyncDataSend* d, void*)
// 0x0808df82：栈上备份 d、经 makeEtcParameters 取 duration / 参数，
// 按 attackType 分派伤害计算并写入 *outB。
{
    if (outPtr == 0 || outB == 0 || d == 0)
        return;

    STSDCActiveStatusSyncDataSend local = *d;      // ORIG 帧 -0x4a..(-0x16) 全拷
    std::vector<int> params;

    makeEtcParameters(static_cast<ENUM_ACTIVESTATUS>(attackType), statusLevel,
                      &local, &params, outPtr);

    if (params.size() != 0)
    {
        // ORIG 0x0808e06b：local.durationTick = *outPtr（0x14），非 *outB。
        local.m_baseDamage = params[0];
        local.m_durationTick = *outPtr;
    }

    switch (attackType)
    {
    case 9:                                        // burn
    {
        bool flag = false;
        *outB = getBurnDamage(flag, &local, 0, local.m_durationTick);
        break;
    }
    case 2:                                        // poison
        *outB = getPoisonDamage(&local, 0, local.m_durationTick);
        break;
    case 11:                                       // bleeding(server)
        *outB = getBleedingDamageServer(&local);
        break;
    case 6:                                        // disease
    case 7:
        *outB = local.m_baseDamage;
        break;
    default:
        break;
    }
}