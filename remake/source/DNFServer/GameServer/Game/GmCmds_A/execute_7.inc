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
