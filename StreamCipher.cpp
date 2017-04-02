#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>  
#include <ctype.h>

#define MAX_KEY_LEN 2048
#define MAX_CHAR_LEN 30000

// ���Է�����λ�Ĵ���
// n�����Է����Ĵ�������Կ���������е��������֣��������ɿ�����������״̬ת�����С�
class LFSR
{
private:
	unsigned int n;			// n������
	unsigned int a[16];		// ��i���洢��
	unsigned int *s;		// ��Կ���������
	bool c[16];				// LFSR���������ĳ���
	unsigned char *key;		// ��Կ
	unsigned long long keyL;// ��Կlong long��
	unsigned int keyLen;

	/**
	* LFSR�ķ�������
	*
	* @param cur
	*	��n��������
	* @return ��������ֵ
	*
	* TODO(lingxiao007a@gmail.com): ʹ��Curֵ����ʵ��a�������ѭ�����ơ�
	*/
	unsigned int f(int cur) {
		unsigned int res = 0;	// ���ؽ��
		unsigned int t = 0;		// ���Ա���c
		unsigned int i = cur;	// ʹ��Curֵ����ʵ��a�������ѭ������

								// F(a[]) =  c[0]a[n-1] ^ c[1]a[n-2] ^ ...
		while (t < n) {
			res ^= a[i] * c[t];
			// i = (i - 1) mod 16
			i = (i - 1 + 16) % 16;
			t++;
		}
		return res;
	}

public:
	LFSR() {
		s = (unsigned int *)malloc(sizeof(unsigned int) * 65535);
		key = (unsigned char *)malloc(sizeof(unsigned char) * MAX_KEY_LEN);
	}
	/**
	* LFSR��ʼ��
	*
	* @param c[16]
	*	����������������
	* @param K[16]
	*   ��Կ
	* @param N
	*	N��������λ��
	* @return
	*/
	void init(
		bool c[16],			// ��������ciֵ
		unsigned char K[],	// ��Կ
		unsigned int kLen,	// ��Կ����
		int N = 16				// n��������λ��
		) {
		n = N;
		keyLen = kLen;
		keyL = 0;
		/* ��ʼLFSR���������ĳ��� */
		for (int i = 0; i < n; i++) this->c[i] = c[i];
		/* �����Կ */
		// 0 ~ 9�� a(A) = 10, b(B) = 11 ... z(Z) = 35
		for (int i = 0; i < keyLen; i++) {
			key[i] = K[i];
			keyL *= 35;
			if (isdigit(K[i])) keyL += K[i] - '0';
			else if (isalpha) {
				keyL += K[i] - 'A';
				if (keyL >= 'a') keyL -= 0x20;
			}
		}
		/* ��ʼ������ */
		for (int i = 0; i < 16; i++)
			a[i] = i % 2;	// �ȳ�ʼ0101...
		for (int i = 0; i < 16; i++)
			a[i] ^= bool(keyL & (1 << i)) ^ c[i]; // ��keyL�ĵ�iλ��c[i]�����
	}

	/**
	* ����������
	* @return LFSR���������
	*/
	unsigned int *getSeq() { return s; }

	/*
	* LFSR״̬ת������������
	* @return
	*/
	void generator() {
		// �����ʼ������ֵ
		printf("Before LSFR(Const c: ");
		for (int i = 0; i < 16; i++)
			printf("%d", c[i]);
		puts(")generator...");
		for (int i = 0; i < 16; i++)
			printf("%d", a[i]);
		puts("");

		int cur = n - 1, endpos = 2 << n - 1;
		for (int i = 0; i < endpos; i++) {
			s[i] = a[cur];		// �������
			a[cur] = f(cur);	// ��������
			if (--cur < 0) cur = n - 1; // ����ѭ������
		}

	}
};

// ������Geffe����������
// ��Կ���������ķ�������ϲ���
class Geffe
{
private:
	// 3��LFSR
	// LFSR[1]����������������ʹ��
	// LFSR[1] = 1, then LFSR[0] ^ LFSR[1], else LFSR[1] ^ LFSR[2]
	LFSR m_sLFSR[3];
	unsigned char *key;				// ��ԿMAX_KEY_LEN
									//unsigned char key[MAX_KEY_LEN];	
	unsigned int keyLen;			// ��Կ����
	unsigned char *keyStream;		// ��Կ��65535

public:
	Geffe() {
		key = (unsigned char *)malloc(sizeof(unsigned char) * MAX_KEY_LEN);
		keyStream = (unsigned char *)malloc(sizeof(unsigned char) * 65535);
	}
	/*
	* Geffe��ʼ��
	*
	* @param K[]
	*	��Կ
	* @param kLen
	*	��Կ����
	* @param c[i][]
	*	LFSR[i]�ĳ���
	* @return
	*/
	void init(unsigned char K[], unsigned int kLen, bool c[3][16]) {
		keyLen = kLen;
		for (int i = 0; i < keyLen; i++) key[i] = K[i];		// ��ʼ��Կ
		for (int i = 0; i < 3; i++) m_sLFSR[i].init(c[i], key, keyLen);	// ��ʼLFSR[i]
	}

