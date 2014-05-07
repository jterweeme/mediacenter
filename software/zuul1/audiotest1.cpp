#include <altera_up_avalon_audio.h>
#include <stdio.h>

int main()
{
    alt_up_audio_dev *audio_dev;
    unsigned int l_buf, r_buf;

    audio_dev = alt_up_audio_open_dev("/dev/Audio");

    if (audio_dev == NULL)
        ::printf("Error: could not open audio device\n");
    else
        ::printf("Opened audio device\n");

    unsigned int buf[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

    while (1)
    {
        alt_up_audio_play_r(audio_dev, buf, 19);
/*
        int fifospace = ::alt_up_audio_read_fifo_avail(audio_dev, ALT_UP_AUDIO_RIGHT);

        if (fifospace > 0)
        {
            ::alt_up_audio_read_fifo(audio_dev, &(r_buf), 1, ALT_UP_AUDIO_RIGHT);
            ::alt_up_audio_write_fifo(audio_dev, &(l_buf), 1, ALT_UP_AUDIO_RIGHT);
        }
*/
    }
    return 0;
}


