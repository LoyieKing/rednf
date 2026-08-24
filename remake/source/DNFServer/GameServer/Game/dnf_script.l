/* ==========================================================================
 * dnf_script.l — DNFLex / __dnf_script__FlexLexer
 * flex 2.5.35  -+ -C -P__dnf_script__
 * %option yyclass="DNFLex"  =>  stub on __dnf_script__FlexLexer::yylex
 *                            =>  scanner body is DNFLex::yylex
 * Rules recovered from ORIG DFA (yy_accept/def/base/chk/nxt) + yylex.md
 * ========================================================================== */

%option c++ yyclass="DNFLex" yylineno nounistd

%x C1 C2 C3

%{
#define YY_NO_UNISTD_H 1
#ifndef yyFlexLexer
#define yyFlexLexer __dnf_script__FlexLexer
#endif
#include "DNFLex.h"
#include <cstdio>
#include <cstring>
%}

%%

[+-]?[0-9]+			{ return DNF_TOK_DECIMAL; }
0x[0-9A-Fa-f]+			{ return DNF_TOK_HEXADECIMAL; }
[+-]?[0-9]+\.[0-9]+		{ return DNF_TOK_FLOAT; }
"<include>"			{ return DNF_TOK_INCLUDE; }
\[[^\n\r\]]+\]			{ return DNF_TOK_TYPE; }
\([^)\n\r]+\)			{ return DNF_TOK_CONNECTOR; }
[!&,~]				{ return DNF_TOK_CUSTOM_DATA; }
"<"[0-9]+:			{ return DNF_TOK_PAIR_INT; }
:[0-9A-Za-z_]+>			{ return DNF_TOK_PAIR_STR; }

"/*"				{ yy_start = 3; }
<C1>"*/"			{ yy_start = 1; }
<C1>.|\n|\r			{ }

"//"				{ yy_start = 5; }
<C2>[\r\n]			{ yy_start = 1; }
<C2>.				{ }

`				{
					is_dbcs_ = false;
					yy_start = 7;
					m_str_tok_pos = strTokBuf;
				}
<C3>.|\n|\r			{
					if (is_dbcs_)
					{
						char *p = m_str_tok_pos;
						*p = *yytext;
						m_str_tok_pos = p + 1;
						is_dbcs_ = false;
					}
					else
					{
						char c = *yytext;
						if (c == '`')
						{
							*m_str_tok_pos = '\0';
							yy_start = 1;
							return DNF_TOK_STRING;
						}
						if (strTokBuf + 0x1ffe <= m_str_tok_pos)
						{
							if (IsStream())
							{
								stream_t* stream = GetInputStream();
								sprintf(errBuf,
									"%s - %s/%s, line %d, string buffer overflow detected.",
									__FUNCTION__, stream->dir.c_str(), stream->filename.c_str(), yylineno);
								LexerError(errBuf);
								return DNF_TOK_ERROR;
							}
							sprintf(errBuf,
								"%s - the input stream is empty.", __FUNCTION__);
							LexerError(errBuf);
							return DNF_TOK_ERROR;
						}
						char *p = m_str_tok_pos;
						*p = c;
						m_str_tok_pos = p + 1;
						is_dbcs_ = (*isDbcsLeadChar)(c);
					}
				}

[ \t\r\n]+			{ }

.				{
					if (IsStream())
					{
						stream_t* stream = GetInputStream();
						char c = *yytext;
						if (c != '\0')
							sprintf(errBuf,
								"%s - %s/%s, line %d, unrecognized character '%c'.",
								__FUNCTION__, stream->dir.c_str(), stream->filename.c_str(), yylineno, c);
						else
							sprintf(errBuf,
								"%s - %s/%s, line %d, unrecognized character 'NULL'.",
								__FUNCTION__, stream->dir.c_str(), stream->filename.c_str(), yylineno);
						LexerError(errBuf);
						return DNF_TOK_ERROR;
					}
				}

<C1><<EOF>>			{
					memcpy(errBuf, "comment nest mismatch", 0x16);
					LexerError(errBuf);
					return DNF_TOK_ERROR;
				}

%%
