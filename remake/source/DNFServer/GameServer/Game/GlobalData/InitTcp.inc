// [G7-sub 2026-09-11 判词] CPCRoomServerProxy::InitTcp（ORIG T 0x084718e2）：私有 .o
// 实测 ORIG 73 / 我方 71（gap 2，我方短）。逐条对照（/tmp/g7c/fdiff.py --side）：
//   · 0x0847196d..0x084719c5 ORIG 在 `call cMyTrace::C1` **之前**右起求 varargs 并
//     跨 call 保活（0x0847197c strerror→%ebx、0x08471983 errno→%edi、
//     0x08471985 `mov -0x1c(%ebp),%esi`），我方 4.4 恒 ctor 后求且 errno 落 %esi、
//     ret 从槽重读 ⇒ 缺 `push %edi/pop %edi`（N80，已由 /tmp/l7 probe 否定三形）。
//   · 帧 0x4c vs 0x40：cMyTrace 局部槽 -0x2c vs -0x1c（N82 槽基）。
// p167 调用点差集 CLEAR（8/8，仅别名）。结论：**封顶，无改动**（语义无缺口）。
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
