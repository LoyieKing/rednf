bool put_compressed_blob_data(MySQL* db, int fieldIdx, char* src, int srcLen,
                              char** outStr)
{
    static CCompressedBlob compressed_blob;
    return compressed_blob.compress(db, fieldIdx, src, srcLen, outStr);
