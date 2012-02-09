#include "endianio.h"



VOID PutByte(BYTE v, FILE *fp)
{
  fputc(v, fp);
}

VOID PutLittleWord(WORD v, FILE *fp)
{
  unsigned char c1 = v & 0xff;
  unsigned char c2 = (v >> 8) & 0xff;
  fputc(c1, fp);
  fputc(c2, fp);
}

VOID PutLittleDword(DWORD v, FILE *fp)
{
  unsigned char c1 = v & 0xff;
  unsigned char c2 = (v >> 8) & 0xff;
  unsigned char c3 = (v >> 16) & 0xff;
  unsigned char c4 = (v >> 24) & 0xff;

  fputc(c1, fp);
  fputc(c2, fp);
  fputc(c3, fp);
  fputc(c4, fp);
}

