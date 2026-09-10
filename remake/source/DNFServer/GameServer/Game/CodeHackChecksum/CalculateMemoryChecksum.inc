void CalculateMemoryChecksum(CodeHackChecksum* ck, unsigned long size, void* data)
{
    sha256_context ctx;
    sha256_starts(&ctx);
    sha256_update(&ctx, static_cast<unsigned char*>(data), size);
    sha256_finish(&ctx, ck->m_data);
