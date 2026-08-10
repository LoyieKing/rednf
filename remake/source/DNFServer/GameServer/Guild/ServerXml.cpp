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

namespace np_server_xml
{
CServerXml::CServerXml()
{
    memset(m_data, 0, sizeof(m_data));
    new (m_doc) TiXmlDocument;
    m_field50 = 0;
    m_path = std::string();
    InitString();
}

CServerXml::~CServerXml()
{
    InitString();
    m_rgba.~map();
    m_str3.~map();
    m_str2.~map();
    m_str1.~map();
    m_path.~basic_string();
    ((TiXmlDocument*)m_doc)->~TiXmlDocument();
}

void CServerXml::InitString()
{
    m_field50 = 0;
    memset(m_data, 0, 5);
    m_str1.clear();
    m_str2.clear();
    m_str3.clear();
    m_rgba.clear();
}

void CServerXml::StrLoading()
{
    StrLoading(std::string("server_str.xml"));
}

void CServerXml::StrLoading(std::string path)
{
    InitString();
    m_path = path;
    TiXmlDocument* doc = (TiXmlDocument*)m_doc;
    doc->Clear();
    if (doc->LoadFile(m_path.c_str(), TIXML_ENCODING_UNKNOWN))
    {
        TiXmlNode* xml = doc->FirstChild("xml");
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
    else
    {
        printf("[CServerXml] Load Fail File : %s\n", "server_str.xml");
    }
}

void CServerXml::CharsetInit(TiXmlNode* node)
{
    TiXmlNode* opt = node->FirstChild("option");
    if (opt == 0)
    {
        puts("[CServerXml] <option> Tag Error");
        exit(-1);
    }
    TiXmlNode* charset = opt->FirstChild("charset");
    if (charset == 0)
    {
        puts("[CServerXml] <charset> Tag Error");
        exit(-1);
    }
    TiXmlElement* e = charset->ToElement();
    const char* type = e->Attribute("type");
    if (type == 0)
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    if (strcmp(type, "kor") == 0)
    {
        *(unsigned int*)((char*)this + 0x50) = 0;
    }
    else if (strcmp(type, "chn") == 0)
    {
        *(unsigned int*)((char*)this + 0x50) = 1;
    }
    else if (strcmp(type, "jpn") == 0)
    {
        *(unsigned int*)((char*)this + 0x50) = 2;
    }
    else if (strcmp(type, "usa") == 0)
    {
        *(unsigned int*)((char*)this + 0x50) = 3;
    }
    else if (strcmp(type, "twn") == 0)
    {
        *(unsigned int*)((char*)this + 0x50) = 4;
    }
    else
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    strcpy((char*)this, type);
}

void CServerXml::EventLoad(TiXmlNode* node)
{
    TiXmlNode* ev = node->FirstChild("event_str");
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
        TiXmlElement* e = ev->ToElement();
        e->Attribute("id", &id);
        RGBALoad(id, ev);
        TiXmlNode* text = ev->FirstChild((const char*)this);
        if (text == 0)
        {
            printf("%s Tag Error\n", (char*)this);
            exit(-1);
        }
        TiXmlElement* e2 = text->ToElement();
        StrPunish(id, e2->Attribute("start_msg"), (_eStringType)1);
        e2 = text->ToElement();
        StrPunish(id, e2->Attribute("end_msg"), (_eStringType)2);
    }
}

void CServerXml::RGBALoad(int idx, TiXmlNode* node)
{
    TiXmlNode* color = node->FirstChild("color");
    if (color == 0)
    {
        printf("%s Tag Error\n", (char*)this);
        exit(-1);
    }
    unsigned int rgba = 0;
    TiXmlElement* e = color->ToElement();
    int r = atoi(e->Attribute("red"));
    rgba = (rgba & 0xffffff00U) | ((unsigned int)(unsigned char)r);
    e = color->ToElement();
    int g = atoi(e->Attribute("green"));
    rgba = (rgba & 0xffff00ffU) | (((unsigned int)(unsigned char)g) << 8);
    e = color->ToElement();
    int b = atoi(e->Attribute("blue"));
    rgba = (rgba & 0xff00ffffU) | (((unsigned int)(unsigned char)b) << 16);
    e = color->ToElement();
    int a = atoi(e->Attribute("alpha"));
    rgba = (rgba & 0x00ffffffU) | (((unsigned int)(unsigned char)a) << 24);
    m_rgba.insert(std::make_pair(idx, (int)rgba));
}

void CServerXml::ProcessLoad(TiXmlNode* node)
{
    TiXmlNode* str = node->FirstChild("str");
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
        TiXmlElement* e = str->ToElement();
        e->Attribute("id", &id);
        TiXmlNode* text = str->FirstChild((const char*)this);
        if (text == 0)
        {
            printf("%s Tag Error\n", (char*)this);
            return;
        }
        TiXmlElement* e2 = text->ToElement();
        StrPunish(id, e2->Attribute("text"), (_eStringType)0);
    }
}

void CServerXml::StrPunish(int idx, const char* str, _eStringType type)
{
    if (str != 0)
    {
        std::string s = str;
        if ((int)type == 1)
        {
            m_str2.insert(std::make_pair(idx, s));
        }
        else if ((int)type == 2)
        {
            m_str3.insert(std::make_pair(idx, s));
        }
        else if ((int)type == 0)
        {
            m_str1.insert(std::make_pair(idx, s));
        }
    }
}

std::string CServerXml::GetServerString(int idx, bool* ok) const
{
    std::string ret("");
    std::map<int, std::string>::const_iterator it = m_str1.find(idx);
    if (it != m_str1.end())
    {
        if (ok != 0)
        {
            *ok = 1;
        }
        return it->second;
    }
    if (ok != 0)
    {
        *ok = 0;
    }
    return ret;
}

unsigned int CServerXml::GetEventRGBA(int idx) const
{
    std::map<int, int>::const_iterator it = m_rgba.find(idx);
    if (it == m_rgba.end())
    {
        return 0;
    }
    return (unsigned int)it->second;
}

std::string CServerXml::GetEventString(int idx, _eStringType type, bool* ok) const
{
    const std::map<int, std::string>* m = &m_str1;
    if ((int)type == 1)
    {
        m = &m_str2;
    }
    else if ((int)type == 2)
    {
        m = &m_str3;
    }
    std::map<int, std::string>::const_iterator it = m->find(idx);
    if (it == m->end())
    {
        if (ok != 0)
        {
            *ok = 0;
        }
        return "";
    }
    if (ok != 0)
    {
        *ok = 1;
    }
    return it->second;
}
}

np_server_xml::CServerXml g_ServerString_;