	/*
	* Geffe����������
	* @return
	*/
	void generator() {
		/* ����LFSR������ */
		for (int i = 0; i < 3; i++)
			m_sLFSR[i].generator();

		/* ���LFSR���� */
		unsigned int *ctl = m_sLFSR[1].getSeq();
		unsigned int *s1 = m_sLFSR[0].getSeq();
		unsigned int *s2 = m_sLFSR[2].getSeq();

		/* ���LFSR����ǰ100λ */
		puts("LFSR1: ");
		for (int i = 0; i < 100; i++)
			printf("%d", s1[i]);
		puts("");

		puts("LFSR2: ");
		for (int i = 0; i < 100; i++)
			printf("%d", ctl[i]);
		puts("");

		puts("LFSR3: ");
		for (int i = 0; i < 100; i++)
			printf("%d", s1[i]);
		puts("");

		/* Geffe������������Կ�� */
		for (int i = 0; i < 65535; i++) {
			if (ctl[i] == 1) {
				keyStream[i] = ctl[i] ^ s1[i];
			}
			else {
				keyStream[i] = ctl[i] ^ s2[i];
			}
		}
	}

	/*
	* ����
	*
	* @param message[]
	*	����
	* @param *cipherText
	*	����
	* @return
	*/
	void encrypt(unsigned char message[], unsigned char *cipherText) {
		int i;
		// c = (m + k) ^ keyStream
		for (i = 0; message[i]; i++)
			cipherText[i] = (message[i] + key[i % keyLen]) ^ keyStream[i % 65535];
		cipherText[i] = 0;	// �ַ�����ֹ��
	}

	/*
	* ����
	*
	* @param *cipherText
	*	����
	* @param message[]
	*	����
	* @return
	*/
	void decrypt(unsigned char cipherText[], unsigned char *message) {
		int i;
		// m = c ^ keyStream - k
		for (i = 0; cipherText[i]; i++)
			message[i] = (cipherText[i] ^ keyStream[i % 65535]) - key[i % keyLen];	// ���
		message[i] = 0;	// �ַ�����ֹ��
	}

};

int main(int argc, char const *argv[])
{
	unsigned char message[MAX_CHAR_LEN];	// ����
	unsigned char cipherText[MAX_CHAR_LEN];	// ����
	unsigned char key[MAX_KEY_LEN];			// ��Կ
	int mCnt = 0;							// ���ĳ���
	int opt = 0;
	char ch;
	/* ���ײ˵� */
	puts("----------------------------------------------");
	puts("|                    MENU                    |");
	puts("|--------------------------------------------|");
	puts("|           1. �ӿ���̨��ȡ����              |");
	puts("|           2. ���ļ��ж�ȡ����              |");
	puts("----------------------------------------------");
	/* ѡ��˵��� */
	while (true) {
		scanf("%c", &ch);
		if (ch == '1' || ch == '2') {
			opt = ch - '0';
			break;
		}
		if (ch == '\n') puts("���벻�Ϸ��������䡣");
	}
	char filename[256];						// �ļ���

	/* �ӿ���̨��ȡ���� */
	if (opt == 1) {
		mCnt = 0;
		puts("����������: ");
		while (!isalnum(ch = getchar()));
		message[mCnt++] = ch;
		while ((ch = getchar()) != '\n')
			message[mCnt++] = ch;
		message[mCnt] = 0;
		printf("%s\n", message);
	}
	/* ���ļ��ж�ȡ���� */
	else {
		FILE * fp;								// �ļ�ָ��
		puts("�������ļ���: ");
		scanf("%s", filename);

		if ((fp = fopen(filename, "r")) == NULL) {	// ֻ����
			printf("No dirtory or file. Now exit.\n");
			fclose(fp);
			return 1;
		}
		mCnt = 0;
		fseek(fp, 0, SEEK_SET);			// ����ƫ����Ϊ0
		while ((ch = fgetc(fp)) != EOF)	// ���ַ���ȡ
			message[mCnt++] = ch;
		message[mCnt] = 0;				// ��ֹ��

		fclose(fp);
	}

	/* Input a key */
	printf("Password: \n");
	int kCnt = 0;
	while (!isalnum(ch = getchar()));
	while ((ch = getchar()) != '\n')
		key[kCnt++] = ch;
	key[kCnt] = 0;

	/* ����LFSR���Է����Ĵ�������״̬���� */
	bool c[3][16];
	memset(c, 0, sizeof(c));

	/* ��ʼ��LFSR���������ĳ��� */
	// f1(x) = x ^ 15 + x ^ 14 + x ^ 13 + x ^ 10 + x ^ 7 + 1��
	c[0][15] = c[0][14] = c[0][13] = c[0][10] = c[0][7] = c[0][0] = 1;
	// f2(x) = x ^ 15 + x ^ 13 + x ^ 12 + x ^ 10 + x ^ 7 + x ^ 6 + x ^ 5 + 1;
	c[1][15] = c[1][13] = c[1][12] = c[1][10] = c[1][7] = c[1][6] = c[1][5] = c[1][0] = 1;
	// f3(x) = x ^ 15 + x ^ 13 + x ^ 12 + x ^ 11 + x ^ 10 + x ^ 7 + x ^ 6 + x + 1
	c[2][15] = c[2][13] = c[2][12] = c[2][11] = c[2][10] = c[2][7] = c[2][6] = c[2][1] = c[2][0] = 1;

	/* ����Geffe���������������� */
	Geffe geffe;
	geffe.init(key, kCnt, c);
	geffe.generator();

	/* Cipher Text Generate*/
	printf("Encrypting...\n");
	geffe.encrypt(message, cipherText);

	/* Save cipher text*/
	if (opt == 2) {
		sprintf(filename, "LSFR_%s", filename);
		FILE *fp;
		fp = fopen(filename, "w");
		for (int i = 0; i < mCnt; i++)
			putc(cipherText[i], fp);
		fclose(fp);
	}
	/* Or print cipher */
	else {
		printf("%s\n", cipherText);
	}

	/* Decrpty */
	printf("Decrypting...\n");
	unsigned char msg[MAX_CHAR_LEN];
	/* Print message */
	geffe.decrypt(cipherText, msg);
	printf("%s\n", msg);

	return 0;
}
