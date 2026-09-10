void _NS_PI_Check_CMD(ENUM_PACKETCLASS packetClass, unsigned short cmd,
                      PacketBuf& packet, CUser* user)
{
    if (user == 0)
        return;

    char type = 0;  // -0x29(%ebp)
    // [b28d-solo-GameSecurity] ORIG 先 `mov 0x8(%ebp),%eax; test %eax,%eax; je` 再
    // `cmp $0x1,%eax; jne`（P261 无状态槽 cmp 链）⇒ 原码是 switch，if/else 链在
    // 4.4 恒折 `cmpl $imm,mem` 直比（P209）。
    switch (packetClass)
    {
        case 1:
            type = 1;
            break;
        case 0:
            type = 2;
            break;
    }

    char cmdType = PI_GetType_CS(type, static_cast<short>(cmd));  // -0x2a(%ebp)
    if (cmdType == 0)
        return;

    char* data = packet.get_packet(0) + 0xd;                        // -0x20
    short idx = static_cast<short>(packet.get_index() - 0xd);       // -0x1a

    int expected;  // -0x30（ORIG 无清零，P171 照抄不写）
    packet.get_int(expected);

    std::pair<char, char> key = std::make_pair(type, cmdType);      // -0x32

    std::map<std::pair<char, char>, int>* pics =
        (std::map<std::pair<char, char>, int>*)user->GetPICSMap();  // -0x18

    if (pics->find(key) == pics->end())
        (*pics)[key] = 0;

    int& cnt = (*pics)[key];  // -0x14
    if (cnt > 0x64)
        return;

    int hash;  // -0x38（ORIG 无清零）
    if (!_NS_MakeHash_(data, idx, hash))
        return;

    if (hash == expected)
        return;

    ++cnt;

    // ORIG 0x82748fb：movzbl t → imul $0x67 魔数除 5 取余 → r*10 + 0xc9。
    int penalty = static_cast<int>(type % 10) * 10 + 0xc9;  // -0x10
    WongWork::ENUM_HACKTYPE hackType = static_cast<WongWork::ENUM_HACKTYPE>(
        PI_GetHackType(static_cast<short>(cmdType)));  // -0xc
    PIAddServerHackCnt(user, hackType, penalty);
