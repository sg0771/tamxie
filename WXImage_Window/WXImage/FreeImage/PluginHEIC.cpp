/*
基于 libde265+libheif 的 HEIC 解码处理
*/
#ifdef _MSC_VER 
#pragma warning (disable : 4786) // identifier was truncated to 'number' characters
#endif

#include "FreeImage.h"
#include "Utilities.h"

#include "FreeImageTag.h"
#include <libheif/heif.h>

#include "libyuv.h"

typedef struct {
	FreeImageIO* s_io;
	fi_handle    s_handle;
} fi_ioStructure, * pfi_ioStructure;

// ==========================================================
// Plugin Interface
// ==========================================================

static int s_format_id;//HEIC

static const char*
Format() {
	return "HEIC";
}

static const char*
Description() {
	return "High Efficiency Image File Format";
}

static const char*
Extension() {
	return "heic";
}

static const char*
RegExpr() {
	return "^.HEIC\r";
}

static const char*
MimeType() {
	return "image/heic";
}

//文件头处理
static BOOL
Validate(FreeImageIO* io, fi_handle handle) {

	BYTE magic[12] = { 0, };
	io->read_proc(&magic, 1, 12, handle);
	enum heif_filetype_result filetype_check = heif_check_filetype(magic, 12);
	if (filetype_check == heif_filetype_no ||
		filetype_check == heif_filetype_yes_unsupported) {
		//printf("Is Not Heif Image\r\n");
		return 0;//不是HEIC图像
	}

	//printf("Is A Heif Image\r\n");
	return 1;//是HEIC图像
}

static BOOL
SupportsExportDepth(int depth) {
	return depth == 8;
}

static BOOL
SupportsExportType(FREE_IMAGE_TYPE type) {
	return  (type == FIT_BITMAP);
}

static BOOL
SupportsICCProfiles() {
	return TRUE;
}

static BOOL
SupportsNoPixels() {
	return TRUE;
}

// --------------------------------------------------------------------------

static FIBITMAP* Load(FreeImageIO* io, fi_handle handle, int page, int flags, void* data) {

	FIBITMAP* dib = NULL; //输出图像

	fi_ioStructure fio;
	fio.s_handle = handle;//操作句柄 一般是FILE* 类型
	fio.s_io = io;//文件IO

	if (handle) {
		BOOL header_only = (flags & FIF_LOAD_NOPIXELS) == FIF_LOAD_NOPIXELS;//仅仅读取文件头
		
		io->seek_proc(handle, 0, SEEK_END);
		int64_t file_size = io->tell_proc(handle);
		io->seek_proc(handle, 0, SEEK_SET);
		std::shared_ptr<uint8_t> file_buffer = std::shared_ptr<uint8_t>(new uint8_t[file_size]);
		io->read_proc(file_buffer.get(), file_size, 1, handle);

		{
			//HEIC 文件容器
			std::shared_ptr<heif_context> m_ctx = std::shared_ptr<heif_context>(heif_context_alloc(),
				[](heif_context* c) { heif_context_free(c); });

			struct heif_error err;
			err = heif_context_read_from_memory(m_ctx.get(), file_buffer.get(), file_size, nullptr);
			if (err.code != 0) {
				//printf("heif_context_read_from_memory Error\r\n");
				return NULL;
			}
			int num_images = heif_context_get_number_of_top_level_images(m_ctx.get());
			if (num_images == 0) {
				//printf(" heif_context_get_number_of_top_level_images Error\r\n");
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
				//printf(" heif_context_get_image_handle Error\r\n");
				return NULL;
			}

			int has_alpha = heif_image_handle_has_alpha_channel(m_handle);  //是否包含透明通道
			struct heif_decoding_options* m_decode_options = heif_decoding_options_alloc();

			int _bit_depth = heif_image_handle_get_luma_bits_per_pixel(m_handle);
			if (_bit_depth < 0) { //图像位深度
				heif_decoding_options_free(m_decode_options);
				heif_image_handle_release(m_handle);

				//printf(" heif_image_handle_get_luma_bits_per_pixel Error\r\n");
				return NULL;
			}
			//解码操作
			struct heif_image* m_image = nullptr;
			err = heif_decode_image(m_handle, &m_image, 
				heif_colorspace_YCbCr,
				heif_chroma_420, m_decode_options);
			if (err.code) {
				heif_image_handle_release(m_handle);
				//printf(" heif_decode_image Error\r\n");
				return NULL;
			}

			if (m_image) { //解码成功
				int width = heif_image_get_width(m_image, heif_channel_Y);  //分辨率宽度
				int height = heif_image_get_height(m_image, heif_channel_Y);//分辨率高度
				int pixel_depth = 24; //RGB
				//分配内存
				dib = FreeImage_AllocateHeaderT(header_only,
					FIT_BITMAP,
					width,
					height,
					pixel_depth,
					FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);

				if (!header_only) {
					//convert yuvj420p  to RGB
					   //YUVJ420P
					int stride_y = 0;
					const uint8_t* src_y = heif_image_get_plane_readonly(m_image, heif_channel_Y,
						&stride_y);
					int stride_u = 0;
					const uint8_t* src_u = heif_image_get_plane_readonly(m_image, heif_channel_Cb,
						&stride_u);
					int stride_v = 0;
					const uint8_t* src_v = heif_image_get_plane_readonly(m_image, heif_channel_Cr,
						&stride_v);


					std::shared_ptr<uint8_t> rgba_buffer = std::shared_ptr<uint8_t>(new uint8_t[width * height * 4]);

					libyuv::J420ToARGB(src_y, stride_y,
						src_u, stride_u,
						src_v, stride_v,
						rgba_buffer.get(), 
						width * 4,
						width, height
					);


					uint8_t* dst = FreeImage_GetBits(dib);;
					int dst_stride = FreeImage_GetPitch(dib);

					libyuv::ARGBToRGB24(
						rgba_buffer.get(),
						width * 4,
						dst + (height - 1) * dst_stride,
						-dst_stride,
						width, height
					);

					size_t profile_size = heif_image_handle_get_raw_color_profile_size(m_handle);//icc数据
					if (profile_size > 0) {
						uint8_t* profile_data = static_cast<uint8_t*>(malloc(profile_size));
						heif_image_handle_get_raw_color_profile(m_handle, profile_data);//icc数据
						//复制ICC
						FreeImage_CreateICCProfile(dib, profile_data, profile_size);//設置ICC
						free(profile_data);
					}

					//设置DIB
					FreeImage_SetDotsPerMeterX(dib, 2835);
					FreeImage_SetDotsPerMeterY(dib, 2835);

				}
				heif_image_release(m_image);
			}
			heif_decoding_options_free(m_decode_options); //解码出图像
			heif_image_handle_release(m_handle);
		}
		return dib;
	}

	return NULL;
}


// ==========================================================
//   Init
// ==========================================================


void InitHEIC(Plugin* plugin, int format_id) {
	s_format_id = format_id;

	plugin->format_proc = Format;
	plugin->description_proc = Description;
	plugin->extension_proc = Extension;
	plugin->regexpr_proc = RegExpr;
	plugin->open_proc = NULL;
	plugin->close_proc = NULL;
	plugin->pagecount_proc = NULL;
	plugin->load_proc = Load;
	plugin->save_proc = NULL;
	plugin->validate_proc = Validate;
	plugin->mime_proc = MimeType;
	plugin->supports_export_bpp_proc = SupportsExportDepth;
	plugin->supports_export_type_proc = SupportsExportType;
	plugin->supports_icc_profiles_proc = SupportsICCProfiles;
	plugin->supports_no_pixels_proc = SupportsNoPixels;
}
