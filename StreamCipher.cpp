#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>  
#include <ctype.h>

#define MAX_KEY_LEN 2048
#define MAX_CHAR_LEN 30000

// 线性反馈移位寄存器
// n级线性反馈寄存器是密钥流生成器中的驱动部分，用以生成控制生成器的状态转移序列。
class LFSR
{
private:
	unsigned int n;			// n级反馈
	unsigned int a[16];		// 第i级存储器
	unsigned int *s;		// 密钥流输出序列
	bool c[16];				// LFSR反馈函数的常数
	unsigned char *key;		// 密钥
	unsigned long long keyL;// 密钥long long型
	unsigned int keyLen;

	/**
	* LFSR的反馈函数
	*
	* @param cur
	*	第n级储存器
	* @return 反馈函数值
	*
	* TODO(lingxiao007a@gmail.com): 使用Cur值左移实现a数组相对循环右移。
	*/
	unsigned int f(int cur) {
		unsigned int res = 0;	// 返回结果
		unsigned int t = 0;		// 用以遍历c
		unsigned int i = cur;	// 使用Cur值左移实现a数组相对循环右移

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
	* LFSR初始化
	*
	* @param c[16]
	*	反馈函数常数数组
	* @param K[16]
	*   密钥
	* @param N
	*	N级反馈移位器
	* @return
	*/
	void init(
		bool c[16],			// 反馈函数ci值
		unsigned char K[],	// 密钥
		unsigned int kLen,	// 密钥长度
		int N = 16				// n级反馈移位器
		) {
		n = N;
		keyLen = kLen;
		keyL = 0;
		/* 初始LFSR反馈函数的常数 */
		for (int i = 0; i < n; i++) this->c[i] = c[i];
		/* 获得密钥 */
		// 0 ~ 9， a(A) = 10, b(B) = 11 ... z(Z) = 35
		for (int i = 0; i < keyLen; i++) {
			key[i] = K[i];
			keyL *= 35;
			if (isdigit(K[i])) keyL += K[i] - '0';
			else if (isalpha) {
				keyL += K[i] - 'A';
				if (keyL >= 'a') keyL -= 0x20;
			}
		}
		/* 初始化序列 */
		for (int i = 0; i < 16; i++)
			a[i] = i % 2;	// 先初始0101...
		for (int i = 0; i < 16; i++)
			a[i] ^= bool(keyL & (1 << i)) ^ c[i]; // 与keyL的第i位、c[i]做异或
	}

	/**
	* 获得输出序列
	* @return LFSR的输出序列
	*/
	unsigned int *getSeq() { return s; }

	/*
	* LFSR状态转移序列生成器
	* @return
	*/
	void generator() {
		// 输出初始储存器值
		printf("Before LSFR(Const c: ");
		for (int i = 0; i < 16; i++)
			printf("%d", c[i]);
		puts(")generator...");
		for (int i = 0; i < 16; i++)
			printf("%d", a[i]);
		puts("");

		int cur = n - 1, endpos = 2 << n - 1;
		for (int i = 0; i < endpos; i++) {
			s[i] = a[cur];		// 输出序列
			a[cur] = f(cur);	// 反馈函数
			if (--cur < 0) cur = n - 1; // 数组循环右移
		}

	}
};

// 非线性Geffe序列生成器
// 密钥流生成器的非线性组合部分
class Geffe
{
private:
	// 3个LFSR
	// LFSR[1]用来控制生成器的使用
	// LFSR[1] = 1, then LFSR[0] ^ LFSR[1], else LFSR[1] ^ LFSR[2]
	LFSR m_sLFSR[3];
	unsigned char *key;				// 密钥MAX_KEY_LEN
									//unsigned char key[MAX_KEY_LEN];	
	unsigned int keyLen;			// 密钥长度
	unsigned char *keyStream;		// 密钥流65535

public:
	Geffe() {
		key = (unsigned char *)malloc(sizeof(unsigned char) * MAX_KEY_LEN);
		keyStream = (unsigned char *)malloc(sizeof(unsigned char) * 65535);
	}
	/*
	* Geffe初始化
	*
	* @param K[]
	*	密钥
	* @param kLen
	*	密钥长度
	* @param c[i][]
	*	LFSR[i]的常数
	* @return
	*/
	void init(unsigned char K[], unsigned int kLen, bool c[3][16]) {
		keyLen = kLen;
		for (int i = 0; i < keyLen; i++) key[i] = K[i];		// 初始密钥
		for (int i = 0; i < 3; i++) m_sLFSR[i].init(c[i], key, keyLen);	// 初始LFSR[i]
	}

