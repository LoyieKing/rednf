// df_statics_r — FrameLagCollector（ORIG FrameLagCollector.o 拆分）
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "FrameLagCollector.h"
#include "DNFServerHandler.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "UdpCharacteristic.h"
#include "Packet_Frame_Lag_Statistic_Add.h"

struct PairView { int m_key; int m_f0; char m_rest[0x1c]; };
struct PktView { char m_pad[0xa]; int m_a; } __attribute__((packed));

int FrameLagCollector::GetCollectInterval()
{
    return m_collectInterval;
}
FrameLagCollector::FrameLagCollector()
    : m_field0(0), m_field4(0), m_field8(0), m_fieldc(0), m_field10(0), m_field14(0),
      m_field18(0), m_field19(0), m_map1c(), m_map34(), m_field4c(0), m_field50(0),
      m_monitor(), m_field6c(0), m_data(), m_collectInterval(0x1e)
{
    m_map1c.clear();
    m_map34.clear();
    m_monitor.clear();
    m_data.clear();
    m_field8c = 0;
    m_field90 = 0;
    m_field94 = 0;
    Init();
    m_directx.init();
    m_renewCnt = 0;
    m_field9c = m_today;
    for (int i = 0; i < 6; i++)
    {
        m_memory[i].init();
    }
    m_field1e4 = 0;
}
FrameLagCollector::~FrameLagCollector()
{
    m_data.clear();
    m_monitor.clear();
    m_map34.clear();
    m_map1c.clear();
}
bool FrameLagCollector::Init()
{
    if (m_field0 != 0)
    {
        return 1;
    }
    m_field0 = 1;
    time_t now = time(0);
    tm* pt = localtime(&now);
    m_today = pt->tm_mday;
    return 0;
}
void FrameLagCollector::RenewToday()
{
    for (;;)
    {
        m_renewCnt++;
        if (9 < m_renewCnt)
        {
            m_renewCnt = 0;
            time_t now = time(0);
            tm* pt = localtime(&now);
            m_today = pt->tm_mday;
        }
        break;
    }
}
int FrameLagCollector::LoadSpec(CServerHandler* handler)
{
    if (m_field4 != 2)
    {
        char needLoad = 0;
        if (m_field4 == 0)
        {
            needLoad = 1;
        }
        else if (m_field4 == 1)
        {
            m_field8++;
            if (m_field8 == 0x3c)
            {
                m_field8 = 0;
                needLoad = 1;
            }
        }
        if (needLoad != 0)
        {
            m_field4 = 1;
            m_field18++;
            if (m_field18 == 0x7f)
            {
                m_field18 = 1;
            }
            Packet_Frame_Lag_Statistic_Load_Spec pkt;
            pkt.m_fieldA = m_field18;
            m_map1c.clear();
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
    return 0;
}
int FrameLagCollector::ReLoadSpec(CServerHandler* handler)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    m_field14++;
    if (m_field14 == 5)
    {
        m_field14 = 0;
        m_field19++;
        if (m_field19 == 0x7f)
        {
            m_field19 = 1;
        }
        m_map34.clear();
        Packet_Frame_Lag_Statistic_Reload_Spec pkt;
        pkt.m_fieldA = m_field19;
        pkt.m_fieldB = m_field4c;
        handler->SendToDB((PacketHeader*)&pkt);
    }
    return 0;
}
int FrameLagCollector::PushOneFrameLagData(Packet_Frame_Lag_Statistic_Add* pkt)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    if (is_valid_statistic_packet(pkt) != 1)
    {
        return 4;
    }
    m_field6c++;
    m_directx.add_cnt(*(unsigned int*)((char*)pkt + 0x6c));
    for (int i = 0; i < 6; i++)
    {
        m_memory[i].SetUsedMemory((char)*(char*)((char*)pkt + i * 0x38 + 0x174),
                                  *(short*)((char*)pkt + i * 0x38 + 0x170));
    }
    for (std::map<int, MonitoringSpecCase>::iterator it = m_monitor.begin();
         it != m_monitor.end(); ++it)
    {
        char match = 1;
        char* s = (char*)&it->second;
        if (!(*(char*)(s + 0x8) == -1 || *(char*)(s + 0x8) == *(char*)((char*)pkt + 0x3c)))
            match = 0;
        if (match && !(*(char*)(s + 0x9) == -1 || *(char*)(s + 0x9) == *(char*)((char*)pkt + 0x38)))
            match = 0;
        if (match && !(*(int*)(s + 0xc) == -1 || *(int*)((char*)pkt + 0x28) < *(int*)(s + 0xc)))
            match = 0;
        if (match && !(*(int*)(s + 0x10) == -1 || *(int*)(s + 0x10) <= *(int*)((char*)pkt + 0x28)))
            match = 0;
        if (match && !(*(short*)(s + 0x14) == -1 || *(short*)(s + 0x14) == *(short*)((char*)pkt + 0x60)))
            match = 0;
        if (match && !(*(int*)(s + 0x18) == -1 || *(unsigned int*)(s + 0x18) == (unsigned int)*(unsigned short*)((char*)pkt + 0x48)))
            match = 0;
        if (match && !(*(int*)(s + 0x1c) == -1 || *(unsigned int*)(s + 0x1c) == (unsigned int)*(unsigned short*)((char*)pkt + 0x50)))
            match = 0;
        if (match && !(*(short*)(s + 0x20) == -1 || *(short*)(s + 0x20) == *(short*)((char*)pkt + 0x58)))
            match = 0;
        if (match && !(*(char*)(s + 0x22) == -1 || *(char*)(s + 0x22) == *(char*)((char*)pkt + 0x68)))
            match = 0;
        if (match)
        {
            std::map<int, FrameLagDataStruct>::iterator fd = m_data.find(it->first);
            if (fd != m_data.end())
            {
                FrameLagDataStruct* v = &fd->second;
                *(int*)((char*)v + 4) = *(int*)((char*)v + 4) + 1;
                if (-1 < (char)*(char*)((char*)pkt + 0x7c) &&
                    (char)*(char*)((char*)pkt + 0x7c) < 8)
                {
                    *(short*)((char*)v + 0x10 + ((char)*(char*)((char*)pkt + 0x7c) + 8) * 2) += 1;
                }
                if (-1 < *(short*)((char*)pkt + 0x80))
                {
                    *(int*)((char*)v + 0x34) += (int)*(short*)((char*)pkt + 0x80);
                    *(int*)((char*)v + 0x38) += (int)*(short*)((char*)pkt + 0x88);
                    *(int*)((char*)v + 0x30) += 1;
                }
                accFrameLagStruct(*v, (FrameLagStruct*)((char*)pkt + 0x90));
            }
        }
    }
    return 0;
}
int FrameLagCollector::PopMonitoringSpecData(Packet_Frame_Lag_Spec_Delete_Notify* pkt)
{
    std::map<int, MonitoringSpecCase>::iterator it;
    char again = 1;
    char erased = 0;
    while (again)
    {
        again = 0;
        it = m_monitor.begin();
        for (; it != m_monitor.end(); ++it)
        {
            if (((PairView*)it.operator->())->m_f0 == ((PktView*)pkt)->m_a)
            {
                m_monitor.erase(it);
                again = 1;
                erased = 1;
            }
        }
    }
    if (erased != 0)
    {
        m_data.erase(*(int*)((char*)pkt + 10));
    }
    return 0;
}
int FrameLagCollector::PushMonitoringSpecData(Packet_Frame_Lag_Statistic_Result_Load_Spec* pkt)
{
    if (m_field4 != 1)
    {
        return 3;
    }
    if (m_field18 != *(char*)((char*)pkt + 0xa))
    {
        return 0;
    }
    int specId = *(int*)((char*)pkt + 0xb);
    if (m_map1c.find(specId) == m_map1c.end())
    {
        return 0;
    }
    m_map1c[specId] = 1;
    for (int i = 0; i <= 5; i++)
    {
        int sid = *(int*)((char*)pkt + (i + 4) * 4 + 3);
        if (sid == -1)
        {
            break;
        }
        int ts = *(int*)((char*)pkt + (i + 8) * 4 + 0xb);
        if (m_field4c < ts)
        {
            m_field4c = ts;
        }
        MonitoringSpecCase mc;
        *(int*)((char*)&mc + 0x0) = *(int*)((char*)pkt + (i + 0x10) * 4 + 3);
        *(int*)((char*)&mc + 0x4) = *(unsigned char*)((char*)pkt + 0x5b + i);
        *(int*)((char*)&mc + 0x8) = *(int*)((char*)pkt + (i + 0x18) * 4 + 7);
        *(int*)((char*)&mc + 0xc) = *(int*)((char*)pkt + (i + 0x1c) * 4 + 0xf);
        *(int*)((char*)&mc + 0x10) = *(unsigned short*)((char*)pkt + (i + 0x48) * 2 + 7);
        *(int*)((char*)&mc + 0x14) = *(int*)((char*)pkt + (i + 0x28) * 4 + 3);
        *(int*)((char*)&mc + 0x18) = *(int*)((char*)pkt + (i + 0x2c) * 4 + 0xb);
        *(int*)((char*)&mc + 0x1c) = *(unsigned short*)((char*)pkt + (i + 0x68) * 2 + 3);
        m_monitor[sid] = mc;
        FrameLagDataStruct fd;
        m_data[*(int*)((char*)&mc + 0x0)] = fd;
    }
    if ((int)m_map1c.size() == *(int*)((char*)pkt + 0xf))
    {
        m_field4 = 2;
        puts("============FirstSpecLoad Complete!!!==========");
    }
    return 0;
}
int FrameLagCollector::PushMonitoringSpecData(Packet_Frame_Lag_Statistic_Result_Reload_Spec* pkt)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    if (m_field19 != *(char*)((char*)pkt + 0xa))
    {
        return 0;
    }
    int specId = *(int*)((char*)pkt + 0xb);
    if (m_map34.find(specId) == m_map34.end())
    {
        return 0;
    }
    m_map34[specId] = 1;
    for (int i = 0; i <= 5; i++)
    {
        int sid = *(int*)((char*)pkt + (i + 4) * 4 + 3);
        if (sid == -1)
        {
            break;
        }
        int ts = *(int*)((char*)pkt + (i + 8) * 4 + 0xb);
        if (m_field50 < ts)
        {
            m_field50 = ts;
        }
        std::map<int, MonitoringSpecCase>::iterator it = m_monitor.find(sid);
        if (it == m_monitor.end())
        {
            MonitoringSpecCase mc;
            *(int*)((char*)&mc + 0x0) = *(int*)((char*)pkt + (i + 0x10) * 4 + 3);
            *(int*)((char*)&mc + 0x4) = *(unsigned char*)((char*)pkt + 0x5b + i);
            *(int*)((char*)&mc + 0x8) = *(int*)((char*)pkt + (i + 0x18) * 4 + 7);
            *(int*)((char*)&mc + 0xc) = *(int*)((char*)pkt + (i + 0x1c) * 4 + 0xf);
            *(int*)((char*)&mc + 0x10) = *(unsigned short*)((char*)pkt + (i + 0x48) * 2 + 7);
            *(int*)((char*)&mc + 0x14) = *(int*)((char*)pkt + (i + 0x28) * 4 + 3);
            *(int*)((char*)&mc + 0x18) = *(int*)((char*)pkt + (i + 0x2c) * 4 + 0xb);
            *(int*)((char*)&mc + 0x1c) = *(unsigned short*)((char*)pkt + (i + 0x68) * 2 + 3);
            m_monitor[sid] = mc;
        }
        else
        {
            *(int*)((char*)&it->second + 0x4) = *(int*)((char*)pkt + (i + 0x10) * 4 + 3);
            *(char*)((char*)&it->second + 0x8) = *(char*)((char*)pkt + 0x5b + i);
            *(char*)((char*)&it->second + 0x9) = *(char*)((char*)pkt + 0x61 + i);
            *(int*)((char*)&it->second + 0xc) = *(int*)((char*)pkt + (i + 0x18) * 4 + 7);
            *(int*)((char*)&it->second + 0x10) = *(int*)((char*)pkt + (i + 0x1c) * 4 + 0xf);
            *(short*)((char*)&it->second + 0x14) = *(short*)((char*)pkt + (i + 0x48) * 2 + 7);
            *(int*)((char*)&it->second + 0x18) = *(int*)((char*)pkt + (i + 0x28) * 4 + 3);
            *(int*)((char*)&it->second + 0x1c) = *(int*)((char*)pkt + (i + 0x2c) * 4 + 0xb);
            *(short*)((char*)&it->second + 0x20) = *(short*)((char*)pkt + (i + 0x68) * 2 + 3);
            *(char*)((char*)&it->second + 0x22) = *(char*)((char*)pkt + 0xdf + i);
        }
        FrameLagDataStruct fd;
        m_data[*(int*)((char*)pkt + (i + 0x10) * 4 + 3)] = fd;
    }
    if ((int)m_map34.size() == *(int*)((char*)pkt + 0xf))
    {
        if (m_field4c < m_field50)
        {
            m_field4c = m_field50;
        }
        puts("============ReloadSpec Complete!!!==========");
    }
    return 0;
}
int FrameLagCollector::CollectIntervalCheck(Packet_Frame_Lag_Collect_Interval_Check* pkt)
{
    if (pkt->m_fieldA != 0)
    {
        m_collectInterval = (short)pkt->m_fieldA;
    }
    else
    {
        m_collectInterval = 0x1e;
    }
    printf("FrameLagCollector::CollectIntervalCheck(), Interval check : %d", m_collectInterval);
    return 0;
}
int FrameLagCollector::SaveDailyBadSpec(CServerHandler* handler)
{
    return 0;
}
int FrameLagCollector::SaveFrameLagData(CServerHandler* handler)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    m_field10++;
    if (!(m_collectInterval <= m_field10))
    {
        return 0;
    }
    m_field10 = 0;
    if (m_field6c == 0)
    {
        return 0;
    }
    Packet_Frame_Lag_Statistic_Write_Lag_Index pkt;
    char* pb = (char*)&pkt;
    pb[10] = (char)handler->GetServerGroupNo();
    time_t now = time(0);
    *(time_t*)(pb + 0xb) = now;
    for (std::map<int, FrameLagDataStruct>::iterator it = m_data.begin();
         it != m_data.end(); ++it)
    {
        FrameLagDataStruct* v = &it->second;
        *(int*)(pb + 0xb) = it->first;
        *(int*)(pb + 0xf) = *(int*)((char*)v + 4);
        for (int i = 0; i < 8; i++)
        {
            *(short*)(pb + 0x13 + i * 2) = *(short*)((char*)v + 0x10 + (i + 8) * 2);
        }
        if (*(int*)((char*)v + 0x30) == 0)
        {
            *(short*)(pb + 0x23) = -1;
            *(short*)(pb + 0x25) = -1;
        }
        else
        {
            *(short*)(pb + 0x23) = (short)((double)(*(int*)((char*)v + 0x34)) /
                                           (double)(*(unsigned int*)((char*)v + 0x30)) + 0.5);
            *(short*)(pb + 0x25) = (short)((double)(*(int*)((char*)v + 0x38)) /
                                           (double)(*(unsigned int*)((char*)v + 0x30)) + 0.5);
        }
        for (int i = 0; i < 6; i++)
        {
            *(short*)(pb + 0x27 + i * 0x1c + 0) = 0;
            *(short*)(pb + 0x27 + i * 0x1c + 2) = 0;
            *(short*)(pb + 0x27 + i * 0x1c + 4) = 0;
            *(short*)(pb + 0x27 + i * 0x1c + 6) = 0;
            if (0 < *(int*)((char*)v + (i + 9) * 0x10 + 0xc))
            {
                *(short*)(pb + 0x27 + i * 0x1c + 0) =
                    (short)(*(int*)((char*)v + 0xc + (i + 0xc) * 4) /
                            *(int*)((char*)v + (i + 9) * 0x10 + 0xc));
            }
            if (0 < *(int*)((char*)v + i * 0x10 + 0xa0))
            {
                *(short*)(pb + 0x27 + i * 0x1c + 2) =
                    (short)(*(int*)((char*)v + 4 + (i + 0x14) * 4) /
                            *(int*)((char*)v + i * 0x10 + 0xa0));
            }
            if (0 < *(int*)((char*)v + i * 0x10 + 0xa4))
            {
                *(short*)(pb + 0x27 + i * 0x1c + 4) =
                    (short)(*(int*)((char*)v + 0xc + (i + 0x18) * 4) /
                            *(int*)((char*)v + i * 0x10 + 0xa4));
            }
            if (0 < *(int*)((char*)v + i * 0x10 + 0xa8))
            {
                *(short*)(pb + 0x27 + i * 0x1c + 6) =
                    (short)(*(int*)((char*)v + 4 + (i + 0x20) * 4) /
                            *(int*)((char*)v + i * 0x10 + 0xa8));
            }
            for (int j = 0; j < 6; j++)
            {
                *(int*)(pb + 0x2f + (i * 7 + j + 4) * 8) =
                    *(int*)((char*)v + 0x14 + (i * 7 + j + 0x1e) * 8);
                *(int*)(pb + 0x33 + (i * 7 + j + 4) * 8) =
                    *(int*)((char*)v + 0x18 + (i * 7 + j + 0x1e) * 8);
            }
        }
        handler->SendToDB((PacketHeader*)&pkt);
        v->init();
        DNFFLib::Sleep_Ext(0, 1);
    }
    m_field6c = 0;
    time_t t2 = time(0);
    if (t2 < m_field8c || m_field90 < t2)
    {
        m_collectInterval = 0x1e;
    }
}
int FrameLagCollector::SaveCollectedDirectxVersion(CServerHandler* handler)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    if (m_field9c == m_today)
    {
        return 0;
    }
    m_field9c = m_today;
    Packet_Frame_Lag_Statistic_Write_Query pkt;
    time_t now = time(0);
    snprintf((char*)&pkt + 10, 0x400,
             "inSert into directx_version(occ_time,server_group,ver_etc,ver_8_x,ver_9_0,ver_9_0_a,ver_9_0_b,ver_9_0_c,ver_10_x,ver_11_x) values(from_unixtime(%d),%hhd,%u,%u,%u,%u,%u,%u,%u,%u)",
             (int)now, (signed char)handler->GetServerGroupNo(),
             (unsigned int)m_directx.m_data[0], (unsigned int)m_directx.m_data[1],
             (unsigned int)m_directx.m_data[2], (unsigned int)m_directx.m_data[3],
             (unsigned int)m_directx.m_data[4], (unsigned int)m_directx.m_data[5],
             (unsigned int)m_directx.m_data[6], (unsigned int)m_directx.m_data[7]);
    handler->SendToDB((PacketHeader*)&pkt);
    m_directx.init();
    return 0;
}
int FrameLagCollector::is_valid_statistic_packet(Packet_Frame_Lag_Statistic_Add* pkt)
{
    if ((char)*(char*)((char*)pkt + 0x1f) < 0 || 8 < (char)*(char*)((char*)pkt + 0x1f))
    {
        return 0;
    }
    for (int i = 0; i < 6; i++)
    {
        if (*(short*)((char*)pkt + i * 0x38 + 0x24) < 0) return 0;
        if (*(short*)((char*)pkt + i * 0x38 + 0x26) < 0) return 0;
        if (*(short*)((char*)pkt + i * 0x38 + 0x28) < 0) return 0;
        if (*(short*)((char*)pkt + i * 0x38 + 0x2a) < 0) return 0;
        for (int j = 0; j < 6; j++)
        {
            if (99999 < *(int*)((char*)pkt + (i * 7 + j + 4) * 8 + 0xc)) return 0;
            if (*(float*)((char*)pkt + (i * 7 + j + 4) * 8 + 0x10) == 0.0 &&
                0 < *(int*)((char*)pkt + (i * 7 + j + 4) * 8 + 0xc)) return 0;
        }
    }
    return 1;
}
void FrameLagCollector::accFrameLagStruct(FrameLagDataStruct& data, FrameLagStruct* pkt)
{
    for (int i = 0; i < 6; i++)
    {
        if (0 < *(short*)((char*)pkt + i * 0x38 + 0))
        {
            *(int*)((char*)&data + (i + 0xc) * 4 + 8) +=
                (int)*(short*)((char*)pkt + i * 0x38 + 0);
            *(int*)((char*)&data + (i + 9) * 0x10 + 8) += 1;
        }
        if (0 < *(short*)((char*)pkt + i * 0x38 + 2))
        {
            *(int*)((char*)&data + (i + 0x14) * 4) +=
                (int)*(short*)((char*)pkt + i * 0x38 + 2);
            *(int*)((char*)&data + i * 0x10 + 0x9c) += 1;
        }
        if (0 < *(short*)((char*)pkt + i * 0x38 + 4))
        {
            *(int*)((char*)&data + (i + 0x18) * 4 + 8) +=
                (int)*(short*)((char*)pkt + i * 0x38 + 4);
            *(int*)((char*)&data + i * 0x10 + 0xa0) += 1;
        }
        if (0 < *(short*)((char*)pkt + i * 0x38 + 6))
        {
            *(int*)((char*)&data + (i + 0x20) * 4) +=
                (int)*(short*)((char*)pkt + i * 0x38 + 6);
            *(int*)((char*)&data + i * 0x10 + 0xa4) += 1;
        }
        for (int j = 0; j < 6; j++)
        {
            *(int*)((char*)&data + (i * 7 + j + 0x1e) * 8 + 0x10) =
                *(int*)((char*)&data + i * 4 + 4) *
                *(int*)((char*)&data + (i * 7 + j + 0x1e) * 8 + 0x10);
            *(float*)((char*)&data + (i * 7 + j + 0x1e) * 8 + 0x14) =
                (float)*(unsigned int*)((char*)&data + i * 4 + 4) *
                *(float*)((char*)&data + (i * 7 + j + 0x1e) * 8 + 0x14);
            *(int*)((char*)&data + (i * 7 + j + 0x1e) * 8 + 0x10) +=
                *(int*)((char*)pkt + j * 8 + i * 0x38 + 8);
            *(float*)((char*)&data + (i * 7 + j + 0x1e) * 8 + 0x14) +=
                *(float*)((char*)pkt + j * 8 + i * 0x38 + 0xc);
            *(unsigned int*)((char*)&data + (i * 7 + j + 0x1e) * 8 + 0x10) /=
                (*(int*)((char*)&data + i * 4 + 4) + 1U);
            *(float*)((char*)&data + (i * 7 + j + 0x1e) * 8 + 0x14) /=
                (float)(*(int*)((char*)&data + i * 4 + 4) + 1);
        }
        *(int*)((char*)&data + i * 4 + 4) += 1;
    }
}
void FrameLagCollector::SaveUsedMemory(CServerHandler* handler)
{
    m_field1e4++;
    if (m_collectInterval <= m_field1e4)
    {
        m_field1e4 = 0;
        Packet_Frame_Lag_Used_Memory_Write_Query pkt;
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                int count = *(int*)((char*)&m_memory[i] + j * 4);
                int sum = *(int*)((char*)&m_memory[i] + (j + 4) * 4 + 8);
                if (count != 0)
                {
                    unsigned int avg = (unsigned int)sum / (unsigned int)count;
                    snprintf((char*)&pkt + 10, 0x400,
                             "inSert into used_memory (occ_time, minute_type, module, memory) values (now(),%d,%d,%d)",
                             i, j, avg);
                    handler->SendToDB((PacketHeader*)&pkt);
                }
            }
            m_memory[i].init();
        }
    }
}
Packet_Frame_Lag_Statistic_Load_Spec::Packet_Frame_Lag_Statistic_Load_Spec()
    : PacketHeader(0xc27, 0xb)
{
}
FrameLagCollector::FrameLagDataStruct::FrameLagDataStruct()
{
    init();
}
struct FrameLagDataLayout
{
    int m0;          // +0x00
    int m_a[2];      // +0x04 (view; accessed up to [5])
    short m_b[16];   // +0x0c
    int m_c[3];      // +0x2c
    int m_d[6];      // +0x38
    int m_e[6];      // +0x50
    int m_f[6];      // +0x68
    int m_g[6];      // +0x80
    int m_h[6][4];   // +0x98
    int m_i[41][2];  // +0x100
};

