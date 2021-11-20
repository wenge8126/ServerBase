#if __LINUX__
#include "BaseCommon.h"
#include <stdio.h>

void   _itoa_s(unsigned   long   val, char   *buf, int len, unsigned   radix)
{
	snprintf(buf, len, "%d", val);

	//char   *p;                                 /*   pointer   to   traverse   string   */
	//char   *firstdig;                   /*   pointer   to   first   digit   */
	//char   temp;                             /*   temp   char   */
	//unsigned   digval;                 /*   value   of   digit   */

	//p = buf;
	//firstdig = p;                       /*   save   pointer   to   first   digit   */

	//do {
	//	digval = (unsigned)(val   %   radix);
	//	val /= radix;               /*   get   next   digit   */

	//	/*   convert   to   ascii   and   store   */
	//	if (digval > 9)
	//		*p++ = (char)(digval - 10 + 'a ');     /*   a   letter   */
	//	else
	//		*p++ = (char)(digval + '0 ');               /*   a   digit   */
	//} while (val > 0);

	///*   We   now   have   the   digit   of   the   number   in   the   buffer,   but   in   reverse
	//	  order.     Thus   we   reverse   them   now.   */

	//*p-- = '\0 ';                         /*   terminate   string;   p   points   to   last   digit   */

	//do {
	//	temp = *p;
	//	*p = *firstdig;
	//	*firstdig = temp;       /*   swap   *p   and   *firstdig   */
	//	--p;
	//	++firstdig;                   /*   advance   to   next   two   digits   */
	//} while (firstdig < p);   /*   repeat   until   halfway   */
}
//void sprintf_s(char *sz, int x, const char *format, ...)
//{
//	//???
//}
void _i64toa_s(Int64 x, char *sz, int maxSize, int f)
{
	snprintf(sz, maxSize, "%lld", x);
}
#endif