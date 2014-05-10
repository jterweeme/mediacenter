#include <altera_up_avalon_video_character_buffer_with_dma.h>

int main()
{
    alt_up_char_buffer_dev *charbuffer;
    charbuffer = alt_up_char_buffer_open_dev("/dev/video_character_buffer_with_dma_0");
    alt_up_char_buffer_draw(charbuffer, 'Z', 30, 30);
    return 0;
}


