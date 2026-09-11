// [b28d-solo-DB_Login] verdict=AE（SGBC 宏形 off 字面量折叠 + std::strrchr）。
// [b28d-PoolDBLogin] 旧登记的头版本残差**已消**：经上方同体 weak 包装发射
// `_ZSt7strrchrPci` ⇒ fresh 口径 ndiff 2→0、p167 调用点差集归零（原
// MISSING/EXTRA 各 2 双向消失），本 TU 同时补上 ORIG `nm` 有而我方缺的该弱符号
// （ORIG `W 0x08265244`，包装体 10/10 AE-EQUAL；非发明符号）。
bool DB_Login::CheckLoginIP(SIG_LOGIN_DATA* sig)
{
    if (sig == 0)
    {
        return 0;
    }
    sig->m_ingameHackItemFlag = false;           // +0x4124 ip 变更标志清零

    // [b23S3C] P48/§3.2②：偏移地址用 SGBC 宏直算（ORIG 每用点 `mov 0xc;add $imm`
    // 2 条）；SGB always_inline 实参 off 仍落槽（+3 条/处、帧 0x58→0x78），
    // 非模板参数化不可免；truthy 直判去 `!=0`（ORIG test%al je 无 setne 物化）。
    // dot 声明先于双缓冲（ORIG 预清序 -0x10(dot) → curIp → oldIp）。
    char* dot = 0;
    char curIp[16] = {0};
    char oldIp[16] = {0};
    strcpy(curIp, SGBC(sig, OFF_LOGIN_IP));
    // ORIG：双 16 字节缓冲，当前 ip 与旧登录 ip(+0x4114)
    strcpy(oldIp, SGBC(sig, 0x4114));

    // 单 dot 变量两次复用（ORIG 两 strrchr 结果同写 -0x10 槽）。
    // [b28d-38] P167：ORIG 调 `_ZSt7strrchrPci`（std::strrchr）非 C 库 strrchr。
    // [b28d-PoolDBLogin] 经上方同体 weak 包装发射该 mangled 调点。
    dot = dbLoginStrrchrLibcpp(curIp, '.');       // 截掉最后一段（去端口化前缀）
    if (dot != 0)
    {
        *dot = '\0';
    }
    dot = dbLoginStrrchrLibcpp(oldIp, '.');
    if (dot != 0)
    {
        *dot = '\0';
    }
    if (strcmp(curIp, oldIp) != 0)
    {
        if (SGBC(sig, 0x4114)[0])
        {
            sig->m_ingameHackItemFlag = true;
        }
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
        db->set_query("upDate login_common set login_ip='%s' where m_id=%u",
                      SGBC(sig, OFF_LOGIN_IP),
                      sig->m_accId);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__, 0x3e63,
                                  "LOGIN_IP : IP Address UDATE DB ERROR!!");
            return 0;
        }
    }
    return 1;
}