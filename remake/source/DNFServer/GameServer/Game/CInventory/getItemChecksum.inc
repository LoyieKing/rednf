int getItemChecksum(int slot, int itemIdx, int a, int b, int c)
{
    // ORIG 0x889234d：先置 0 的局部复用；slot*10 产生 shl2/add/add 形态；/10 为 magic 常数除。
    int checksum = 0;
    checksum = (itemIdx + a + b + c) * (slot * 10) / 10;
    int high = checksum & 0xffff0000;
    checksum &= 0xffff;
    checksum += high >> 16;
    return checksum;
}