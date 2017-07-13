/*  ======================================================================== *

                                    주 의 사 항


    1. 계산 중간 흐름은 다음 문서를 보고 비교하는 것을 추천함

		http://grouper.ieee.org/groups/1619/email/pdf00046.pdf

 *  ======================================================================== */

#include <stdio.h>
#include "XTS_AES.h"

typedef unsigned char BYTE;

uint8_t iv[]  = { 
	// 직접 작성해서 테스트
};

uint8_t key[] = { 
	// 직접 작성해서 테스트
};

uint8_t plain[] = { 
	// 직접 작성해서 테스트
};

uint8_t cipher[] = { 
	// 직접 작성해서 테스트
};

int main(){
	BYTE tmp[64];

	// 암호화 테스트
	XTS_AES128(plain, tmp, 64, key, ENC);
	printf("XTS_AES Encryption: %s\n", 0 == strncmp((char*) cipher, (char*) tmp, 64) ? "SUCCESS!" : "FAILURE!");

	// 복호화 테스트
	XTS_AES128(tmp, cipher, 64, key, DEC);
	printf("XTS_AES Decryption: %s\n", 0 == strncmp((char*) tmp, (char*) plain, 64) ? "SUCCESS!" : "FAILURE!");

	return 0;
}