// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0820b322 orig/ours=122/116 ndiff-blocks=23
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: push   %edi | push   %esi | sub    $0xb5c,%esp
int Dispatcher_Antibot_DP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820b322：读嵌套包长度→checker vtbl+0x2c 解密→doDispatch 内层包。
    // ORIG 0x0820b32e `lea -0x3c(%ebp),%eax` 前**无**清零 store（P328⑤ 去预清）；
    // get_int 成功路径必已赋值。
    unsigned int binarySize;
    if (!packet.get_int(binarySize))
    {
        return LineFunc(0xa795, __PRETTY_FUNCTION__, 0, 0);
    }
    char* nestedPacket = packet.get_buf_ptr(packet.get_index());
    int local_28 = 0xd;  // ORIG 局部（写后未用，保持帧形态）
    (void)local_28;
    if (binarySize > 0x567)
    {
        return LineFunc(0xa7a3, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 段直证：`sub $0xb5c,%esp` + `lea -0xb2e(%ebp)`（outBuf 基址）⇒ 解密输出缓冲
    // 容量 = 0xaf0（与 outLen 初值一致），非 4 字节视图（原 4B 声明令帧 0xb5c→0x64，
    // 且 Decrypt 出参写 0xaf0 上限即越界）。消费侧仍按 +0x00 class / +0x01 short 视图。
    unsigned char outBuf[0xaf0];
    unsigned short outLen = 0xaf0;
    typedef int (*DecryptFn)(void*, unsigned int, void*, unsigned short, unsigned char*, unsigned short*);
    // ORIG 0x0820b3cd-0x0820b3d7 `mov sym,%eax; mov (%eax),%eax; add $0x2c,%eax;
    // mov (%eax),%esi`（fn 直落 callee-saved，**无**具名槽）+ 0x0820b411 `mov sym,%eax`
    // 作 this 实参（P48 每用点重取）⇒ 删 checker(-0x1c)/fn(-0x18) 两具名局部，表达式内联。
    // 2026-09-11 K1DispG1 语义修复：ORIG `mov (%eax),%esi` 是**读 vtbl 槽的值**；
    // 旧码 (`char*)base + 0x2c` 把槽地址当函数指针调用（调错目标）。
    // ORIG 0x0820b3e5 段：`call getAntibotKey; mov %eax,%edx; mov <A>,%eax` —— key 实参
    // 经转换中转入 %edx（P211 档①：形参按 unsigned int 传递）。
    int ret = (*reinterpret_cast<DecryptFn*>(reinterpret_cast<char*>(
                   *reinterpret_cast<void**>(GlobalData::s_antibotChecker_)) +
               0x2c))(GlobalData::s_antibotChecker_, (unsigned int)user->getAntibotKey(), nestedPacket,
                      (unsigned short)(binarySize & 0xffff), outBuf, &outLen);
    if (ret != 0)
    {
        // ORIG 0x0820b41f..0x0820b476：key 在 C1 之前求值（%ebx）跨构造保活（P227
        // 全表达式临时链）；具名 key 槽会改帧。
        cMyTrace(__PRETTY_FUNCTION__, 0xa7af, 0)(
            "[Dispatcher_Antibot_DP] Key : %d, nestedPacket : %x, binarySize : %d, return : %d",
            user->getAntibotKey(), nestedPacket, binarySize, ret);
        return 0;
    }
    // ORIG 0x0820b482..0x0820b4f9：`unsigned short` 具名槽(-0x1a) + outLen int 副本
    // (-0xb3c) + `call G_PacketDispatcher` 结果留 %eax 跨实参装载（无 dispatcher 具名槽）；
    // 尾部 `xor $1; test; je` ⇒ doDispatch 返回值**必须检查**（旧码丢弃 = 语义缺口）。
    unsigned short nestedCmd = *reinterpret_cast<unsigned short*>(outBuf + 1);
    if (!G_PacketDispatcher()->doDispatch(user,
                                          (ENUM_PACKETCLASS)outBuf[0],
                                          nestedCmd,
                                          reinterpret_cast<char*>(outBuf),
                                          static_cast<int>(outLen),
                                          (ENUM_PACKETCLASS)1,
                                          (unsigned short)0x25c))
    {
        return 0;
    }
    return 0;
}