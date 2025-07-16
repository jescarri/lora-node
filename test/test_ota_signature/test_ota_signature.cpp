#include <unity.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cassert>
#include <vector>
#include <string>
#include <algorithm>
#include <unity.h>
#include <sodium.h>

#ifdef UNIT_TEST
#if defined(HAVE_SODIUM) || defined(__SODIUM_H__)
#define TEST_HAS_SODIUM 1
#else
#define TEST_HAS_SODIUM 0
#endif
#else
#define TEST_HAS_SODIUM 0
#endif

constexpr int OTA_MAX_CHUNKS = 10;
constexpr int OTA_CHUNK_SIZE = 51;

struct OtaChunkBuffer {
    std::string decoded_chunks[OTA_MAX_CHUNKS];
    bool received[OTA_MAX_CHUNKS];
    int total_chunks = 0;
    int received_count = 0;
    void reset() {
        for (int i = 0; i < OTA_MAX_CHUNKS; ++i) {
            decoded_chunks[i] = "";
            received[i] = false;
        }
        total_chunks = 0;
        received_count = 0;
    }
    void update_total_chunks(int idx) {
        if (idx + 1 > total_chunks) total_chunks = idx + 1;
    }
};
OtaChunkBuffer ota_chunk_buffer;

// Helper: base64 decode (replace with your project's base64 if needed)
std::string b64decode(const std::string& in) {
    static const std::string b64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[b64_chars[i]] = i;
    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

// Each chunk is base64-encoded; decode and store
void handleOtaChunk(const uint8_t* data, uint8_t dataLen, uint8_t fport) {
    if (fport < 1 || fport > OTA_MAX_CHUNKS) return;
    int idx = fport - 1;
    std::string b64chunk((const char*)data, dataLen);
    std::string decoded = b64decode(b64chunk);
    if (!ota_chunk_buffer.received[idx]) {
        ota_chunk_buffer.decoded_chunks[idx] = decoded;
        ota_chunk_buffer.received[idx] = true;
        ota_chunk_buffer.received_count++;
        ota_chunk_buffer.update_total_chunks(idx);
    }
}

// Reassemble the decoded binary data in order
bool tryReassembleOtaJson(std::string& out_json) {
    if (ota_chunk_buffer.total_chunks == 0 || ota_chunk_buffer.received_count < ota_chunk_buffer.total_chunks) return false;
    // Ensure all chunks from 0 to total_chunks-1 are present
    for (int i = 0; i < ota_chunk_buffer.total_chunks; ++i) {
        if (!ota_chunk_buffer.received[i]) return false;
    }
    out_json.clear();
    for (int i = 0; i < ota_chunk_buffer.total_chunks; ++i) {
        out_json += ota_chunk_buffer.decoded_chunks[i];
    }
    return true;
}

void test_chunked_reassembly() {
    ota_chunk_buffer.reset();
    std::string json = "{\"u\":\"https://t.co/abc\",\"m\":\"deadbeefdeadbeefdeadbeefdeadbeef\",\"v\":\"214\",\"s\":\"SIG\"}";
    int chunk_size = 20;
    int total_chunks = (json.size() + chunk_size - 1) / chunk_size;
    ota_chunk_buffer.total_chunks = total_chunks;
    for (int i = 0; i < total_chunks; ++i) {
        int len = std::min(chunk_size, (int)json.size() - i * chunk_size);
        std::string chunk = json.substr(i * chunk_size, len);
        // Base64-encode each chunk
        std::string b64chunk;
        static const char b64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        int val = 0, valb = -6;
        for (unsigned char c : chunk) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                b64chunk.push_back(b64_chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) b64chunk.push_back(b64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (b64chunk.size() % 4) b64chunk.push_back('=');
        handleOtaChunk((const uint8_t*)b64chunk.data(), b64chunk.size(), i + 1);
    }
    std::string reassembled;
    TEST_ASSERT_TRUE(tryReassembleOtaJson(reassembled));
    TEST_ASSERT_EQUAL_STRING(json.c_str(), reassembled.c_str());
}

void test_chunked_missing_chunk() {
    ota_chunk_buffer.reset();
    std::string json = "{\"u\":\"https://t.co/abc\",\"m\":\"deadbeefdeadbeefdeadbeefdeadbeef\",\"v\":\"214\",\"s\":\"SIG\"}";
    int chunk_size = 20;
    int total_chunks = (json.size() + chunk_size - 1) / chunk_size;
    ota_chunk_buffer.total_chunks = total_chunks;
    for (int i = 0; i < total_chunks; ++i) {
        if (i == 1) continue; // skip chunk 2
        int len = std::min(chunk_size, (int)json.size() - i * chunk_size);
        std::string chunk = json.substr(i * chunk_size, len);
        // Base64-encode each chunk
        std::string b64chunk;
        static const char b64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        int val = 0, valb = -6;
        for (unsigned char c : chunk) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                b64chunk.push_back(b64_chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) b64chunk.push_back(b64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (b64chunk.size() % 4) b64chunk.push_back('=');
        handleOtaChunk((const uint8_t*)b64chunk.data(), b64chunk.size(), i + 1);
    }
    std::string reassembled;
    TEST_ASSERT_FALSE(tryReassembleOtaJson(reassembled));
}

void test_ttn_chunked_downlink() {
    // Example TTN downlink (2 chunks, base64-encoded)
    const char* chunk1 = "eyJ1IjoiaHR0cDovL2Zvby5iYXIvZi5lbGYiLCJtIjoiWFhYWFhYWFhYWFhYWFhYWFhY";
    const char* chunk2 = "WFgiLCJ2IjoiMjAwIiwicyI6Ik5OTk5OTk5OTk5OTk5OTk5OTk5OIn0=";
    // Simulate receiving chunks on fport 1 and 2 (pass base64 directly)
    ota_chunk_buffer.reset();
    handleOtaChunk((const uint8_t*)chunk1, strlen(chunk1), 1);
    handleOtaChunk((const uint8_t*)chunk2, strlen(chunk2), 2);
    // Try to reassemble
    std::string reassembled;
    TEST_ASSERT_TRUE(tryReassembleOtaJson(reassembled));
    // Expected JSON
    const char* expected = "{\"u\":\"http://foo.bar/f.elf\",\"m\":\"XXXXXXXXXXXXXXXXXXXX\",\"v\":\"200\",\"s\":\"NNNNNNNNNNNNNNNNNNNN\"}";
    TEST_ASSERT_EQUAL_STRING(expected, reassembled.c_str());
}

void test_chunked_out_of_order() {
    ota_chunk_buffer.reset();
    std::string json = "{\"u\":\"https://t.co/abc\",\"m\":\"deadbeefdeadbeefdeadbeefdeadbeef\",\"v\":\"214\",\"s\":\"SIG\"}";
    int chunk_size = 20;
    int total_chunks = (json.size() + chunk_size - 1) / chunk_size;
    ota_chunk_buffer.total_chunks = total_chunks;
    std::vector<std::pair<int, std::string>> chunks;
    for (int i = 0; i < total_chunks; ++i) {
        int len = std::min(chunk_size, (int)json.size() - i * chunk_size);
        std::string chunk = json.substr(i * chunk_size, len);
        // Base64-encode each chunk
        std::string b64chunk;
        static const char b64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        int val = 0, valb = -6;
        for (unsigned char c : chunk) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                b64chunk.push_back(b64_chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) b64chunk.push_back(b64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (b64chunk.size() % 4) b64chunk.push_back('=');
        chunks.push_back({i, b64chunk});
    }
    // Shuffle chunks to simulate out-of-order arrival
    std::swap(chunks[0], chunks[1]);
    // Add first chunk (actually chunk 1)
    handleOtaChunk((const uint8_t*)chunks[0].second.data(), chunks[0].second.size(), chunks[0].first + 1);
    std::string reassembled;
    TEST_ASSERT_FALSE(tryReassembleOtaJson(reassembled));
    // Add second chunk (actually chunk 0)
    handleOtaChunk((const uint8_t*)chunks[1].second.data(), chunks[1].second.size(), chunks[1].first + 1);
    // Add remaining chunks in order
    for (size_t i = 2; i < chunks.size(); ++i) {
        handleOtaChunk((const uint8_t*)chunks[i].second.data(), chunks[i].second.size(), chunks[i].first + 1);
    }
    TEST_ASSERT_TRUE(tryReassembleOtaJson(reassembled));
    TEST_ASSERT_EQUAL_STRING(json.c_str(), reassembled.c_str());
}

// --- Signature validation tests ---
// Test keys (ed25519) for unit test only
const unsigned char test_private_key[64] = {
    0x1a,0x2b,0x3c,0x4d,0x5e,0x6f,0x7a,0x8b,0x9c,0xad,0xbe,0xcf,0xda,0xeb,0xfc,0x0d,
    0x1e,0x2f,0x3a,0x4b,0x5c,0x6d,0x7e,0x8f,0x9a,0xab,0xbc,0xcd,0xde,0xef,0xfa,0x0b,
    0x1a,0x2b,0x3c,0x4d,0x5e,0x6f,0x7a,0x8b,0x9c,0xad,0xbe,0xcf,0xda,0xeb,0xfc,0x0d,
    0x1e,0x2f,0x3a,0x4b,0x5c,0x6d,0x7e,0x8f,0x9a,0xab,0xbc,0xcd,0xde,0xef,0xfa,0x0b
};
const unsigned char test_public_key[32] = {
    0x1a,0x2b,0x3c,0x4d,0x5e,0x6f,0x7a,0x8b,0x9c,0xad,0xbe,0xcf,0xda,0xeb,0xfc,0x0d,
    0x1e,0x2f,0x3a,0x4b,0x5c,0x6d,0x7e,0x8f,0x9a,0xab,0xbc,0xcd,0xde,0xef,0xfa,0x0b
};
std::string base64_encode(const unsigned char* data, size_t len) {
    static const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, valb = -6;
    for (size_t i = 0; i < len; ++i) {
        val = (val << 8) + data[i];
        valb += 8;
        while (valb >= 0) {
            out.push_back(b64[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(b64[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}
void test_signature_validation_valid() {
#if TEST_HAS_SODIUM
    std::string url = "http://foo.bar/f.elf";
    std::string md5 = "cafebabe1234567890abcdef12345678";
    std::string message = url + md5;
    unsigned char sig[64];
    crypto_sign_detached(sig, nullptr, (const unsigned char*)message.data(), message.size(), test_private_key);
    std::string sig_b64 = base64_encode(sig, 64);
    extern bool verify_signature(const std::string&, const std::string&, const std::string&);
    TEST_ASSERT_TRUE(verify_signature(url, md5, sig_b64));
#else
    TEST_IGNORE_MESSAGE("libsodium not available");
#endif
}
void test_signature_validation_invalid() {
#if TEST_HAS_SODIUM
    std::string url = "http://foo.bar/f.elf";
    std::string md5 = "cafebabe1234567890abcdef12345678";
    std::string message = url + md5;
    unsigned char sig[64];
    crypto_sign_detached(sig, nullptr, (const unsigned char*)message.data(), message.size(), test_private_key);
    std::string sig_b64 = base64_encode(sig, 64);
    extern bool verify_signature(const std::string&, const std::string&, const std::string&);
    // Tamper with message
    TEST_ASSERT_FALSE(verify_signature(url, "badmd5", sig_b64));
    // Tamper with signature
    std::string bad_sig = sig_b64;
    bad_sig[0] = (bad_sig[0] == 'A' ? 'B' : 'A');
    TEST_ASSERT_FALSE(verify_signature(url, md5, bad_sig));
#else
    TEST_IGNORE_MESSAGE("libsodium not available");
#endif
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_chunked_reassembly);
    RUN_TEST(test_chunked_missing_chunk);
    RUN_TEST(test_ttn_chunked_downlink);
    RUN_TEST(test_chunked_out_of_order);
    RUN_TEST(test_signature_validation_valid);
    RUN_TEST(test_signature_validation_invalid);
    return UNITY_END();
} 