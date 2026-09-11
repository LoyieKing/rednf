// ORIG 0x08694224 (sz 0x19):
//   return static_cast<int>(stHeader::GetLength()) + 3;
int stPacket::GetLength()
{
    return static_cast<int>(stHeader::GetLength()) + 3;
}