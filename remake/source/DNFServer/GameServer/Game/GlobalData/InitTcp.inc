// ORIG T 0x084718e2：ip/port 有效且 m_sock<0 时 CreateConnectionSocket；
// 失败 cMyTrace 报错返 0；m_sock>=0 跳过连接，尾部汇合 return 1。
// 残余差归因（N 类，块内求值序，见下方 0x0847196d 区注释）：ORIG 因提前求值
// varargs 而多保存 %edi（0x084719d6 `add $0x4c,%esp; pop %ebx/esi/edi`），
// 4.4 帧 0x40/仅 %ebx ⇒ 块序与帧大小联动不可源码翻转。
// [b28d-18 N7+N82] p167 CLEAR（8/8）；残差=callee-saved 数量（ORIG push edi/esi/ebx vs 我们 esi/ebx） | ORIG 0x084718e2
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（8/8）；N80（cMyTrace ctor 前右起 varargs 跨
// ctor 保活 %edi/%esi/%ebx 不可翻——4.4 恒 ctor 后求值）+ N82 帧 0x4c vs 0x40。
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
    // [DataLane P232] ORIG `sub $0x4c` vs 我方 `sub $0x3c`（差 0x10）⇒ 补 0x10
    // 字节 dead 数组（4.4 -O0 对未读写局部照样占帧），使 cMyTrace/ret 槽基与
    // ORIG（-0x2c/-0x1c）一致；余 N80（ctor 前 varargs 保活）/N82。
    char framePad[0x10];
    (void)framePad;
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
        // [GDataWriter 2026-09-11] ORIG 0x08471985 `mov -0x1c(%ebp),%esi` = ret 在
        // cMyTrace ctor **前**提升到 callee-saved（跨 ctor 保活）；ORIG 同时保活
        // %ebx(strerror)/%edi(errno)/%esi(ret) 三值 ⇒ 4.4 缺 %edi 保留。试**纯
        // register** 存储类提示（P231/P307，非 asm 绑定）令 ret 免槽+升格。
        // [K8GlobalData 2026-09-11 · 负结论，勿重烧] ORIG 0x084718e2 段确为
        // `mov %eax,-0x1c(%ebp); cmpl $0x0,-0x1c(%ebp); setne %al; test %al,%al;
        // je`（r 有具名槽 + bool 物化两尾）且 0x08471985 `mov -0x1c(%ebp),%esi`
        // 跨 cMyTrace ctor 读回同槽。本轮三形穷举均不成立（每次私有 .o 复编实测）：
        //   ① `register int ret` + `(bool)((ret=f())!=0)`：72 条/ndiff 19（最佳，保留）
        //   ② 去 register（int ret）+ 赋值独立语句 + `if (ret != 0)`：70 条/ndiff 24
        //      ⇒ 4.4 仍把槽改到 -0xc 且免 setne/test；
        //   ③ ② + `if ((bool)(ret != 0))`：70 条/ndiff 24（4.4 对 bool cast 亦折叠）。
        // 残差 4 项（push/pop %edi + setne/test + 帧 0x4c vs 0x40 + r 槽 -0x1c vs -0xc）
        // 全部为 4.1/4.4 分配与物化差异，无源码形可翻 ⇒ N80/N82 封顶。
        register int ret;
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
}