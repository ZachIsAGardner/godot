/*************************************************************************/
/*  movie_writer.h                                                       */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef MOVIE_WRITER_H
#define MOVIE_WRITER_H

#include "core/templates/local_vector.h"
#include "servers/audio/audio_driver_dummy.h"
#include "servers/audio_server.h"

class MovieWriter : public Object {
	GDCLASS(MovieWriter, Object);

	uint64_t fps = 0;
	uint64_t mix_rate = 0;
	uint32_t audio_channels = 0;

	LocalVector<int32_t> audio_mix_buffer;

	enum {
		MAX_WRITERS = 8
	};
	static MovieWriter *writers[];
	static uint32_t writer_count;

protected:
	virtual uint32_t get_audio_mix_rate() const;
	virtual AudioServer::SpeakerMode get_audio_speaker_mode() const;

	virtual Error write_begin(const Size2i &p_movie_size, uint32_t p_fps, const String &p_base_path);
	virtual Error write_frame(const Ref<Image> &p_image, const int32_t *p_audio_data);
	virtual void write_end();

	GDVIRTUAL0RC(uint32_t, _get_audio_mix_rate)
	GDVIRTUAL0RC(AudioServer::SpeakerMode, _get_audio_speaker_mode)

	GDVIRTUAL1RC(bool, _handles_file, const String &)
	GDVIRTUAL0RC(Vector<String>, _get_supported_extensions)

	GDVIRTUAL3R(Error, _write_begin, const Size2i &, uint32_t, const String &)
	GDVIRTUAL2R(Error, _write_frame, const Ref<Image> &, GDNativeConstPtr<int32_t>)
	GDVIRTUAL0(_write_end)

	static void _bind_methods();

public:
	virtual bool handles_file(const String &p_path) const;
	virtual void get_supported_extensions(List<String> *r_extensions) const;

	static void add_writer(MovieWriter *p_writer);
	static MovieWriter *find_writer_for_file(const String &p_file);

	void begin(const Size2i &p_movie_size, uint32_t p_fps, const String &p_base_path);
	void add_frame(const Ref<Image> &p_image);

	static void set_extensions_hint();

	void end();
};

class MovieWriterPNGWAV : public MovieWriter {
	GDCLASS(MovieWriterPNGWAV, MovieWriter)

	enum {
		MAX_TRAILING_ZEROS = 8 // more than 10 days at 60fps, no hard drive can put up with this anyway :)
	};

	uint32_t mix_rate = 48000;
	AudioServer::SpeakerMode speaker_mode = AudioServer::SPEAKER_MODE_STEREO;
	String base_path;
	uint32_t frame_count = 0;
	uint32_t fps = 0;

	uint32_t audio_block_size = 0;

	Ref<FileAccess> f_wav;
	uint32_t wav_data_size_pos = 0;

	String zeros_str(uint32_t p_index);

protected:
	virtual uint32_t get_audio_mix_rate() const override;
	virtual AudioServer::SpeakerMode get_audio_speaker_mode() const override;
	virtual void get_supported_extensions(List<String> *r_extensions) const override;

	virtual Error write_begin(const Size2i &p_movie_size, uint32_t p_fps, const String &p_base_path) override;
	virtual Error write_frame(const Ref<Image> &p_image, const int32_t *p_audio_data) override;
	virtual void write_end() override;

	virtual bool handles_file(const String &p_path) const override;

public:
	MovieWriterPNGWAV();
};

#endif // VIDEO_WRITER_H
