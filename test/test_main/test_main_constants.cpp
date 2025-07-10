#include <unity.h>

#include <fstream>
#include <sstream>
#include <string>

// Helper that loads the whole file into a string.  Throws on failure because
// the unit-test framework will treat uncaught exceptions as test failures.
static std::string load_source_file(const std::string &path) {
    std::ifstream ifs(path);
    if (!ifs) {
        throw std::runtime_error("Failed to open " + path);
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

// Extract the integer literal that appears after an '=' in a `#define` line.
static int extract_define_value(const std::string &source, const std::string &name) {
    auto pos = source.find("#define " + name);
    TEST_ASSERT_TRUE_MESSAGE(pos != std::string::npos, ("Missing #define for " + name).c_str());

    // Move past the define and any whitespace up to the value.
    pos = source.find_first_of("0123456789", pos);
    TEST_ASSERT_NOT_EQUAL(-1, static_cast<int>(pos));

    // Read the number.
    size_t end = pos;
    while (end < source.size() && isdigit(static_cast<unsigned char>(source[end]))) {
        ++end;
    }
    return std::stoi(source.substr(pos, end - pos));
}

void test_constants_are_as_expected() {
    std::string src;
    try {
        src = load_source_file("src/main.cpp");
    } catch (...) {
        // When the executable is launched from the build directory the path
        // needs one extra level up.
        src = load_source_file("../src/main.cpp");
    }

    // Validate a handful of critical compile-time constants.  The concrete
    // numbers encode hardware layout or timing behaviour and should therefore
    // remain stable.
    TEST_ASSERT_EQUAL_INT_MESSAGE(13, extract_define_value(src, "VCC_ENA_PIN"), "VCC_ENA_PIN changed – verify hardware wiring");
    TEST_ASSERT_EQUAL_INT_MESSAGE(16, extract_define_value(src, "START_WEB_CONFIG_PIN"), "START_WEB_CONFIG_PIN changed – update schematic");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, extract_define_value(src, "NUM_LEDS"), "NUM_LEDS changed – revisit power budget and animations");
    TEST_ASSERT_EQUAL_INT_MESSAGE(17, extract_define_value(src, "LED_DATA_PIN"), "LED_DATA_PIN changed – verify PCB routing");

    // TX_INTERVAL is declared as a `const unsigned` rather than a macro – we
    // locate it textually as well for simplicity.
    TEST_ASSERT_TRUE_MESSAGE(src.find("const unsigned TX_INTERVAL = 3600") != std::string::npos,
                             "TX_INTERVAL expected to be 3600 seconds");
}

// ---------------------------------------------------------------------------
//  Unity test runner
// ---------------------------------------------------------------------------

void setUp() {}
void tearDown() {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_constants_are_as_expected);
    return UNITY_END();
}
