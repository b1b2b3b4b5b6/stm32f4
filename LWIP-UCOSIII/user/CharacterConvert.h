#ifndef CHARACTERCONVERT_H
#define CHARACTERCONVERT_H

#include "integer.h"
WCHAR ff_wtoupper (	/* Returns upper converted character */
	WCHAR chr		/* Unicode character to be upper converted (BMP only) */
);

WCHAR ff_convert (	/* Converted code, 0 means conversion error */
	WCHAR	chr,	/* Character code to be converted */
	UINT	dir		/* 0: Unicode to OEM code, 1: OEM code to Unicode */
);
unsigned char get_utf8_size( unsigned char byte);
unsigned char unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput);
unsigned char utf8_to_unicode_one(const unsigned char *pInput, unsigned long *Unic)  ;

unsigned short  gbk_to_unicode_one(unsigned short chr);
unsigned short  unicode_to_gbk_one(unsigned short chr);

unsigned short gbk_to_unicode_str(unsigned char *s1,unsigned char *s2);
unsigned short unicode_to_gbk_str(unsigned char *s1,unsigned char *s2);

unsigned short utf8_to_unicode_str(unsigned char *s1,unsigned char *s2);
unsigned short unicode_to_utf8_str(unsigned char *s1,unsigned char *s2);
#endif