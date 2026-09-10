void CRYPTO_free_ex_data(int class_index, void* obj, void* ex_data) // ORIG 0x087f9080
{
    (void)class_index; (void)obj;
    EX_DATA_VIEW* d = static_cast<EX_DATA_VIEW*>(ex_data);
    d->sk = 0;
    d->dummy = 0;
