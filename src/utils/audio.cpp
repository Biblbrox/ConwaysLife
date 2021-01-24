#include <boost/format.hpp>
#include <filesystem>

#include "exceptions/fsexception.hpp"
#include "exceptions/sdlexception.hpp"
#include "utils/audio.hpp"

using boost::format;

void utils::audio::Audio::setMusic(const std::string &musicPath)
{
    if (!std::filesystem::exists(musicPath))
        throw FSException((format("File %s doesn't exists\n") % musicPath).str());

    Mix_Music *mus = Mix_LoadMUS(musicPath.c_str());
    if (!mus)
        throw SdlException((format("Unable to load music %s\n") % musicPath).str());

    m_music = std::unique_ptr<Mix_Music, musDeleter>(mus, [](Mix_Music* mus){
        if (mus)
            Mix_FreeMusic(mus);
    });
}

void
utils::audio::Audio::playChunk(int channel, size_t idx, int loops, bool faded)
{
    assert(channel >= -1);
    assert(idx < m_chunks.size());
    if (faded)
        Mix_FadeInChannel(channel, m_chunks[idx].get(), loops, m_fadeIn);
    else if (Mix_Paused(channel))
        Mix_Resume(channel);
    else
        Mix_PlayChannel(channel, m_chunks[idx].get(), loops);
}

void utils::audio::Audio::pauseChannel(int channel, bool faded)
{
    assert(channel >= -1);
    if (faded)
        Mix_FadeOutChannel(channel, m_fadeOut);
    else
        Mix_Pause(channel);
}

void utils::audio::Audio::resumeChannel(int channel)
{
    assert(channel >= -1);

    Mix_Resume(channel);
}

void utils::audio::Audio::playMusic()
{
    if (!Mix_PlayingMusic())
        Mix_PlayMusic(m_music.get(), -1);
    else if (Mix_PausedMusic())
        Mix_ResumeMusic();
}

void utils::audio::Audio::pauseMusic()
{
    if (Mix_PlayingMusic())
        Mix_PauseMusic();
}

void utils::audio::Audio::addChunk(const std::string &chunkPath)
{
    if (!std::filesystem::exists(chunkPath))
        throw FSException((format("File %s doesn't exists\n") % chunkPath).str());

    Mix_Chunk *chunk = Mix_LoadWAV(chunkPath.c_str());
    if (!chunk)
        throw SdlException((format("Unable to load chunk %s\n") % chunkPath).str());

    m_chunks.push_back(
            std::unique_ptr<Mix_Chunk, chunkDeleter>(chunk, [](Mix_Chunk* ch) {
                if (ch)
                    Mix_FreeChunk(ch);
            }));
}

bool utils::audio::Audio::isChannelPaused(int channel)
{
    assert(channel >= -1);
    return Mix_Paused(channel);
}

bool utils::audio::Audio::isChannelPlaying(int channel)
{
    assert(channel >= -1);
    return Mix_Playing(channel);
}

void utils::audio::Audio::setFadeIn(GLfloat in)
{
    m_fadeIn = in;
}

void utils::audio::Audio::setFadeOut(GLfloat out)
{
    m_fadeOut = out;
}

utils::audio::Audio::Audio(): m_fadeIn(0.0), m_fadeOut(0.0) {}

void utils::audio::Audio::haltChannel(int channel, bool faded)
{
    assert(channel >= -1);
    if (faded)
        Mix_FadeOutChannel(channel, m_fadeOut);
    else
        Mix_HaltChannel(channel);
}

utils::audio::Audio::~Audio()
{

}