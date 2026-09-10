bool CPCRoomServerProxy::InitTcp()
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return 0;
    }
    if (m_tcp.m_sock < 0)
    {
        // ORIG 0x08471xxx：ret 常规局部落 -0x1c 槽（cmp $0,-0x1c; setne 物化）。
        // [FL7GlobalData 2026-09-10 · P290 实测否定] ORIG 在 cMyTrace ctor **之前**
        // 把 ret 载入 callee-saved（0x08471985 `mov -0x1c(%ebp),%esi`）跨 ctor 保活；
        // 按 P290 试 `unsigned int ret;` 后 ndiff 仍 22（push/pop edi 仍缺失、coder 后
        // 仍从槽重读）⇒ 维持 int，记 N80/N82 族。
        int ret;
        // ORIG 0x0847196d..0x084719c5（临时链式实证）：4.1 在 cMyTrace ctor
        // call 之前先按右起序求 varargs 并全部落 callee-saved —— 0x0847197c
        // strerror(errno)→%ebx、0x08471983 errno→%edi、0x08471985 ret→%esi，
        // 之后才 0x08471988 装填 ctor 实参；4.4 恒先 ctor 后 varargs（改具名
        // 对象反而把 errno 推后 ⇒ probe 证两写法均不可翻）⇒ 记 N 类。
        if ((bool)((ret = CNetwork<100000, 100000>::CreateConnectionSocket(
                       m_tcp.m_sock, m_ip.c_str(), m_tcpPort)) != 0))
        {
            cMyTrace(__FUNCTION__, 0x116, 0)
                ("CPCRoomServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    return 1;
