// df_statics_r — HWSpecResearcher（ORIG HWSpecResearcher.o 拆分）
#include <stdio.h>
#include <string.h>

#include "HWSpecResearcher.h"
#include "DNFDBServer.h"
#include "DNFServerHandler.h"
#include "DNFFileLog.h"

CHWSpecResearcher::CHWSpecResearcher()
    : m_field48(0), m_field4c(0), m_field68(0)
{
}
CHWSpecResearcher::~CHWSpecResearcher()
{
    ResetSpec();
    ResetErrorSpec();
}
void CHWSpecResearcher::ResetSpec()
{
    std::map<STSpecStatic, unsigned int>::iterator it;
    for (int i = 0; i < 3; i++)
    {
        std::map<STSpecStatic, unsigned int>* p = &m_spec[i];
        p->clear();
    }
}
void CHWSpecResearcher::WriteSpecStatics(unsigned char param, const HWSpec& spec)
{
    if (2 < param)
    {
        DNF_LOG_SCOPE_LINE(0x27, "./log/HWSpec.log", "CHWSpecResearcher::WriteSpecStatics Over db_type(%d)",
            (unsigned int)param);
        return;
    }
    std::map<STSpecStatic, unsigned int>* pMap = &m_spec[param];
    STSpecStatic key(spec);
    std::map<STSpecStatic, unsigned int>::iterator it = pMap->find(key);
    if (pMap->empty() || it == pMap->end())
    {
        pMap->insert(std::make_pair(key, 1));
    }
    else
    {
        it->second += 1;
    }
}
void CHWSpecResearcher::SendDBMWHWSpec(CServerHandler* handler, unsigned char param)
{
    Packet_DBMW_Save_Client_Spec_Statistic pkt;
    std::map<STSpecStatic, unsigned int>* pMap = &m_spec[param];
    pkt.m_fieldA = (char)param;
    unsigned int count = 0;
    if (!pMap->empty())
    {
        for (std::map<STSpecStatic, unsigned int>::iterator it = pMap->begin();
             it != pMap->end(); ++it)
        {
            const STSpecStatic* pSpec = &it->first;
            memcpy((char*)&pkt + 0xf + count * 0xe + 2, pSpec, 0xc);
            *(short*)((char*)&pkt + 0xf + count * 0xe) = (short)it->second;
            if (0x1b3 < (++count))
            {
                pkt.m_fieldB = 0x1b4;
                handler->SendToDB((PacketHeader*)&pkt);
                count = 0;
            }
        }
        if (count != 0)
        {
            pkt.m_fieldB = count;
            pkt.packetSize = (unsigned short)(count * 0xe + 0xf);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void CHWSpecResearcher::DBSaveProcess(CServerHandler* handler)
{
    if ((++m_field48) > 0x1d)
    {
        m_field48 = 0;
        for (int i = 0; i < 3; i++)
        {
            SendDBMWHWSpec(handler, (unsigned char)i);
        }
        ResetSpec();
    }
    if ((++m_field68) > 0x2c)
    {
        m_field68 = 0xf;
        SendDBMWErrorLine(handler);
        m_field4c = 0;
        ResetErrorSpec();
    }
}
void CHWSpecResearcher::WriteErrorLineStatics(unsigned short param, int value)
{
    ErrorValue errorValue;
    errorValue.m_field4 = (unsigned int)value;
    errorValue.m_field0 = param;
    STErrorStatic key(errorValue);
    std::map<STErrorStatic, unsigned int>::iterator it = m_errorSpec.find(key);
    if (m_errorSpec.empty() || it == m_errorSpec.end())
    {
        m_errorSpec.insert(std::make_pair(key, 1));
    }
    else
    {
        it->second += 1;
    }
}
void CHWSpecResearcher::SendDBMWErrorLine(CServerHandler* handler)
{
    Packet_DBMW_Save_Error_Line_Statistic pkt;
    unsigned int count = 0;
    if (!m_errorSpec.empty())
    {
        for (std::map<STErrorStatic, unsigned int>::iterator it = m_errorSpec.begin();
             it != m_errorSpec.end(); ++it)
        {
            pkt.m_items[count].m_field4 = it->first.m_field0;
            pkt.m_items[count].m_field0 = it->first.m_field4;
            pkt.m_items[count].m_field6 = (int)it->second;
            if (0x263 < (++count))
            {
                pkt.m_count = 0x264;
                handler->SendToDB((PacketHeader*)&pkt);
                count = 0;
            }
        }
        if (count != 0)
        {
            pkt.m_count = count;
            pkt.packetSize = (unsigned short)(((count << 2) + count) * 2 + 0xe);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void CHWSpecResearcher::ResetErrorSpec()
{
    m_errorSpec.clear();
}
HWSpec::HWSpec()
{
    m_field0 = 0xff;
    m_field4 = 0xffffffff;
    m_field8 = 0xffffffff;
}
