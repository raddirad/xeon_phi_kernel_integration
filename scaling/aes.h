/*
 * Common values for AES algorithms
 */
#include <stdint.h>

#ifndef _CRYPTO_AES_H
#define _CRYPTO_AES_H

#include "crypto.h"

#define AES_MIN_KEY_SIZE	16
#define AES_MAX_KEY_SIZE	32
#define AES_KEYSIZE_128		16
#define AES_KEYSIZE_192		24
#define AES_KEYSIZE_256		32
#define AES_BLOCK_SIZE		16
#define AES_MAX_KEYLENGTH	(15 * 16)
#define AES_MAX_KEYLENGTH_U32	(AES_MAX_KEYLENGTH / sizeof(uint32_t))

/*
 * Please ensure that the first two fields are 16-byte aligned
 * relative to the start of the structure, i.e., don't move them!
 */

struct crypto_aes_ctx {
	uint32_t key_enc[AES_MAX_KEYLENGTH_U32];
	uint32_t key_dec[AES_MAX_KEYLENGTH_U32];
	uint32_t key_length;
};

extern const uint32_t crypto_ft_tab[4][256];
extern const uint32_t crypto_fl_tab[4][256];
extern const uint32_t crypto_it_tab[4][256];
extern const uint32_t crypto_il_tab[4][256];

int crypto_aes_set_key(struct crypto_tfm *tfm, const uint8_t *in_key,
		unsigned int key_len);

int crypto_aes_expand_key(struct crypto_aes_ctx *ctx, const uint8_t *in_key,
		unsigned int key_len);

void aes_encrypt(struct crypto_tfm *tfm, uint8_t *out, const uint8_t *in);

void aes_decrypt(struct crypto_tfm *tfm, uint8_t *out, const uint8_t *in);

#endif
