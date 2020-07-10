// by Mark Featherstone (C) 2013 All Rights Reserved.
#include "CRC.h"



unsigned int SCRC::crc_table[256];

// required for setting up the CRC table
SCRC g_CRC;

unsigned int SCRC::Reflect(unsigned int ref, char ch) 
{
      unsigned int value=0;

      // Swap bit 0 for bit 7 
      // bit 1 for bit 6, etc. 
      for(int i = 1; i < (ch + 1); i++) 
      { 
            if(ref & 1) 
                  value |= 1 << (ch - i); 
            ref >>= 1; 
      } 
      return value; 
} 

//set the table up with the codes we need
void SCRC::Init_CRC_Table()
{
      unsigned int ulPolynomial = 0x04c11db7; 

      for(int i = 0; i <= 0xFF; i++) 
      { 
            crc_table[i]=Reflect(i, 8) << 24; 
            for (int j = 0; j < 8; j++) 
                  crc_table[i] = (crc_table[i] << 1) ^ (crc_table[i] & (1 << 31) ? ulPolynomial : 0); 
            crc_table[i] = Reflect(crc_table[i], 32); 
      } 
} 

unsigned int SCRC::GenerateCRC32( const char *data, unsigned int dataLength )
{
	unsigned int ulCRC=0xffffffff;
	unsigned char* buffer; 

	buffer = (unsigned char*)data;
	while(dataLength--)  
	{
		unsigned int tableIdx = (ulCRC&0xff) ^ *buffer;

		ulCRC >>= 8;
		ulCRC ^= crc_table[tableIdx];
		//ulCRC = (ulCRC >> 8) ^ crc_table[tableIdx];
		buffer++;
	}
	return (ulCRC ^ 0xffffffff);
} 

unsigned short SCRC::GenerateCRC16( const char *data, unsigned int dataLength )
{
	unsigned int val;
	unsigned short ret;

	val = GenerateCRC32( data, dataLength );
	ret = ((unsigned short)(val&0xffff))^((unsigned short)((val>>16)&0xffff));

	return ret;
}

unsigned char SCRC::GenerateCRC8( const char *data, unsigned int dataLength )
{
	unsigned int val;
	unsigned char ret;

	val = GenerateCRC32( data, dataLength );
	ret = ((unsigned char)(val&0xff))^((unsigned char)((val>>8)&0xff))^((unsigned char)((val>>16)&0xff))^((unsigned char)((val>>24)&0xff));

	return ret;
}

