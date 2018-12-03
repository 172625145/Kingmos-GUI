/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：BASE64加密与解密的实现
版本号：1.0.0
定版日期：2003-10-20
作者：魏海龙
修改记录：
******************************************************/

#define LWSP_CHAR_SPACE 0x20
#define LWSP_CHAR_HTAB  0x9
#define MIME_CR 0xd
#define MIME_LF 0xa 



#define BASE64_NEED_SPACE(len) (len/3*4+4)
#define QPRINT_NEED_SPACE(len) (len*3)

unsigned char base64_table[64] =
{
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V','W','X','Y','Z',
  'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
  'q','r','s','t','u','v','w','x','y','z',
  '0','1','2','3','4','5','6','7','8','9',
  '+','/'
};


const char * const mail_month[] =  
{
  "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

unsigned long base64_encode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr );
unsigned long base64_decode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr );
unsigned long quotedprint_encode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr );
unsigned long quotedprint_decode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr );

unsigned long base64_encode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr )
{
  unsigned char * pData,TempByte;
  //unsigned int dst_len;

  if( !src_len )
    return 0;
  pData = dst_ptr;

  while( src_len )
  {  
    TempByte = *src_ptr++;
    * pData++ = base64_table[TempByte >> 2];
    if( !--src_len )
    {//stuff 0 and '='
      * pData++ = base64_table[ ((TempByte<<4) & 0x30) ];
      * pData++ = '=';
      * pData++ = '=';
      break;
    }
    * pData++ = base64_table[ ((TempByte<<4) & 0x30) | (*src_ptr>>4) ];
    if( !--src_len )
    {//stuff 0 and '='
      * pData++ = base64_table[(((*src_ptr++)<<2) & 0x3c)];
      * pData++ = '=';
      break;
    }
    TempByte = ((*src_ptr++)<<2) & 0x3c;
    * pData++ = base64_table[ TempByte | (*src_ptr>>6) ];
    * pData++ = base64_table[ (*src_ptr++) & 0x3f ];
    src_len--;
  }
 
  return pData-dst_ptr;
}

unsigned long base64_decode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr )
{
  unsigned char * pData,TempByte;
  unsigned long i=0;//,dst_len;

  if( !src_len )
    return 0;

  //dst_len = (src_len/4)*3;
  pData = dst_ptr;
  while( src_len-- )
  {
    TempByte = *src_ptr++;
    switch( TempByte )
    {
    case '=':
      //(*dst_ptrLen) --;
      //-- dst_len;
      continue;

    case '+':
      TempByte = 62;
      break;

    case '/':
      TempByte = 63;
      break;

    default:
      if( TempByte >= 'a' && TempByte <= 'z' )
        TempByte = TempByte - 'a' + 26;
      else
      {
        if( TempByte >= 'A' && TempByte <= 'Z' )
          TempByte -= 'A';
        else
        {
          if( TempByte >= '0' && TempByte <= '9' )
            TempByte = TempByte - '0' + 52;
          else
            continue;//skip the octet,may exist invalid octet
        }
      }
      break;
    }
 
    switch( i )
    {
    case 0:
      * pData = TempByte << 2;
      i++;
      break;

    case 1:
      * pData++ |= TempByte >> 4;
      * pData = TempByte << 4;
      i++;
      break;

    case 2:
      * pData++ |= TempByte >> 2;
      * pData = TempByte << 6;
      i++;
      break;

    case 3:
      * pData++ |= TempByte;
      i = 0;
      break;
    }
 }
 
  //return dst_len;
  return pData - dst_ptr;
}

unsigned long quotedprint_encode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr )
{
  unsigned char * pData,* pEnd,TempByte,i;

  if( !src_len )
    return 0;

  pData = dst_ptr;
  pEnd = src_ptr + src_len;
  i=0;
  while( src_ptr < pEnd )
  {
    TempByte = *src_ptr++;
    if( (TempByte >= 33 && TempByte <= 60) ||
        (TempByte >= 62 && TempByte <= 126 ) )
    {
      i++;
      if( i >= 76 )
      {
        * pData ++ = '=';
        * pData ++ = MIME_CR;
        * pData ++ = MIME_LF;
        i = 0;
      }
      * pData ++ = TempByte;
    }
    else
    {
      i += 3;
      if( i >= 76 )
      {
        * pData ++ = '=';
        * pData ++ = MIME_CR;
        * pData ++ = MIME_LF;
        i = 0;
      }
      * pData ++ = '=';
      * pData = TempByte >> 4;
      if( *pData >= 0xa )
        *pData += 'A' - 0xa;
      else
        *pData += '0';
      * ++pData = TempByte & 0xf;
      if( *pData >= 0xa )
        *pData += 'A' - 0xa;
      else
        *pData += '0';
      pData ++;
    }
  }

  return pData-dst_ptr;
}

unsigned long quotedprint_decode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr )
{
  unsigned char * pData,* pEnd;
  unsigned char TempByte;

  if( !src_len )
    return 0;

  pData = dst_ptr;
  pEnd = src_ptr+src_len;
  while( src_ptr < pEnd )
  {
    if( * src_ptr == '=' )
    {//sequence "=CRLF" is software CRLF,must be deleted
      if( *(src_ptr+1)!=MIME_CR || *(src_ptr+2)!=MIME_LF )
      {
        if( *(src_ptr+1) > '9' )
          TempByte = *(src_ptr+1) - 'A' + 0xa;
        else
          TempByte = *(src_ptr+1) - '0';
        TempByte <<= 4;
        if( *(src_ptr+2) > '9' )
          TempByte |= *(src_ptr+2) - 'A' + 0xa;
        else
          TempByte |= *(src_ptr+2) - '0';
        * pData++ = TempByte;
      }
      src_ptr += 3;
    }
    else
      * pData ++ = * src_ptr++;
  }

  return pData - dst_ptr;
}
