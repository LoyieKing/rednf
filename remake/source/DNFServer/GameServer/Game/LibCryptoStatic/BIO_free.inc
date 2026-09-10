int    BIO_free(void* bio);
void*  PEM_ASN1_read_bio(void* (*d2i)(void**, const unsigned char**, long),
                         const char* name, void* bp, void** x,
                         void* cb, void* u);
void*  d2i_RSA_PUBKEY(void** a, const unsigned char** in, long len);
