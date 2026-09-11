// ORIG 0x0818b443：初始 equipslot 快照（+0x4e01 起 0x2dc 字节）zip 后 upDate 入库。
// b28d-solo-ARAD_Database：ORIG 0x818b44e 起为内联 memset 对齐剥离慢路（P214③/
//   P253：`char zipBuf[0x333]={0}` 复现 rep stos 慢路与 push edi/ebx 帧）；
//   compress_zip 结果无槽往返（`xor $0x1,%eax; test %al,%al`，P0 右值内联）；
//   update 实参右→左求值：charac_no 先入 %ebx 跨 blob_to_str 保活，blob 返回值
//   %eax 直传（无 equipZip 槽）。
// b28d-solo-ARAD_Database verdict=DIFF(91/91)：P167 4/4 CLEAR；残余=compress_zip
//   第3参局部读与第4参立即数压栈先后（ORIG 先读局部，4.4 先压常量，P75/N-G 跨版本
//   调度）。语义闭合。
int CREATE_QUERY::arad_sp_create_equip(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    // [符号名轴 2026-09-11 W13D] 同 inven：原函数内 static const 会在 .o 发射 ORIG 侧
    //   不存在的 `…arad_sp_create_equip…E12kEquipSrcLen` ⇒ 改用命名空间常量
    //   sourceEquipSize（ORIG 有 `_ZN4ARAD8DATABASEL15sourceEquipSizeE`）。
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
                      sourceEquipSize))   // +0x4e01 初始装备区
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
}