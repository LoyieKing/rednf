// ---------------------------------------------------------------------------
// Connect — ORIG 0x8490829 size 0xc9
// [N96 清绑 2026-09-09] 删除历史 `register bool failed asm("eax")`（P0 限定② /
// N96 全域禁用；P248 不得恢复）。报告缺失（docs/class_func_reports 无
// exchange_server 目录；同名 CDBConnector/Connect.md 经签名核对为撞名错配，
// 仅用 ORIG 二进制切片，§3.2 兜底并标注）。ORIG 0x849084c
// `cmpl $0x0,-0x10(%ebp); setne %al; test %al,%al; je`——failed 值全程在寄存器、
// 无栈槽 ⇒ 原码为裸 register 局部（当时的 `if (ok != 0)` 直判是过度删除）。
// [M1S5 2026-09-11 **修正 N96 清绑过度删除**] 裸 `register`（**无 asm 绑定**）是
// 合法存储类提示（P231；docs/asm_translation_patterns.md 该条登记本文件即此形），
// N96/P0 禁的是 `register T x asm("reg")` 显式绑定。恢复
// `register bool failed = (ok != 0); if (failed)` 后复现 ORIG 0x849084c 形
// `cmpl $0x0,-0x10(%ebp); setne %al; test %al,%al; je`（4.4 分配 %bl 而非 4.1 的
// %al，寄存器角色差、助记符序列全等）。
// 读数：清绑过度 [ae=13 合计16 near=0 diff=4，orig57/our55，缺 setne+test 两条]
//       → 本波 [near=1 diff=3，orig57/our57]；本函数 DIFF→NEAR。
// ---------------------------------------------------------------------------
CSession* CSession::Connect(const char* ip, int port)
{
    int sock;                                 // -0x24(%ebp)
    // [b28c-6][N7] AllocSession 虚调用槽寄存器 %eax↔%edx 互换（4.4 恒形），源码不可翻（ndiff 8）。
    // CreateConnectionSocket 为静态成员（ORIG 同）；返回非 0=失败码。
    const int ok = CNetwork<95000, 450000>::CreateConnectionSocket(sock, ip, port);

    // [N96 清绑] 原 `register bool failed asm("eax")` 已删（P0/N96 全域禁绑）。
    // ORIG 0x849084c `cmpl $0x0,-0x10(%ebp); setne %al; test %al,%al; je` ——
    // failed 值全程在寄存器、无栈槽 ⇒ 原码为**裸 register 存储类提示**
    // （P231：禁的是 `register T x asm("reg")` 绑定，裸 register 合法且 -O0
    // 仍参与分配决策；本形即 docs 中登记的 ExchangeServer_Session 计数族）。
    // [C4A0Nine 2026-09-11 复验] 另 4 形全败：`register char failed` /
    //   `register const bool` / 分离赋值（先声明后赋）/ `= !!ok` ⇒ 4.4.7 恒
    //   `setne %bl; test %bl,%bl`（57/58 不变）。ORIG 0x08490850 `setne %al` 为
    //   4.1 QImode 分配独有形 ⇒ N7/N109 封顶。
    register bool failed = (ok != 0);
    if (failed)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24b, 0)(
            "CreateConnectionSocket failed - %s", strerror(errno));
        return 0;
    }

    // vtable+8 AllocSession
    CSession* session = static_cast<CSession*>(ES()->AllocSession());
    if (session != 0)
    {
        session->set_socket(sock, true);
        return session;
    }
    return 0;                                  // ORIG 0x84908e7: mov $0,%eax
}