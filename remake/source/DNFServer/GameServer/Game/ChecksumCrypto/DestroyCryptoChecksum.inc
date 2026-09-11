void DestroyCryptoChecksum(IChecksum*& p)
{
    if (p != 0) {
        delete p;
        p = 0;
    }
}