void FrameLagCollector::FrameLagDataStruct::init()
{
    ((FrameLagDataLayout*)this)->m0 = 0;
    for (int i = 0; i < 8; i++)
    {
        ((FrameLagDataLayout*)this)->m_b[i + 8] = 0;
    }
    ((FrameLagDataLayout*)this)->m_c[0] = 0;
    ((FrameLagDataLayout*)this)->m_c[1] = 0;
    ((FrameLagDataLayout*)this)->m_c[2] = 0;
    for (int i = 0; i < 6; i++)
    {
        ((FrameLagDataLayout*)this)->m_a[i] = 0;
        ((FrameLagDataLayout*)this)->m_d[i] = 0;
        ((FrameLagDataLayout*)this)->m_e[i] = 0;
        ((FrameLagDataLayout*)this)->m_f[i] = 0;
        ((FrameLagDataLayout*)this)->m_g[i] = 0;
        ((FrameLagDataLayout*)this)->m_h[i][0] = 0;
        ((FrameLagDataLayout*)this)->m_h[i][1] = 0;
        ((FrameLagDataLayout*)this)->m_h[i][2] = 0;
        ((FrameLagDataLayout*)this)->m_h[i][3] = 0;
        for (int j = 0; j < 6; j++)
        {
            ((FrameLagDataLayout*)this)->m_i[i * 7 + j][0] = 0;
            ((FrameLagDataLayout*)this)->m_i[i * 7 + j][1] = 0;
        }
    }
}
void FrameLagCollector::DirectxVersionStruct::init()
{
    memset(this, 0, 0x20);
}
void FrameLagCollector::DirectxVersionStruct::add_cnt(unsigned int version)
{
    if (version == 0xffffffff)
    {
        return;
    }
    if (version <= 0x7ffff)
    {
        m_data[0] += 1;
    }
    else if (version > 0x7ffff && version <= 0x8ffff)
    {
        m_data[1] += 1;
    }
    else if (version == 0x90000)
    {
        m_data[2] += 1;
    }
    else if (version == 0x90001)
    {
        m_data[3] += 1;
    }
    else if (version == 0x90002)
    {
        m_data[4] += 1;
    }
    else if (version == 0x90003)
    {
        m_data[5] += 1;
    }
    else if (version > 0x9ffff || version <= 0xaffff)
    {
        m_data[6] += 1;
    }
    else if (version > 0xaffff || version <= 0xbffff)
    {
        m_data[7] += 1;
    }
    else if (version > 0xbffff)
    {
        m_data[0] += 1;
    }
}
void FrameLagCollector::UsedMemoryStruct::init()
{
    memset(this, 0, 0x30);
}
void FrameLagCollector::UsedMemoryStruct::SetUsedMemory(char idx, short value)
{
    if (idx >= 6)
    {
        return;
    }
    m_sums[idx] += value;
    m_counts[idx]++;
}
