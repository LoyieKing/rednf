// ---- SetUserManagerToolIP ORIG @ 0x08655327 ----
// SIG_UPDATE_LOGOUT 布局（本函数实证）：+0x22 premium 管理工具标志(byte)、
// +0x23 ip 相关 dword、+0x27 ip[0x10]、+0x37 前段、+0x43 后段（'.' 拆分）。
// [b28d-L1] N103 封顶（工具链头/库差异）：ORIG 0x08655327 调 `_ZSt7strrchrPci`（libstdc++ C++ 重载，需 __CORRECT_ISO_CPP_STRING_H_PROTO）；本工具链 cstring 把 std::strrchr 映射为 ::strrchr（P167 MISSING×1/EXTRA×1）。源码不可控，语义等价。
// [SweepPIHash] 尾部 4 个 nop+jmp 岛已按 P223 多岛形（4 条独立 goto + 尾标签串）
// 复现（旧 4 条早返只产 3 岛）。
// [P1T5 修法 u1 · 已落] ORIG 0x08655375/0x0865538e（前缀/后缀长度比较）是
// `cmpl $0xc,-0x10(%ebp)`/`cmpl $0x4,-0xc(%ebp)` + `ja`（mem 直比），旧码把
// prefixLen/suffixLen 声明为 int 且每处 `static_cast<unsigned int>` ⇒ 4.4 产
// `mov -0x10(%ebp),%eax; cmp $0xc,%eax`（多 1 条载入 ×2，且整体错位）。改
// 局部本身为 `unsigned int`（赋值处 cast 同步）+ 去比较处 cast ⇒ 两处转直比，
// fdiff blocks 9→3、ours 111→109（ORIG 110），逐槽偏移同时对齐。
// 残余：①N102 m_ipBuf/m_pcRoomIp 读 ORIG 恒
// `mov base,%eax; add $0x27/$0x8e418,%eax; movzbl (%eax)` 两步，4.4 折单条 disp
// （七源形探针全折）；②N103 std::strrchr 重载；③`m_ipDword`(unsigned) ←
// `m_field24`(int) 异型 ⇒ 4.4 多 1 条 `mov %eax,%edx` 中转（P265；字段类型改动属
// P321-S2 共享头旋钮；[P1T5 probe c1] 显式 `static_cast<unsigned int>` 同形，已回滚）。
void SetUserManagerToolIP(CUser* user, SIG_UPDATE_LOGOUT* sig)
{
    if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
        sig->m_pcbangFlag = 1;
    else
        sig->m_pcbangFlag = 0;

    user->m_network.GetPeerIP2(sig->m_ipBuf, 0x10);

    // peer IP 为空时回退用户 m_mcap.m_pcRoomIp（+0x8e418）记录的 IP。
    // [g03 · P-g03-b 修法（Main 登记为「改善」保留）] ORIG 该两处首字节读恒为两步
    //   `mov base,%eax; add $0x27/$0x8e418,%eax; movzbl (%eax),%eax`；成员访问形被
    //   4.4 折成单条 disp（旧 N102 判词），char* 常量算术形恒产 add+movzbl 两步
    //   （探针 probe3 c2/c5/c6/c8 实证；偏移为既有文档化偏移，语义逐字等价）。
    if (*(char*)((char*)sig + 0x27) == '\0' &&
        *(char*)((char*)user + 0x8e418) != '\0')
    {
        std::memset(sig->m_ipBuf, 0, 0x10);
        std::strncpy(sig->m_ipBuf, user->m_mcap.m_pcRoomIp, 0x10);
    }

    // [SweepPIHash] ORIG 0x08655327 尾部为 4 个独立 nop+jmp 岛（落点 9d/a0/a3/a6，
    // 均 jmp 出口 a7）⇒ 按 P223 多岛形写 4 条独立 goto + 尾标签串（标签序=守卫序）。
    // 变量先声明（无初始化）以合法跨越 goto（P223 BM8）。
    char* dot;
    // [P1T5 probe u1] ORIG 67/77 直接 `cmpl $0xc,-0x10`/`cmpl $0x4,-0xc` + ja
    // （mem 直比无载入）⇒ 试局部本身为 unsigned（无符号比较 + 无 cast 中转）。
    unsigned int prefixLen;
    unsigned int suffixLen;
    if (std::strlen(sig->m_ipBuf) > 0x10)
        goto tool_ip_end1;
    // 以最后一个 '.' 把 a.b.c.d 拆为前缀(+0x37)/后缀(+0x43)。
    dot = std::strrchr(sig->m_ipBuf, '.');
    if (dot == 0)
        goto tool_ip_end2;
    prefixLen = static_cast<unsigned int>(dot - sig->m_ipBuf);
    if (prefixLen > 0xc)
        goto tool_ip_end3;
    // [b23S3C] P48：后缀起点用 ipBuf+prefixLen+1 表达式（ORIG 两使用点各自
    // `mov 0xc;lea 0x27;mov -0x10;add$1;lea(,%eax,1)` 重取 5 条；dot+1 缓存形
    // 少 3 条/处，非 ORIG 表达式）。
    suffixLen = static_cast<unsigned int>(std::strlen(sig->m_ipBuf + prefixLen + 1));
    if (suffixLen > 4)
        goto tool_ip_end4;
    std::strncpy(sig->m_ipPrefix, sig->m_ipBuf, prefixLen);
    std::strncpy(sig->m_ipSuffix, sig->m_ipBuf + prefixLen + 1, suffixLen);
    // [P1T5 probe c1 · 回滚] 显式 `static_cast<unsigned int>` 与隐式转换同形
    // （4.4 仍产 `mov %eax,%edx` 中转），ORIG 101 直入 edx 属 m_field24(int) →
    // m_ipDword(unsigned) 异型（P265/P321-S2 共享头旋钮），源码不可翻。
    sig->m_ipDword = user->m_mcap.m_field24;  // +0x8e414
tool_ip_end1:;
tool_ip_end2:;
tool_ip_end3:;
tool_ip_end4:;
}