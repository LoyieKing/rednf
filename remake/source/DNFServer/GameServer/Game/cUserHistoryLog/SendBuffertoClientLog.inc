// ORIG _ZN15cUserHistoryLog21SendBuffertoClientLogEishb @0x08686e3a
void cUserHistoryLog::SendBuffertoClientLog(int kind, short a, unsigned char b, bool c)
{
    // b28d-50：ORIG 0x08686e3a 在右起 varargs（c→%esi, b→%ebx, a→%ecx）之后才求
    // %s 三元并直留 %eax（无槽，push esi/ebx + sub 0x30）；具名 `name` 局部会提前
    // 物化到 -0xc 槽并改帧（0x44）。内联三元即复现。
    m_user->LogHistory("SendBuffertoClient [%s],%d,%d,%d",
                       (kind == 0) ? "ANTIBOT" : "DPROTO",
                       (int)a, (unsigned)b, (unsigned)c);
}