#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#pragma warning(disable:4996)

void initialize(void);

static struct
{
	char name[3];
	char version[3];
	short xres, yres;
	unsigned short packed;
	char back_col_index;
	char aspect_ratio;
} gif_header;

void write_code(unsigned short code);

static FILE *fsave;
static short clear, codebits, colors, end, length, lastentry,
	nbits, nbytes, entries, actual,
	startbits, xres, yres;
static unsigned char buffer[16384],block[266],test[100];
static unsigned short hashcode, next, str_index[5003];

int	save_screen(const char *filename,const unsigned char *image_buffer,int wid,int hit,const unsigned char *pal)
{
	short i, row, col, color,temp;
	unsigned short hashentry;
	unsigned char bits;

	xres = (short) wid;
	yres = (short) hit;

	fsave = fopen(filename, "wb");

	if ( fsave== NULL )
	{
    return 0;
	}

	strcpy(gif_header.name,"GIF");
	strcpy(gif_header.version, "87a");

	gif_header.xres = (short) wid;
	gif_header.yres = (short) hit;
	gif_header.packed = 0xF7;
	bits = 8;
	colors = 256;


	gif_header.back_col_index = 0;
	gif_header.aspect_ratio = 0;
	fwrite(&gif_header,1,13,fsave);

	fwrite(pal,1,768,fsave);

	fputc(',',fsave);
	fputc(0,fsave);
	fputc(0,fsave);
	fputc(0,fsave);
	fputc(0,fsave);
	fwrite(&xres,1,2,fsave);
	fwrite(&yres,1,2,fsave);
	fputc(0,fsave);
	startbits = bits+1;
	clear = 1 << (startbits - 1);
	end = clear+1;
	fputc(bits,fsave);
	codebits = startbits;
	nbytes = 0;
	nbits = 0;
	for (i = 0; i < 266; i++)
		block[i] = 0;
	initialize();

	for (row = 0; row < yres; row++)
	{
		for (col = 0; col < xres; col++)
		{
			color = (short) image_buffer[row*wid+col];
			test[0] = (unsigned char) ++length;
			test[length] = (unsigned char) color;
			switch(length)
			{
				case 1:
					lastentry = color;
					break;
				case 2:
					hashcode = 301 * (test[1]+1);
				default:
					hashcode *= (color + length);
					hashentry = ++hashcode % 5003;
					for( i = 0; i < 5003; i++)
					{
						hashentry = (hashentry + 1) % 5003;
						if (memcmp(&buffer[str_index[hashentry]+2],
							test,length+1)	== 0)
							break;
						if (str_index[hashentry] == 0)
							i = 5003;
					}
					if (str_index[hashentry] != 0 && length < 97)
					{
						memcpy(&lastentry,&buffer[str_index[hashentry]],2);
						break;
					}
					write_code(lastentry);
					entries++;
					if (str_index[hashentry] == 0)
					{
						temp = entries+end;
						str_index[hashentry] = next;
						memcpy(&buffer[next],&temp,2);
						memcpy(&buffer[next+2],test,length+1);
						next += length+3;
						actual++;
					}
					test[0] = 1;
					test[1] = (unsigned char) color;
					length = 1;
					lastentry = color;
					if ((entries+end) == (1<<codebits))
						codebits++;
					if ( entries + end > 4093 || actual > 3335
						|| next > 15379)
					{
						write_code(lastentry);
						initialize();
					}
			}
		}
	}
	write_code(lastentry);
	write_code(end);
	fputc(0,fsave);
	fputc(';',fsave);
	fclose(fsave);
	return(1);
}

void initialize(void)
{
	write_code(clear);
	entries = 0;
	actual = 0;
	next = 1;
	length = 0;
	codebits = startbits;
	buffer[0] = 0;
	memset(str_index,0x00,10006);
}

void write_code(unsigned short code)
{
	block[nbytes  ] |= ((code << nbits) & 0xFF);
	block[nbytes+1] |= ((code >> (8 - nbits)) & 0xFF);
	block[nbytes+2] |= (((code>>(8 - nbits)) >> 8) & 0xFF);
#pragma warning(disable:4244)
	nbits += codebits;

	while (nbits >= 8)
	{
		nbits -= 8;
		nbytes++;
	}

	if (nbytes < 251 && code != end) return;
	if (code == end)
	{
		while (nbits > 0)
		{
			nbits -= 8;
			nbytes++;
		}
	}
	fputc(nbytes,fsave);
	fwrite(block,nbytes,1,fsave);
	memcpy(block,&block[nbytes],5);
	memset(&block[5],0x00,260);
	nbytes = 0;
}

