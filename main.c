#include <stdio.h>
#include <stdbool.h>

#define MSG_MAX_LENGTH 1024
#define OFFSET 97
#define CHARACTERS_SIMPLE 26
#define DEBUG false
#define EXTENDED true
#define MODE_ENCRYPT 0x01
#define MODE_DECRYPT 0x02

char charset[72] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
					'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
					'1','2','3','4','5','6','7','8','9','0',
					' ','.',',',';',':','-','_','(',')','@'};

int readSequenceStart();
int readMessage();
int readEncrypted();
int readPad();
int readMode();
int readCharset();
int encodeChar(char c);
char decodeChar(int n);
void encrypt();
void decrypt();

int mode;
int characters;
int sequence_start;
int msg_length;
int pad_length;
char seq_input_buf[2];
char msg_buf[MSG_MAX_LENGTH+1];
char msg_pad[MSG_MAX_LENGTH];
char msg_out[MSG_MAX_LENGTH+1];


void init() {
	sequence_start = -1;
	msg_length = 0;
	pad_length = 0;
	characters = 0;
	mode = 0;
}

int main (void) {
	init();
	printf("\n-----------------------\n");
	printf("One-Time Pad Calculator\n");
	printf("-----------------------\n");

	printf("\nSelect character set:\n");
	printf("\t1) Simple (a-z)\n\t2) Extended\n");
	while(characters == 0) {
		characters = readCharset();
	}

	printf("\nSelect operation mode:\n");
	printf("\t1) Encrypt message\n\t2) Decrypt message");
	while(mode == 0) {
		mode = readMode();
	}

	if (mode == MODE_ENCRYPT) encrypt();
	if (mode == MODE_DECRYPT) decrypt();
}

int readCharset() {
	char c = 0;
	int s = 0;
	printf("\t");
	bool read = true;
	while (read) {
		c = fgetc(stdin);
		if (c == 49) s = CHARACTERS_SIMPLE;
		if (c == 50) s = sizeof(charset);
		if (c == 10) read = false;
	}
	return s;
}

int readMode() {
	char c = 0;
	int s = 0;
	printf("\n\t");
	bool read = true;
	while (read) {
		c = fgetc(stdin);
		if (c == 49) s = 1;
		if (c == 50) s = 2;
		if (c == 10) read = false;
	}
	return s;
}

void encrypt() {
	while (sequence_start == -1) {
		sequence_start = readSequenceStart();
	}
	
	msg_length = readMessage();
	pad_length = readPad();
	if (pad_length != msg_length) {
		while (pad_length != msg_length) {
			printf("Invalid pad length, try again!");
			pad_length = readPad();
		}
	}

	printf("\n-------Encrypted-------\n");
	for (int i = 0; i < msg_length; i++) {
		char c = msg_buf[i];
		char p = msg_pad[i];
		int a = encodeChar(c) + encodeChar(p);
		int e = a % characters;

		#if DEBUG == true
			printf("%c=%d(%d)\t%c=%d(%d)\t(a)=%d\t(e)=%d\t(c)=%d\t%c\n",
			c,c,encodeChar(c), p,p,encodeChar(p), a, e, decodeChar(e), decodeChar(e));
		#else
			printf("%c", decodeChar(e));
		#endif
	}
	printf("\n-----------------------\n");
}

void decrypt() {
	while (sequence_start == -1) {
		sequence_start = readSequenceStart();
	}

	msg_length = readEncrypted();
	pad_length = readPad();
	if (pad_length != msg_length) {
		while (pad_length != msg_length) {
			printf("Invalid pad length, try again!");
			pad_length = readPad();
		}
	}

	printf("\n-------Decrypted-------\n");
	for (int i = 0; i < msg_length; i++) {
		char e = msg_buf[i];
		char p = msg_pad[i];

		int a = encodeChar(e) - encodeChar(p);
		int d = (characters + a) % characters;

		#if DEBUG == true
			printf("%c=%d(%d)\t%c=%d(%d)\t(a)=%d\t(d)=%d\t(c)=%d\t%c\n",
			e,e,encodeChar(e), p,p,encodeChar(p), a, d, decodeChar(d), decodeChar(d));
		#else
			printf("%c", decodeChar(d));
		#endif
	}
	printf("\n-----------------------\n");

}

int encodeChar(char c) {
	for (int i = 0; i < characters; i++) {
		if (charset[i] == c) {
			int n = (i+sequence_start)%characters;
			//if (n == 0) n = 1;
			return n;
		}
	}
	return -1;
}

char decodeChar(int n) {
	if (n >= 0 && n < characters) {
		n = n-sequence_start;
		if (n < 0) n = characters + n - 1;

		return charset[n];
	}
	return -1;
}

int readPad() {
	int i = 0;
	bool read = true;
	printf("Enter %d characters from your one-time pad: ",msg_length);
	while(read) {
		char c = fgetc(stdin);
		if (c != 10 && i < msg_length) {
			msg_pad[i++] = c;
		} else {
			read = false;
		}
	}
	return i;
}

int readMessage() {
	int i = 0;
	bool read = true;
	printf("Type in your message: ");
	while(read) {
		char c = fgetc(stdin);
		if (c != 10 && i < MSG_MAX_LENGTH) {
			msg_buf[i++] = c;
		} else {
			msg_buf[i] = 0x00;
			read = false;
		}
	}
	return i;
}

int readEncrypted() {
	int i = 0;
	bool read = true;
	printf("Type in the encrypted message: ");
	while(read) {
		char c = fgetc(stdin);
		if (c != 10 && i < MSG_MAX_LENGTH) {
			msg_buf[i++] = c;
		} else {
			msg_buf[i] = 0x00;
			read = false;
		}
	}
	return i;
}

int readSequenceStart() {
	int s = 0;
	bool read = true;
	printf("\nA is number: ");
	while(read) {
		char c = fgetc(stdin);
		if ((c < 48 || c > 57) && c != 10) {
			printf("Invalid input\n");
			read = false;
		} else {
			if (c == 10) {
				read = false;
				if (s < 1 || s > characters) {
					printf("Invalid input\n");
					return 0;
				} else {
					return s-1;
				}
			} else {
				if (s == 0) {
					s += c-48;
				} else {
					s *= 10;
					s += c-48;
				}
			}
		}
	}
	return 0;
}