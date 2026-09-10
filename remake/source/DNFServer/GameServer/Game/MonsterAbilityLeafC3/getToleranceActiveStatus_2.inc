int getToleranceActiveStatus(ENUM_ACTIVEPROPERTY prop)
{
    switch (static_cast<int>(prop))
    {
    // case 书写序 = ORIG 叶块排放序（P46）：0xf,0x10,0..0xa,0x11,0xb，
    // 同值 0x12 的 12/13/default 连续书写合并为单叶直落。
    case 14: return 0x0f;
    case 15: return 0x10;
    case 0: return 0x00;
    case 1: return 0x01;
    case 2: return 0x02;
    case 3: return 0x03;
    case 4: return 0x04;
    case 5: return 0x05;
    case 6: return 0x06;
    case 7: return 0x07;
    case 8: return 0x08;
    case 9: return 0x09;
    case 10: return 0x0a;
    case 16: return 0x11;
    case 11: return 0x0b;
    case 12: case 13: default: return 0x12;  // 调用方以 0x12 跳过；合并单叶直落
    }
