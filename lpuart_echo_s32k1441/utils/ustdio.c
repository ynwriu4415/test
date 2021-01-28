/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>  // for isdigit

#include "ustdio.h"

/* Private types ------------------------------------------------------------*/

/* Private macro ------------------------------------------------------------*/

#define ZEROPAD 1		/* pad with zero */
#define SIGN    2		/* unsigned/signed long */
#define PLUS    4		/* show plus */
#define SPACE   8		/* space if plus */
#define LEFT    16		/* left justified */
#define SMALL   32		/* Must be 32 == 0x20 */
#define SPECIAL	64		/* 0x */

/* Private variables ------------------------------------------------------------*/

/* Public variables ------------------------------------------------------------*/

fmt_puts_t current_puts = NULL;
fmt_puts_t default_puts = NULL;


const char none        []= "\033[0m";  
const char black       []= "\033[0;30m";  
const char dark_gray   []= "\033[1;30m";  
const char blue        []= "\033[0;34m";  
const char light_blue  []= "\033[1;34m";  
const char green       []= "\033[0;32m";  
const char light_green []= "\033[1;32m";  
const char cyan        []= "\033[0;36m";  
const char light_cyan  []= "\033[1;36m";  
const char red         []= "\033[0;31m";  
const char light_red   []= "\033[1;31m";  
const char purple      []= "\033[0;35m";  
const char light_purple[]= "\033[1;35m";  
const char brown       []= "\033[0;33m";  
const char yellow      []= "\033[1;33m";  
const char light_gray  []= "\033[0;37m";  
const char white       []= "\033[1;37m"; 

char * default_color = (char *)cyan;


/* Gorgeous Split-line -----------------------------------------------*/


#ifdef __GNUC__ //for TrueStudio ,Makefile

/*
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
PUTCHAR_PROTOTYPE
{
	char  cChar = (char)ch;
	if (current_puts)
		current_puts(&cChar,1);
	return ch;
}
*/
//C/C++ build->Settings->Tool Settings->C Linker->Miscellaneous->Other options 閫夐」绌轰腑濉啓锛�-u_printf_float

int _write(int file, char *ptr, int len)
{
	if (current_puts)
		current_puts(ptr,len);
	return len;
}


#else // for keil5


#pragma import(__use_no_semihosting)
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;
void _sys_exit(int x) 
{ 
	x = x; 
}

int fputc(int ch, FILE *f)
{
	char  cChar = (char)ch;
	if (current_puts)
		current_puts(&cChar,1);
	return ch;
}


#endif 



static char *number(char *str, long num,int base, int size, int precision,
                    int type)
{
	/* we are called with base 8, 10 or 16, only, thus don't need "G..." */
	static const char digits[16] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */
	unsigned long unum ;
	char tmp[66];
	char sign, locase,padding;
	int chgsize;

	/* locase = 0 or 0x20. ORing digits or letters with 'locase'
	 * produces same digits or (maybe lowercased) letters */
	locase = (type & SMALL);
	if (type & LEFT)
		type &= ~ZEROPAD;  

	padding = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} 
		else 
		if (type & PLUS) {
			sign = '+';
			size--;
		} 
		else 
		if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}

	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else
		if (base == 8)
			size--;
	}

	chgsize = 0;
	unum = (unsigned)num;
	do {
		tmp[chgsize] = (digits[unum % base] | locase) ;
		++chgsize;
		unum /= base;
	}while(unum);

	if (precision < chgsize)
		precision = chgsize;

	size -= precision;
	if (!(type & (ZEROPAD + LEFT))){
		for ( ; size > 0 ; --size )
			*str++ = ' ';
	}
	
	if (sign)
		*str++ = sign;
	
	if (type & SPECIAL) {
		if (base == 8)
			*str++ = '0';
		else 
		if (base == 16) {
			*str++ = '0';
			*str++ = ('X' | locase);
		}
	}

	if (!(type & LEFT)){
		for ( ; size > 0 ; --size)
			*str++ = padding ;
	}

	for ( ; chgsize < precision ; --precision ) 
		*str++ = '0';

	for ( ; chgsize > 0 ; *str++ = tmp[--chgsize]);

	for ( ; size > 0 ; --size)
		*str++ = ' ' ;

	return str;
}

