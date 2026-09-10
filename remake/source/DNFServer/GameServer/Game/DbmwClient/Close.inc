int DbmwClient::Close()
{
    // ORIG @0x08120ab0：`call close; cmp $0xffffffff,%eax; sete %al;
    // test %al,%al; je else` —— **结果不落栈**，直接在 %eax 上比较并 sete 归一。
    // 技巧 9（`(bool)(intcall()==K)` → cmp+sete+test）：引入 `int result` 具名
    // 局部会让 4.4 先 `mov %eax,-0xc(%ebp)` 再 `cmpl $-1,-0xc`（形态失配 4 条、
    // 且帧从 0x18 涨到 0x28）。免局部直比同时解决两处。
    if ((bool)(::close(m_sock) == -1))
        return errno;

    m_connected = false;
    m_initialized = false;
    m_sock = -1;
    return 0;
