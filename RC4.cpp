#include <stdio.h>
#include <string.h>
#define MAX_KEY_LEN 2048
#define MAX_CHAR_LEN 30000

// RC4算法
// 流密码生成器
class RC4
{
private:
	unsigned int keyLen;	// 密钥长度
	unsigned int msgLen;	// 明文长度

	unsigned int key[MAX_KEY_LEN];		// 密钥
	unsigned int keyStream[MAX_KEY_LEN];// 密钥流

public:
	/*
	* RC4初始化
	* 
	* @param K[]
	*	密钥
	* @param klen
	*	密钥长度
	* @param mlen
	*	明文长度
	* @return
	*/
	void init (unsigned char K[],
		unsigned int klen,
		unsigned int mLen) {
		keyLen = klen;
		msgLen = mLen;
		for (int i = 0; i < keyLen; i++) {
			key[i] = K[i];
			keyStream[i] = key[i];
		}
	}

	/*
	* RC4密钥流生成器
	* 
	* @return
	*/
	void generator () {
		int i, j = 0, k;
		unsigned char s[256]; // 序列
		unsigned char tmp;	  

		for (i = 0; i < 256; i++)
			s[i] = i;

		// 使用密钥对序列初始化
		for (i = 0; i < 256; i++) {
			j = (j + s[i] + keyStream[i % keyLen]) % 256;
			tmp = s[i];
			s[i] = s[j];
			s[j] = tmp;
		}

		i = j = k = 0;
		while (k < msgLen) {
			i = (i + 1) % 256;
			j = (j + s[i]) % 256;
			tmp = s[i];
			s[i] = s[j];
			s[j] = tmp;
			keyStream[k++] = s[(s[i] + s[j]) % 256];
		}
	}

	/*
	* 加密
	*
	* @param message[]
	*	明文
	* @param cipherText[]
	*	密文
	* @return
	*/
	void encrypt (unsigned char message[], unsigned char *cipherText) {
		// c = m ^ keyStream
		for (int i = 0; message[i]; i++)
			cipherText[i] = message[i] ^ keyStream[i % 256];
		cipherText[msgLen] = 0; // 字符串终止符
	}

	/*
	* 解密
	*
	* @param cipherText[]
	*	密文
	* @param message[]
	*	明文
	* @return
	*/
	void decrypt (unsigned char cipherText[], unsigned char *message) {
		// m = c ^ keyStream
		for (int i = 0; cipherText[i]; i++)
			message[i] = cipherText[i] ^ keyStream[i % 256];
		message[msgLen] = 0; // 字符串终止符
	}
};

int main(int argc, const char **argv)
{
	unsigned char message[MAX_CHAR_LEN];	// 明文
	unsigned char cipherText[MAX_CHAR_LEN];	// 密文
	unsigned char key[MAX_KEY_LEN];			// 密钥

	char c;
	/* 打开待加密文件 */
	FILE * fp;
	char file[] = "message.txt";	// 文件名

	if ( (fp = fopen(file, "r")) == NULL) {
		printf("No dirtory or file.\n");
		fclose(fp);
		return 1;
	}
	// 调整文件初始位置
	fseek(fp, 0, SEEK_SET);
	/* 读取文件内容 */
	int mCnt = 0;
	while ( (c = fgetc(fp) ) != EOF)
		message[mCnt++] = c;
	message[mCnt] = 0;

	fclose(fp);
	/* Input a key */
	printf("Password: \n");
	int kCnt = 0;
	while ((c = getchar()) != '\n')
		key[kCnt++] = c;
	key[kCnt] = 0;

	/* Key Stream Generate */
	RC4 rc4;
	rc4.init(key, kCnt, mCnt);
	rc4.generator();


	/* Cipher Text Generate*/
	printf("Encrypting...\n");
	rc4.encrypt(message, cipherText);

	/* Save cipher text*/
	char filename[25];
	sprintf(filename, "RC4_%s", file);
	fp = fopen(filename, "w");
	for (int i = 0; i < mCnt; i++)
		putc(cipherText[i], fp);
	fclose(fp);

	/* 密文输出到屏幕 */
	printf("%s\n", cipherText);

	/* Decrpty */
	printf("Decrypting...\n");
	unsigned char msg[MAX_CHAR_LEN];
	rc4.decrypt(cipherText, msg);
	printf("%s\n", msg);
	return 0;
}
