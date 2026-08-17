// df_monitor_r — ServerXml（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "ServerXml.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

namespace np_server_xml
{
static const char FILE_NAME[] = "server_str.xml";

CServerXml::CServerXml() { InitString(); }

CServerXml::~CServerXml() { InitString(); }

void CServerXml::StrLoading()
{
    StrLoading(std::string(FILE_NAME));
}

void CServerXml::StrLoading(std::string path)
{
    InitString();
    m_str54 = path;
    m_doc.Clear();
    char ok = (char)m_doc.LoadFile(m_str54, (TiXmlEncoding)0);
    if (ok == 1)
    {
        TiXmlNode* root = m_doc.FirstChild("xml");
        if (root == 0)
        {
            puts("[CServerXml] <xml> Tag Error");
        }
        else
        {
            CharsetInit(root);
            EventLoad(root);
            ProcessLoad(root);
        }
    }
    else
    {
        printf("[CServerXml] Load Fail File : %s\n", "server_str.xml");
    }
    __asm__ __volatile__("nop");
}

void CServerXml::InitString()
{
    m_charset = 0;
    memset((char*)this, 0, 5);
    m_map58.clear();
    m_map70.clear();
    m_map88.clear();
    m_mapa0.clear();
}

void CServerXml::CharsetInit(TiXmlNode* node)
{
    TiXmlNode* option = node->FirstChild("option");
    if (option == 0)
    {
        puts("[CServerXml] <option> Tag Error");
        exit(-1);
    }
    TiXmlNode* charset = option->FirstChild("charset");
    if (charset == 0)
    {
        puts("[CServerXml] <charset> Tag Error");
        exit(-1);
    }
    TiXmlElement* element = charset->ToElement();
    const char* type = element->Attribute("type");
    if (type == 0)
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    if (strcmp(type, "kor") == 0)
    {
        m_charset = 0;
    }
    else if (strcmp(type, "chn") == 0)
    {
        m_charset = 1;
    }
    else if (strcmp(type, "jpn") == 0)
    {
        m_charset = 2;
    }
    else if (strcmp(type, "usa") == 0)
    {
        m_charset = 3;
    }
    else if (strcmp(type, "twn") == 0)
    {
        m_charset = 4;
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
    TiXmlNode* eventStr = node->FirstChild("event_str");
    if (eventStr == 0)
    {
        puts("[CServerXml] <event_str> Tag Skip!!");
        return;
    }
    TiXmlNode* event = eventStr->FirstChild("event");
    if (event == 0)
    {
        puts("[CServerXml] <event> Tag Error");
        exit(-1);
    }
    for (; event != 0; event = event->NextSibling())
    {
        int id = 0;
        TiXmlElement* element = event->ToElement();
        element->Attribute("id", &id);
        RGBALoad(id, event);
        TiXmlNode* sub = event->FirstChild((const char*)this);
        if (sub == 0)
        {
            printf("%s Tag Error\n", (const char*)this);
            exit(-1);
        }
        TiXmlElement* subElement = sub->ToElement();
        const char* startMsg = subElement->Attribute("start_msg");
        StrPunish(id, startMsg, (np_server_xml::_eStringType)1);
        TiXmlElement* subElement2 = sub->ToElement();
        const char* endMsg = subElement2->Attribute("end_msg");
        StrPunish(id, endMsg, (np_server_xml::_eStringType)2);
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0022 | monitor | 与ORIG差异=DIFF | np_server_xml::CServerXml::StrPunish | 详见 function_reports/monitor/_ZN13np_server_xml10CServerXml9StrPunishEiPKcNS_12_eStringTypeE.md
void CServerXml::StrPunish(int idx, const char* str, _eStringType type)
{
    if (str != 0)
    {
        std::string s(str);
        switch (type)
        {
        case STRING_TYPE_0:
            m_map58.insert(std::pair<int, std::string>(idx, s));
            break;
        case STRING_TYPE_1:
            m_map70.insert(std::pair<int, std::string>(idx, s));
            break;
        case STRING_TYPE_2:
            m_map88.insert(std::pair<int, std::string>(idx, s));
            break;
        default:
            break;
        }
    }
}

std::string CServerXml::GetServerString(int idx, bool* ok) const
{
    std::string s("");
    std::map<int, std::string>::const_iterator it;
    it = m_map58.find(idx);
    if (it != m_map58.end())
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
        return s;
    }
    return it->second;
}

unsigned int CServerXml::GetEventRGBA(int idx) const
{
    std::map<int, int>::const_iterator it;
    it = m_mapa0.find(idx);
    if (it != m_mapa0.end())
    {
        return (unsigned int)it->second;
    }
    return 0;
}

std::string CServerXml::GetEventString(int idx, _eStringType type, bool* ok) const
{
    std::string s("");
    std::map<int, std::string>::const_iterator it;
    switch (type)
    {
    case STRING_TYPE_1:
        it = m_map70.find(idx);
        if (it != m_map70.end())
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
            return s;
        }
        return it->second;
    case STRING_TYPE_2:
        it = m_map88.find(idx);
        if (it != m_map88.end())
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
            return s;
        }
        return it->second;
    default:
        return s;
    }
}

void CServerXml::RGBALoad(int idx, TiXmlNode* node)
{
    TiXmlNode* colorNode = node->FirstChild("color");
    if (colorNode == 0)
    {
        printf("%s Tag Error\n", (char*)this);
        exit(-1);
    }
    unsigned int rgba = 0;
    TiXmlElement* e = colorNode->ToElement();
    rgba = (unsigned int)(unsigned char)atoi(e->Attribute("red"));
    e = colorNode->ToElement();
    rgba |= (unsigned int)(unsigned char)atoi(e->Attribute("green")) << 8;
    e = colorNode->ToElement();
    rgba |= (unsigned int)(unsigned char)atoi(e->Attribute("blue")) << 16;
    e = colorNode->ToElement();
    rgba |= (unsigned int)(unsigned char)atoi(e->Attribute("alpha")) << 24;
    m_mapa0.insert(std::pair<const int, int>(idx, (int)rgba));
}

void CServerXml::ProcessLoad(TiXmlNode* node)
{
    TiXmlNode* str = node->FirstChild("str");
    if (str == 0)
    {
        puts("[CServerXml] <str> Tag Skip!!");
    }
    else
    {
        TiXmlNode* s = str->FirstChild("string");
        if (s == 0)
        {
            puts("[CServerXml] <string> Tag Error");
            exit(-1);
        }
        for (; s != 0; s = s->NextSibling())
        {
            int id = 0;
            TiXmlElement* e = s->ToElement();
            e->Attribute("id", &id);
            TiXmlNode* text = s->FirstChild("text");
            if (text == 0)
            {
                printf("%s Tag Error\n", (char*)this);
                return;
            }
            TiXmlElement* te = text->ToElement();
            const char* t = te->Attribute("text");
            StrPunish(id, t, STRING_TYPE_0);
        }
    }
}
}

np_server_xml::CServerXml g_ServerString_;
