//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FILE_STATISTIC_H
#define PACKET_FILE_STATISTIC_H

#include "PacketHeader.h"
#include <string.h>

class Packet_File_Statistic : public PacketHeader {
public:
    char m_fileName[0x100];  // +a..+109（df_statics_r OnFileStatistic: pb+10 作日志路径，空串→"filestatics"）
    char m_fileData[0x800];  // +10a..+909（OnFileStatistic: CMyRawFileLog 写入的文件内容）

    Packet_File_Statistic() : PacketHeader(0x27f3, 0x90a)
    {
    memset(m_fileName, 0, sizeof(m_fileName));
    memset(m_fileData, 0, sizeof(m_fileData));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_File_Statistic, 0x90a);

#endif  // PACKET_FILE_STATISTIC_H
