// ============================================================================
// CheckLoginIP（ORIG T 0x08415472）
// ============================================================================

// [b28d-PoolDBLogin] P306 族（工具链头版本差；先例 = Divide_C_ClassIP_EndIP.cpp 的
// `namespace std { inline char* strrchr(char*, int) }` wrapper）。ORIG 的 libstdc++(4.1)
// 以 inline 发射 std::strrchr(char*, int)（nm: `W 0x08265244`，体 = 8 条转发
// strrchr@plt），本件两处调点即 `call _ZSt7strrchrPci`。本 TU 的 <cstring>（经
// DB_Login.h/GlobalData.h 链亦不可避）已由 glibc `__CORRECT_ISO_CPP_STRING_H_PROTO`
// 把 `using ::strrchr;` 注入 std ⇒ 同签名定义**冲突**（先例 TU 能直写只因它不引
// <cstring>）。故按同一 inline 体 + 同一 ELF 符号名落地：`weak` 保绑定（ORIG = W，与
// 先例 TU 的 W 合并），体逐条 = ORIG 0x08265244。非 §4.5 所禁的游戏代码 mangled 桥
// （被桥接对象是 libstdc++ 库包装，其定义在 ORIG 二进制里就在 .text）。
extern "C" char* dbLoginStrrchrLibcpp(char* s, int c)
    __asm__("_ZSt7strrchrPci") __attribute__((weak));
inline char* dbLoginStrrchrLibcpp(char* s, int c)
{
    return __builtin_strrchr(s, c);
}

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
