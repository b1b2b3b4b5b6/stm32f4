#include "CharacterConvert.h"

unsigned char unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput)  
{   
    if ( unic <= 0x0000007F )  
    {  
        // * U-00000000 - U-0000007F:  0xxxxxxx  
        *pOutput     = (unic & 0x7F);  
        return 1;  
    }  
    else if ( unic >= 0x00000080 && unic <= 0x000007FF )  
    {  
        // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx  
        *(pOutput+1) = (unic & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 6) & 0x1F) | 0xC0;  
        return 2;  
    }  
    else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )  
    {  
        // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx  
        *(pOutput+2) = (unic & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 12) & 0x0F) | 0xE0;  
        return 3;  
    }  
    else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )  
    {  
        // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+3) = (unic & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 18) & 0x07) | 0xF0;  
        return 4;  
    }  
    else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )  
    {  
        // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+4) = (unic & 0x3F) | 0x80;  
        *(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 24) & 0x03) | 0xF8;  
        return 5;  
    }  
    else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )  
    {  
        // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+5) = (unic & 0x3F) | 0x80;  
        *(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 30) & 0x01) | 0xFC;  
        return 6;  
    }  
  
    return 0;  
}  
unsigned char get_utf8_size( unsigned char byte)
{
	if((byte&0x80)==0) return 1;
	byte<<=2;
	if((byte&0x80)==0) return 2;
	byte<<=1;
	if((byte&0x80)==0) return 3;
	byte<<=1;
	if((byte&0x80)==0) return 4;
	byte<<=1;
	if((byte&0x80)==0) return 5;
	byte<<=1;
	if((byte&0x80)==0) return 6;
	return 0;
}
unsigned char utf8_to_unicode_one(const unsigned char *pInput, unsigned long *Unic)  
{   
  
    // b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...  
    char b1, b2, b3, b4, b5, b6;  
	b1 = *pInput;  
	b2 = *(pInput + 1);  
	b3 = *(pInput + 2);  
	b4 = *(pInput + 3);  
	b5 = *(pInput + 4);  
	b6 = *(pInput + 5);  
    *Unic = 0x0; // 把 *Unic 初始化为全零  
    unsigned char utfbytes = get_utf8_size(*pInput);  
    unsigned char *pOutput = (unsigned char *) Unic;  
  
    switch ( utfbytes )  
    {  
        case 1:  
            *pOutput     = *pInput;    
            break;  
			
        case 2:  
  
            if ( (b2 & 0xE0) != 0x80 )  
                return 0;  
            *pOutput     = (b1 << 6) + (b2 & 0x3F);  
            *(pOutput+1) = (b1 >> 2) & 0x07;  
            break;  
			
        case 3:  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b2 << 6) + (b3 & 0x3F);  
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);  
            break;  
			
        case 4:  

            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b3 << 6) + (b4 & 0x3F);  
            *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);  
            *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);  
            break;  
			
        case 5:  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b4 << 6) + (b5 & 0x3F);  
            *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);  
            *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);  
            *(pOutput+3) = (b1 << 6);  
            break;  
			
        case 6:  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)  
                    || ((b6 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b5 << 6) + (b6 & 0x3F);  
            *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);  
            *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);  
            *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);  
            break;  
			
        default:  
            return 0;  
            break;  
    }  
  
    return utfbytes;  
}  

unsigned short  gbk_to_unicode_one(unsigned short chr)
{
	return  ff_convert (chr,1);		/* 0: Unicode to OEM code, 1: OEM code to Unicode */
}

unsigned short  unicode_to_gbk_one(unsigned short chr)
{
	return  ff_convert (chr,0);		/* 0: Unicode to OEM code, 1: OEM code to Unicode */
}

unsigned short gbk_to_unicode_str(unsigned char *s1,unsigned char *s2)
{
	unsigned short n;
	unsigned short gbk_temp;
	unsigned char *gbk=s1;
	unsigned short *uni=(unsigned short*)s2;
	if(*gbk==0) return 0;
	for(n=0;*gbk!=0;n++)
	{
		if(*gbk<0x80) 
		{
			gbk_temp=*gbk;
			*uni=gbk_to_unicode_one(gbk_temp);
			gbk+=1;
		}
		else	
		{
			gbk_temp=0;
			gbk_temp=*gbk;
			gbk_temp<<=8;
			gbk_temp|=*(gbk+1);
			*uni=gbk_to_unicode_one(gbk_temp);
			gbk+=2;
		}
		uni++;
			
	}
	*uni=0;
	return (n+1);
}
unsigned short unicode_to_gbk_str(unsigned char *s1,unsigned char *s2)
{
	unsigned short n;
	unsigned short gbk_temp;
	unsigned short *uni=(unsigned short*)s1;
	unsigned char *gbk=s2;
	if(*uni==0) return 0;
	for(n=0;*uni!=0;n++)
	{
		if(*uni<0x80)
		{
			*gbk=unicode_to_gbk_one(*uni);	
			gbk+=1;;
		}
		else
		{
			gbk_temp==unicode_to_gbk_one(*uni);
			*gbk=gbk_temp>>8;
			*(gbk+1)=gbk_temp;
			gbk+=2;
		}
		uni++;
	}
	*gbk=0;
	return (n+1);
}

unsigned short utf8_to_unicode_str(unsigned char *s1,unsigned char *s2)
{
	unsigned  short n;
	unsigned char temp;
	unsigned char *utf=s1;
	unsigned short *uni=(unsigned short*)s2;
	if(*utf==0) return 0;
	for(n=0;*utf!=0;n++)
	{
		temp=utf8_to_unicode_one(utf,(unsigned long *)uni);
		utf+=temp;
		uni++;
	}
	*uni=0;
	return (n+1);
}
unsigned short unicode_to_utf8_str(unsigned char *s1,unsigned char *s2)
{
	unsigned  short n;
	unsigned char temp;
	unsigned short	*uni=(unsigned short*)s1;
	unsigned char 	*utf=s2;
	if(*uni==0) return 0;
	for(n=0;*uni!=0;n++)
	{
		temp=unicode_to_utf8_one((unsigned long)*uni,utf);
		utf+=temp;
		uni++;
	}
	*utf=0;
	return (n+1);
}