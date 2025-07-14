#include <unity.h>
#include <string>
#include <vector>
#include <algorithm>

constexpr int OTA_MAX_CHUNKS = 10;
constexpr int OTA_CHUNK_SIZE = 51;

struct OtaChunkBuffer {
    std::string chunks[OTA_MAX_CHUNKS];
    bool received[OTA_MAX_CHUNKS];
    int total_chunks = 0;
    int received_count = 0;
    void reset() {
        for (int i = 0; i < OTA_MAX_CHUNKS; ++i) {
            chunks[i] = "";
            received[i] = false;
        }
        total_chunks = 0;
        received_count = 0;
    }
};
OtaChunkBuffer ota_chunk_buffer;

void handleOtaChunk(const uint8_t* data, uint8_t dataLen, uint8_t fport) {
    if (fport < 1 || fport > OTA_MAX_CHUNKS) return;
    int idx = fport - 1;
    std::string chunk((const char*)data, dataLen);
    if (!ota_chunk_buffer.received[idx]) {
        ota_chunk_buffer.chunks[idx] = chunk;
        ota_chunk_buffer.received[idx] = true;
        ota_chunk_buffer.received_count++;
    }
    if (ota_chunk_buffer.total_chunks == 0) {
        int max_idx = 0;
        for (int i = 0; i < OTA_MAX_CHUNKS; ++i) {
            if (ota_chunk_buffer.received[i]) max_idx = i + 1;
        }
        ota_chunk_buffer.total_chunks = max_idx;
    }
}

bool tryReassembleOtaJson(std::string& out_json) {
    if (ota_chunk_buffer.total_chunks == 0 || ota_chunk_buffer.received_count < ota_chunk_buffer.total_chunks) return false;
    out_json.clear();
    for (int i = 0; i < ota_chunk_buffer.total_chunks; ++i) {
        out_json += ota_chunk_buffer.chunks[i];
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
        handleOtaChunk((const uint8_t*)chunk.data(), len, i + 1);
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
        handleOtaChunk((const uint8_t*)chunk.data(), len, i + 1);
    }
    std::string reassembled;
    TEST_ASSERT_FALSE(tryReassembleOtaJson(reassembled));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_chunked_reassembly);
    RUN_TEST(test_chunked_missing_chunk);
    return UNITY_END();
} 