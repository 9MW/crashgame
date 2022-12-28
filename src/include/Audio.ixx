
#include<miniaudio/miniaudio.h>
#include <cstdlib>
#include <cassert>
#include <vector>
export module Audio;
export ma_decoder_config;
export ma_format;
export ma_device_config;
namespace pj2::audio{
#define SAMPLE_FORMAT   ma_format_f32
#define CHANNEL_COUNT   2
#define SAMPLE_RATE     44100
	//export void MyFunc();
    export
    //template<class FM=float>

    struct Audiodev
    {
        static constexpr int bufsz = 8 * 1024;
        static constexpr int framesize = 2*1024;
        int buf[bufsz];
        int back = 0, front = 0;
        ma_format format;
        int SRATE;
        int channelsz;
        ma_event g_stopEvent; /* <-- Signaled by the audio thread, waited on by the main thread. */
        ma_device device;
        ma_device_config deviceConfig;
        void init() {
            ma_result result;
            ma_uint32 iDecoder;
            deviceConfig = ma_device_config_init(ma_device_type_playback);
            deviceConfig.playback.format = SAMPLE_FORMAT;
            deviceConfig.playback.channels = CHANNEL_COUNT;
            deviceConfig.sampleRate = SAMPLE_RATE;
            deviceConfig.dataCallback = data_callback;
            deviceConfig.pUserData = NULL;
            deviceConfig.periodSizeInMilliseconds = 17
            if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
                std::abord();
            }
           /* switch (format) {
            case ma_format_s16: return MA_PA_SAMPLE_S16LE;
            case ma_format_s24: return MA_PA_SAMPLE_S24LE;
            case ma_format_s32: return MA_PA_SAMPLE_S32LE;
            case ma_format_f32: return MA_PA_SAMPLE_FLOAT32LE;
            default: break;
            }*/
        }
        void pushFrame(void* const data){
            
        }
        template<class B>
        ma_uint32 decodeto(B&& WriteTo,char* path,
            ma_decoder_config decoderConfig = ma_decoder_config_init(SAMPLE_FORMAT, CHANNEL_COUNT, SAMPLE_RATE),
            const uint32_t perWritesz=4096) {

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
        void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
        {
            float* pOutputF32 = (float*)pOutput;
            ma_uint32 iDecoder;

            assert(pDevice->playback.format == SAMPLE_FORMAT);   /* <-- Important for this example. */
            for (size_t i = 0; i < frameCount; i++)
            {

            }
            for (iDecoder = 0; iDecoder < g_decoderCount; ++iDecoder) {
                if (!g_pDecodersAtEnd[iDecoder]) {
                    fator = iDecoder == 0 ? f0 : f1;
                    ma_uint32 framesRead = read_and_mix_pcm_frames_f32(&g_pDecoders[iDecoder], pOutputF32, frameCount);
                    if (framesRead < frameCount) {
                        g_pDecodersAtEnd[iDecoder] = MA_TRUE;
                    }
                }
            }

            (void)pInput;
        }
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
}