void upgradeAbilityDeathTower(int idx, ActiveStaticInfo* info)
{
    // 槽基垫形（探针 /tmp/l35_probe2.cpp v1）：4.4 槽域需 +0x14 才与 ORIG
    // （slot@-0x1c / cw@-0x1e·-0x20 / 循环 i@-0xc，帧 0x38）同槽；引用型
    // double 4B pad 位于槽域底使后续槽整体 -4，其余 +0x10 转发为 outgoing。
    // i 声明需先于首次赋值，否则 4.4 零行删守卫并转 i 下 -4（级联全帧）。
    // 关键：本 TU 中 callee getToleranceActiveStatus(129 行) 定义已见 → 4.4 按
    // 其真实 outgoing 塌缩本帧（P210）。本函数定义须位于其之前。独立 TU 验证
    // /tmp/l35_dt_tu.cpp（真实函数体 + 声明）= ORIG 帧 0x38 槽 -0x1c。
    double framePad; (void)&framePad;
    int i;
    // ORIG 0x088b93bf：全局表直接索引（无 tbl 指针变量）。
    info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                      DEATH_TOWER_BONUS[0][idx + 0x10]);
    info->m_attr17Base[13] += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0xc]);
    info->m_statBase6 += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0x28]);
    for (i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0x1c]);
    }
    info->m_statBase4 = static_cast<int>(static_cast<float>(info->m_statBase4) *
                                         DEATH_TOWER_BONUS[0][idx + 0x14]);
    info->m_statBase3 += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0x18]);
    info->m_statBase11 = static_cast<int>(static_cast<float>(info->m_statBase11) *
                                          DEATH_TOWER_BONUS[0][idx + 0x20]);
    info->m_statBase13 = static_cast<int>(static_cast<float>(info->m_statBase13) *
                                          DEATH_TOWER_BONUS[0][idx + 0x20]);
    info->m_statBase12 = static_cast<int>(static_cast<float>(info->m_statBase12) *
                                          DEATH_TOWER_BONUS[0][idx + 0x24]);
    info->m_statBase14 = static_cast<int>(static_cast<float>(info->m_statBase14) *
                                          DEATH_TOWER_BONUS[0][idx + 0x24]);
}