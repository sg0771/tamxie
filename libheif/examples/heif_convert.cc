/*
  libheif example application "convert".

  MIT License

  Copyright (c) 2017 struktur AG, Joachim Bauch <bauch@struktur.de>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
//#if defined(HAVE_CONFIG_H)
//#include "heif-config.h"
//#endif

#define HAVE_LIBJPEG  1

#include "string.h"

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include <fstream>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <cctype>

#include <libheif/heif.h>

#include "encoder.h"
#if HAVE_LIBJPEG
#include "encoder_jpeg.h"
#endif


void _Register_Default_Plugins();//初始化DE265 库，原来的写法会崩溃
int main()
{
    _Register_Default_Plugins();
  int quality = 80;  // Use default quality.
  std::string input_filename = "D:\\a.heic";
  std::string output_filename = "D:\\___a.heic.jpg";

  std::unique_ptr<Encoder> m_encoder;
  m_encoder.reset(new JpegEncoder(quality));
  if (!m_encoder) {
    fprintf(stderr, "Unknown file type in %s\n", output_filename.c_str());
    return 1;
  }

    //通过前12个字节判断是否HEIC文件
  std::ifstream istr(input_filename.c_str(), std::ios_base::binary);
  uint8_t magic[12];
  istr.read((char*)magic,12);
  enum heif_filetype_result filetype_check = heif_check_filetype(magic,12);
  if (filetype_check == heif_filetype_no ||
      filetype_check == heif_filetype_yes_unsupported) {
    fprintf(stderr, "Input file is Format Error\n");
    return 1;
  }
  istr.close();


  FILE* handle = fopen(input_filename.c_str(), "rb");
  fseek(handle, 0, SEEK_END);
  int64_t file_size = ftell(handle);
  fseek(handle, 0, SEEK_SET);
  std::shared_ptr<uint8_t> buffer = std::shared_ptr<uint8_t>(new uint8_t[file_size]);
  fread(buffer.get(), file_size, 1, handle);
  fclose(handle);

  std::shared_ptr<heif_context> m_ctx = std::shared_ptr<heif_context>(heif_context_alloc(),
                                        [] (heif_context* c) { heif_context_free(c); });

  struct heif_error err;
  err = heif_context_read_from_memory(m_ctx.get(), buffer.get(), file_size, nullptr);
  if (err.code != 0) {
    return NULL;
  }
  int num_images = heif_context_get_number_of_top_level_images(m_ctx.get());
  if (num_images == 0) { 
    return NULL;
  }

  heif_item_id* m_image_IDs = (heif_item_id*)malloc(num_images * sizeof(heif_item_id));
  heif_context_get_list_of_top_level_image_IDs(m_ctx.get(), m_image_IDs, num_images);

  //如果有多帧图像，解码第2帧
  size_t image_index = 0;  // Image filenames are "1" based.
  if (num_images > 1) {
      image_index = 1;
  }
  struct heif_image_handle* m_handle = nullptr;
  err = heif_context_get_image_handle(m_ctx.get(), m_image_IDs[image_index], &m_handle);
  if (err.code) {//获取第几帧的句柄
      return NULL;
  }

  int has_alpha = heif_image_handle_has_alpha_channel(m_handle);  //是否包含透明通道
  struct heif_decoding_options* m_decode_options = heif_decoding_options_alloc();

  int _bit_depth = heif_image_handle_get_luma_bits_per_pixel(m_handle);
  if (_bit_depth < 0) { //图像位深度
      heif_decoding_options_free(m_decode_options);
      heif_image_handle_release(m_handle);
      return NULL;
  }
  //解码操作
  struct heif_image* m_image = nullptr;
  err = heif_decode_image(m_handle, &m_image,heif_colorspace_YCbCr,heif_chroma_420,  m_decode_options);
  if (err.code) {
      heif_image_handle_release(m_handle);
      return NULL;
  }

  if (m_image) { //解码成功
      bool written = m_encoder->Encode(m_handle, m_image, output_filename.c_str());
      if (!written) {
          printf("could not write image\n");
      }else {
          printf("Written to %s\n", output_filename.c_str());
      }
  }
  heif_decoding_options_free(m_decode_options); //解码出图像
  heif_image_release(m_image);
  heif_image_handle_release(m_handle);

  system("pause");
  return 0;
}
