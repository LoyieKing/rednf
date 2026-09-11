// ORIG T _Z24put_compressed_blob_dataP5MySQLiPciPS1_ @0x083fbd06：
// 函数局部 static CCompressedBlob（guard 0x943c1e8 / 实例 0x943d244）直传
// compress——与 ORIG guard/atexit 结构一致；gcc 额外生成 EH cleanup 保护
// （ORIG 该 TU 无异常处理路径，差异属结构性豁免）。
bool put_compressed_blob_data(MySQL* db, int fieldIdx, char* src, int srcLen,
                              char** outStr)
{
    static CCompressedBlob compressed_blob;
    return compressed_blob.compress(db, fieldIdx, src, srcLen, outStr);
}