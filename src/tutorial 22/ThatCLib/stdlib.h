// A basic C Library written from scratch - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/ThatCLib

#ifndef STDLIB_H
#define STDLIB_H

#define assert(assertion) ((void)((assertion) \
		|| (__assert_fail(#assertion, __FILE__, __LINE__, __func__), 0)))
		
void itoa(unsigned long int n, unsigned short int* buffer, int basenumber)
{
	unsigned long int hold;
	int i, j;
	hold = n;
	i = 0;
	
	do{
		hold = n % basenumber;
		buffer[i++] = (hold < 10) ? (hold + '0') : (hold + 'a' - 10);
	} while(n /= basenumber);
	buffer[i--] = 0;
	
	for(j = 0; j < i; j++, i--)
	{
		hold = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = hold;
	}
}

#endif // STDLIB_H