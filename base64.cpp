#include "base64.h"

const char enc_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G',
							'H', 'I', 'J', 'K', 'L', 'M', 'N',
							'O', 'P', 'Q', 'R', 'S', 'T', 'U',
							'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
							'c', 'd', 'e', 'f', 'g', 'h', 'i',
							'j', 'k', 'l', 'm', 'n', 'o', 'p',
							'q', 'r', 's', 't', 'u', 'v', 'w',
							'x', 'y', 'z', '0', '1', '2', '3',
							'4', '5', '6', '7', '8', '9', '+',
							'/', '=' };

const int dec_table[] = {
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63, 
	52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  
	15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1, 
	-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  
	41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   
};

char* base64_encode(const char *plain) {
	int i = 0, j = 0;
	int len = strlen(plain);
	int bs = len - (len % 3);
	int remain_size = len - bs;
	char buffer[MAX_SIZE];

	// 문자열에서 3단어씩 끊어서 처리를 한다. 
	for (; i < bs; i += 3) {
		// 3단어를 비트연산으로 4단어로 쪼갠다. (8bit를 6비트로 만든다.)

		// 첫번째 단어에서 2칸을 오른쪽으로 땡기고, AND 연산으로 0x3f는 2^6이니 6자리를 제외한 나머지는 0으로 만들어 첫번째 단어를 가져옴.
		// 버퍼에 한 단어 완성
		buffer[j++] = enc_table[(plain[i] >> 2) & 0x3f]; 

		// 첫번째 단어에서 버려진 나머지 2개를 2^2와 AND 연산으로 걸러내서 왼쪽으로 4칸 밀어 공간을 만든 다음에 두번째 단어의 4자리르 가져온다. 똑같이 2^4이니 4자리를 0으로 포맷
		// 버퍼에 두 단어 완성
		buffer[j++] = enc_table[((plain[i] & 0x03) << 4) + ((plain[i+1] >> 4) & 0x0f)];

		// 두번째 단어에서 버려진 나머지 4개의 값을 가져와 앞으로 2간 밀어주고 세번째 단어의 앞의 2자리를 가져온다.
		//버퍼에 세 단어 완성
		buffer[j++] = enc_table[((plain[i+1] & 0x0f) << 2) + ((plain[i+2] >> 6) & 0x03)];

		// 나머지 찌끄레기들은 네번째 단어가 된다.
		buffer[j++] = enc_table[plain[i+2] & 0x3f];
	}

	//반복하다가 비트수가 남으면
	if (remain_size != 0) {
		buffer[j++] = enc_table[plain[i] >> 2 & 0x3f];

		if (len - i == 2) { // 2 바이트가 남으면
			buffer[j++] = enc_table[((plain[i] & 0x03) << 4) + ((plain[i + 1] >> 4) & 0x0f)];
			buffer[j++] = enc_table[(plain[i + 1] & 0x0f) << 2];
		}else { // 1 바이트가 남으면
			buffer[j++] = enc_table[(plain[i] & 0x03) << 4];
			buffer[j++] = enc_table[PADDING];
		}

		// 똑같이 한번 더 처리하고 싱크를 맞추기 위해 PADDING '='을 붙여준다.
		buffer[j++] = enc_table[PADDING];
	}

	buffer[j++] = NULL;

	int size = sizeof(char) * j;

	char *result = (char *)malloc(size);
	memset(result, NULL, size);
	memmove(result, buffer, size);

	return result;
}

char* base64_decode(const char *encoded) {
	int i = 0, j = 0;
	int len = strlen(encoded);

	char buffer[MAX_SIZE];
	
	// 반대로 디코딩하기 위해서 4자리씩 처리한다.
	// 현재 6bit 상태이다
	for (; i < len; i+=4) {
		// 디코딩 테이블에서 인코딩된 단어들과 매칭되는 값을 가져오고
		int c0 = dec_table[encoded[i]];
		int c1 = dec_table[encoded[i+1]];
		int c2 = dec_table[encoded[i+2]];
		int c3 = dec_table[encoded[i+3]];
		
		// 원래 (8bit) 대로 만들어준다.
		// 인코딩과 거꾸로 된거다.
		buffer[j++] = (c0 << 2) | (c1 >> 4);
		buffer[j++] = (c1 << 4) | (c2 >> 2);
		buffer[j++] = (c2 << 6) | c3;
	}

	buffer[j++] = NULL;

	int size = sizeof(char) * j;
	char *result = (char *)malloc(size);
	memset(result, NULL, size);
	memmove(result, buffer, size);

	return result;
}
