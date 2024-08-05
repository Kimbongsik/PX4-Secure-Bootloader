#include <iostream>
#include <fstream>

#include "mc.h"
#include "RSA.h"
#include "SHA.h"

using namespace std;

int get_filesize(fstream& file);


int main(int argc, char* argv[]) {

	fstream fs_bl, sc_bl, firmware;

	fstream  fin_img;
	fstream m_sc_bl, m_firmware;

	//change the path to your username
	fs_bl.open("/home/*/First_BL/Debug/First_BL.bin", ios_base::binary | ios::in);
	sc_bl.open("/home/*/Secure-PX4-Autopilot/build/px4_fmu-v6c_bootloader/px4_fmu-v6c_bootloader.bin", ios_base::binary | ios::in);
	firmware.open("/home/*/Secure-PX4-Autopilot/build/px4_fmu-v6c_default/px4_fmu-v6c_default.bin", ios_base::binary | ios::in);

	fin_img.open("/home/*/projects/ImgBuilder/fin_img.bin", ios_base::binary | ios::out);

	const uint8_t pub_key[128] = { 0x6F, 0xB7, 0xA5, 0x69, 0xE4, 0x40, 0x36, 0x3D, 0xE4, 0x5D, 0xBD, 0xE5, 0x19, 0xA3, 0x99, 0x28,
						 0xD9, 0xF4, 0xAC, 0x85, 0x12, 0x1B, 0x75, 0x26, 0x85, 0x1A, 0xDF, 0x1E, 0x62, 0xD4, 0xAE, 0x42,
						 0xF5, 0xEE, 0xA0, 0x43, 0x94, 0xB8, 0xB3, 0xA6, 0x7B, 0x38, 0xAB, 0x02, 0x31, 0x9C, 0x0E, 0x5D,
						 0x5F, 0xF0, 0xD9, 0xD5, 0xA9, 0x99, 0xA1, 0x65, 0x8D, 0x95, 0xD5, 0x23, 0x93, 0x55, 0x21, 0xA0,
						 0xE1, 0x34, 0xEA, 0x7F, 0xE8, 0x2E, 0xDB, 0x28, 0xF7, 0x07, 0x9D, 0x7A, 0x72, 0xBE, 0x2E, 0xAE,
						 0xBB, 0x2C, 0xAC, 0x96, 0x67, 0xDB, 0x9A, 0x11, 0xBB, 0x8B, 0xE3, 0x86, 0x56, 0xA7, 0x6E, 0xBF,
						 0x3A, 0xFF, 0x77, 0x00, 0x14, 0x2D, 0x3B, 0xEB, 0x09, 0xE6, 0xA7, 0x9E, 0xF2, 0x7C, 0xEC, 0x7D,
						 0xB3, 0xE0, 0xD2, 0x43, 0x33, 0xA9, 0xD0, 0xC9, 0x8F, 0xDA, 0x2F, 0x16, 0x0B, 0xA6, 0xD9, 0x34
	};


	/* Generate Final Image */

	if (fin_img.is_open()) {

		/* Write first bootloader */

		long fs_bl_size = get_filesize(fs_bl);
		long _fs_bl_size = fs_bl_size;
		uint8_t _data[100];

		while (0 < fs_bl_size) { // First Bootloader
			if (fs_bl_size > 100) {
				fs_bl.read((char*)_data, 100);
				fin_img.write((char*)_data, 100);
				fs_bl_size -= 100;
			}
			else {
				fs_bl.read((char*)_data, fs_bl_size);
				fin_img.write((char*)_data, fs_bl_size);
				break;
			}
		}

		uint8_t ch = 0xFF;
		while (_fs_bl_size < 48888) { //49152(0xC000) - 128(bytes) - 4(bytes) - 4(bytes) - 128(bytes)
			fin_img.put(ch);
			_fs_bl_size++;
		}

		fs_bl.close();

		printf("System >>> First Bootloader....Done\n");


		/* Write Second bootloader header */

		int key_num = 0x0;
		uint8_t plain_data[100];
		SHA256_CTX ctx;

		uint8_t hash[32];

		int file_size = get_filesize(sc_bl); 
		int sc_file_size = file_size; // Second Bootloader file size
		int sc_file_version = 2; // Second Bootloader version

		if (sc_bl.is_open()) {

			sha256_init(&ctx);

			while (0 < file_size) {
				if (file_size > 100) {
					sc_bl.read((char*)plain_data, 100);
					sha256_update(&ctx, plain_data, 100);
					file_size -= 100;
				}
				else {
					sc_bl.read((char*)plain_data, file_size);
					sha256_update(&ctx, plain_data, file_size);
					break;
				}
			}
			sha256_final(&ctx, hash);
		}

		//cout << "hash: ";
		//for (int i = 0; i < 32; i++) {
		//	cout << (int)hash[i];
		//}
		//cout << '\n';

		uint8_t RSA_enc_data[128]; // Second Bootloader signature
		int RSA_enc_len;

		//if (Encrypt_RSA1024(0, hash, 32, RSA_enc_data, &RSA_enc_len))
		//	printf("System >>> RSA1024 Encryption Success\n");
		//else
		//	printf("System >>> RSA1024 Encryption Failure\n");

		//cout << "Encrypt data : \n";
		//for (int i = 0; i < 32; i++) {
		//	printf("%02x ", RSA_enc_data[i]);
		//}
		//cout << '\n';

		//cout << "Hash data : \n";
		//for (int i = 0; i < 32; i++) {
		//	printf("%02x ", hash[i]);
		//}
		//cout << '\n';


		//cout << "Decrypt_RSA : \n";
		//for (int i = 0; i < 32; i++) {
		//	printf("%02x ", _plain_data[i]);
		//}
		//cout << '\n';


		fin_img.write((char*)RSA_enc_data, 128);
		fin_img.write((char*)&sc_file_size, 4);
		fin_img.write((char*)&sc_file_version, 4);
		fin_img.write((char*)pub_key, 128);


		sc_bl.seekg(0, ios_base::beg);

		long _sc_bl_size = sc_file_size; //save

		uint8_t sc_data[100];


		while (0 < sc_file_size) { // Second Bootloader
			if (sc_file_size > 100) {
				sc_bl.read((char*)sc_data, 100);
				fin_img.write((char*)sc_data, 100);
				sc_file_size -= 100;
			}
			else {
				sc_bl.read((char*)sc_data, sc_file_size);
				fin_img.write((char*)sc_data, sc_file_size);
				break;
			}
		}

		sc_bl.close();

		printf("System >>> Second Bootloader .....Done\n");



		///* Write firmware */

		for (int i = 0; i < (0x20000 - (0xC000 + _sc_bl_size + 128 + 4 + 4)); i++)
			fin_img.put(ch);


		SHA256_CTX _ctx;

		memset(plain_data, 0, 100);
		memset(hash, 0, 32);

		file_size = get_filesize(firmware);
		int firm_file_size = file_size; // Second Bootloader file size
		int firm_file_version = 3; // Second Bootloader version
		
		if (firmware.is_open()) {

			sha256_init(&_ctx);

			while (0 < file_size) {
				if (file_size > 100) {
					firmware.read((char*)plain_data, 100);
					sha256_update(&_ctx, plain_data, 100);
					file_size -= 100;
				}
				else {
					firmware.read((char*)plain_data, file_size);
					sha256_update(&_ctx, plain_data, file_size);
					break;
				}
			}
			sha256_final(&_ctx, hash);
		}

		memset(RSA_enc_data, 0, 128);
		RSA_enc_len = 0;

		if (Encrypt_RSA1024(0, hash, 32, RSA_enc_data, &RSA_enc_len));
		//printf("System >>> RSA1024 Encryption Success\n");
		else;
			//printf("System >>> RSA1024 Encryption Failure\n");


		// int Decrypt_RSA1024(int key_num, uint8_t* enc_data, int enc_len, uint8_t* plain_data, int* plain_len);

		uint8_t _plain_data[32] = { 0, };
		int _plain_len;

		_plain_len = 0;
		Decrypt_RSA1024(0x0, RSA_enc_data, RSA_enc_len, _plain_data, &_plain_len);

		//cout << "Encrypt data : \n";
		//for (int i = 0; i < 128; i++) {
		//	printf("%02x ", RSA_enc_data[i]);
		//}
		//cout << '\n';

		//cout << "Hash data : \n";
		//for (int i = 0; i < 32; i++) {
		//	printf("%02x ", hash[i]);
		//}
		//cout << '\n';


		//cout << "Decrypt_RSA : \n";
		//for (int i = 0; i < 32; i++) {
		//	printf("%02x ", _plain_data[i]);
		//}
		//cout << '\n';


		fin_img.write((char*)RSA_enc_data, 128);
		fin_img.write((char*)&firm_file_size, 4);
		fin_img.write((char*)&firm_file_version, 4);


		firmware.seekg(0, ios_base::beg);

		long _firm_file_size = firm_file_size;

		uint8_t firm_data[100];


		while (0 < _firm_file_size) { // Firmware
			if (_firm_file_size > 100) {
				firmware.read((char*)firm_data, 100);
				fin_img.write((char*)firm_data, 100);
				_firm_file_size -= 100;
			}
			else {
				firmware.read((char*)firm_data, _firm_file_size);
				fin_img.write((char*)firm_data, _firm_file_size);
				break;
			}
		}

		for (int i = 0; i < 0x00200000 - (0x20000 + firm_file_size); i++)
			fin_img.put(ch);


		firmware.close();

		printf("System >>> Firmware .....Done\n");

		fin_img.close();


		printf("System >>> The final image has been created\n");
	}

	return 0;

}

int get_filesize(fstream& file) {

	int base_pos = file.tellg();
	file.seekg(0, ios_base::end);
	int end_pos = file.tellg();
	file.seekg(0, ios_base::beg);
	int file_size = end_pos - base_pos;

	return file_size;
}