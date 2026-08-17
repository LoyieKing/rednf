#include "InterfacePacketBuf.h"

InterfacePacketBuf::InterfacePacketBuf()
{
}

InterfacePacketBuf::~InterfacePacketBuf()
{
}

void InterfacePacketBuf::AcquirePacketBuf(PacketBuf* packetBuf)
{
    m_packetBuf = packetBuf;
}

bool InterfacePacketBuf::bind_packet(char* packet, int size)
{
    return m_packetBuf->bind_packet(packet, size);
}

void InterfacePacketBuf::clear()
{
    m_packetBuf->clear();
}

void InterfacePacketBuf::finalize(bool isEncryptRequired)
{
    m_packetBuf->finalize(isEncryptRequired);
}

PacketBuf* InterfacePacketBuf::get() const
{
    return m_packetBuf;
}

PacketBuf::PACKETBUF_ERROR_TYPE InterfacePacketBuf::getLastError()
{
    return m_packetBuf->getLastError();
}

bool InterfacePacketBuf::get_binary(char* out, int size)
{
    return m_packetBuf->get_binary(out, size);
}

int InterfacePacketBuf::get_index()
{
    return m_packetBuf->get_index();
}

bool InterfacePacketBuf::get_int(int& out)
{
    return m_packetBuf->get_int(out);
}

int InterfacePacketBuf::get_len() const
{
    return m_packetBuf->get_len();
}

char* InterfacePacketBuf::get_packet(int size)
{
    return m_packetBuf->get_packet(size);
}

bool InterfacePacketBuf::get_short(short& out)
{
    return m_packetBuf->get_short(out);
}

bool InterfacePacketBuf::is_finallized() const
{
    return m_packetBuf->is_finallized();
}

bool InterfacePacketBuf::put_binary(const char* data, int len)
{
    return m_packetBuf->put_binary(data, len);
}

bool InterfacePacketBuf::put_binary(char* data, int len)
{
    return m_packetBuf->put_binary(data, len);
}

bool InterfacePacketBuf::put_byte(int value)
{
    return m_packetBuf->put_byte(value);
}

bool InterfacePacketBuf::put_byte(int& index, int value)
{
    return m_packetBuf->put_byte(index, value);
}

bool InterfacePacketBuf::put_header(int packetId, int packetSize)
{
    return m_packetBuf->put_header(packetId, packetSize);
}

bool InterfacePacketBuf::put_int(int value)
{
    return m_packetBuf->put_int(value);
}

bool InterfacePacketBuf::put_int(int& index, int value)
{
    return m_packetBuf->put_int(index, value);
}

bool InterfacePacketBuf::put_item_idx(unsigned long value)
{
    return m_packetBuf->put_item_idx(value);
}

void InterfacePacketBuf::put_packet(const Inven_Item& item)
{
    m_packetBuf->put_packet(item);
}

bool InterfacePacketBuf::put_short(int value)
{
    return m_packetBuf->put_short(value);
}

bool InterfacePacketBuf::put_short(int& index, int value)
{
    return m_packetBuf->put_short(index, value);
}

bool InterfacePacketBuf::put_str(const char* str, int len)
{
    return m_packetBuf->put_str(str, len);
}

bool InterfacePacketBuf::put_str(char* str, int len)
{
    return m_packetBuf->put_str(str, len);
}

void InterfacePacketBuf::set_index(int index)
{
    m_packetBuf->set_index(index);
}
