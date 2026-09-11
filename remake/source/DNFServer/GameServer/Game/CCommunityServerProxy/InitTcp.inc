// ORIG T 0x0846c674：ip/port 有效且 socket 未连时 CreateConnectionSocket；
// 失败 cMyTrace 报错返 0（P15 赋值即条件 + P37 临时链式 trace）。
// 返回类型 bool：ORIG prett 串 `_ZZN21CCommunityServerProxy7InitTcpEvE19__PRETTY_FUNCTION__`
// @0x08c662e0 内容 = "bool CCommunityServerProxy::InitTcp()"（strall 审计 + objdump -s 实读），
// 且同族 BaseServerProxy/CGuildServerProxy/CMonitorServerProxy/CPCRoomServerProxy 的
// InitTcp 均为 bool。返回类型不入 mangled ⇒ 零 ABI 风险；调用点
// （CCommunityServerProxy::Init 转发、GlobalData.cpp:3124 重连）均丢弃返回值 ⇒ 行为等价。
bool CCommunityServerProxy::InitTcp()
{
    // b28d-40: N80 封顶——P226 语句表达式已试，strerror/errno/ret 升格 callee-saved
    // 的求值序与保活寄存器仍差 2 条；帧 0x4c vs 0x40。ORIG 0x0846c674。
    // [N5C_EventChar 2026-09-11 复核] 维持封顶。新证据（镜像探针
    // /tmp/r5/n5/N5C_EventChar/probe6.cpp 逐条复现本函数：帧 0x40/push esi,ebx/寄存器
    // 分配与本 .o 全等）：
    //   ① `({ ret; })` 裸语句表达式作第 3 实参（probeB）与基线**同形**（P226 对槽变量无效）；
    //   ② `*__errno_location()` 显式形（probeC，BaseServerProxy 惯用法）只交换 %esi/%ebx
    //      角色，助记符序列与条数不变；P290 int→unsigned（probe5）两形完全相同；
    //   ③ ORIG 独有 2 条 = `push %edi` + `mov -0x1c(%ebp),%esi`（ret 在 ctor 前升格跨调用
    //      保活）⇒ 4.1 独有寄存器保活族（P332：callee-saved push 数源级不可诱导）；
    //   ④ 帧 0x4c 非 4.4 帧量化形（≡4 mod 16，P190 附注）且槽差 = N126 首槽常数（4.1 -0x4
    //      / 4.4 -0xc）⇒ N115+N126+N18a 封顶，勿再试。
    // prett/返回类型轴已核并修正（N5C_EventChar 2026-09-11）：ORIG 串
    // `_ZZN21CCommunityServerProxy7InitTcpEvE19__PRETTY_FUNCTION__` @0x08c662e0 =
    // "bool CCommunityServerProxy::InitTcp()"（P174 串定返回类型）⇒ 声明/定义已由 int 改
    // bool（返回类型不入 mangled，零 ABI 风险；调用点 Init 转发与 GlobalData 重连均丢弃返回值）。
    // 修正后 strall 串多重集差异 1→0（仅余下方 SendTcpPacket 一处真串错）。
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
}