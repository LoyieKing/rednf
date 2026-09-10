void _NS_PI_2ND_RecvHash_CS(char* packet, CUser* user, unsigned int recvSize)
{
    // 非 const：ORIG make_pair 实例为 <uchar&, ushort&>（`IRhRtE`，P61；t=unsigned short）。
    unsigned char cls = static_cast<unsigned char>(packet[0]);
    unsigned short index =
        static_cast<unsigned short>(_NS_PI_2ND_GetPacketIndex(packet));

    std::set<std::pair<unsigned char, unsigned short> >* seen =
        static_cast<std::set<std::pair<unsigned char, unsigned short> >*>(
            user->GetCSHashSet());

    // ORIG 只构造一次 make_pair（-0x44），find 与 insert 复用同一 pair（-0x44）。
    std::pair<unsigned char, unsigned short> entry = std::make_pair(cls, index);
    const bool existing = (seen->find(entry) != seen->end());

    const unsigned int hashDword =
        *reinterpret_cast<const unsigned int*>(packet + 7);

    // ORIG 0x0827501f `mov $1,%eax; jmp; mov $0,%eax` 短路物化。
    const bool isSpecial = (cls == 1 && index == 1);

    const unsigned int hackPackId = hashDword >> 8;
    unsigned char* body = reinterpret_cast<unsigned char*>(packet + 13);
    const unsigned int bodyLen = recvSize - 13;

    // b28d-solo 真 bug 修正①：ORIG 0x0827505c 传 key=hashDword>>8（hackPackId），
    // 原写 hashDword（实参取错）。
    const bool check =
        static_cast<PIReceiverManager*>(user->GetPIReceiverManager())
            ->CheckHashReceived(body, bodyLen, hackPackId, isSpecial);
    // b28d-solo 真 bug 修正②：ORIG 0x08275080/0827508b 双 `xor $1; test; je`
    // 同落 false 块 ⇒ record = !check && !existing（原写 `!check && existing` 极性反）。
    // ORIG 0x08275092 `mov $1,%eax; jmp; mov $0,%eax; mov %al,-0xd` ⇒ 赋值物化。
    const bool record = (!check && !existing);
    if (record != 0)
    {
        // b28d-solo 真 bug 修正③：ORIG 0x082750c1 `movl $0x0,(%esp)` ⇒ 第一实参为常量 0
        //（原写 hackPackId 取错数据源）。
        const unsigned int hackType = _NS_PI_2ND_GetHackType(
            0, cls, static_cast<unsigned short>(index));
        // P0：ORIG 无 analyzer 局部槽（getHackAnalyzer 结果直传 this）。
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1, 0, 0);
        user->lock();
        seen->insert(entry);
        user->unlock();
    }
