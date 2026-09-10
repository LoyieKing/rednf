int RSA_size(const void* rsa)
{
    const RSA_VIEW* r = static_cast<const RSA_VIEW*>(rsa);
    return (BN_num_bits(r->n) + 7) / 8;
