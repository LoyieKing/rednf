// df_statics_r — HWSpecResearcher（ORIG HWSpecResearcher.o 拆分）
#include <stdio.h>
#include <string.h>

#include "HWSpecResearcher.h"
#include "DNFDBServer.h"
#include "DNFServerHandler.h"
#include "DNFFileLog.h"

CHWSpecResearcher::CHWSpecResearcher()
    : m_specSaveTick(0), m_field4c(0), m_errorSaveTick(0)
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
// [DNF-NONIDENTICAL] DNF-STA-DIFF-0014 | statics | 与ORIG差异=DIFF | CHWSpecResearcher::SendDBMWHWSpec | 详见 function_reports/statics/_ZN17CHWSpecResearcher14SendDBMWHWSpecEP14CServerHandlerh.md
void CHWSpecResearcher::SendDBMWHWSpec(CServerHandler* handler, unsigned char param)
{
    Packet_DBMW_Save_Client_Spec_Statistic pkt;
    std::map<STSpecStatic, unsigned int>* pMap = &m_spec[param];
    pkt.m_flag = (char)param;
    int count = 0;
    if (!pMap->empty())
    {
        for (std::map<STSpecStatic, unsigned int>::iterator it = pMap->begin();
             it != pMap->end(); ++it)
        {
            const STSpecStatic* pSpec = &it->first;
            memcpy(&pkt.m_items[count].m_spec, (const void*)pSpec, 0xc);
            pkt.m_items[count].m_total = (short)it->second;
            if (0x1b3U < (++count))
            {
                pkt.m_count = 0x1b4;
                handler->SendToDB((PacketHeader*)&pkt);
                count = 0;
            }
        }
        if (count != 0)
        {
            pkt.m_count = count;
            pkt.packetSize = (unsigned short)(count * 0xe + 0xf);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void CHWSpecResearcher::DBSaveProcess(CServerHandler* handler)
{
    if ((++m_specSaveTick) > 0x1d)
    {
        m_specSaveTick = 0;
        for (int i = 0; i < 3; i++)
        {
            SendDBMWHWSpec(handler, (unsigned char)i);
        }
        ResetSpec();
    }
    if ((++m_errorSaveTick) > 0x2c)
    {
        m_errorSaveTick = 0xf;
        SendDBMWErrorLine(handler);
        m_field4c = 0;
        ResetErrorSpec();
    }
}
void CHWSpecResearcher::WriteErrorLineStatics(unsigned short param, int value)
{
    ErrorValue errorValue;
    errorValue.m_errorCode = (unsigned int)value;
    errorValue.m_errorLine = param;
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
// [DNF-NONIDENTICAL] DNF-STA-DIFF-0015 | statics | 与ORIG差异=DIFF | CHWSpecResearcher::SendDBMWErrorLine | 详见 function_reports/statics/_ZN17CHWSpecResearcher17SendDBMWErrorLineEP14CServerHandler.md
void CHWSpecResearcher::SendDBMWErrorLine(CServerHandler* handler)
{
    Packet_DBMW_Save_Error_Line_Statistic pkt;
    int count = 0;
    if (!m_errorSpec.empty())
    {
        for (std::map<STErrorStatic, unsigned int>::iterator it = m_errorSpec.begin();
             it != m_errorSpec.end(); ++it)
        {
            pkt.m_items[count].m_errorLine = it->first.m_errorLine;
            pkt.m_items[count].m_errorCode = it->first.m_errorCode;
            pkt.m_items[count].m_cnt = (int)it->second;
            if (0x263U < (++count))
            {
                pkt.m_count = 0x264;
                handler->SendToDB((PacketHeader*)&pkt);
                count = 0;
            }
        }
        if (count != 0)
        {
            pkt.m_count = count;
            pkt.packetSize = (unsigned short)(count * 10 + 0xe);
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
    m_category1 = 0xff;
    m_category2 = 0xffffffff;
    m_category3 = 0xffffffff;
}
