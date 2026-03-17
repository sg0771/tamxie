#include <assert.h>
#include <errno.h>
#include <string.h>

#include <iostream>

#include "encoder_jpeg.h"

JpegEncoder::JpegEncoder(int quality) : quality_(quality) {
  if (quality_ < 0 || quality_ > 100) {
    quality_ = kDefaultQuality;
  }
}

void JpegEncoder::UpdateDecodingOptions(const struct heif_image_handle* handle,
    struct heif_decoding_options *options) const {
  if (HasExifMetaData(handle)) {
    // options->ignore_transformations = 1;
  }
}

// static
void JpegEncoder::OnJpegError(j_common_ptr cinfo) {
  ErrorHandler* handler = reinterpret_cast<ErrorHandler*>(cinfo->err);
  longjmp(handler->setjmp_buffer, 1);
}

bool JpegEncoder::Encode(const struct heif_image_handle* handle,
    const struct heif_image* image, const std::string& filename) {
  FILE* fp = fopen(filename.c_str(), "wb");
  if (!fp) {
    fprintf(stderr, "Can't open %s: %s\n", filename.c_str(), strerror(errno));
    return false;
  }

  struct jpeg_compress_struct cinfo;
  struct ErrorHandler jerr;
  cinfo.err = jpeg_std_error(reinterpret_cast<struct jpeg_error_mgr*>(&jerr));
  jerr.pub.error_exit = &JpegEncoder::OnJpegError;
  if (setjmp(jerr.setjmp_buffer)) {
    cinfo.err->output_message(reinterpret_cast<j_common_ptr>(&cinfo));
    jpeg_destroy_compress(&cinfo);
    fclose(fp);
    return false;
  }

  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, fp);

  cinfo.image_width  = heif_image_get_width(image, heif_channel_Y);  //分辨率宽度
  cinfo.image_height = heif_image_get_height(image, heif_channel_Y);//分辨率高度
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_YCbCr;
  jpeg_set_defaults(&cinfo);
  static const boolean kForceBaseline = TRUE;
  jpeg_set_quality(&cinfo, quality_, kForceBaseline); //设置JPEG编码参数
  static const boolean kWriteAllTables = TRUE;
  jpeg_start_compress(&cinfo, kWriteAllTables);

  size_t exifsize = 0;
  uint8_t* exifdata = GetExifMetaData(handle, &exifsize);//EXIF 数据
  if (exifdata && exifsize > 4) {
    static const uint8_t kExifMarker = JPEG_APP0 + 1;
    jpeg_write_marker(&cinfo, kExifMarker, exifdata + 4,
        static_cast<unsigned int>(exifsize - 4));
    free(exifdata);
  }

  size_t profile_size = heif_image_handle_get_raw_color_profile_size(handle);//icc数据
  if (profile_size > 0){
    uint8_t* profile_data = static_cast<uint8_t*>(malloc(profile_size));
    heif_image_handle_get_raw_color_profile(handle, profile_data);//icc数据
    jpeg_write_icc_profile(&cinfo, profile_data, (unsigned int)profile_size);
    free(profile_data);
  }


  if (heif_image_handle_get_luma_bits_per_pixel(handle) != 8 ||
      heif_image_handle_get_chroma_bits_per_pixel(handle) != 8) {
    fprintf(stderr, "JPEG writer cannot handle image with >8 bpp.\n");
    return false;
  }


  //yuv 解码数据
   //YUVJ420P
  int stride_y;
  const uint8_t* row_y = heif_image_get_plane_readonly(image, heif_channel_Y,
      &stride_y);
  int stride_u;
  const uint8_t* row_u = heif_image_get_plane_readonly(image, heif_channel_Cb,
      &stride_u);
  int stride_v;
  const uint8_t* row_v = heif_image_get_plane_readonly(image, heif_channel_Cr,
      &stride_v);

  JSAMPARRAY buffer = cinfo.mem->alloc_sarray(
      reinterpret_cast<j_common_ptr>(&cinfo), JPOOL_IMAGE,
      cinfo.image_width * cinfo.input_components, 1);
  JSAMPROW row[1] = { buffer[0] };

  while (cinfo.next_scanline < cinfo.image_height) {
    size_t offset_y = cinfo.next_scanline * stride_y;
    const uint8_t* start_y = &row_y[offset_y];
    size_t offset_u = (cinfo.next_scanline / 2) * stride_u;
    const uint8_t* start_u = &row_u[offset_u];
    size_t offset_v = (cinfo.next_scanline / 2) * stride_v;
    const uint8_t* start_v = &row_v[offset_v];

    JOCTET* bufp = buffer[0];
    for (JDIMENSION x = 0; x < cinfo.image_width; ++x) {
      *bufp++ = start_y[x];
      *bufp++ = start_u[x / 2];
      *bufp++ = start_v[x / 2];
    }
    jpeg_write_scanlines(&cinfo, row, 1);
  }
  jpeg_finish_compress(&cinfo);
  fclose(fp);
  jpeg_destroy_compress(&cinfo);
  return true;
}
