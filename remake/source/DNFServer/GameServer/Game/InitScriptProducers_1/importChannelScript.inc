bool importChannelScript(channel_script_t* script)
{
    if (!loadRDARScriptFile("", "Etc/channel_info.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x38,
                              "importChannelScript - can't find channel_info.etc");
        return 0;
    }
    return applyChannelScript(script);   // ORIG 直返 raw eax（P9）
