// ============================================================================
// 3) _Z34getComputeDamageBasisLevelAttackerP23STComputeDamageVariableb @0x080957c3 (0x4d)
//    bool 为真且 isObjectType(m_attackerObjType,0x111) → (signed char)0x1b4；
//    否则 (signed char)0x1b3。两字节 ctor 初值均为 1。
// [b28d-lane15 判词·N（跨 TU 帧继承，P210②）] ORIG 帧 sub $0xc / bool spill -0x4；
// 4.4 对**仅声明**的 _SDC::isObjectType 恒保守 sub $0x28 / spill -0xc（探针
// /tmp/b28d_l15_probe3.cpp fA..fE 五形全 0x28）。把 isObjectType **定义**置于
// caller 之前（探针 /tmp/b28d_l15_probe4.cpp fF/fG）即复现 0xc。ORIG 中
// isObjectType @0x080950e8 与本族地址相邻（同 TU）；我方定义在 W9_SDCCommon.cpp
// （非本泳道 TU，R1 禁编辑）⇒ 已上报 Main 定夺 TU 归属迁移，本件暂 N 封顶。
// ============================================================================
int getComputeDamageBasisLevelAttacker(STComputeDamageVariable* v, bool useAttackerLevel)
{
    if (useAttackerLevel)
    {
        if (_SDC::isObjectType(v->m_attackerObjType, 0x111))
            return (signed char)v->m_char1B4;
    }
    return (signed char)v->m_char1B3;
}