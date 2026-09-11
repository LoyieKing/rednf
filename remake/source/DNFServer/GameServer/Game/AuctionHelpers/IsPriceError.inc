bool IsPriceError(int price, int limit)
{
    // ORIG 单布尔形态（Ghidra 等）：`(a<-1||b<-1) || (b!=-1 && b<=a)`。
    if ((price < -1 || limit < -1) || (limit != -1 && price >= limit))
        return 1;
    return 0;
}