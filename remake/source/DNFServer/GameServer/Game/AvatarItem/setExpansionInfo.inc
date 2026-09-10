void Avatar_Item::setExpansionInfo(const ::stAvatarExpansionInfo_t& info)
{
    // ORIG 0x82fa326：4 字节整读 info（m_color[0..1] 连续）整写 this+0x3f
    //（覆盖 m_color1/+0x3f 与 m_color2/+0x41）。4.4-O0 对两次 ushort 赋值
    // 恒拆 movzwl+mov %dx（14 条），整型写复现 ORIG 单 mov %edx,0x3f(%eax)（8 条）。
    // 复现 ORIG 单 `mov %edx,0x3f(%eax)`（无 add 折合）：P82 pack(1) 尾部视图
    // 具名成员访问保留 disp 寻址（探针实证；cast 指针 store 会折 add $0x3f）。
    struct __attribute__((packed)) V { char m_pad[0x3f]; unsigned int m_dw; };
    ((V*)this)->m_dw = *(const unsigned int*)&info.m_color[0];
