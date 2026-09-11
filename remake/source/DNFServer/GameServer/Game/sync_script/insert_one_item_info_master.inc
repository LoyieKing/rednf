bool CSyncScript::insert_one_item_info_master(unsigned char masterNo,
                                              unsigned short subNo,
                                              const char* name,
                                              const char* explain)
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；84=84）。
    //   归位要点：P310 翻案——循环变量改 int + 显式 `(unsigned int)i < 2u`
    //   复现 ORIG 两段寄存器尾测 `mov -0xc; cmp $1; setbe`。ORIG @0x08625c54。
    static const int BUFSIZE = 0x64;  // ORIG _ZZN11sync_script11CSyncScript27insert_one_item_info_master…E7BUFSIZE
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    const char* names[2];
    // b28d-35：ORIG 在 db 取值后先对 names[0]/names[1] 各发一次
    //   `movl $0x0,-N(%ebp)`（P198① 预清两句式）⇒ 照抄预清。
    names[0] = 0;
    names[1] = 0;
    names[0] = toMbcs(name);
    names[1] = toMbcs(explain);
    // b28d-35：P253 真 bug 修正——ORIG 0x0864… 的清零为 `mov $0x32,%edx` +
    //   `rep stos`，0x32 是 **dword 计数**（0x32*4 = 0xc8 = sizeof(escaped)）；
    //   原 `memset(escaped, 0, 0x32)` 少清 0x96 字节（escaped[1] 前半未清）。
    //   改 `={0}` 同时复现 4.1 内联 rep stos（P116/P214③/P253）。
    char escaped[2][BUFSIZE] = {0};
    // [b28d-solo-sync_script] P310 翻案：ORIG 0x08625d0a 尾测为
    //   `mov -0xc(%ebp),%eax; cmp $0x1,%eax; setbe %al; test; jne`（两段寄存器形）；
    //   4.4 对 `unsigned int` 局部恒折 `cmpl $0x1,-0xc`（内存直比，少 1 条）⇒
    //   局部改 int + 显式 `(unsigned int)i < 2u` 即逐条复现。
    for (int i = 0; (unsigned int)i < 2u; ++i)
    {
        GetEscapeString(db, names[i], escaped[i]);
    }
    if (!db->set_query(
            "inSert into dnf_item_info_master(master_no, sub_no, name, master_explain) values(%u, %u, '%s', '%s')",
            (unsigned int)masterNo, (unsigned int)subNo, escaped[0], escaped[1]))
    {
        return false;
    }
    // b28d-35：同 insert_quest_category —— ORIG 尾含 `xor $1,%eax; test;
    //   je; mov $0; jmp; mov $1`（P101 形）。
    if (!db->exec(true))
        return false;
    return true;
}