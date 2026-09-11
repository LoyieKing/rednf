unsigned int SimpleEncryption::decryptUINT(unsigned int val)
{
    unsigned int out = 0;
    encryptUINT(val, out);   // ORIG 0x0808cfd8 call 双参 encryptUINT（0x0808cf24），照抄
    return out;
}