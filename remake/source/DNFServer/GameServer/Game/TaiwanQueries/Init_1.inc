// ORIG T 0x0816c28a：path 空 → false；BIO_new(BIO_s_file()) →
// BIO_ctrl(bio,0x6c,3,(GetAppFolder()+path).c_str())（BIO_C_SET_FILENAME）→
// PEM_read_bio_RSA_PUBKEY(bio,0,0,0) 存 m_rsa → BIO_free_all → key 非空真。
// b28d-36 判词（ORIG 0x0816c28a）已被 b28d-solo-D02 推翻并修真：
// **IDENTICAL（83=83，difflines=0）** —— 修法 = 把 full/rel 两 string 收进内层
// 作用域（析构点=BIO_ctrl 之后、PEM 之前，与 ORIG 0x0816c31d/0x0816c345 一致）
// ⇒ 后续 return 语句脱离 EH region，4.4 不再用 %ebx 汇聚返回值、unwind 块位置
// 同时归位（原判词的 N104 EH 尾块序 + P78 返回寄存器两处残差一次性消失）。
// 原判词保留备查：
// ①EH 尾块相对序——ORIG 正常路径尾块居最末、unwind 块居中（0x816c308..0x816c345），
// 4.4 把同一段 epilogue 就地内联、unwind 压最末，difflib 放大成 ~24 行 rotate；
// ②返回汇聚寄存器——ORIG 各 return 直接 `mov $0/1,%eax`（0x816c298/0x816c38c/0x816c393），
// 4.4 因 return 前需析构 full/rel 两个 string 而用 %ebx 汇聚、尾 `mov %ebx,%eax`。
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
}