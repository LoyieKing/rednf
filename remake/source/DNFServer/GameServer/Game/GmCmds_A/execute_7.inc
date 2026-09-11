// ORIG execute 0x084acc1e：职业 0..10 时遍历 bind×recipe×equip 组合跑合成测试，
// 每组结果落一份 bind_<job>_<bind>_<recipe>_<equip>.txt 回显给 GM。
// [b28d-47] verdict=DIFF(routed) [N78/N84] ORIG 0x084acc1e orig/ours=166/134
//   残差：EXTRA memset（我方缓冲零初始化 call vs ORIG 内联 rep stos，N78）+
//   槽/寄存器分配差。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=5 [N-待解] p167 CLEAR(14/14)。
// [b28e-solo-2] 真因定位：ORIG 函数头 fileBuf 清零是**数组初始化**形而非显式
// memset —— +0x085 段 `mov $0x17318,%edx; mov %edx,0x8(%esp)`（size 经 %edx
// 中转），而循环内 +0x101 段才是 `movl $0x17318,0x8(%esp)` 直写（同一常量两形
// 并存 ⇒ 原码两处写法不同）。4.4 对 `char buf[N] = {0};` 精确复现中转形
// （探针 /tmp/pg/p1.cpp init_arr vs init_arr2），改后 gap 1→0。
void CBindCubeTest::execute()
{
    if (!(static_cast<signed char>(m_flag) >= 0 &&
          static_cast<signed char>(m_flag) <= 10))
    {
        return;
    }

    // [b28d-solo-GmCmds_A] P312：ORIG 对 fileName[0xff](-0x13b) 发内联对齐剥离
    // 清零（无 memset call，p167 EXTRA memset）。
    char fileName[0xff] = {0};
    // [b28e-solo-2] 函数头清零 = 数组初始化形（size 经 %edx 中转，见上）；
    // 循环内仍是显式 memset(fileBuf,0,95000) 直写形。
    char fileBuf[95000] = {0};

    std::map<int, int> result;
    for (int bind = 0; bind <= 1; ++bind)
    {
        for (int recipe = 0; recipe <= 2; ++recipe)
        {
            for (int equip = 0; equip <= 7; ++equip)
            {
                memset(fileName, 0, 0xff);
                memset(fileBuf, 0, 95000);
                static_cast<WongWork::CCompoundAvatar*>(
                    GlobalData::s_pCompoundAvatar)
                    ->ProcTest(static_cast<ENUM_CHARACTERJOB>(
                                   static_cast<int>(static_cast<signed char>(m_flag))),
                               static_cast<ENUM_EQUIPMENTTYPE>(equip),
                               recipe, bind, result);
                CBoosterTest::makeFileContents(GetUser(), fileBuf, result);
                sprintf(fileName, "bind_%s_%s_%s_%s.txt",
                        CHARACTER_JOB_NAMES[static_cast<int>(
                            static_cast<signed char>(m_flag))],
                        BIND_NAMES[bind], RECIPE_NAMES[recipe], EQUIP_NAMES[equip]);
                CBoosterTest::printClient(GetUser(), false, fileName, fileBuf);
                result.clear();
            }
        }
    }
}