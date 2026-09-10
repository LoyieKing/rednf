int Dispatcher_CharacterStatistic::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82051d0：get_state < 3 → 0；依次 get_int 6 个字段（任一失败 →
    // LineFunc(0x9d1f+k, __PRETTY_FUNCTION__, 0, 0) 并返回其返回值），
    // 成功后写 CUser+0x8e03c..0x8e050（6 个 int，Character 统计区），并置
    // CUser+0x8e074 = 1。零外部调用（自包含）。
    if (user->get_state() < 3)
    {
        return 0;
    }

    // ORIG 0x082051d0：6 个槽连续（-0x20..-0xc）且读取点用 `lea -0x20(%ebp)[+k*4]`
    //   ⇒ 原码是数组 `int values[6]`；写回用 `mov %edx,0x8e03c(%eax)` 直 disp store
    //   ⇒ 无 base 具名指针（我方旧形 6 独立变量 + base 视图多 4 槽且产 lea 链）。
    int values[6];
    if (packet.get_int(values[0]) != 1) return LineFunc(0x9d1f, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(values[1]) != 1) return LineFunc(0x9d20, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(values[2]) != 1) return LineFunc(0x9d21, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(values[3]) != 1) return LineFunc(0x9d22, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(values[4]) != 1) return LineFunc(0x9d23, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(values[5]) != 1) return LineFunc(0x9d24, __PRETTY_FUNCTION__, 0, 0);

    *(int*)((char*)user + 0x8e03c) = values[0];
    *(int*)((char*)user + 0x8e044) = values[2];
    *(int*)((char*)user + 0x8e048) = values[3];
    *(int*)((char*)user + 0x8e040) = values[1];
    *(int*)((char*)user + 0x8e04c) = values[4];
    *(int*)((char*)user + 0x8e050) = values[5];
    *(char*)((char*)user + 0x8e074) = 1;
    return 0;
