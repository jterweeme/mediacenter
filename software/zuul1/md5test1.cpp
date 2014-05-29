#include <system.h>
#include <stdlib.h>
#include "misc.h"
#include "md5.h"

class MD5Test1
{
public:
    void init();
private:
    Uart *uart;
    SDCardEx *sdCard;
};

void to_bytes(uint32_t val, uint8_t *bytes)
{
    bytes[0] = (uint8_t) val;
    bytes[1] = (uint8_t) (val >> 8);
    bytes[2] = (uint8_t) (val >> 16);
    bytes[3] = (uint8_t) (val >> 24);
}

uint32_t to_int32(const uint8_t *bytes)
{
    return (uint32_t) bytes[0]
        | ((uint32_t) bytes[1] << 8)
        | ((uint32_t) bytes[2] << 16)
        | ((uint32_t) bytes[3] << 24);
}

void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest) {

    // These vars will contain the hash
    uint32_t h0, h1, h2, h3;

    // Message (to prepare)
    uint8_t *msg = NULL;

    size_t new_len, offset;
    uint32_t w[16];
    uint32_t a, b, c, d, i, f, g, temp;

    // Initialize variables - simple count in nibbles:
    h0 = 0x67452301;
    h1 = 0xefcdab89;
    h2 = 0x98badcfe;
    h3 = 0x10325476;

    //Pre-processing:
    //append "1" bit to message    
    //append "0" bits until message length in bits ≡ 448 (mod 512)
    //append length mod (2^64) to message
    for (new_len = initial_len + 1; new_len % (512/8) != 448/8; new_len++)
        ;

    //msg = malloc(new_len + 8);
    msg = new uint8_t[new_len + 8];
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 0x80; // append the "1" bit; most significant bit is "first"
    for (offset = initial_len + 1; offset < new_len; offset++)
        msg[offset] = 0; // append "0" bits

    // append the len in bits at the end of the buffer.
    to_bytes(initial_len*8, msg + new_len);
    // initial_len>>29 == initial_len*8>>32, but avoids overflow.
    to_bytes(initial_len>>29, msg + new_len + 4);

    // Process the message in successive 512-bit chunks:
    //for each 512-bit chunk of message:
    for(offset=0; offset<new_len; offset += (512/8)) {

        // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
        for (i = 0; i < 16; i++)
            w[i] = to_int32(msg + offset + i*4);
        // Initialize hash value for this chunk:
        a = h0;
        b = h1;
        c = h2;
        d = h3;

        // Main loop:
        for(i = 0; i<64; i++) {

            if (i < 16) {
                f = (b & c) | ((~b) & d);
                g = i;
            } else if (i < 32) {
                f = (d & b) | ((~d) & c);
                g = (5*i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3*i + 5) % 16;
            } else {
                f = c ^ (b | (~d));
                g = (7*i) % 16;
            }

            temp = d;
            d = c;
            c = b;
            b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
            a = temp;

        }

        // Add this chunk's hash to result so far:
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;

    }

    // cleanup
    free(msg);

    //var char digest[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)
    to_bytes(h0, digest);
    to_bytes(h1, digest + 4);
    to_bytes(h2, digest + 8);
    to_bytes(h3, digest + 12);
}

void toHex(uint8_t input, char *output)
{
    uint8_t foo = input & 0x0f;
    uint8_t bar = input >> 4;
    output[1] = foo < 10 ? (char)(foo + 48) : (char)(foo + 55);
    output[0] = bar < 10 ? (char)(bar + 48) : (char)(bar + 55);
}

void MD5Test1::init()
{
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("Initialize MD5Test1\r\n");
    sdCard = new SDCardEx();
    sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
    size_t fileSize = 41044;
    uint8_t buf[fileSize];
    MyFile *myFile;
    uint8_t result[16];
    char outputString[100] = {0};

    if (sdCard->isPresent() && sdCard->isFAT16())
    {
        myFile = sdCard->openFile("VOGUE.KAR");

        for (int i = 0; i < fileSize && myFile != NULL; i++)
            buf[i] = myFile->read();

        uart->puts("bestand ingelezen\r\n");
        md5(buf, fileSize, result);
        char x[3] = {0};

        for (int i = 0; i < 16; i++)
        {
            toHex(result[i], x);
            uart->puts(x);
        }
            

        uart->puts("\r\ndone\r\n");
    }
}

int main()
{
    MD5Test1 md5test1;
    md5test1.init();
    return 0;
}


