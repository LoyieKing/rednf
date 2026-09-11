// 自由函数 getAvatarEmblemTypeIdx（ORIG W 0x0822a98c，定义见下）：
// cmpw $0x10; jbe → type>0x10 返 -1；自 4 位向 0 位 sar 扫描首个 (type>>i)==1
// 的位序号，未命中返回 -1。
int getAvatarEmblemTypeIdx(unsigned short type)
{
    if (type > 0x10)
        return -1;
    for (int i = 4; i >= 0; --i)
    {
        if ((type >> i) == 1)
            return i;
    }
    return -1;
}