
#pragma once
#ifndef CIPHERS_H
#define CIPHERS_H

#define CIPHER_CHAR_KEY  'p'


void simple_xor_crypt(const char *key, int key_len, char *data, int data_len);
void recursive_xor_encrypt(const char key, char *data, int data_len);
void recursive_xor_decrypt(const char key, char *data, int data_len);
int three_cip_types(int mode, char* path, char* key);

#endif 
