/****************************************************************************
 *
 *   Copyright (c) 2012-2022 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mc.cpp
 * Minimal application example for PX4 autopilot
 *
 * @author Example User <mail@example.com>
 */

#include "mc.h"


#define PRIVATE 0
#define PUBLIC 1

#define MAX_AES_KEY_IDX 32
#define MAX_AES_KEY_SIZE 16

int publicKey[2] = { 14351, 11 };
int privateKey[2] = { 14351, 1283 };

byte AES_key[MAX_AES_KEY_IDX][16] = { 0x7C, 0x85, 0x20, 0x89, 0xFF, 0xDB, 0x16, 0x12, 0x6D, 0xBE, 0xE7, 0xC9, 0x68, 0xA7, 0x51, 0xB5 };


void dump(byte* buf, int len) {
	int i;
	for (i = 0; i < len; i++) {
		printf("%d", (int)buf[i]);
		printf(" ");
	}
	printf("\n");
}

int Init_SE() {
	return 1;
}

int Generate_AES128Key(int keyNum) {
    /*
	for (int i = 0; i < MAX_AES_KEY_SIZE; i++) {
		AES_key[keyNum][i] = rand() % 255;
	}
	*/
	return 1;
}

int Encrypt_AES128(int keyNum, uint8_t* plain_data, int plain_len, uint8_t* enc_data, int* enc_len) {
	AES aes;

	plain_data[plain_len++] = 0x00;
	aes.do_aes_encrypt(plain_data, plain_len, enc_data, AES_key[keyNum], 128);
	*enc_len = (plain_len) % 16 ? ((plain_len / 16) + 1) * 16: plain_len;

	return 1;
}

int Decrypt_AES128(int keyNum, uint8_t* enc_data, int enc_len, uint8_t* dec_data, int* dec_len) {
	AES aes;

	aes.do_aes_decrypt(enc_data, enc_len, dec_data, AES_key[keyNum], 128);
	*dec_len = strlen((char *)dec_data);

	return 1;
}

int Generate_RSA1024Key(int keyNum) {
	return 1;
}

int Encrypt_RSA1024(int key_num, uint8_t* plain_data, int plain_len, uint8_t* enc_data, int* enc_len) {
	rsa.encrypt(plain_data, plain_len, enc_data, enc_len, publicKey);

	if (enc_len == 0)
		return 0;
	return 1;
}

int Decrypt_RSA1024(int key_num, uint8_t* enc_data, int enc_len, uint8_t* plain_data, int* plain_len) {
	rsa.decrypt(plain_data, plain_len, enc_data, enc_len, privateKey);

	if (plain_len == 0)
		return 0;
	return 1;
}

int Sign_RSA1024(int key_num, uint8_t* plain_data, int plain_len, uint8_t* sign_data, int* sign_len) {
	uint8_t hash[32];
	int hashlen = 32;

	printf("Sign_RSA1024\n");
	SHA_256(plain_data, plain_len, hash, &hashlen);

	rsa.encrypt(hash, hashlen, sign_data, sign_len, privateKey);
	return 1;
}

int Verify_RSA1024(int key_num, uint8_t* sign_data, int sign_len, uint8_t* org_data, int* org_len) {

/*
	printf("sign_data: ");
	for(int i = 0; i < 128; i++){
		printf("%02x ", sign_data[i]);
	}
	printf("\n");
*/

	uint8_t dec_data[32];
	int dec_datalen;
	rsa.decrypt(dec_data, &dec_datalen, sign_data, sign_len, publicKey);

	uint8_t hash[32];
	int hashlen = 32;

	//printf("org_data_len: %d \n", *org_len);
	SHA_256(org_data, *org_len, hash, &hashlen);

/*
	printf("hash data: ");
	for(int i = 0; i < 32; i++){
		printf("%02x ", hash[i]);
	}
	printf("\n");
*/

/*	
	printf("verify data: ");
		for(int i = 0; i < 32; i++){
			printf("%02x ", dec_data[i]);
		}
		printf("\n");
*/

	for (int i = 0; i < 32; i++) {
		if (hash[i] != dec_data[i])
			return 0;
	}

	return 1;
}

int SHA_256(uint8_t* plain_data, int plain_len, uint8_t* digest, int* digest_len) {
	SHA256_CTX ctx;
	ctx.datalen = 0;
	ctx.bitlen = 512;

	sha256_init(&ctx);
	sha256_update(&ctx, (uint8_t*)plain_data, plain_len);

	sha256_final(&ctx, (uint8_t*)digest);

	*digest_len = 32;

	return 1;
}
