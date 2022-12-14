
#include "ChaChaPoly.h"
#include "Crypto.h"
#include "utility/EndianUtil.h"
#include <string.h>


ChaChaPoly::ChaChaPoly()
{
    state.authSize = 0;
    state.dataSize = 0;
    state.dataStarted = false;
    state.ivSize = 8;
}

/**
 * \brief Destroys this ChaChaPoly authenticated cipher.
 */
ChaChaPoly::~ChaChaPoly()
{
    clean(state);
}

size_t ChaChaPoly::keySize() const
{
    // Default key size is 256-bit, but any key size is allowed.
    return 32;
}

size_t ChaChaPoly::ivSize() const
{
    // Return 8 but we also support 12-byte nonces in setIV().
    return 8;
}

size_t ChaChaPoly::tagSize() const
{
    // Any tag size between 1 and 16 is supported.
    return 16;
}

bool ChaChaPoly::setKey(const uint8_t *key, size_t len)
{
    return chacha.setKey(key, len);
}

bool ChaChaPoly::setIV(const uint8_t *iv, size_t len)
{
    // ChaCha::setIV() supports both 64-bit and 96-bit nonces.
    if (!chacha.setIV(iv, len))
        return false;

    // Generate the key and nonce to use for Poly1305.
    uint32_t data[16];
    chacha.keystreamBlock(data);
    poly1305.reset(data);
    memcpy(state.nonce, data + 4, 16);
    clean(data);

    // Reset the size counters for the auth data and payload.
    state.authSize = 0;
    state.dataSize = 0;
    state.dataStarted = false;
    state.ivSize = len;
    return true;
}

void ChaChaPoly::encrypt(uint8_t *output, const uint8_t *input, size_t len)
{
    if (!state.dataStarted) {
        poly1305.pad();
        state.dataStarted = true;
    }
    chacha.encrypt(output, input, len);
    poly1305.update(output, len);
    state.dataSize += len;
}

void ChaChaPoly::decrypt(uint8_t *output, const uint8_t *input, size_t len)
{
    if (!state.dataStarted) {
        poly1305.pad();
        state.dataStarted = true;
    }
    poly1305.update(input, len);
    chacha.encrypt(output, input, len); // encrypt() is the same as decrypt()
    state.dataSize += len;
}

void ChaChaPoly::addAuthData(const void *data, size_t len)
{
    if (!state.dataStarted) {
        poly1305.update(data, len);
        state.authSize += len;
    }
}

void ChaChaPoly::computeTag(void *tag, size_t len)
{
    uint64_t sizes[2];

    // Pad the final Poly1305 block and then hash the sizes.
    poly1305.pad();
    sizes[0] = htole64(state.authSize);
    sizes[1] = htole64(state.dataSize);
    poly1305.update(sizes, sizeof(sizes));

    // Compute the tag and copy it to the return buffer.
    poly1305.finalize(state.nonce, tag, len);
    clean(sizes);
}

bool ChaChaPoly::checkTag(const void *tag, size_t len)
{
    // Can never match if the expected tag length is too long.
    if (len > 16)
        return false;

    // Compute the tag and check it.
    uint8_t temp[16];
    computeTag(temp, len);
    bool equal = secure_compare(temp, tag, len);
    clean(temp);
    return equal;
}

void ChaChaPoly::clear()
{
    chacha.clear();
    poly1305.clear();
    clean(state);
    state.ivSize = 8;
}
