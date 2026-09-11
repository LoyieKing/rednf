// ORIG T 0x088dc008：loadRDARScriptFile("","Etc/channel_info.etc") 门禁 +
// LogManager 报错 + 转发 applyChannelScript 返回值。
// [P248 档①] 返回 bool：CDataManager::init 调用点 ORIG 形
// `call; xor $1,%eax; test %al,%al; je`（报告 idx 3963-3966）；bool 化后尾调
// 透传形态与 ORIG 0x088dc062 `call applyChannelScript; leave; ret` 逐条一致。
bool importChannelScript(channel_script_t* script)
{
    if (!loadRDARScriptFile("", "Etc/channel_info.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x38,
                              "importChannelScript - can't find channel_info.etc");
        return 0;
    }
    return applyChannelScript(script);   // ORIG 直返 raw eax（P9）
}
