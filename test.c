//#include <bsd/string.h>

int main()
{
	char a[10];
	const char *b = "asdfghjklkjhgfd";
	strlcpy(a, b, 5);
	return 0;
}
