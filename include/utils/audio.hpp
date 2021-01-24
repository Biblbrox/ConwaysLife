#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <SDL_mixer.h>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <functional>
#include <memory>

using chunkDeleter = std::function<void(Mix_Chunk*)>;
using musDeleter = std::function<void(Mix_Music*)>;

namespace utils::audio
{
    class Audio
    {
    public:
        Audio();
        ~Audio();

        void addChunk(const std::string &chunkPath);

        void setMusic(const std::string &musicPath);

        void playChunk(int channel, size_t idx, int loops, bool faded = false);

        void pauseChannel(int channel, bool faded = false);

        void haltChannel(int channel, bool faded = false);

        bool isChannelPaused(int channel);

        void resumeChannel(int channel);

        bool isChannelPlaying(int channel);

        void playMusic();

        void pauseMusic();

        /**
         * Set fade time for chunk start
         * @param in
         */
        void setFadeIn(GLfloat in);

        /**
         * Set fade time for chunk end
         * @param out
         */
        void setFadeOut(GLfloat out);

    private:
        std::vector<std::unique_ptr<Mix_Chunk, chunkDeleter>> m_chunks;
        std::unique_ptr<Mix_Music, musDeleter> m_music;
        GLfloat m_fadeIn;
        GLfloat m_fadeOut;
    };
}

#endif //AUDIO_HPP
