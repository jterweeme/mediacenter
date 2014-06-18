#include <system.h>
#include <stdlib.h>
#include "misc.h"
#include "md5.h"
#include "sdcard.h"

class MD5Test1
{
private:
    Uart uart;
    VGATerminal vga;
    SDCardEx sdCard;
    static const uint32_t SD_BASE = ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE;
public:
    MD5Test1();
    int run();
};

void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest)
{
    uint32_t h0, h1, h2, h3;
    uint8_t *msg = NULL;
    size_t new_len, offset;
    uint32_t w[16];
    uint32_t a, b, c, d, i, f, g, temp;
    h0 = 0x67452301;
    h1 = 0xefcdab89;
    h2 = 0x98badcfe;
    h3 = 0x10325476;
    for (new_len = initial_len + 1; new_len % (512/8) != 448/8; new_len++);
    msg = new uint8_t[new_len + 8];
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 0x80; // append the "1" bit; most significant bit is "first"
    for (offset = initial_len + 1; offset < new_len; offset++)
        msg[offset] = 0; // append "0" bits

    Utility::to_bytes(initial_len*8, msg + new_len);
    Utility::to_bytes(initial_len>>29, msg + new_len + 4);

    for(offset=0; offset<new_len; offset += (512/8)) {

        for (i = 0; i < 16; i++)
            w[i] = Utility::to_int32(msg + offset + i*4);

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
    Utility::to_bytes(h0, digest);
    Utility::to_bytes(h1, digest + 4);
    Utility::to_bytes(h2, digest + 8);
    Utility::to_bytes(h3, digest + 12);
}

MD5Test1::MD5Test1()
  :
    uart((uint32_t *)UART_BASE),
    vga("/dev/video_character_buffer_with_dma_0"),
    sdCard(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME, (void *)SD_BASE)
{
    uart.puts("Initialze MD5Test1\r\n");
    vga.clear();
    vga.puts("Initialize MD5Test1\r\n");
}

int MD5Test1::run()
{
    if (sdCard.isPresent() && sdCard.isFAT16())
    {
        MyFile *myFile;
        uint8_t result[16];
        myFile = sdCard.openFile("vogue.kar");
        t_file_record testRecord = sdCard.active_files[myFile->fd];
        size_t fileSize = testRecord.file_size_in_bytes;
        uint8_t buf[fileSize];

        for (size_t i = 0; i < fileSize && myFile != NULL; i++)
            buf[i] = myFile->read();

        uart.puts("bestand ingelezen\r\n");
        vga.puts("bestand ingelezen\r\n");
        md5(buf, fileSize, result);
        char x[3] = {0};

        for (int i = 0; i < 16; i++)
        {
            Utility::toHex(result[i], x);
            uart.puts(x);
            vga.puts(x);
        }

        uart.puts("\r\ndone\r\n");
        vga.puts("\r\ndone\r\n");
    }
    return 0;
}

int main()
{
    MD5Test1 md5test1;
    return md5test1.run();
}


