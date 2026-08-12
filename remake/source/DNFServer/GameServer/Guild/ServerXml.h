#ifndef SERVERXML_H_
#define SERVERXML_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"
#include "tinyxml.h"
#include "ServerXml.h"

class CServerXml;

// from GuildDomain.h
namespace np_server_xml
{
enum _eStringType {};

class CServerXml
{
public:
    CServerXml();
    ~CServerXml();
    void InitString();
    void StrLoading();
    void StrLoading(std::string path);
    void CharsetInit(TiXmlNode* node);
    void EventLoad(TiXmlNode* node);
    void RGBALoad(int idx, TiXmlNode* node);
    void ProcessLoad(TiXmlNode* node);
    void StrPunish(int idx, const char* str, _eStringType type);
    std::string GetServerString(int idx, bool* ok) const;
    unsigned int GetEventRGBA(int idx) const;
    std::string GetEventString(int idx, _eStringType type, bool* ok) const;
    char m_data[5];   // +0
    TiXmlDocument m_doc; // +8 TiXmlDocument（0x48）
    int m_field50;    // +0x50
    std::string m_path;  // +0x54
    std::map<int, std::string> m_str1;  // +0x58
    std::map<int, std::string> m_str2;  // +0x70
    std::map<int, std::string> m_str3;  // +0x88
    std::map<int, int> m_rgba;          // +0xa0
};
}

extern np_server_xml::CServerXml g_ServerString_;

#endif
