#ifndef TYPETESTERDYNTRACER_ZSTD_COMPRESSION_STREAM_H
#define TYPETESTERDYNTRACER_ZSTD_COMPRESSION_STREAM_H

#include "Stream.h"

#include <zstd.h>

class ZstdCompressionStream: public Stream {
  public:
    ZstdCompressionStream(Stream* sink, int compression_level)
        : Stream(sink)
        , compression_level_{compression_level}
        , input_buffer_{nullptr}
        , input_buffer_size_{0}
        , input_buffer_index_{0}
        , output_buffer_{nullptr}
        , output_buffer_size_{0}
        , compression_stream_{nullptr} {
        input_buffer_size_ = ZSTD_CStreamInSize();
        input_buffer_ = static_cast<char*>(malloc_or_die(input_buffer_size_));

        output_buffer_size_ = ZSTD_CStreamOutSize();
        output_buffer_ = static_cast<char*>(malloc_or_die(output_buffer_size_));

        compression_stream_ = ZSTD_createCStream();
        if (compression_stream_ == NULL) {
            fprintf(stderr, "ZSTD_createCStream() error \n");
            exit(EXIT_FAILURE);
        }

        const size_t init_result =
            ZSTD_initCStream(compression_stream_, compression_level_);

        if (ZSTD_isError(init_result)) {
            fprintf(stderr,
                    "ZSTD_initCStream() error : %s \n",
                    ZSTD_getErrorName(init_result));
            exit(EXIT_FAILURE);
        }
    }

    int get_compression_level() const {
        return compression_level_;
    }

    void write(const void* buffer, std::size_t bytes) override {
        const char* buf = static_cast<const char*>(buffer);
        while (bytes != 0) {
            std::size_t copied_bytes =
                std::min(input_buffer_size_ - input_buffer_index_, bytes);
            std::memcpy(input_buffer_ + input_buffer_index_, buf, copied_bytes);
            buf += copied_bytes;
            input_buffer_index_ += copied_bytes;
            bytes -= copied_bytes;
            if (input_buffer_index_ == input_buffer_size_)
                flush();
        }
    }

    void flush() override {
        if (output_buffer_ == nullptr || input_buffer_ == nullptr) {
            return;
        }
        ZSTD_inBuffer input{input_buffer_, input_buffer_index_, 0};
        ZSTD_outBuffer output{output_buffer_, output_buffer_size_, 0};

        while (input.pos < input.size) {
            output.pos = 0;
            /* compressed_bytes is guaranteed to be <= ZSTD_CStreamInSize() */
            std::size_t compressed_bytes =
                ZSTD_compressStream(compression_stream_, &output, &input);

            if (ZSTD_isError(compressed_bytes)) {
                fprintf(stderr,
                        "ZSTD_compressStream() error : %s \n",
                        ZSTD_getErrorName(compressed_bytes));
                exit(EXIT_FAILURE);
            }

            /* Safely handle case when `buffInSize` is manually changed to a
               value < ZSTD_CStreamInSize()*/
            if (compressed_bytes > input.size) {
                compressed_bytes = input.size;
            }

            get_sink()->write(output.dst, output.pos);
        }

        input_buffer_index_ = 0;
    }

    void finalize() {
        if (output_buffer_ == nullptr || input_buffer_ == nullptr) {
            return;
        }
        flush();
        size_t unflushed;
        ZSTD_outBuffer output = {output_buffer_, output_buffer_size_, 0};
        while ((unflushed = ZSTD_endStream(compression_stream_, &output))) {
            /* close frame */
            if (ZSTD_isError(unflushed)) {
                fprintf(stderr,
                        "ZSTD_endStream() error : %s \n",
                        ZSTD_getErrorName(unflushed));
                exit(EXIT_FAILURE);
            }
            get_sink()->write(output.dst, output.pos);
            output.pos = 0;
        }
        /* handles the remaining output in buffer after the loop */
        get_sink()->write(output.dst, output.pos);

        ZSTD_freeCStream(compression_stream_);
        std::free(input_buffer_);
        input_buffer_ = nullptr;
        input_buffer_size_ = 0;
        input_buffer_index_ = 0;
        std::free(output_buffer_);
        output_buffer_ = nullptr;
        output_buffer_size_ = 0;
    }

    virtual ~ZstdCompressionStream() {
        finalize();
    }

  private:
    int compression_level_;
    char* input_buffer_;
    std::size_t input_buffer_size_;
    std::size_t input_buffer_index_;
    char* output_buffer_;
    std::size_t output_buffer_size_;
    ZSTD_CStream* compression_stream_;
};

#endif /* TYPETESTERDYNTRACER_ZSTD_COMPRESSION_STREAM_H */
