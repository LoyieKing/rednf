pack_ret_t PackSet::output_file_list(char const* pszOutputPath) const    // 0x08ac6988
{
    if (!is_ready())
    {
        fprintf(stdout, "%s\n", "Not yet opened");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x573, __FUNCTION__);
        return PACK_ERROR;
    }

    FILE* fp = fopen(toMbcs(pszOutputPath), "wt");
    if (fp == 0)
    {
        fprintf(stdout, "%s\n", "Failed to output file open.");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x57a, __FUNCTION__);
        return PACK_ERROR;
    }

    fwrite("hash,type,name,size,aligned size,crc,offset\n", 1, 0x2c, fp);

    for (index_vector_t::const_iterator it = m_indexes.begin();
         it != m_indexes.end(); ++it)
    {
        fprintf(fp, "\"%d\"", static_cast<int>(it->first));
        fputc(',', fp);
        fprintf(fp, "\"%d\"", static_cast<int>(it->second.type_));
        fputc(',', fp);
        fprintf(fp, "\"%s\"", toMbcs(it->second.name_));
        fputc(',', fp);
        fprintf(fp, "\"%d\"", static_cast<int>(it->second.size_));
        fputc(',', fp);
        fprintf(fp, "\"%d\"", static_cast<int>(it->second.aligned_size_));
        fputc(',', fp);
        fprintf(fp, "\"%d\"", static_cast<int>(it->second.crc_));
        fputc(',', fp);
        fprintf(fp, "\"%d\"", static_cast<int>(it->second.offset_));
        fputc('\n', fp);
    }
    fclose(fp);
    return PACK_OK;
