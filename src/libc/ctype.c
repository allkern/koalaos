#include "ctype.h"

int isdigit(int c) {
	return ((unsigned)c-'0') < 10;
}

int isspace(int c) {
	return c == ' ' || (unsigned)c-'\t' < 5;
}

int isalpha(int c) {
	return ((unsigned)c|32)-'a' < 26;
}

int isupper(int c) {
	return (unsigned)c-'A' < 26;
}

int isprint(int c) {
	return (unsigned)c-0x20 < 0x5f;
}