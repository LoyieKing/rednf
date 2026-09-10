int stPacket::GetLength()
{
    return static_cast<int>(stHeader::GetLength()) + 3;