static char * float2string(char *str,float num, int size, int precision,int type)
{
	char tmp[66];
	char sign,padding;
	int chgsize;
	unsigned int ipart ;

	if (type & LEFT)
		type &= ~ZEROPAD;

	padding = (type & ZEROPAD) ? '0' : ' ';

	if (precision < 0 || precision > 6) // 绮惧害锛屾澶勭簿搴﹂檺鍒朵负鏈�澶� 6 浣嶅皬鏁�
		precision = 6;

	if (num < 0.0f) {  // 濡傛灉鏄礋鏁帮紝鍒欏厛杞崲涓烘鏁帮紝骞跺崰鐢ㄤ竴涓瓧鑺傚瓨鏀捐礋鍙�
		sign = '-';
		num  = -num ;
		size--;
	}
	else 
		sign = 0;

	chgsize = 0;
	ipart = (unsigned int)num; // 鏁存暟閮ㄥ垎

	if (precision) {           // 濡傛灉鏈夊皬鏁拌浆鎹紝鍒欐彁鍙栧皬鏁伴儴鍒�
		static const float mulf[7] = {
			1.0f,10.0f,100.0f,1000.0f,10000.0f,100000.0f,1000000.0f};
		unsigned int fpart = (unsigned int)((num - (float)ipart) * mulf[precision]) ;
		
		for(int i = 0 ; i < precision ; ++i) { 
			tmp[chgsize++] = (char)(fpart % 10 + '0');
			fpart /= 10;
		}
		tmp[chgsize++] = '.';
	}

	do {
		tmp[chgsize++] = (char)(ipart % 10 + '0');
		ipart /= 10;
	}while(ipart);

	size -= chgsize;                 // 鍓╀綑闇�瑕佸～鍏呯殑澶у皬

	if (!(type & LEFT)){             // 鍙冲榻�
		if ('0' == padding && sign) {// 濡傛灉鏄～鍏� 0 涓斾负璐熸暟锛屽厛鏀剧疆璐熷彿
			*str++ = sign;
			sign = 0;
		}
		for ( ; size > 0 ; --size)   // 濉厖 0
			*str++ = padding ;
	}

	if (sign)
		*str++ = sign;

	for ( ; chgsize > 0 ; *str++ = tmp[--chgsize]);

	for ( ; size > 0 ; --size)   // 宸﹀榻愭椂锛屽～鍏呭彸杈圭殑绌烘牸
		*str++ = ' ' ;

	return str;
}

