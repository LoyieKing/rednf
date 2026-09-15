// df_guild_r — ServerXml (split from source/guild per ORIG layout)
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "ServerXml.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

namespace np_server_xml {
static const char FILE_NAME[] = "server_str.xml";
}

namespace np_server_xml
{
CServerXml::CServerXml()
{
    InitString();
}

CServerXml::~CServerXml()
{
    InitString();
}

void CServerXml::InitString()
{
    m_state = 0;
    memset(m_type, 0, 5);
    m_str1.clear();
    m_str2.clear();
    m_str3.clear();
    m_rgba.clear();
}

void CServerXml::StrLoading()
{
    StrLoading(std::string(np_server_xml::FILE_NAME));
}

void CServerXml::StrLoading(std::string path)
{
    InitString();
    m_path = path;
    m_doc.Clear();
    if (!m_doc.LoadFile(m_path, TIXML_ENCODING_UNKNOWN))
    {
        printf("[CServerXml] Load Fail File : %s\n", "server_str.xml");
    }
    else
    {
        TiXmlNode* xml = m_doc.FirstChild("xml");
        if (xml == 0)
        {
            puts("[CServerXml] <xml> Tag Error");
        }
        else
        {
            CharsetInit(xml);
            EventLoad(xml);
            ProcessLoad(xml);
        }
    }
}

void CServerXml::CharsetInit(TiXmlNode* node)
{
    const char* type = 0;
    TiXmlNode* opt = 0;
    opt = node->FirstChild("option");
    if (opt == 0)
    {
        puts("[CServerXml] <option> Tag Error");
        exit(-1);
    }
    opt = opt->FirstChild("charset");
    if (opt == 0)
    {
        puts("[CServerXml] <charset> Tag Error");
        exit(-1);
    }
    type = opt->ToElement()->Attribute("type");
    if (type == 0)
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    if (strcmp(type, "kor") == 0)
    {
        m_state = 0;
    }
    else if (strcmp(type, "chn") == 0)
    {
        m_state = 1;
    }
    else if (strcmp(type, "jpn") == 0)
    {
        m_state = 2;
    }
    else if (strcmp(type, "usa") == 0)
    {
        m_state = 3;
    }
    else if (strcmp(type, "twn") == 0)
    {
        m_state = 4;
    }
    else
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    strcpy(m_type, type);
}

void CServerXml::EventLoad(TiXmlNode* node)
{
    TiXmlNode* ev = 0;
    ev = node->FirstChild("event_str");
    if (ev == 0)
    {
        puts("[CServerXml] <event_str> Tag Skip!!");
        return;
    }
    ev = ev->FirstChild("event");
    if (ev == 0)
    {
        puts("[CServerXml] <event> Tag Error");
        exit(-1);
    }
    for (; ev != 0; ev = ev->NextSibling())
    {
        int id = 0;
        TiXmlElement* e = 0;
        const char* msg = 0;
        TiXmlNode* text = 0;
        ev->ToElement()->Attribute("id", &id);
        RGBALoad(id, ev);
        text = ev->FirstChild((const char*)this);
        if (text == 0)
        {
            printf("%s Tag Error\n", (char*)this);
            exit(-1);
        }
        msg = text->ToElement()->Attribute("start_msg");
        StrPunish(id, msg, (_eStringType)1);
        msg = text->ToElement()->Attribute("end_msg");
        StrPunish(id, msg, (_eStringType)2);
    }
}

void CServerXml::RGBALoad(int idx, TiXmlNode* node)
{
    TiXmlNode* color = 0;
    color = node->FirstChild("color");
    if (color == 0)
    {
        printf("%s Tag Error\n", (char*)this);
        exit(-1);
    }
    union
    {
        int color;
        char c[4];
    } rgba;
    rgba.color = 0;
    rgba.c[0] = (char)atoi(color->ToElement()->Attribute("red"));
    rgba.c[1] = (char)atoi(color->ToElement()->Attribute("green"));
    rgba.c[2] = (char)atoi(color->ToElement()->Attribute("blue"));
    rgba.c[3] = (char)atoi(color->ToElement()->Attribute("alpha"));
    m_rgba.insert(std::pair<int, int>(idx, rgba.color));
}

void CServerXml::ProcessLoad(TiXmlNode* node)
{
    TiXmlNode* str = 0;
    str = node->FirstChild("str");
    if (str == 0)
    {
        puts("[CServerXml] <str> Tag Skip!!");
        return;
    }
    str = str->FirstChild("string");
    if (str == 0)
    {
        puts("[CServerXml] <string> Tag Error");
        exit(-1);
    }
    for (; str != 0; str = str->NextSibling())
    {
        int id = 0;
        TiXmlElement* e = 0;
        const char* msg = 0;
        TiXmlNode* text = 0;
        str->ToElement()->Attribute("id", &id);
        text = str->FirstChild((const char*)this);
        if (text == 0)
        {
            printf("%s Tag Error\n", (char*)this);
            return;
        }
        msg = text->ToElement()->Attribute("text");
        StrPunish(id, msg, (_eStringType)0);
    }
}

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-GLD-DIFF-0019 | guild | 与ORIG差异=DIFF | CServerXml::StrPunish | 详见 function_reports/guild/_ZN13np_server_xml10CServerXml9StrPunishEiPKcNS_12_eStringTypeE.md
void CServerXml::StrPunish(int idx, const char* str, _eStringType type)
{
    if (str != 0)
    {
        std::string s = str;
        switch (type)
        {
        case 0:
            m_str1.insert(std::pair<int, std::string>(idx, s));
            break;
        case 1:
            m_str2.insert(std::pair<int, std::string>(idx, s));
            break;
        case 2:
            m_str3.insert(std::pair<int, std::string>(idx, s));
            break;
        default:
            break;
        }
    }
}

std::string CServerXml::GetServerString(int idx, bool* ok) const
{
    std::string ret("");
    std::map<int, std::string>::const_iterator it;
    it = m_str1.find(idx);
    if (it != m_str1.end())
    {
        if (ok != 0)
        {
            *ok = 1;
        }
    }
    else
    {
        if (ok != 0)
        {
            *ok = 0;
        }
        return ret;
    }
    return it->second;
}

unsigned int CServerXml::GetEventRGBA(int idx) const
{
    std::map<int, int>::const_iterator it;
    it = m_rgba.find(idx);
    if (it != m_rgba.end())
    {
        return (unsigned int)it->second;
    }
    return 0;
}

std::string CServerXml::GetEventString(int idx, _eStringType type, bool* ok) const
{
    std::string ret("");
    std::map<int, std::string>::const_iterator it;
    switch (type)
    {
    case 1:
        it = m_str2.find(idx);
        if (it != m_str2.end())
        {
            if (ok != 0)
            {
                *ok = 1;
            }
        }
        else
        {
            if (ok != 0)
            {
                *ok = 0;
            }
            return ret;
        }
        return it->second;
    case 2:
        it = m_str3.find(idx);
        if (it != m_str3.end())
        {
            if (ok != 0)
            {
                *ok = 1;
            }
        }
        else
        {
            if (ok != 0)
            {
                *ok = 0;
            }
            return ret;
        }
        return it->second;
    default:
        return ret;
    }
}
}

np_server_xml::CServerXml g_ServerString_;
