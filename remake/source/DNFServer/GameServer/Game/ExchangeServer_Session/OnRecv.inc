bool CSession::OnRecv()
{
    int ret = recv_packet();
    if (ret > 0)
    {
        // b28d-68 P0 清绑：删 register asm("eax") 绑定，改 bool callee 自然形
        // （P292：`if (!Parsing(ret))` 直产 `call; xor $0x1,%eax; test %al,%al; je`，
        // 与 ORIG 0x0849033a..0x0849034d 逐条同形；X1 调用点自救已废）。
        if (!Parsing(ret))
            return false;                     // 失败分支（Parsing 返回 0）
        return true;                          // 成功（Parsing 返回 1）
    }
    if (ret < 0)
    {
        return false;                          // eax=0 错误路径
    }
    return true;                              // ret==0 → eax=1
