// b28d-31: N 封顶（N82 巨件）——ORIG 0x082906be；2232 vs 2228（§6.2 三件套
// 口径：gap≤4、槽位集对齐，残余帧/槽分配）。
bool CEnvironment::parse(char* line)
{
    char* key;
    char* value;
    const char* sep = "=";
    int id;

    key = strtok(line, sep);
    value = strtok(NULL, sep);
    trim(key);
    trim(value);

    do
    {
loop_top:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited;

        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it = __lookup.find(key);
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            id = 0;
        }

        switch (id)
        {
        case 0:
        case 0x3fb:   // gc_no
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("gc_no", 0x3fb));
            else
            {
                m_serverEnvir.m_gcNo = atoi(value);
                break;
            }
        case 0x40c:   // server_type
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("server_type", 0x40c));
            else
            {
                m_serverEnvir.m_serverType = atoi(value);
                break;
            }
        case 0x417:   // small_pool_size
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("small_pool_size", 0x417));
            else
            {
                m_serverEnvir.m_smallPoolSize = atoi(value);
                break;
            }
        case 0x41b:   // default_pool_size
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("default_pool_size", 0x41b));
            else
            {
                m_serverEnvir.m_defaultPoolSize = atoi(value);
                break;
            }
        case 0x41f:   // big_pool_size
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("big_pool_size", 0x41f));
            else
            {
                m_serverEnvir.m_bigPoolSize = atoi(value);
                break;
            }
        case 0x42b:   // process_sequence
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("process_sequence", 0x42b));
            else
            {
                m_serverEnvir.m_processSequence = atoi(value);
                break;
            }
        case 0x42f:   // channel_no
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("channel_no", 0x42f));
            else
            {
                m_serverEnvir.m_channelNo = atoi(value);
                break;
            }
        case 0x432:   // channel_name
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("channel_name", 0x432));
            else
            {
                set_channel_name(value);
                break;
            }
        case 0x435:   // ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("ip", 0x435));
            else
            {
                strncpy(m_serverEnvir.m_serverIP, value, 0x10);
                m_serverEnvir.m_serverAddr = inet_addr(m_serverEnvir.m_serverIP);
                break;
            }
        case 0x43b:   // tcp_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("tcp_port", 0x43b));
            else
            {
                m_serverEnvir.m_tcpPort = atoi(value);
                break;
            }
        case 0x43e:   // udp_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_port", 0x43e));
            else
            {
                m_serverEnvir.m_udpPort = atoi(value);
                break;
            }
        case 0x442:   // udp_ip_of_monitor
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_ip_of_monitor", 0x442));
            else
            {
                strncpy(m_serverEnvir.m_udpIPMonitor, value, 0x10);
                break;
            }
        case 0x445:   // udp_port_of_monitor
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_port_of_monitor", 0x445));
            else
            {
                m_serverEnvir.m_udpPortMonitor = atoi(value);
                break;
            }
        case 0x448:   // tcp_port_of_monitor
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("tcp_port_of_monitor", 0x448));
            else
            {
                m_serverEnvir.m_tcpPortMonitor = atoi(value);
                break;
            }
        case 0x44c:   // udp_ip_of_hades
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_ip_of_hades", 0x44c));
            else
            {
                strncpy(m_serverEnvir.m_udpIPHades, value, 0x10);
                break;
            }
        case 0x44f:   // udp_port_of_hades
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_port_of_hades", 0x44f));
            else
            {
                m_serverEnvir.m_udpPortHades = atoi(value);
                break;
            }
        case 0x452:   // tcp_port_of_hades
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("tcp_port_of_hades", 0x452));
            else
            {
                m_serverEnvir.m_tcpPortHades = atoi(value);
                break;
            }
        case 0x456:   // tcp_port_of_guild
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("tcp_port_of_guild", 0x456));
            else
            {
                m_serverEnvir.m_tcpPortGuild = atoi(value);
                break;
            }
        case 0x459:   // udp_ip_of_guild
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_ip_of_guild", 0x459));
            else
            {
                strncpy(m_serverEnvir.m_udpIPGuild, value, 0x10);
                break;
            }
        case 0x45c:   // udp_port_of_guild
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_port_of_guild", 0x45c));
            else
            {
                m_serverEnvir.m_udpPortGuild = atoi(value);
                break;
            }
        case 0x46a:   // udp_ip_of_statistic
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_ip_of_statistic", 0x46a));
            else
            {
                strncpy(m_serverEnvir.m_udpIPStatistic, value, 0x10);
                break;
            }
        case 0x46d:   // udp_port_of_statistic
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_port_of_statistic", 0x46d));
            else
            {
                m_serverEnvir.m_udpPortStatistic = atoi(value);
                break;
            }
        case 0x472:   // udp_ip_of_doublecheck
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_ip_of_doublecheck", 0x472));
            else
            {
                strncpy(m_serverEnvir.m_udpIPDoubleCheck, value, 0x10);
                break;
            }
        case 0x475:   // udp_port_of_doublecheck
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_port_of_doublecheck", 0x475));
            else
            {
                m_serverEnvir.m_udpPortDoubleCheck = atoi(value);
                break;
            }
        case 0x47c:   // udp_ip_of_channel
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_ip_of_channel", 0x47c));
            else
            {
                strncpy(m_serverEnvir.m_udpIPChannel, value, 0x10);
                break;
            }
        case 0x47f:   // udp_port_of_channel
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("udp_port_of_channel", 0x47f));
            else
            {
                m_serverEnvir.m_udpPortChannel = atoi(value);
                break;
            }
        case 0x482:   // tcp_port_of_channel
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("tcp_port_of_channel", 0x482));
            else
            {
                m_serverEnvir.m_udpPortChannel = atoi(value);
                break;
            }
        case 0x490:   // exchange_server_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("exchange_server_ip", 0x490));
            else
            {
                strncpy(m_serverEnvir.m_exchangeServerIP, value, 0x10);
                break;
            }
        case 0x493:   // exchange_server_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("exchange_server_port", 0x493));
            else
            {
                m_serverEnvir.m_exchangeServerPort = atoi(value);
                break;
            }
        case 0x496:   // accept_lock_dbqueue_count
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("accept_lock_dbqueue_count", 0x496));
            else
            {
                m_serverEnvir.m_acceptLockDBQueueCount = atoi(value);
                printf("\naccept_lock_dbqueue_count: %d\n", m_serverEnvir.m_acceptLockDBQueueCount);
                break;
            }
        case 0x49a:   // accept_unlock_dbqueue_count
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("accept_unlock_dbqueue_count", 0x49a));
            else
            {
                m_serverEnvir.m_acceptUnlockDBQueueCount = atoi(value);
                printf("accept_unlock_dbqueue_count: %d\n", m_serverEnvir.m_acceptUnlockDBQueueCount);
                break;
            }
        case 0x4a5:   // lls_keys
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("lls_keys", 0x4a5));
            else
            {
                strncpy(m_serverEnvir.m_llsKey, value, 0x2d);
                printf("m_Server_info.lls_key,: %s\n", m_serverEnvir.m_llsKey);
                break;
            }
        case 0x4ae:   // relay_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("relay_ip", 0x4ae));
            else
            {
                strncpy(m_serverEnvir.m_relayIP, value, 0x10);
                break;
            }
        case 0x4b1:   // relay_tcp_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("relay_tcp_port", 0x4b1));
            else
            {
                m_serverEnvir.m_relayTCPPort = atoi(value);
                break;
            }
        case 0x4b4:   // relay_udp_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("relay_udp_port", 0x4b4));
            else
            {
                m_serverEnvir.m_relayUDPPort = atoi(value);
                break;
            }
        case 0x4b9:   // stun_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("stun_ip", 0x4b9));
            else
            {
                SK_IP stunIP;
                memset(&stunIP, 0, 0x10);
                strncpy(stunIP.m_ip, value, 0x10);
                m_serverEnvir.m_stunIP.push_back(stunIP);
                break;
            }
        case 0x4c1:   // stun_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("stun_port", 0x4c1));
            else
            {
                int stunPort;
                stunPort = atoi(value);
                m_serverEnvir.m_stunPort.push_back(stunPort);
                break;
            }
        case 0x4ca:   // ipg_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("ipg_ip", 0x4ca));
            else
            {
                strncpy(m_serverEnvir.m_ipgIP, value, 0x10);
                break;
            }
        case 0x4d8:   // master_db_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("master_db_ip", 0x4d8));
            else
            {
                strncpy(m_dbConnInfo[DB_HANDLE_0].m_host, value, 0x10);
                break;
            }
        case 0x4dc:   // master_db_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("master_db_port", 0x4dc));
            else
            {
                m_dbConnInfo[DB_HANDLE_0].m_port = atoi(value);
                break;
            }
        case 0x4e0:   // master_db_acc
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("master_db_acc", 0x4e0));
            else
            {
                strncpy(m_dbConnInfo[DB_HANDLE_0].m_user, value, 0x14);
                break;
            }
        case 0x4e3:   // master_db_pwd
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("master_db_pwd", 0x4e3));
            else
            {
                char buf1[0x40] = {0};
                unsigned char buf2[0x40] = {0};
                if (!DNFFLib::Hex2Binary(value, buf2, 0x18))
                    return 0;
                m_cipher.Decrypt((const char*)buf2, buf1, 0x18);
                strncpy(m_dbConnInfo[DB_HANDLE_0].m_pass, buf1, 0x14);
                break;
            }
        case 0x4ed:   // master_db_name
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("master_db_name", 0x4ed));
            else
            {
                strncpy(m_dbConnInfo[DB_HANDLE_0].m_db, value, 0x1e);
                break;
            }
        case 0x74d:   // db_tbl_file
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("db_tbl_file", 0x74d));
            else
            {
                strncpy(m_dbTblFile, value, 0x100);
                break;
            }
        case 0x75b:   // server_group
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("server_group", 0x75b));
            else
            {
                m_serverEnvir.m_serverGroup = atoi(value);
                break;
            }
        case 0x75e:   // fatigue_time
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("fatigue_time", 0x75e));
            else
            {
                m_serverEnvir.m_fatigueTime = atoi(value);
                break;
            }
        case 0x761:   // avatar_time
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("avatar_time", 0x761));
            else
            {
                m_serverEnvir.m_avatarTime = atoi(value);
                break;
            }
        case 0x764:   // max_user_num
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("max_user_num", 0x764));
            else
            {
                m_serverEnvir.m_maxUserNum = atoi(value);
                break;
            }
        case 0x77e:   // school_server_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("school_server_ip", 0x77e));
            else
            {
                strncpy(m_serverEnvir.m_schoolServerIP, value, 0x10);
                break;
            }
        case 0x781:   // school_server_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("school_server_port", 0x781));
            else
            {
                m_serverEnvir.m_schoolServerPort = atoi(value);
                break;
            }
        case 0x786:   // pcroom_server_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("pcroom_server_ip", 0x786));
            else
            {
                strncpy(m_serverEnvir.m_pcroomServerIP, value, 0x10);
                break;
            }
        case 0x789:   // pcroom_server_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("pcroom_server_port", 0x789));
            else
            {
                m_serverEnvir.m_pcroomServerPort = atoi(value);
                break;
            }
        case 0x78e:   // community_server_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("community_server_ip", 0x78e));
            else
            {
                strncpy(m_serverEnvir.m_communityServerIP, value, 0x10);
                break;
            }
        case 0x791:   // community_server_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("community_server_port", 0x791));
            else
            {
                m_serverEnvir.m_communityServerPort = atoi(value);
                break;
            }
        case 0x7b8:   // auction_server_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("auction_server_ip", 0x7b8));
            else
            {
                strncpy(m_serverEnvir.m_auctionServerIP, value, 0x10);
                break;
            }
        case 0x7bb:   // auction_server_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("auction_server_port", 0x7bb));
            else
            {
                m_serverEnvir.m_auctionServerPort = atoi(value);
                break;
            }
        case 0x7c0:   // cera_auction_server_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("cera_auction_server_ip", 0x7c0));
            else
            {
                strncpy(m_serverEnvir.m_ceraAuctionServerIP, value, 0x10);
                break;
            }
        case 0x7c3:   // cera_auction_server_port
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("cera_auction_server_port", 0x7c3));
            else
            {
                m_serverEnvir.m_ceraAuctionServerPort = atoi(value);
                break;
            }
        case 0x7d0:   // system_monitoring_ip
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("system_monitoring_ip", 2000));
            else
            {
                memset(m_serverEnvir.m_systemMonitoringIP, 0, 0x10);
                strncpy(m_serverEnvir.m_systemMonitoringIP, value, 0x10);
                break;
            }
        case 0x7d6:   // script_dir
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("script_dir", 0x7d6));
            else
            {
                char buf[0x100] = {0};
                strncpy(buf, value, 0x100);
                m_serverEnvir.m_baseScriptDirectory = buf;
                break;
            }
        case 0x7de:   // priority_script_dir
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("priority_script_dir", 0x7de));
            else
            {
                char buf[0x100] = {0};
                strncpy(buf, value, 0x100);
                size_t len = strlen(buf);
                if (buf[len - 1] != '\\' && buf[len - 1] != '/')
                    strcat(buf, "/");
                m_serverEnvir.m_priorityScriptDir.push_back(std::string(buf));
                break;
            }
        case 0x7f1:   // scriptpacks
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("scriptpacks", 0x7f1));
            else
            {
                if (IsEnableValue(value))
                    m_serverEnvir.m_scriptpacks = 1;
                else
                    m_serverEnvir.m_scriptpacks = 0;
                break;
            }
        case 0x7f7:   // channel_type
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("channel_type", 0x7f7));
            else
            {
                m_serverEnvir.m_channelType = atoi(value);
                break;
            }
        case 0x7fd:   // validate_script
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("validate_script", 0x7fd));
            else
            {
                if (IsEnableValue(value))
                    m_serverEnvir.m_validateScript = 1;
                else
                    m_serverEnvir.m_validateScript = 0;
                break;
            }
        case 0x803:   // garena_publickey
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("garena_publickey", 0x803));
            else
            {
                char buf[0x80] = {0};
                strncpy(buf, value, 0x80);
                m_serverEnvir.m_garenaPublickey = buf;
                break;
            }
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto loop_top;
        }
        else
        {
            parse_se_event_db(key, value);
            return 1;
        }
    } while (true);
}