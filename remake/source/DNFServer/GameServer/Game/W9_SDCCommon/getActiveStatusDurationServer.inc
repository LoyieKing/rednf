int SDCActiveStatusSyncCommon::getActiveStatusDurationServer(
    int status, int level, int objectType, int a4, int hpPenalty)
{
    int duration = 0;
    switch (status)
    {
    // P46：叶块物理顺序 = case 书写顺序。ORIG 叶块序为 duration(0x0808d2bd)
    // → power1(0x0808d2d2) → zero(0x0808d30b)，故 duration 组必须写在前面。
    case 0:
    case 2:
    case 4:
    case 6:
    case 9:
    case 11:
    case 12:
    case 13:
        duration = ACTIVESTATUS_DURATION[status] * 1000;
        break;
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 15:
    case 16:
    case 17:
        duration = static_cast<int>(1000.0f * getActiveStatusPower1(status, level));
        break;
    case 14:
        duration = 0;
        break;
    default:
        break;
    }

    if (_SDC::isObjectType(objectType, 0x211))
    {
        float f1 = static_cast<float>(hpPenalty) / 3.0f;
        float x = f1 / static_cast<float>(hpPenalty);
        // ORIG 0x0808d358：fistpl 落具名 int dec 槽 -0xc（非 duration 槽），
        // `mov -0xc,%eax; sub %eax,-0x18` 两段扣减；该具名局部同时把 duration
        // 定到 -0x18、f1/x 到 -0x14/-0x10（探针 /tmp/sdc_p10.cpp c 全等）。
        int dec = static_cast<int>(static_cast<float>(duration) * x);
        duration -= dec;
    }
    if (duration < 0)
        duration = 0;
    return duration;
