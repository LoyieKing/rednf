// [b28d-solo-W6_IPGDeps] verdict=AE（48=48，原判 N(P75) 作废）。唯一残差原为
// connect 三实参求值序（ORIG arg3(len)→arg2(&addr)→arg1(m_sock) 严格右到左；
// 4.4.7 裸形恒 arg2→arg1→arg3）**且** ORIG 的 len 占浅槽 -0xc、addr 占深槽 -0x1c。
// 复现形（探针 /tmp/ct_probe6.cpp g15，与 ORIG 逐条同形）：
//   ① len 声明**前置**于 `sockaddr_in addr;` 之前（P309 头部声明方向 ⇒ 占浅槽）；
//   ② volatile 限定（P316 ⇒ volatile 读先于非 volatile 实参求值）；
//   ③ 赋值语句 `addrLen = sizeof(addr);` 留在原位（避免重复初始化）。
// 语义/调用点/帧一致；报告 ConnectTo.md 的 Ghidra C `local_10 = 0x10;` 对应一致。
int CIPGMgr::CIPGWorker::CIPGNetwork::ConnectTo(const char* ip, unsigned short port)
{
    // [b28d-solo-W6_IPGDeps] 头部前置声明 + volatile：ORIG 0x0810455d 段为
    // arg3(len)→arg2(&addr)→arg1(m_sock) 严格右到左求值，且 len 占**浅**槽 -0xc、
    // addr 占深槽 -0x1c。裸形 4.4.7 恒 arg2→arg1→arg3 且 volatile 就地声明会使
    // len 抢深槽（DIFF→NEAR）。唯一复现形 = len 声明前置于 addr 之前（P309 头部
    // 声明方向）+ volatile 限定（P316），赋值语句留在原位（探针 /tmp/ct_probe6.cpp
    // g15 与 ORIG 逐条同形）。
    volatile unsigned int addrLen;
    sockaddr_in addr;
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock < 0)
    {
        return -1;
    }
    memset(&addr, 0, sizeof(addr));                 // ORIG memset 0x10（sockaddr 前 16B）
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    // ORIG len 为具名局部（movl $0x10,-0xc 槽 + 读回 %ecx）。
    addrLen = sizeof(addr);
    if (connect(m_sock, (sockaddr*)&addr, addrLen) < 0)
    {
        return -2;
    }
    return 0;
}
