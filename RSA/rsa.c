/**
	@file		rsa.c
	@author		작성자 임계현 / 2012037421
	@date		2016.11.17
	@brief		mini RSA implementation code
	@details	세부 설명
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rsa.h"

uint p, q, e, d, n;

/**
	@brief		모듈러 덧셈 연산을 하는 함수.
	@param		uint a		: 피연산자1.
	@param		uint b		: 피연산자2.
	@param		byte op		: +, - 연산자.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 덧셈에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
*/
uint ModAdd(uint a, uint b, byte op, uint mod) {
	uint result = 0;

	if(op == '-') {
        if(a >= b) {
            result = a - b;
        }
        else {
            result = mod - (b - a);
        }
	}
	else if(op == '+') {
        if(((a + b) < a) || ((a + b) < b)) {
            result = a - (mod - b);
        }
        else if((a + b) >= mod) {
            result = (a + b) - mod;
        }
        else {
            result = (a + b);
        }
	}

	return result;
}



/**
	@brief		모듈러 곱셈 연산을 하는 함수.
	@param		uint x		: 피연산자1.
	@param		uint y		: 피연산자2.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 곱셈에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
*/

uint ModMul(uint x, uint y, uint mod) {
    uint i;
    uint temp = x;
    uint result = 0;

    while(y > 0) {
        if((y & 1) == 1) {
            result = ModAdd(result, temp, '+', mod);
        }
		temp = ModAdd(temp, temp, '+', mod);
        y >>= 1;
    }

    return result;
}

/**
	@brief		모듈러 거듭제곱 연산을 하는 함수.
	@param		uint base	: 피연산자1.
	@param		uint exp	: 피연산자2.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 연산에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
				'square and multiply' 알고리즘을 사용하여 작성한다.
*/

uint ModPow(uint base, uint exp, uint mod) {
    uint result = 1;

    while(exp > 0) {
        if((exp & 1) == 1) {
            result = ModMul(result, base, mod);
        }
        base = ModMul(base, base, mod);
        exp >>= 1;
    }

    return result;
}



/**
	@brief		입력된 수가 소수인지 입력된 횟수만큼 반복하여 검증하는 함수.
	@param		uint testNum	: 임의 생성된 홀수.
	@param		uint repeat		: 판단함수의 반복횟수.
	@return		uint result		: 판단 결과에 따른 TRUE, FALSE 값.
	@todo		Miller-Rabin 소수 판별법과 같은 확률적인 방법을 사용하여,
				이론적으로 4N(99.99%) 이상 되는 값을 선택하도록 한다.
*/
bool IsPrime(uint testNum, uint repeat) {
    int i;
    int d;
	if (testNum == 2) {
        return TRUE;
    }
	if (testNum == 1 || (testNum & 1) == 0) {
        return FALSE;
    }
	d = testNum - 1;
	while ((d & 1) == 0) {
		d >>= 1;
	}

	for(i = 0; i < repeat; i++) {
		int a = (int)((double)WELLRNG512a() * (testNum - 2) + 1);
		int t = d;
		int y = ModPow(a, t, testNum);
		while (t != testNum - 1 && y != 1 && y != testNum - 1) {
			y = ModMul(y, y, testNum);
			t <<= 1;
		}
		if (y != testNum - 1 && (t & 1) == 0) {
            return FALSE;
        }

	}
	return TRUE;
}

uint GCD(uint a, uint b) {
	uint prev_a;

	while(b != 0) {
		printf("GCD(%u, %u)\n", a, b);
		prev_a = a;
		a = b;
		while(prev_a >= b) prev_a -= b;
		b = prev_a;
	}
	printf("GCD(%u, %u)\n\n", a, b);
	return a;
}

/**
	@brief		모듈러 역 값을 계산하는 함수.
	@param		uint a	: 피연산자1.
	@param		uint m	: 모듈러 값.
	@return		uint result	: 피연산자의 모듈러 역수 값.
	@todo		확장 유클리드 알고리즘을 사용하여 작성하도록 한다.
*/

uint ModInv(uint a, uint m) {
	uint r1 = a, r2 = m;
	uint r, q;
	uint s, s1 = 1, s2 = 0;
	uint t, t1 = 0, t2 = 1;

    while (r2 != 0) {
		q = 0;

		while(r1 >= r2) {
			r1 -= r2;
			q++;
		}

		r = r1;
		r1 = r2;
        r2 = r;

        s = s1 - q * s2;
		s1 = s2;
        s2 = s;

        t = t1 - q * t2;
        t1 = t2;
        t2 = t;
    }
    if(r1 == 1) {
        return t1;
    }
    return 0;
}

