// ============================================================================
// importPVPChannelList —— ORIG 0x08901f10
// 报告 _lt_global_gt_/importPVPChannelList.md；调用点 init.md +0x1cce：
//   importPVPChannelList("Etc/pvpChannelList.etc", this+0xb4ec, 0)。
// [PVP CHANNEL INFO] 段：grade=ScanInt、count=ScanInt；每条 ScanStr(名字) +
// pvp_channel_info_t 的 9 个 int（扫描序→偏移见 CDataManager.h 注释）+
// 尾随 ScanStr；完成后把 map[grade]=vector insert 进 live_server_info_t 的
// m_pvpChannelMap。第三形参 flag 在 ORIG 函数体内未使用。
// 任一 channel 字段缺失 → 清理后返回 false。
// ============================================================================
bool importPVPChannelList(char* path, live_server_info_t* info, int flag)
{
    (void)flag;
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    bool ok = false;

    // ORIG 0x08901f10：cond 前置（`if (!ScanType) goto end` ⇒
    // `call; xor $1; test; jne exit`，P292 bool callee）、body 后置、尾部
    // `jmp` 回边。4.4 对 while 恒旋转为「body 前置 + cond 尾部 `test; je`」。
pvp_loop:
    if (!ScanType(token, true))
        goto pvp_end;
    {
        if (token == "[PVP CHANNEL INFO]")
        {
            while (true)
            {
                int grade;
                if (!(grade = ScanInt(&ok), ok))
                    break;
                int count;
                if (!(count = ScanInt(&ok), ok))
                    break;

                std::vector<pvp_channel_info_t> channels;
                std::string name;

                for (int i = 0; i < count; ++i)
                {
                    ScanStr(&name);

                    pvp_channel_info_t channel;
                    channel.m_field0 = ScanInt(&ok);           // 扫描序 1 → +0x00
                    if (!ok)
                        return false;
                    channel.m_minGrade = ScanInt(&ok);          // 扫描序 2 → +0x04
                    if (!ok)
                        return false;
                    channel.m_maxGrade = ScanInt(&ok);         // 扫描序 3 → +0x08
                    if (!ok)
                        return false;
                    channel.m_channelType = ScanInt(&ok);      // 扫描序 4 → +0x10
                    if (!ok)
                        return false;
                    channel.m_upgradeRevision = ScanInt(&ok);  // 扫描序 5 → +0x0c
                    if (!ok)
                        return false;
                    channel.m_minLevel = ScanInt(&ok);         // 扫描序 6 → +0x14
                    if (!ok)
                        return false;
                    channel.m_maxLevel = ScanInt(&ok);        // 扫描序 7 → +0x18
                    if (!ok)
                        return false;
                    channel.m_field1c = ScanInt(&ok);          // 扫描序 8 → +0x1c
                    if (!ok)
                        return false;
                    channel.m_field20 = ScanInt(&ok);          // 扫描序 9 → +0x20
                    if (!ok)
                        return false;

                    channels.push_back(channel);
                    ScanStr(&name);
                }

                info->m_pvpChannelMap.insert(std::make_pair(grade, channels));
            }
        }
    }
    goto pvp_loop;

pvp_end:
    return true;
}