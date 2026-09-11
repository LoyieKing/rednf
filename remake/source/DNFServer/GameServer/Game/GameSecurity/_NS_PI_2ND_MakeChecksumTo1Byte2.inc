// ---------------------------------------------------------------------------
// _NS_PI_2ND_MakeChecksumTo1Byte2  ORIG T 0x808cc4b (0x1f)
// ORIG：*a = movzbl(*a)；*b = movzbl(*b)（各截低 8 位回写 int）。
// ---------------------------------------------------------------------------
void _NS_PI_2ND_MakeChecksumTo1Byte2(unsigned int& checksum1,
                                     unsigned int& checksum2)
{
    checksum1 = static_cast<unsigned int>(static_cast<unsigned char>(checksum1));
    checksum2 = static_cast<unsigned int>(static_cast<unsigned char>(checksum2));
}