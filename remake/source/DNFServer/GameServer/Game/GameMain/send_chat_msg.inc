// send_chat_msg(char*, int) —— ORIG 0x829f6ff（0xc6 字节，逐指令转写主体）：
// UDP socket → sockaddr{AF_INET, htons(env->get_udp_port()), htonl(0)} → sendto。
// 目标地址 htonl(0)=0.0.0.0 为 ORIG 原样（推断实际由配置决定回环地址）。
int send_chat_msg(char* msg, int len)
{
    // ORIG 0x829f6ff 逐指令转写：UDP socket → sockaddr{AF_INET,
    // htons(get_udp_port()), htonl(0)}（其余字段不初始化，无 memset）；
    // len>0x4f 返回 -1；包 = {0x03, msg[:0x50]} 共 0x52 字节发送；无 close、无 fd 检查。
    char buf[0x52];
    sockaddr_in addr;
    int fd = socket(2, 2, 0);  // AF_INET, SOCK_DGRAM（与 ORIG 立即数一致）
    addr.sin_family = 2;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons(static_cast<unsigned short>(
        G_CEnvironment()->get_udp_port()));
    // ORIG：`movb $0x3,-0x72` 在 len 检查之前（hoist）；len<=0x4f 分支
    // `mov %al,-0x21` → buf[0x51] = (char)len（发送 0x52 字节含尾 len 字节）。
    buf[0] = 0x3;
    if (len > 0x4f)
        return -1;
    buf[0x51] = (char)len;
    strncpy(buf + 1, msg, 0x50);
    int sent = sendto(fd, buf, 0x52, 0,
                      reinterpret_cast<sockaddr*>(&addr), 0x10);
    return sent;
}