void printk(const char* fmt, ...) 
{
	if (!current_puts) // 鏃犵‖浠惰緭鍑猴紝杩斿洖
		return ;

	char tmp[88] ;      // 姝ゆ鍐呭瓨浠呯敤浜庣紦瀛樻暟瀛楄浆鎹㈡垚鐨勫瓧绗︿覆
	char * substr;
	unsigned long num;
	int len , base;
	int flags;          /* flags to number() */
	int field_width;    /* width of output field */
	int precision;      /* min. # of digits for integers; max
                           number of chars for from string */
	int qualifier;      /* 'h', 'l', or 'L' for integer fields */

	char * fmthead = (char *)fmt;
	char * fmtout = fmthead;

	va_list args;
	va_start(args, fmt);

	for ( ; *fmtout; ++fmtout) {
		if (*fmtout == '%') {
			char * str = tmp ;

			if (fmthead != fmtout)   { //鍏堟妸 % 鍓嶉潰鐨勯儴鍒嗚緭鍑�
				current_puts(fmthead,fmtout - fmthead);
				fmthead = fmtout;
			}

			/* process flags */
			flags = 0;
			base  = 0;
			do {
				++fmtout; /* this also skips first '%' */
				switch (*fmtout) {
					case '-': flags |= LEFT;    break;
					case '+': flags |= PLUS;    break;
					case ' ': flags |= SPACE;   break;
					case '#': flags |= SPECIAL; break;
					case '0': flags |= ZEROPAD; break;
					default : base = 1;
				}
			}while(!base);

			/* get field width */
			if (isdigit(*fmtout)) {
				field_width = 0 ;
				do {
					field_width = field_width * 10 + *fmtout - '0';
					++fmtout;
				}while(isdigit(*fmtout));
				if (field_width > sizeof(tmp))
					field_width = sizeof(tmp);
			}
			else 
				field_width = -1;

			/* get the precision */
			if (*fmtout == '.') {
				precision = 0;
				for (++fmtout ; isdigit(*fmtout) ; ++fmtout) 
					precision = precision * 10 + *fmtout - '0';
				if (precision > sizeof(tmp))
					precision = sizeof(tmp);
			}
			else
				precision = -1;

			/* get the conversion qualifier *fmt == 'h' ||  || *fmt == 'L'*/
			if (*fmtout == 'l') {
				qualifier = *fmtout;
				++fmtout;
			}
			else 
				qualifier = -1;

			/* default base */
			base = 10;

			switch (*fmtout) {
				case 'c':
					if (!(flags & LEFT))
						for ( ; --field_width > 0 ; *str++ = ' '); // 鍙冲榻愶紝琛ュ叏宸﹁竟鐨勭┖鏍�
					*str++ = (char)va_arg(args, int);
					for ( ; --field_width > 0 ; *str++ = ' ') ;    // 宸﹀榻愶紝琛ュ叏鍙宠竟鐨勭┖鏍�
					current_puts(tmp,str-tmp);
					fmthead = fmtout + 1;
					continue;

				case 's':
					substr = va_arg(args, char *); 
					if (!substr)
						substr = "(NULL)";
					str = substr ;
					if (precision > 0)
						while(*str++ && --precision);
					else 
						while(*str++);
					len = str - substr;          // 鍏跺疄灏辨槸涓轰簡瀹炵幇 strnlen 锛屾澶勪笉甯屾湜鍐嶈繘琛屽嚱鏁板帇鏍�
					str = tmp;
					if ((!(flags & LEFT)) && (len < field_width)){  // 鍙冲榻愪笖闇�瑕佽ˉ鍏ㄧ┖鏍�
						do{*str++ = ' ';}while(len < --field_width);// 濉厖绌烘牸涓�
						current_puts(tmp,str-tmp);
					} 
					current_puts(substr,len);                       // 杈撳嚭瀛愬瓧绗︿覆
					if (len < field_width) {                        // 宸﹀榻愪笖闇�瑕佽ˉ鍏ㄥ彸杈圭┖鏍�
						do{*str++ = ' ';}while(len < --field_width);
						current_puts(tmp,str-tmp);
					}
					fmthead = fmtout + 1;
					continue;

				case 'p':
					if (field_width == -1) {
						field_width = 2 * sizeof(void *);
						flags |= ZEROPAD;
					}
					str = number(tmp,
						(unsigned long)va_arg(args, void *), 16,
						field_width, precision, flags);
					current_puts(tmp,str-tmp);
					fmthead = fmtout + 1;
					continue;

				case 'f':
					str = float2string(tmp,va_arg(args, double),field_width, precision, flags);
					current_puts(tmp,str-tmp);
					fmthead = fmtout + 1;
					continue;

				/* case '%':
					*str++ = '%';
					continue;*/

				/* integer number formats - set up the flags and "break" */
				case 'o':
					base = 8;
					break;

				case 'x':
					flags |= SMALL;
				case 'X':
					base = 16;
					break;

				case 'd':
				case 'i':
					flags |= SIGN;
				case 'u':
					break;

				default: 
					continue;
			}// switch()

			if (qualifier == 'l')
				num = va_arg(args, unsigned long);
			else 
				num = va_arg(args, int);

			str = number(tmp, num, base, field_width, precision, flags);
			current_puts(tmp,str-tmp);
			fmthead = fmtout + 1;
		}//if (*fmtout == '%')
	}
			
	if (fmthead != fmtout)
		current_puts(fmthead,fmtout - fmthead);
		
	va_end(args);
}

