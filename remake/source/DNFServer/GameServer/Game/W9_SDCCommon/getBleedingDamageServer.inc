// [b28d-28] NEAR | ORIG 0x0808df1a | N82：x87 槽组平移 0xc（raw -0x8 vs ORIG
// -0x14、cw -0x12/-0x14 vs -0x16/-0x18）；P232 撑槽探针实测反劣（帧 0x24）。
int SDCActiveStatusSyncCommon::getBleedingDamageServer(
    STSDCActiveStatusSyncDataSend* d)
{
    // [b28d-solo-W9_SDCCommon] ORIG 0x0808df1a：冗余 (int) cast 强制 4.4 把
    // int 中间值物化到 -0x14 槽（mov 0x3(%eax),%eax; mov %eax,-0x14; fildl
    // -0x14），fistpl 复用同槽返回；float 具名 dmg -0x4、x87 控制词
    // -0x16/-0x18、帧 0x18 与 ORIG 全等（探针 /tmp/sdc_p3.cpp v12 23/23）。
    float dmg = static_cast<float>((int)d->m_baseDamage);
    dmg = dmg * 1.5f;
    return static_cast<int>(dmg);
}