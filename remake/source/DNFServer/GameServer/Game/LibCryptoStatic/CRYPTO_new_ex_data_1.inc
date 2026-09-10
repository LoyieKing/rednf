int CRYPTO_new_ex_data(int class_index, void* obj, void* ex_data)   // ORIG 0x087f91e0
{
    (void)obj;
    EX_DATA_VIEW* d = static_cast<EX_DATA_VIEW*>(ex_data);
    d->sk = 0;
    d->dummy = 0;
    return 1;