/**
	@brief		RSA 키를 생성하는 함수.
	@param		uint *p	: 소수 p.
	@param		uint *q	: 소수 q.
	@param		uint *e	: 공개키 값.
	@param		uint *d	: 개인키 값.
	@param		uint *n	: 모듈러 n 값.
	@return		void
	@todo		과제 안내 문서의 제한사항을 참고하여 작성한다.
*/
void MRSAKeygen(uint *p, uint *q, uint *e, uint *d, uint *n) {
	while(1) {
		*p = (uint)((double)WELLRNG512a() * (65536 - 32768) + 32768);
		printf("random-number1 %u selected\n", *p);
		if(IsPrime(*p, 20) != 0){
			printf("%u may be Prime\n", *p);
			break;
		}
		printf("%u is not PRIME\n", *p);
	}

	uint a = 2147483648 / (*p) + 1;
	uint b = 4294967296 / (*p);

	while(1) {
		*q = (uint)((double)WELLRNG512a() * (b - a) + a);
		printf("random-number2 %u selected\n", *q);
		if(IsPrime(*q,  20) != 0){
			printf("%u may be Prime\n", *q);
			break;
		}
		printf("%u is not PRIME\n", *q);
	}

	*n = (*p) * (*q);
	printf("finally selected prime p, q = %u, %u\n", *p, *q);
	printf("thus, n = %u\n", *n);

	uint pi_n = ((*p) - 1) * ((*q) - 1);

	while(1) {
		*e = (uint)((double)WELLRNG512a() * (pi_n - 2) + 2);
		printf("e : %u selected", *e);
		if(GCD(pi_n, *e) == 1) {
			*d = ModInv(pi_n, *e);
			if(*d < pi_n) {
				printf("d : %u selected\n\n", *d);
				if(ModMul(*e, *d, pi_n)) {
					printf("e, d, n, pi_n : %u, %u, %u, %u\n", *e, *d, *n, pi_n);
					printf("e * d mod pi_n : 1\n");
					break;
				}
			}
		}
	}
}

/**
	@brief		RSA 암복호화를 진행하는 함수.
	@param		FILE *ifp	: 입력 파일 포인터.
	@param		uint len	: 입력 파일 길이.
	@param		FILE *ofp	: 출력 파일 포인터.
	@param		uint key	: 키 값.
	@param		uint n		: 모듈러 n 값.
	@return		uint result	: 암복호화에 성공한 byte 수.
	@todo		과제 안내 문서의 제한사항을 참고하여 작성한다.
*/

uint MRSACipher(FILE *ifp, uint len, FILE *ofp, uint key, uint n) {
	unsigned char buf[4];

	int i;
	uint ptx = 0;
	uint ctx = 0;
	uint result = 0;
	printf("MSACipher start. file len is %u\n", len);

	while(len > 0) {
		bzero(buf, 4);

		if(len < 4) {
			fread(buf, len, 1, ifp);
			for(i = 0; i < len; i++) {
				ptx <<= 8;
				ptx += buf[i];

			}
			for(i = 0; i < 4 - len; i++) {
				ptx <<= 8;
			}
		}
		else {
			fread(buf, 1, 4, ifp);
			for(i = 0; i < 4; i++) {
				ptx <<= 8;
				ptx += buf[i];
			}
		}

		if(ptx > n) {
			printf("M is larger than n\n");
			exit(1);
		}
		else {
			ctx = ModPow(ptx, key, n);

			printf("len : %u\n", len);
			printf("buf : %s\n", buf);
			printf("ptx : %u\n", ptx);
			printf("ctx : %u\n\n", ctx);

			unsigned char change[4];

			for(i = 0; i < 4; i++) {
				uint temp = (ctx >> ((3 - i) * 8)) & 255;

				change[i] = temp;
			}
			result += fwrite(change, sizeof(char), 4, ofp);

		}

		bzero(buf, 4);
		if(len < 4) {
			break;
		}
		else {
			len -= 4;
		}
	}

	return result;
}

void main(int argc, char const *argv[]) {
	uint seed = time(NULL);
	InitWELLRNG512a(&seed);

	FILE *data_fp, *enc_fp, *dec_fp;
	uint fsize;

	if(argc != 4) {
		printf("usage : ./rsa data_file encrypt_file decrypt_file\n");
		exit(1);
	}

	data_fp = fopen(argv[1], "rb");
	enc_fp = fopen(argv[2], "wb");
	if(data_fp == NULL | enc_fp == NULL) {
		printf("file open fail\n");
		exit(1);
	}

	fseek(data_fp, 0, SEEK_END);
	fsize = ftell(data_fp);
	printf("data file size : %u\n\n", fsize);
	fseek(data_fp, 0, SEEK_SET);

	MRSAKeygen(&p, &q, &e, &d, &n);
	fsize = MRSACipher(data_fp, fsize, enc_fp, e, n);

	fclose(data_fp);
	fclose(enc_fp);

	enc_fp = fopen(argv[2], "rb");
	dec_fp = fopen(argv[3], "wb");
	if(dec_fp == NULL | enc_fp == NULL) {
		printf("file open fail\n");
		exit(1);
	}

	printf("encrypted file size : %u\n\n", fsize);

	fsize = MRSACipher(enc_fp, fsize, dec_fp, d, n);

	fclose(enc_fp);
	fclose(dec_fp);
}
