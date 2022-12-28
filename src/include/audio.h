#include<miniaudio/miniaudio.h>
#include <cstdlib>
#include <cassert>
#include <vector>
namespace pj24 {

    namespace pj2::audio {
#define SAMPLE_FORMAT   ma_format_f32
#define CHANNEL_COUNT   2
#define SAMPLE_RATE     44100
        void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
        struct Audiodev
        {
            static constexpr int64_t framecapa = 8;
            static constexpr int64_t framesize = 2 * 1024* CHANNEL_COUNT;
            static constexpr int64_t framebyte = 4;
            ma_event g_stopEvent; /* <-- Signaled by the audio thread, waited on by the main thread. */
            ma_device device;
            ma_device_config deviceConfig;
            int buf[framesize*framecapa];
            int64_t rem = 0, front = 0;
            ma_format format;
            int SRATE;
            int channelsz;
            void init() {
                ma_result result;
                ma_uint32 iDecoder;
                deviceConfig = ma_device_config_init(ma_device_type_playback);
                deviceConfig.playback.format = SAMPLE_FORMAT;
                deviceConfig.playback.channels = CHANNEL_COUNT;
                deviceConfig.sampleRate = SAMPLE_RATE;
                deviceConfig.dataCallback = data_callback;
                deviceConfig.pUserData = this;
                deviceConfig.periodSizeInMilliseconds = 17;
                    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
                        std::abort();
                    }
                /* switch (format) {
                 case ma_format_s16: return MA_PA_SAMPLE_S16LE;
                 case ma_format_s24: return MA_PA_SAMPLE_S24LE;
                 case ma_format_s32: return MA_PA_SAMPLE_S32LE;
                 case ma_format_f32: return MA_PA_SAMPLE_FLOAT32LE;
                 default: break;
                 }*/
            }
            constexpr static void SampleHalf(void* const indata, void* const outdata, size_t size, const int unitblock) {
                size /= 2;
                for (size_t i = 0; i < size; i++)
                {
                    memcpy((char*)outdata + i, (char*)indata + i * 2 * unitblock, unitblock);
                }
            }
            void pushFrame(void* const data) {
                rem += framesize;
                std::memcpy(&buf[(framesize) * (front++ % framecapa)], data, framesize);
            }
            void PopFrame(void* const data, size_t frameCount) {
                auto dis = rem /framesize;
                if (dis >= 2) {
                    SampleHalf(data, &buf[(framesize) * (front % framecapa)-rem], frameCount * 2, CHANNEL_COUNT * framebyte * 4);
                    //rem+= frameCount*2;
                    rem -= 2 * frameCount;
                }
                else
                {
                    for (size_t i = 0; i < frameCount; i++)
                    {
                        std::memcpy(data, &buf[(framesize) * (rem % framecapa)], framesize);
                    }
                    rem -= frameCount;
                }
            }
            template<class B>
            ma_uint32 decodeto(B&& WriteTo, char* path,
                ma_decoder_config decoderConfig = ma_decoder_config_init(SAMPLE_FORMAT, CHANNEL_COUNT, SAMPLE_RATE),
                const uint32_t perWritesz = 4096) {

                ma_decoder dc;
                ma_result result = ma_decoder_init_file(path, &decoderConfig, &dc);
                if (result != MA_SUCCESS) {
                    ma_decoder_uninit(&dc);
                    return 0;
                }
                int frmpross = 0;// bkpros = 4096;
                while (true)
                {
                    ma_uint32 framesRead = ma_decoder_read_pcm_frames(&dc, WriteTo(frmpross), perWritesz);
                    frmpross += perWritesz;
                    if (framesRead < perWritesz)
                        break;
                }
                return frmpross;
            }
            void* const data() {
                return buf;
            }
        private:
            ~Audiodev()
            {
                ma_device_uninit(&device);
                /*
                If at the end all of our decoders are at the end we need to stop. We cannot stop the device in the callback. Instead we need to
                signal an event to indicate that it's stopped. The main thread will be waiting on the event, after which it will stop the device.
                */
                ma_event_signal(&g_stopEvent);
            }
        };
        void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
        {
            float* pOutputF32 = (float*)pOutput;
            ma_uint32 iDecoder;
            Audiodev& ad = *(Audiodev*)pDevice->pUserData;
            assert(pDevice->playback.format == SAMPLE_FORMAT);
            ad.PopFrame(pOutput, frameCount);

            (void)pInput;
        }
    }
}