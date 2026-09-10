SimpleEncryption* SimpleEncryption::instance()
{
    static SimpleEncryption simpleEncryption;
    return &simpleEncryption;
