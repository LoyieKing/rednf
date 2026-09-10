void HeroMissionEvent::StartEvent()
{
    // [b28c-L7] P221：ORIG 两互斥臂 cMyTrace 槽 fail=-0x28(深)/succ=-0x18(浅)，
    // 4.4 对**具名** tr 恒反（先声明浅，N93 推广族）。改「构造即调用」全表达式
    // 临时链 ⇒ 4.4 复现 ORIG 槽深（MySQL init_db_handle 同法 AE 实证）。
    if (!loadScript())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1fe, 5)
            ("[HeroMission] Script load fail!!!");
    }
    else
    {
        // ORIG 0x81666a8：loadScript 成功后 SetEventFlag(true)（写 +0x08 标志）
        SetEventFlag(true);
        cMyTrace(__PRETTY_FUNCTION__, 0x203, 0)("[HeroMission] Start!");
    }
