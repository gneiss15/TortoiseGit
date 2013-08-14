// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2013 Sven Strickroth <email@cs-ware.de>
// Copyright (C) VLC project (http://videolan.org)

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include <stdint.h>
#include <WinCrypt.h>

enum    /* Public key algorithms */
{
	/* we will only use DSA public keys */
	PUBLIC_KEY_ALGO_DSA = 0x11
};

enum    /* Digest algorithms */
{
	/* and DSA use SHA-1 digest */
	DIGEST_ALGO_SHA1    = 0x02
};

enum    /* Packet types */
{
	SIGNATURE_PACKET    = 0x02,
	PUBLIC_KEY_PACKET   = 0x06,
	USER_ID_PACKET      = 0x0d
};

enum    /* Signature types */
{
	BINARY_SIGNATURE        = 0x00,
	TEXT_SIGNATURE          = 0x01,

	/* Public keys signatures */
	GENERIC_KEY_SIGNATURE   = 0x10, /* No assumption of verification */
	PERSONA_KEY_SIGNATURE   = 0x11, /* No verification has been made */
	CASUAL_KEY_SIGNATURE    = 0x12, /* Some casual verification */
	POSITIVE_KEY_SIGNATURE  = 0x13  /* Substantial verification */
};

enum    /* Signature subpacket types */
{
	ISSUER_SUBPACKET    = 0x10
};

struct public_key_packet_t
{ /* a public key packet (DSA/SHA-1) is 418 bytes */

	uint8_t version;      /* we use only version 4 */
	uint8_t timestamp[4]; /* creation time of the key */
	uint8_t algo;         /* we only use DSA */
	/* the multi precision integers, with their 2 bytes length header */
	uint8_t p[2+128];
	uint8_t q[2+20];
	uint8_t g[2+128];
	uint8_t y[2+128];
};

/* used for public key and file signatures */
struct signature_packet_t
{
	uint8_t version; /* 3 or 4 */

	uint8_t type;
	uint8_t public_key_algo;    /* DSA only */
	uint8_t digest_algo;        /* SHA-1 only */

	uint8_t hash_verification[2];
	uint8_t issuer_longid[8];

	union   /* version specific data */
	{
		struct
		{
			uint8_t hashed_data_len[2];     /* scalar number */
			uint8_t *hashed_data;           /* hashed_data_len bytes */
			uint8_t unhashed_data_len[2];   /* scalar number */
			uint8_t *unhashed_data;         /* unhashed_data_len bytes */
		} v4;
		struct
		{
			uint8_t hashed_data_len;    /* MUST be 5 */
			uint8_t timestamp[4];       /* 4 bytes scalar number */
		} v3;
	} specific;

/* The part below is made of consecutive MPIs, their number and size being
 * public-key-algorithm dependent.
 *
 * Since we use DSA signatures only, there is 2 integers, r & s, made of:
 *      2 bytes for the integer length (scalar number)
 *      160 bits (20 bytes) for the integer itself
 *
 * Note: the integers may be less than 160 significant bits
 */
	uint8_t r[2+20];
	uint8_t s[2+20];
};

typedef struct public_key_packet_t public_key_packet_t;
typedef struct signature_packet_t signature_packet_t;

struct public_key_t
{
	uint8_t longid[8];       /* Long id */
	uint8_t *psz_username;    /* USER ID */

	public_key_packet_t key;       /* Public key packet */

	signature_packet_t sig;     /* Signature packet, by the embedded key */
};

typedef struct public_key_t public_key_t;

typedef struct _DSAKEY
{
  BLOBHEADER blobheader;
  DSSPUBKEY_VER3 dsspubkeyver3;
  BYTE p[128]; // prime modulus
  BYTE q[20]; // large factor of P-1
  BYTE g[128]; // the generator parameter
  BYTE y[128]; // (G^X) mod P
} DSAKEY;

int VerifyIntegrity(const CString &filename, const CString &signatureFilename);
