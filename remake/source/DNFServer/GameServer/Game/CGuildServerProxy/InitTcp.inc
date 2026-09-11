// [b28d-solo3 N80 翻案 · DIFF→AE 74/74 ndiff0] 定解 = varargs 首参写
//   `(unsigned int)ret`（**异型化 cast 触发 4.4 提前把该值装 callee-saved 并跨
//   cMyTrace ctor 保活**）：4.4 产 `mov -0x1c(%ebp),%eax; mov %eax,%esi` + push edi
//   + sub $0x5c，与 ORIG 逐条全等（原 N80 判词列的 P226 语句表达式 / P227 全表达
//   式链 / P231 裸 register / volatile 读 四形均无效，唯异型化 cast 命中——
//   属 P41 变体（varargs 冗余 cast 诱发 callee-saved 缓存）的**正向**用法）。
// [b28d-solo-CGuildServerProxy N80: 74/72] P225 while 形已复现 ORIG @0x0846d9c1
//   `cmpl $0x0,-0x1c(%ebp); setne %al; test %al,%al; je`（原 if 形 4.4 恒折
//   `cmpl;je` 少 2 条）。残余=4.1 在 cMyTrace ctor 前把 varargs 右起求值并
//   callee-saved 保活（strerror→ebx、errno→edi、ret→esi ⇒ push edi + sub $0x5c）；
//   4.4 只保活两个调用结果（strerror→ebx、errno→esi），槽变量 ret 恒在使用点
//   `mov -0xc(%ebp),%eax` ⇒ 缺 push edi、帧 -0x10、全槽基差 0x10。P226 语句表达式
//   `({ ret; })`、P227 全表达式临时链均实测零效；P231 裸 register 反使 ret 失槽
//   （ORIG 有槽 -0x1c，违 P0 判读）；b28d 复验追加：varargs 首参改
//   `(*(volatile int*)&ret)` 强制 volatile 读（74→76 劣化，仍缺 push edi）
//   ⇒ N80 封顶。b28d-solo2 复验：ret 声明移至函数头（P212/P309 头部声明律）实测
//   零效（ret 仍 -0xc、帧仍 0x50、缺 push edi）⇒ N80 定案 | ORIG 0x0846d96c
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
bool CGuildServerProxy::InitTcp()
{
    if (m_udpPort == 0)
    {
        return 1;
    }
    if (m_tcp.m_sock < 0)
    {
        int ret = m_tcp.CreateConnectionSocket((int&)m_tcp, m_ip.c_str(),
                                               m_tcpPort);
        // [b28d-solo-CGuildServerProxy P225] ORIG @0x0846d9c1 `cmpl $0x0,-0x1c(%ebp);
        // setne %al; test %al,%al; je` —— 4.1 的槽比零物化形，4.4 对 if 恒折
        // `cmpl;je` 直跳；P225 实证 while+break 形可复现（CVillageAttacked/CDropItem）。
        while (ret != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4ea, 0)
                ("CGuildServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 (unsigned int)ret, errno, strerror(errno));
            return 0;
        }
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x4ef, 9, true, true)
        ("[!] Connect To Guild Server ...");
    return 1;
}