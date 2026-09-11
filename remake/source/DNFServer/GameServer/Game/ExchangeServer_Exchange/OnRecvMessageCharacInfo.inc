// ---------------------------------------------------------------------------
// OnRecvMessageCharacInfo                  ORIG 0x849341c
// 本机（参数 a==0x20，ORIG 比较 0x10(%ebp)）走缓存回收路径；否则带请求体转发。
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：真 bug `b==0x20`→`a==0x20`（ORIG 比较 0x10(%ebp)）；
// P198① localHit 两句式预清；P0 内联 length/get_buffer 实参。
int CExchangeServer::OnRecvMessageCharacInfo(Stream* stream, int a, int b)
{
    // [b28d-solo-Exchange] P198① 预清必写：ORIG 首条 `movb $0x0,-0x9` 后条件
    // 置位 `movb $0x1,-0x9`（0x8493423/0x8493451），旧码 const bool 初始化式
    // 产 sete+存槽。P0：fwdLen/fwdBuf 无槽往返 ⇒ 实参内联（ORIG 0x84934a5
    // `mov %eax,%ebx` 跨 get_buffer call 保活，0x84934f7 同）。
    bool localHit = false;
    int body0;                                        // -0x10 先读
    int body1;                                        // -0x14 后读
    *stream >> body0;
    *stream >> body1;

    if (a == 0x20)
    {
        localHit = true;
    }

    if (ReturnCacheCharacter(a, b, body0, body1, localHit, NULL))
    {
        return 0;
    }

    if (SendRequestCharacInfo(a, b, body0, body1, stream->get_buffer(),
                              stream->length()))
    {
        return 0;
    }

    ForwardRequestCharac(a, b, stream->get_buffer(), stream->length());
    return 0;
}