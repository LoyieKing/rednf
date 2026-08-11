#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();
// df_dbmw_r - ServerXml (ORIG ServerXml.cpp)
// dbmw 版 CServerXml（参考 shared ServerXml.cpp 的 auction 版，去掉 auction 扩展）

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "DNFFunctionLib.h"

namespace np_server_xml
{

static const char FILE_NAME[] = "server_str.xml";

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
    m_eCharSet = ENUM_CHARSET_KOR;
    memset(m_cCharSet, 0, sizeof(m_cCharSet));
    m_mapServerStr.clear();
    m_mapEventStartStr.clear();
    m_mapEventEndStr.clear();
    m_map_EventRGBA.clear();
}

void CServerXml::StrLoading()
{
    StrLoading(std::string(FILE_NAME));
}

void CServerXml::StrLoading(std::string file)
{
    InitString();
    xmlFileName_ = file;
    m_cStrDoc.Clear();
    if (!m_cStrDoc.LoadFile(xmlFileName_, TIXML_ENCODING_UNKNOWN))
    {
        printf("[CServerXml] Load Fail File : %s\n", FILE_NAME);
    }
    else
    {
        TiXmlNode* pRootNode = m_cStrDoc.FirstChild("xml");
        if (pRootNode == (TiXmlNode*)0)
        {
            puts("[CServerXml] <xml> Tag Error");
        }
        else
        {
            CharsetInit(pRootNode);
            EventLoad(pRootNode);
            ProcessLoad(pRootNode);
        }
    }
}

void CServerXml::CharsetInit(TiXmlNode* pRootNode)
{
    const char* cTemp = (const char*)0;
    TiXmlNode* pRoot = (TiXmlNode*)0;
    pRoot = pRootNode->FirstChild("option");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <option> Tag Error");
        exit(-1);
    }
    pRoot = pRoot->FirstChild("charset");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <charset> Tag Error");
        exit(-1);
    }
    cTemp = pRoot->ToElement()->Attribute("type");
    if (cTemp == (const char*)0)
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    if (strcmp(cTemp, "kor") == 0)
    {
        m_eCharSet = ENUM_CHARSET_KOR;
    }
    else if (strcmp(cTemp, "chn") == 0)
    {
        m_eCharSet = ENUM_CHARSET_CHN;
    }
    else if (strcmp(cTemp, "jpn") == 0)
    {
        m_eCharSet = ENUM_CHARSET_JPN;
    }
    else if (strcmp(cTemp, "usa") == 0)
    {
        m_eCharSet = ENUM_CHARSET_USA;
    }
    else if (strcmp(cTemp, "twn") == 0)
    {
        m_eCharSet = ENUM_CHARSET_TWN;
    }
    else
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    strcpy(m_cCharSet, cTemp);
}

void CServerXml::EventLoad(TiXmlNode* pRootNode)
{
    TiXmlNode* pRoot = (TiXmlNode*)0;
    pRoot = pRootNode->FirstChild("event_str");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <event_str> Tag Skip!!");
    }
    else
    {
        pRoot = pRoot->FirstChild("event");
        if (pRoot == (TiXmlNode*)0)
        {
            puts("[CServerXml] <event> Tag Error");
            exit(-1);
        }
        for (; pRoot != (TiXmlNode*)0; pRoot = pRoot->NextSibling())
        {
            int un_ID = 0;
            char* strTemp = (char*)0;
            char* cSentence = (char*)0;
            TiXmlNode* cNode = (TiXmlNode*)0;
            pRoot->ToElement()->Attribute("id", &un_ID);
            RGBALoad(un_ID, pRoot);
            cNode = pRoot->FirstChild(m_cCharSet);
            if (cNode == (TiXmlNode*)0)
            {
                printf("%s Tag Error\n", this);
                exit(-1);
            }
            cSentence = (char*)cNode->ToElement()->Attribute("start_msg");
            StrPunish(un_ID, cSentence, ENUM_EVENT_STRING_START);
            cSentence = (char*)cNode->ToElement()->Attribute("end_msg");
            StrPunish(un_ID, cSentence, ENUM_EVENT_STRING_END);
        }
    }
}

