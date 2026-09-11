bool DbmwClient::RegistToMiddleware()
{
    static const char REGIST_GO = 0;  // ORIG _ZZN10DbmwClient18RegistToMiddlewareEvE9REGIST_GO
    int zero = REGIST_GO;             // ORIG 0x81206cc：movl $0x0,-0xc 槽中转（bodyLen 侧）
    if (!SendHeader(REGIST_GO, zero))
        return false;
    return true;
}