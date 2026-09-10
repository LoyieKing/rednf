int CCommunityServerProxy::InitTcp()
{
    // b28d-40: N80 封顶——P226 语句表达式已试，strerror/errno/ret 升格 callee-saved
    // 的求值序与保活寄存器仍差 2 条；帧 0x4c vs 0x40。ORIG 0x0846c674。
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return 0;
    }
    if (m_sock < 0)
    {
        // ORIG: call 后 mov %eax,-0x1c(%ebp) 落槽、cmpl 槽比零（P0 具名局部，
        // 错误块 mov -0x1c(%ebp),%esi 槽重读）——勿用 register 寄存器直判。
        int ret;
        if ((ret = CreateConnectionSocket(m_sock, m_ip.c_str(), m_tcpPort)) != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbf, 0)
                ("CCommunityServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, ({ strerror(errno); }));
            return 0;
        }
        // 残差 N80（patterns 定论负例，实证即本函数）：ORIG 在 cMyTrace ctor call
        // 之前右起求 varargs 并跨 call 保活 callee-saved（__errno_location→strerror
        // 先于 ctor）；4.4 恒 ctor 后求。具名/临时/register 三探针不可翻。
        // 另残差 N82：ORIG 帧 0x4c/ret@-0x1c vs 4.4 帧 0x40/ret@-0xc（槽号平移）。
    }
    return 1;