void CServerXml::RGBALoad(int eventID, TiXmlNode* pRootNode)
{
    TiXmlNode* cNode = (TiXmlNode*)0;
    cNode = pRootNode->FirstChild("color");
    if (cNode == (TiXmlNode*)0)
    {
        printf("%s Tag Error\n", this);
        exit(-1);
    }
    union uniRGBA
    {
        int color;
        char c[4];
    };
    uniRGBA color;
    color.color = 0;
    color.c[0] = (char)atoi(cNode->ToElement()->Attribute("red"));
    color.c[1] = (char)atoi(cNode->ToElement()->Attribute("green"));
    color.c[2] = (char)atoi(cNode->ToElement()->Attribute("blue"));
    color.c[3] = (char)atoi(cNode->ToElement()->Attribute("alpha"));
    m_map_EventRGBA.insert(std::pair<int, int>(eventID, color.color));
}

void CServerXml::ProcessLoad(TiXmlNode* pRootNode)
{
    TiXmlNode* pRoot = (TiXmlNode*)0;
    pRoot = pRootNode->FirstChild("str");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <str> Tag Skip!!");
    }
    else
    {
        pRoot = pRoot->FirstChild("string");
        if (pRoot == (TiXmlNode*)0)
        {
            puts("[CServerXml] <string> Tag Error");
            exit(-1);
        }
        for (; pRoot != (TiXmlNode*)0; pRoot = pRoot->NextSibling())
        {
            int un_ID = 0;
            char* strTemp = (char*)0;
            char* cSentence = (char*)0;
            TiXmlNode* cNode = (TiXmlNode*)0;
            pRoot->ToElement()->Attribute("id", &un_ID);
            cNode = pRoot->FirstChild(m_cCharSet);
            if (cNode == (TiXmlNode*)0)
            {
                printf("%s Tag Error\n", this);
                return;
            }
            cSentence = (char*)cNode->ToElement()->Attribute("text");
            StrPunish(un_ID, cSentence, ENUM_SERVER_STRING);
        }
    }
}

void CServerXml::StrPunish(int nTextID, const char* pText, _eStringType type)
{
    if (pText != (const char*)0)
    {
        std::string strText = pText;
        switch (type)
        {
        case ENUM_SERVER_STRING:
            m_mapServerStr.insert(std::pair<int, std::string>(nTextID, strText));
            break;
        case ENUM_EVENT_STRING_START:
            m_mapEventStartStr.insert(
                std::pair<int, std::string>(nTextID, strText));
            break;
        case ENUM_EVENT_STRING_END:
            m_mapEventEndStr.insert(
                std::pair<int, std::string>(nTextID, strText));
            break;
        }
    }
}

std::string CServerXml::GetServerString(int nTextID, bool* pbResult) const
{
    std::string ret = "";
    std::map<int, std::string>::const_iterator pIter;
    pIter = m_mapServerStr.find(nTextID);
    if (pIter != m_mapServerStr.end())
    {
        if (pbResult != (bool*)0)
        {
            *pbResult = true;
        }
    }
    else
    {
        if (pbResult != (bool*)0)
        {
            *pbResult = false;
        }
        return ret;
    }
    return pIter->second;
}

int CServerXml::GetEventRGBA(int nEventID) const
{
    std::map<int, int>::const_iterator pIter;
    pIter = m_map_EventRGBA.find(nEventID);
    if (pIter != m_map_EventRGBA.end())
    {
        return pIter->second;
    }
    return 0;
}

std::string CServerXml::GetEventString(int nTextID, _eStringType type,
                                       bool* pbResult) const
{
    std::string ret = "";
    std::map<int, std::string>::const_iterator pIter;
    switch (type)
    {
    case ENUM_EVENT_STRING_START:
        pIter = m_mapEventStartStr.find(nTextID);
        if (pIter != m_mapEventStartStr.end())
        {
            if (pbResult != (bool*)0)
            {
                *pbResult = true;
            }
        }
        else
        {
            if (pbResult != (bool*)0)
            {
                *pbResult = false;
            }
            return ret;
        }
        return pIter->second;
    case ENUM_EVENT_STRING_END:
        pIter = m_mapEventEndStr.find(nTextID);
        if (pIter != m_mapEventEndStr.end())
        {
            if (pbResult != (bool*)0)
            {
                *pbResult = true;
            }
        }
        else
        {
            if (pbResult != (bool*)0)
            {
                *pbResult = false;
            }
            return ret;
        }
        return pIter->second;
    default:
        return ret;
    }
}

} // namespace np_server_xml

np_server_xml::CServerXml g_ServerString_;