	/*
	* Geffe序列生成器
	* @return
	*/
	void generator() {
		/* 调用LFSR生成器 */
		for (int i = 0; i < 3; i++)
			m_sLFSR[i].generator();

		/* 获得LFSR序列 */
		unsigned int *ctl = m_sLFSR[1].getSeq();
		unsigned int *s1 = m_sLFSR[0].getSeq();
		unsigned int *s2 = m_sLFSR[2].getSeq();

		/* 输出LFSR序列前100位 */
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

		/* Geffe生成器产生密钥流 */
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
	* 加密
	*
	* @param message[]
	*	明文
	* @param *cipherText
	*	密文
	* @return
	*/
	void encrypt(unsigned char message[], unsigned char *cipherText) {
		int i;
		// c = (m + k) ^ keyStream
		for (i = 0; message[i]; i++)
			cipherText[i] = (message[i] + key[i % keyLen]) ^ keyStream[i % 65535];
		cipherText[i] = 0;	// 字符串终止符
	}

	/*
	* 解密
	*
	* @param *cipherText
	*	密文
	* @param message[]
	*	明文
	* @return
	*/
	void decrypt(unsigned char cipherText[], unsigned char *message) {
		int i;
		// m = c ^ keyStream - k
		for (i = 0; cipherText[i]; i++)
			message[i] = (cipherText[i] ^ keyStream[i % 65535]) - key[i % keyLen];	// 异或
		message[i] = 0;	// 字符串终止符
	}

};

int main(int argc, char const *argv[])
{
	unsigned char message[MAX_CHAR_LEN];	// 明文
	unsigned char cipherText[MAX_CHAR_LEN];	// 密文
	unsigned char key[MAX_KEY_LEN];			// 密钥
	int mCnt = 0;							// 明文长度
	int opt = 0;
	char ch;
	/* 简易菜单 */
	puts("----------------------------------------------");
	puts("|                    MENU                    |");
	puts("|--------------------------------------------|");
	puts("|           1. 从控制台读取明文              |");
	puts("|           2. 从文件中读取明文              |");
	puts("----------------------------------------------");
	/* 选择菜单项 */
	while (true) {
		scanf("%c", &ch);
		if (ch == '1' || ch == '2') {
			opt = ch - '0';
			break;
		}
		if (ch == '\n') puts("输入不合法，请重输。");
	}
	char filename[256];						// 文件名

	/* 从控制台读取明文 */
	if (opt == 1) {
		mCnt = 0;
		puts("请输入明文: ");
		while (!isalnum(ch = getchar()));
		message[mCnt++] = ch;
		while ((ch = getchar()) != '\n')
			message[mCnt++] = ch;
		message[mCnt] = 0;
		printf("%s\n", message);
	}
	/* 从文件中读取明文 */
	else {
		FILE * fp;								// 文件指针
		puts("请输入文件名: ");
		scanf("%s", filename);

		if ((fp = fopen(filename, "r")) == NULL) {	// 只读打开
			printf("No dirtory or file. Now exit.\n");
			fclose(fp);
			return 1;
		}
		mCnt = 0;
		fseek(fp, 0, SEEK_SET);			// 设置偏移量为0
		while ((ch = fgetc(fp)) != EOF)	// 逐字符读取
			message[mCnt++] = ch;
		message[mCnt] = 0;				// 终止符

		fclose(fp);
	}

	/* Input a key */
	printf("Password: \n");
	int kCnt = 0;
	while (!isalnum(ch = getchar()));
	while ((ch = getchar()) != '\n')
		key[kCnt++] = ch;
	key[kCnt] = 0;

	/* 构造LFSR线性反馈寄存器生成状态序列 */
	bool c[3][16];
	memset(c, 0, sizeof(c));

	/* 初始化LFSR反馈函数的常数 */
	// f1(x) = x ^ 15 + x ^ 14 + x ^ 13 + x ^ 10 + x ^ 7 + 1；
	c[0][15] = c[0][14] = c[0][13] = c[0][10] = c[0][7] = c[0][0] = 1;
	// f2(x) = x ^ 15 + x ^ 13 + x ^ 12 + x ^ 10 + x ^ 7 + x ^ 6 + x ^ 5 + 1;
	c[1][15] = c[1][13] = c[1][12] = c[1][10] = c[1][7] = c[1][6] = c[1][5] = c[1][0] = 1;
	// f3(x) = x ^ 15 + x ^ 13 + x ^ 12 + x ^ 11 + x ^ 10 + x ^ 7 + x ^ 6 + x + 1
	c[2][15] = c[2][13] = c[2][12] = c[2][11] = c[2][10] = c[2][7] = c[2][6] = c[2][1] = c[2][0] = 1;

	/* 构造Geffe非线性序列生成器 */
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
