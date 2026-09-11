// ORIG 0x081af1b8 / 0x081af210：size<=0→false；
// setsockopt(SOL_SOCKET, SO_SNDBUF/SO_RCVBUF, &size, 4)，负→false。
// 结果为具名 ret 槽（0x081af250 mov %eax,-0xc; cmpl; jns），frame 0x38。
bool UDPSocket::setOptResizeSendBuf(int size)
