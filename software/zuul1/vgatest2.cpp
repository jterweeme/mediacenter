#include <altera_up_avalon_video_pixel_buffer_dma.h>

int main()
{
    alt_up_pixel_buffer_dma_dev *pixel_buf_dev;

    pixel_buf_dev = alt_up_pixel_buffer_dma_open_dev("/dev/video_pixel_buffer_dma_0");

    alt_up_pixel_buffer_dma_clear_screen(pixel_buf_dev, 0);
    alt_up_pixel_buffer_dma_clear_screen(pixel_buf_dev, 1);
    alt_up_pixel_buffer_dma_draw_box(pixel_buf_dev, 0, 0, 319, 239, 0x001f, 0);
    return 0;
}


