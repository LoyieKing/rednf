int NexonCash::charge(char* data, unsigned char a, unsigned char b,
                      int encLen, char* encData, char* sendBuf, char* wideBuf,
                      char* more, int count, NXTransaction& tx)
{ return m_impl->chargeCash(data, a, b, encLen, encData, sendBuf, wideBuf,
                            more, count, tx); }
