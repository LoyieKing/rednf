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
    STSpecStatic key(spec);
    std::map<STSpecStatic, unsigned int>::iterator it = m_spec[param].find(key);
    if (m_spec[param].empty() || it == m_spec[param].end())
    {
        m_spec[param].insert(std::make_pair(key, 1));
    }
    else
    {
        it->second += 1;
    }
}
void CHWSpecResearcher::SendDBMWHWSpec(CServerHandler* handler, unsigned char param)
{
    Packet_DBMW_Save_Client_Spec_Statistic pkt;
    unsigned int count = 0;
    pkt.m_fieldA = (char)param;
    if (!m_spec[param].empty())
    {
        for (std::map<STSpecStatic, unsigned int>::iterator it = m_spec[param].begin();
             it != m_spec[param].end(); ++it)
        {
            memcpy((char*)&pkt + 0xf + count * 0xe + 2, &it->first, 0xc);
            *(short*)((char*)&pkt + 0xf + count * 0xe) = (short)it->second;
            count++;
            if (0x1b3 < count)
            {
                pkt.m_fieldB = 0x1b4;
                handler->SendToDB((PacketHeader*)&pkt);
                count = 0;
            }
        }
        if (count != 0)
        {
            pkt.m_fieldB = count;
            *(short*)((char*)&pkt + 8) = (short)(count * 0xe + 0xf);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void CHWSpecResearcher::DBSaveProcess(CServerHandler* handler)
{
    m_field48++;
    if (0x1d < (unsigned char)m_field48)
    {
        m_field48 = 0;
        for (int i = 0; i < 3; i++)
        {
            SendDBMWHWSpec(handler, (unsigned char)i);
        }
        ResetSpec();
    }
    m_field68++;
    if (0x2c < (unsigned char)m_field68)
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
            *(unsigned short*)((char*)&pkt + 0xe + count * 10) = it->first.m_field0;
            *(int*)((char*)&pkt + 0xe + count * 10 + 4) = it->first.m_field4;
            *(int*)((char*)&pkt + 0xe + count * 10 + 8) = (int)it->second;
            count++;
            if (0x263 < count)
            {
                *(int*)((char*)&pkt + 0xa) = 0x264;
                handler->SendToDB((PacketHeader*)&pkt);
                count = 0;
            }
        }
        if (count != 0)
        {
            *(int*)((char*)&pkt + 0xa) = count;
            *(short*)((char*)&pkt + 8) = (short)(((count << 2) + count) * 2 + 0xe);
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
