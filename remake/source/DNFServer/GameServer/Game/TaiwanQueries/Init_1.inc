bool COpensslRSA::Init(const char* path) throw()
{
    if (path == 0)
        return false;
    void* bio = BIO_new(BIO_s_file());
    // [b28d-solo-D02] ORIG 0x0816c31d/0x0816c345 在 BIO_ctrl 之后、PEM 之前析构
    // 两个 string（正常路径）⇒ 原码把两 string 收在**内层作用域**（析构点即块尾）。
    {
        std::string full = GetAppFolder();
        std::string rel(full + path);
        BIO_ctrl(bio, 0x6c, 3, const_cast<char*>(rel.c_str()));
    }
    m_rsa = PEM_read_bio_RSA_PUBKEY(bio, 0, 0, 0);
    BIO_free_all(bio);
    if (m_rsa == 0)
        return false;
    return true;
