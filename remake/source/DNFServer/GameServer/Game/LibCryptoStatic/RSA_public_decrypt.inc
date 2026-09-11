// === RSA_public_decrypt @0x087eaf60 =========================================
// mov padding_arg→rsa槽 / [rsa+0x08]=meth / [meth+0x08]=pub_decrypt / jmp
int RSA_public_decrypt(int flen, const unsigned char* from,
                       unsigned char* to, void* rsa, int padding)
{
    RSA_VIEW* r = static_cast<RSA_VIEW*>(rsa);
    RSA_METH* meth = static_cast<RSA_METH*>(r->meth);
    return meth->pub_decrypt(flen, from, to, r, padding);
}