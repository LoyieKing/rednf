int RSA_private_encrypt(int flen, const unsigned char* from,
                        unsigned char* to, void* rsa, int padding)
{
    RSA_VIEW* r = static_cast<RSA_VIEW*>(rsa);
    RSA_METH* meth = static_cast<RSA_METH*>(r->meth);
    return meth->priv_encrypt(flen, from, to, r, padding);
