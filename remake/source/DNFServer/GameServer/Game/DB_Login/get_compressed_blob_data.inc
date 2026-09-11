// 其余 TU 声明均为 bool（mangled 不含返回型）；ORIG 尾 `call uncompress; leave; ret`
// bool→bool 透传零转换（int 返回+`?1:0` 反产 test/setne 尾段，实测 +5 行）。
bool get_compressed_blob_data(MySQL* db, int col, char* out, int outCap, int recordSize)
{
    static CCompressedBlob compressed_blob;   // ORIG _ZZ24get_compressed_blob_dataP5MySQLiPciiE15compressed_blob 0x0943d24c
    // ORIG 0x083fbc79 直接调成员 uncompress（容量公式 outCap + outCap/10 + 0xc，
    // 见 WindowsPackFiles.cpp），不经过 CCompressedBlobUncompress（/6 公式）。
    return compressed_blob.uncompress(db, col, out, outCap, recordSize);
}