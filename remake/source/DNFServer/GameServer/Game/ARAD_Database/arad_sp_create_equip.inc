int CREATE_QUERY::arad_sp_create_equip(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    static const unsigned int kEquipSrcLen = sourceEquipSize;    // ORIG 初始装备区长度
    char zipBuf[0x333] = {0};
    // ORIG 槽布局：zipLen@-0x10（深）、equipSrc@-0xc（浅）；存储序 zipLen=0x333
    //   先于 equipSrc 计算；调用实参序则 equipSrc 读取先于第 4 参常量压栈
    //   （0x818b4bb）。4.4 恒「后声明深」，故 equipSrc 先声明（浅 −0xc）但延后赋值，
    //   zipLen 后声明（深 −0x10）带初值 ⇒ 同时满足槽位与存储序；equipSrc 以
    //   volatile 变量钉住读取点，复现「先读第 3 参」（create_skill 同法已 AE）。
    char* volatile equipSrc;                                              // ORIG -0xc 具名局部
    unsigned int zipLen = 0x333;                                          // ORIG -0x10 具名局部
    equipSrc = reinterpret_cast<char*>(&sig->m_giftItem);
    if (!compress_zip(zipBuf, zipLen, equipSrc,
                      kEquipSrcLen))   // +0x4e01 初始装备区
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x155, "%s", "equip compress zip fail");
        return -1;
    }

    if (db->update(" upDate inventory set equipslot='%s' where charac_no = %u ",
                   db->blob_to_str(0, zipBuf, static_cast<int>(zipLen)),
                   sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
