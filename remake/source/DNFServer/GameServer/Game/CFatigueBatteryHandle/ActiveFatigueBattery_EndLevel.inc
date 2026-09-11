// ORIG 0x84a81f4：used=(int)(short)GetFatigueBatteryCharging()；result=used；
// 若二次读取与 result 不等则记 trace "FATIGUE BATTERY ACTIVE ERR ..." 并重取。
// ORIG 另有 call G_CDataManager(); add $0x68,%eax 死存储（结果从不读取），按原样保留。
short CFatigueBatteryHandle::ActiveFatigueBattery_EndLevel(CUser* user)
{
    int result = 0;
    int used = static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging()));

    ServerParameterScript& unusedScript = G_CDataManager()->m_serverParameter;  // ORIG 死存储
    (void)unusedScript;

    result = used;
    // [b28c-L29] ORIG 0x084a81f4：左值经 cwtl 拓宽后与 result 做 32 位 cmp
    // （`call; cwtl; cmp -0x14(%ebp),%eax`），右侧不做 short 截断。
    if (static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging())) !=
        result)
    {
        // [b28c-L29] P227：全表达式临时链令 op() 实参（getCurCharacNo）在 ctor
        // 前求值并以 %ebx 跨 ctor 保活（ORIG `call getCurCharacNo; mov %eax,%ebx`）。
        cMyTrace(__PRETTY_FUNCTION__, 0x77, 0)(
            "FATIGUE BATTERY ACTIVE ERR : char no(%d), usedFatigue(%d)",
            user->getCurCharacNo(), result);
        result = static_cast<int>(
            static_cast<short>(user->GetFatigueBatteryCharging()));
    }
    return static_cast<short>(result);
}