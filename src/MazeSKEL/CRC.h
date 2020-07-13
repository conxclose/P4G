// by Mark Featherstone (C) 2013 All Rights Reserved.

#pragma once
#ifndef _MPCRC_H
#define _MPCRC_H


/*
Cyclic Redundancy Check
Primarily used to check transmitted data or saved data for tampering
Generates a unique code based on a large amount of data, if the data
is changed you don't get the same generated code again.
Can also generate unique IDs from text names, etc.
*/
class SCRC
{
public:
	//the smaller the CRC the more likely it is to NOT be unique
	static unsigned int GenerateCRC32( const char *data, unsigned int dataLength );
	static unsigned short GenerateCRC16( const char *data, unsigned int dataLength );
	static unsigned char GenerateCRC8( const char *data, unsigned int dataLength );

	SCRC()
	{
		Init_CRC_Table();
	}

private:
	void Init_CRC_Table();
	static unsigned int Reflect(unsigned int ref, char ch);
	static unsigned int crc_table[256];

};



#endif
