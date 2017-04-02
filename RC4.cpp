#include <stdio.h>
#include <string.h>
#define MAX_KEY_LEN 2048
#define MAX_CHAR_LEN 30000

// RC4�㷨
// ������������
class RC4
{
private:
	unsigned int keyLen;	// ��Կ����
	unsigned int msgLen;	// ���ĳ���

	unsigned int key[MAX_KEY_LEN];		// ��Կ
	unsigned int keyStream[MAX_KEY_LEN];// ��Կ��

public:
	/*
	* RC4��ʼ��
	* 
	* @param K[]
	*	��Կ
	* @param klen
	*	��Կ����
	* @param mlen
	*	���ĳ���
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
	* RC4��Կ��������
	* 
	* @return
	*/
	void generator () {
		int i, j = 0, k;
		unsigned char s[256]; // ����
		unsigned char tmp;	  

		for (i = 0; i < 256; i++)
			s[i] = i;

		// ʹ����Կ�����г�ʼ��
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
	* ����
	*
	* @param message[]
	*	����
	* @param cipherText[]
	*	����
	* @return
	*/
	void encrypt (unsigned char message[], unsigned char *cipherText) {
		// c = m ^ keyStream
		for (int i = 0; message[i]; i++)
			cipherText[i] = message[i] ^ keyStream[i % 256];
		cipherText[msgLen] = 0; // �ַ�����ֹ��
	}

	/*
	* ����
	*
	* @param cipherText[]
	*	����
	* @param message[]
	*	����
	* @return
	*/
	void decrypt (unsigned char cipherText[], unsigned char *message) {
		// m = c ^ keyStream
		for (int i = 0; cipherText[i]; i++)
			message[i] = cipherText[i] ^ keyStream[i % 256];
		message[msgLen] = 0; // �ַ�����ֹ��
	}
};

int main(int argc, const char **argv)
{
	unsigned char message[MAX_CHAR_LEN];	// ����
	unsigned char cipherText[MAX_CHAR_LEN];	// ����
	unsigned char key[MAX_KEY_LEN];			// ��Կ

	char c;
	/* �򿪴������ļ� */
	FILE * fp;
	char file[] = "message.txt";	// �ļ���

	if ( (fp = fopen(file, "r")) == NULL) {
		printf("No dirtory or file.\n");
		fclose(fp);
		return 1;
	}
	// �����ļ���ʼλ��
	fseek(fp, 0, SEEK_SET);
	/* ��ȡ�ļ����� */
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

	/* �����������Ļ */
	printf("%s\n", cipherText);

	/* Decrpty */
	printf("Decrypting...\n");
	unsigned char msg[MAX_CHAR_LEN];
	rc4.decrypt(cipherText, msg);
	printf("%s\n", msg);
	return 0;
}
