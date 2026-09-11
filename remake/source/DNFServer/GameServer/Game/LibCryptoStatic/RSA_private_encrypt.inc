// === RSA_private_encrypt @0x087eaf40 ========================================
// mov padding_arg→rsa槽 / [rsa+0x08]=meth / [meth+0x0c]=rsa_priv_enc / jmp
int RSA_private_encrypt(int flen, const unsigned char* from,
                        unsigned char* to, void* rsa, int padding)
{
    RSA_VIEW* r = static_cast<RSA_VIEW*>(rsa);
    RSA_METH* meth = static_cast<RSA_METH*>(r->meth);
    return meth->priv_encrypt(flen, from, to, r, padding);
}