
#ifdef _MSC_VER 
#pragma warning (disable : 4786) // identifier was truncated to 'number' characters
#endif

extern "C" {
#define XMD_H
#undef FAR
#include <setjmp.h>
#include "../mozjpeg/moz_jinclude.h"
#include "../mozjpeg/moz_jpeglib.h"
#include "../mozjpeg/moz_jerror.h"
}

#include "FreeImage.h"
#include "Utilities.h"

#include "FreeImageTag.h"


// ==========================================================
// Plugin Interface
// ==========================================================

static int s_format_id;

// ----------------------------------------------------------
//   Constant declarations
// ----------------------------------------------------------

#define INPUT_BUF_SIZE  4096	// choose an efficiently fread'able size 
#define OUTPUT_BUF_SIZE 4096    // choose an efficiently fwrite'able size

#define EXIF_MARKER		(JPEG_APP0+1)	// EXIF marker / Adobe XMP marker
#define ICC_MARKER		(JPEG_APP0+2)	// ICC profile marker
#define IPTC_MARKER		(JPEG_APP0+13)	// IPTC marker / BIM marker 

#define ICC_HEADER_SIZE 14				// size of non-profile data in APP2
#define MAX_BYTES_IN_MARKER 65533L		// maximum data length of a JPEG marker
#define MAX_DATA_BYTES_IN_MARKER 65519L	// maximum data length of a JPEG APP2 marker

#define MAX_JFXX_THUMB_SIZE (MAX_BYTES_IN_MARKER - 5 - 1)

#define JFXX_TYPE_JPEG 	0x10	// JFIF extension marker: JPEG-compressed thumbnail image
#define JFXX_TYPE_8bit 	0x11	// JFIF extension marker: palette thumbnail image
#define JFXX_TYPE_24bit	0x13	// JFIF extension marker: RGB thumbnail image

// ----------------------------------------------------------
//   Typedef declarations
// ----------------------------------------------------------

typedef struct tagErrorManager {
	/// "public" fields
	struct jpeg_error_mgr pub;
	/// for return to caller
	jmp_buf setjmp_buffer;
} ErrorManager;



typedef struct tagDestinationManager {
	/// public fields
	struct jpeg_destination_mgr pub;
	/// destination stream
	fi_handle outfile;
	FreeImageIO *m_io;
	/// start of buffer
	JOCTET * buffer;
} DestinationManager;


typedef DestinationManager* freeimage_dst_ptr;
typedef ErrorManager*		freeimage_error_ptr;

// ----------------------------------------------------------
//   Error handling
// ----------------------------------------------------------

/** Fatal errors (print message and exit) */
static inline void
JPEG_EXIT(j_common_ptr cinfo, int code) {
	freeimage_error_ptr error_ptr = (freeimage_error_ptr)cinfo->err;
	error_ptr->pub.msg_code = code;
	error_ptr->pub.error_exit(cinfo);
}

/** Nonfatal errors (we can keep going, but the data is probably corrupt) */
static inline void
JPEG_WARNING(j_common_ptr cinfo, int code) {
	freeimage_error_ptr error_ptr = (freeimage_error_ptr)cinfo->err;
	error_ptr->pub.msg_code = code;
	error_ptr->pub.emit_message(cinfo, -1);
}

/**
	Receives control for a fatal error.  Information sufficient to
	generate the error message has been stored in cinfo->err; call
	output_message to display it.  Control must NOT return to the caller;
	generally this routine will exit() or longjmp() somewhere.
*/
METHODDEF(void)
jpeg_error_exit (j_common_ptr cinfo) {
	freeimage_error_ptr error_ptr = (freeimage_error_ptr)cinfo->err;

	// always display the message
	error_ptr->pub.output_message(cinfo);

	// allow JPEG with unknown markers
	if(error_ptr->pub.msg_code != JERR_UNKNOWN_MARKER) {
	
		// let the memory manager delete any temp files before we die
		moz_jpeg_destroy(cinfo);
		
		// return control to the setjmp point
		longjmp(error_ptr->setjmp_buffer, 1);		
	}
}

/**
	Actual output of any JPEG message.  Note that this method does not know
	how to generate a message, only where to send it.
*/
METHODDEF(void)
jpeg_output_message (j_common_ptr cinfo) {
	char buffer[JMSG_LENGTH_MAX];
	freeimage_error_ptr error_ptr = (freeimage_error_ptr)cinfo->err;

	// create the message
	error_ptr->pub.format_message(cinfo, buffer);
	// send it to user's message proc
	FreeImage_OutputMessageProc(s_format_id, buffer);
}

// ----------------------------------------------------------
//   Destination manager
// ----------------------------------------------------------

/**
	Initialize destination.  This is called by jpeg_start_compress()
	before any data is actually written. It must initialize
	next_output_byte and free_in_buffer. free_in_buffer must be
	initialized to a positive value.
*/
METHODDEF(void)
init_destination (j_compress_ptr cinfo) {
	freeimage_dst_ptr dest = (freeimage_dst_ptr) cinfo->dest;

	dest->buffer = (JOCTET *)
	  (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  OUTPUT_BUF_SIZE * sizeof(JOCTET));

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}

/**
	This is called whenever the buffer has filled (free_in_buffer
	reaches zero). In typical applications, it should write out the
	*entire* buffer (use the saved start address and buffer length;
	ignore the current state of next_output_byte and free_in_buffer).
	Then reset the pointer & count to the start of the buffer, and
	return TRUE indicating that the buffer has been dumped.
	free_in_buffer must be set to a positive value when TRUE is
	returned.  A FALSE return should only be used when I/O suspension is
	desired.
*/
METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo) {
	freeimage_dst_ptr dest = (freeimage_dst_ptr) cinfo->dest;

	if (dest->m_io->write_proc(dest->buffer, 1, OUTPUT_BUF_SIZE, dest->outfile) != OUTPUT_BUF_SIZE) {
		// let the memory manager delete any temp files before we die
		moz_jpeg_destroy((j_common_ptr)cinfo);

		JPEG_EXIT((j_common_ptr)cinfo, JERR_FILE_WRITE);
	}

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

	return TRUE;
}

/**
	Terminate destination --- called by jpeg_finish_compress() after all
	data has been written.  In most applications, this must flush any
	data remaining in the buffer.  Use either next_output_byte or
	free_in_buffer to determine how much data is in the buffer.
*/
METHODDEF(void)
term_destination (j_compress_ptr cinfo) {
	freeimage_dst_ptr dest = (freeimage_dst_ptr) cinfo->dest;

	size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

	// write any data remaining in the buffer

	if (datacount > 0) {
		if (dest->m_io->write_proc(dest->buffer, 1, (unsigned int)datacount, dest->outfile) != datacount) {
			// let the memory manager delete any temp files before we die
			moz_jpeg_destroy((j_common_ptr)cinfo);
			
			JPEG_EXIT((j_common_ptr)cinfo, JERR_FILE_WRITE);
		}
	}
}

GLOBAL(void)
moz_jpeg_freeimage_dst (j_compress_ptr cinfo, fi_handle outfile, FreeImageIO *io) {
	freeimage_dst_ptr dest;

	if (cinfo->dest == NULL) {
		cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small)
			((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(DestinationManager));
	}

	dest = (freeimage_dst_ptr) cinfo->dest;
	dest->pub.init_destination = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
	dest->outfile = outfile;
	dest->m_io = io;
}


static BOOL 
jpeg_write_comment(j_compress_ptr cinfo, FIBITMAP *dib) {
	FITAG *tag = NULL;

	// write user comment as a JPEG_COM marker
	FreeImage_GetMetadata(FIMD_COMMENTS, dib, "Comment", &tag);
	if(tag) {
		const char *tag_value = (char*)FreeImage_GetTagValue(tag);

		if(NULL != tag_value) {
			for(long i = 0; i < (long)strlen(tag_value); i+= MAX_BYTES_IN_MARKER) {
				moz_jpeg_write_marker(cinfo, JPEG_COM, (BYTE*)tag_value + i, MIN((long)strlen(tag_value + i), MAX_BYTES_IN_MARKER));
			}
			return TRUE;
		}
	}
	return FALSE;
}

/** 
	Write JPEG_APP2 marker (ICC profile)
*/
static BOOL 
jpeg_write_icc_profile(j_compress_ptr cinfo, FIBITMAP *dib) {
    // marker identifying string "ICC_PROFILE" (null-terminated)
	BYTE icc_signature[12] = { 0x49, 0x43, 0x43, 0x5F, 0x50, 0x52, 0x4F, 0x46, 0x49, 0x4C, 0x45, 0x00 };

	FIICCPROFILE *iccProfile = FreeImage_GetICCProfile(dib);

	if (iccProfile->size && iccProfile->data) {
		// ICC_HEADER_SIZE: ICC signature is 'ICC_PROFILE' + 2 bytes

		BYTE *profile = (BYTE*)malloc((iccProfile->size + ICC_HEADER_SIZE) * sizeof(BYTE));
		if(profile == NULL) return FALSE;
		memcpy(profile, icc_signature, 12);

		for(long i = 0; i < (long)iccProfile->size; i += MAX_DATA_BYTES_IN_MARKER) {
			unsigned length = MIN((long)(iccProfile->size - i), MAX_DATA_BYTES_IN_MARKER);
			// sequence number
			profile[12] = (BYTE) ((i / MAX_DATA_BYTES_IN_MARKER) + 1);
			// number of markers
			profile[13] = (BYTE) (iccProfile->size / MAX_DATA_BYTES_IN_MARKER + 1);

			memcpy(profile + ICC_HEADER_SIZE, (BYTE*)iccProfile->data + i, length);
			moz_jpeg_write_marker(cinfo, ICC_MARKER, profile, (length + ICC_HEADER_SIZE));
        }

		free(profile);

		return TRUE;		
	}
	
	return FALSE;
}

/** 
	Write JPEG_APPD marker (IPTC or Adobe Photoshop profile)
	@return Returns TRUE if successful, FALSE otherwise
*/
static BOOL  
jpeg_write_iptc_profile(j_compress_ptr cinfo, FIBITMAP *dib) {
	//const char *ps_header = "Photoshop 3.0\x08BIM\x04\x04\x0\x0\x0\x0";
	const unsigned tag_length = 26;

	if(FreeImage_GetMetadataCount(FIMD_IPTC, dib)) {
		BYTE *profile = NULL;
		unsigned profile_size = 0;

		// create a binary profile
		if(write_iptc_profile(dib, &profile, &profile_size)) {

			// write the profile
			for(long i = 0; i < (long)profile_size; i += 65517L) {
				unsigned length = MIN((long)profile_size - i, 65517L);
				unsigned roundup = length & 0x01;	// needed for Photoshop
				BYTE *iptc_profile = (BYTE*)malloc(length + roundup + tag_length);
				if(iptc_profile == NULL) break;
				// Photoshop identification string
				memcpy(&iptc_profile[0], "Photoshop 3.0\x0", 14);
				// 8BIM segment type
				memcpy(&iptc_profile[14], "8BIM\x04\x04\x0\x0\x0\x0", 10);
				// segment size
				iptc_profile[24] = (BYTE)(length >> 8);
				iptc_profile[25] = (BYTE)(length & 0xFF);
				// segment data
				memcpy(&iptc_profile[tag_length], &profile[i], length);
				if(roundup)
					iptc_profile[length + tag_length] = 0;
				moz_jpeg_write_marker(cinfo, IPTC_MARKER, iptc_profile, length + roundup + tag_length);
				free(iptc_profile);
			}

			// release profile
			free(profile);

			return TRUE;
		}
	}

	return FALSE;
}

/** 
	Write JPEG_APP1 marker (XMP profile)
	@return Returns TRUE if successful, FALSE otherwise
*/
static BOOL  
jpeg_write_xmp_profile(j_compress_ptr cinfo, FIBITMAP *dib) {
	// marker identifying string for XMP (null terminated)
	const char *xmp_signature = "http://ns.adobe.com/xap/1.0/";

	FITAG *tag_xmp = NULL;
	FreeImage_GetMetadata(FIMD_XMP, dib, g_TagLib_XMPFieldName, &tag_xmp);

	if(tag_xmp) {
		const BYTE *tag_value = (BYTE*)FreeImage_GetTagValue(tag_xmp);

		if(NULL != tag_value) {
			// XMP signature is 29 bytes long
			unsigned int xmp_header_size = (unsigned int)strlen(xmp_signature) + 1;

			DWORD tag_length = FreeImage_GetTagLength(tag_xmp);

			BYTE *profile = (BYTE*)malloc((tag_length + xmp_header_size) * sizeof(BYTE));
			if(profile == NULL) return FALSE;
			memcpy(profile, xmp_signature, xmp_header_size);

			for(DWORD i = 0; i < tag_length; i += 65504L) {
				unsigned length = MIN((long)(tag_length - i), 65504L);
				
				memcpy(profile + xmp_header_size, tag_value + i, length);
				moz_jpeg_write_marker(cinfo, EXIF_MARKER, profile, (length + xmp_header_size));
			}

			free(profile);

			return TRUE;	
		}
	}

	return FALSE;
}

/** 
	Write JPEG_APP1 marker (Exif profile)
	@return Returns TRUE if successful, FALSE otherwise
*/
static BOOL 
jpeg_write_exif_profile_raw(j_compress_ptr cinfo, FIBITMAP *dib) {
    // marker identifying string for Exif = "Exif\0\0"
    BYTE exif_signature[6] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };

	FITAG *tag_exif = NULL;
	FreeImage_GetMetadata(FIMD_EXIF_RAW, dib, g_TagLib_ExifRawFieldName, &tag_exif);

	if(tag_exif) {
		const BYTE *tag_value = (BYTE*)FreeImage_GetTagValue(tag_exif);
		
		// verify the identifying string
		if(memcmp(exif_signature, tag_value, sizeof(exif_signature)) != 0) {
			// not an Exif profile
			return FALSE;
		}

		if(NULL != tag_value) {
			DWORD tag_length = FreeImage_GetTagLength(tag_exif);

			BYTE *profile = (BYTE*)malloc(tag_length * sizeof(BYTE));
			if(profile == NULL) return FALSE;

			for(DWORD i = 0; i < tag_length; i += 65504L) {
				unsigned length = MIN((long)(tag_length - i), 65504L);
				
				memcpy(profile, tag_value + i, length);
				moz_jpeg_write_marker(cinfo, EXIF_MARKER, profile, length);
			}

			free(profile);

			return TRUE;	
		}
	}

	return FALSE;
}

/**
	Write thumbnail (JFXX segment, JPEG compressed)
*/
static BOOL
jpeg_write_jfxx(j_compress_ptr cinfo, FIBITMAP *dib) {
	// get the thumbnail to be stored
	FIBITMAP* thumbnail = FreeImage_GetThumbnail(dib);
	if(!thumbnail) {
		return TRUE;
	}
	// check for a compatible output format
	if((FreeImage_GetImageType(thumbnail) != FIT_BITMAP) || (FreeImage_GetBPP(thumbnail) != 8) && (FreeImage_GetBPP(thumbnail) != 24)) {
		FreeImage_OutputMessageProc(s_format_id, FI_MSG_WARNING_INVALID_THUMBNAIL);
		return FALSE;
	}
	
	// stores the thumbnail as a baseline JPEG into a memory block
	// return the memory block only if its size is within JFXX marker size limit!
	FIMEMORY *stream = FreeImage_OpenMemory(NULL,0);
	
	if(FreeImage_SaveToMemory(FIF_JPEG, thumbnail, stream, JPEG_BASELINE)) {
		// check that the memory block size is within JFXX marker size limit
		FreeImage_SeekMemory(stream, 0, SEEK_END);
		const long eof = FreeImage_TellMemory(stream);
		if(eof > MAX_JFXX_THUMB_SIZE) {
			FreeImage_OutputMessageProc(s_format_id, "Warning: attached thumbnail is %d bytes larger than maximum supported size - Thumbnail saving aborted", eof - MAX_JFXX_THUMB_SIZE);
			FreeImage_CloseMemory(stream);
			return FALSE;
		}
	} else {
		FreeImage_CloseMemory(stream);
		return FALSE;
	}

	BYTE* thData = NULL;
	DWORD thSize = 0;
	
	FreeImage_AcquireMemory(stream, &thData, &thSize);	
	
	BYTE id_length = 5; //< "JFXX"
	BYTE type = JFXX_TYPE_JPEG;
	
	DWORD totalsize = id_length + sizeof(type) + thSize;
	moz_jpeg_write_m_header(cinfo, JPEG_APP0, totalsize);
	
	moz_jpeg_write_m_byte(cinfo, 'J');
	moz_jpeg_write_m_byte(cinfo, 'F');
	moz_jpeg_write_m_byte(cinfo, 'X');
	moz_jpeg_write_m_byte(cinfo, 'X');
	moz_jpeg_write_m_byte(cinfo, '\0');
	
	moz_jpeg_write_m_byte(cinfo, type);
	
	// write thumbnail to destination.
	// We "cram it straight into the data destination module", because write_m_byte is slow
	
	freeimage_dst_ptr dest = (freeimage_dst_ptr) cinfo->dest;
	
	BYTE* & out = dest->pub.next_output_byte;
	size_t & bufRemain = dest->pub.free_in_buffer;
	
	const BYTE *thData_end = thData + thSize;

	while(thData < thData_end) {
		*(out)++ = *(thData)++;
		if (--bufRemain == 0) {	
			// buffer full - flush
			if (!dest->pub.empty_output_buffer(cinfo)) {
				break;
			}
		}
	}
	
	FreeImage_CloseMemory(stream);

	return TRUE;
}

/**
	Write JPEG special markers
*/
static BOOL 
write_markers(j_compress_ptr cinfo, FIBITMAP *dib) {
	// write thumbnail as a JFXX marker
	jpeg_write_jfxx(cinfo, dib);

	// write user comment as a JPEG_COM marker
	jpeg_write_comment(cinfo, dib);

	// write ICC profile
	jpeg_write_icc_profile(cinfo, dib);

	// write IPTC profile
	jpeg_write_iptc_profile(cinfo, dib);

	// write Adobe XMP profile
	jpeg_write_xmp_profile(cinfo, dib);

	// write Exif raw data
	jpeg_write_exif_profile_raw(cinfo, dib);

	return TRUE;
}


// ----------------------------------------------------------
//Pacth For Mozjpeg
// add By Tam
//2023.01.27
BOOL  mozjpeg_Save(FreeImageIO *io, FIBITMAP *dib, fi_handle handle, int page, int flags, void *data) {
	if ((dib) && (handle)) {
		try {
			// Check dib format

			const char *sError = "only 24-bit RGB, 8-bit greyscale/palette or 32-bit CMYK bitmaps can be saved as JPEG";

			FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType(dib);
			WORD bpp = (WORD)FreeImage_GetBPP(dib);

			if ((bpp != 24) && (bpp != 8) && !(bpp == 32 && (color_type == FIC_CMYK))) {
				throw sError;
			}

			if(bpp == 8) {
				// allow grey, reverse grey and palette 
				if ((color_type != FIC_MINISBLACK) && (color_type != FIC_MINISWHITE) && (color_type != FIC_PALETTE)) {
					throw sError;
				}
			}


			struct jpeg_compress_struct cinfo;
			ErrorManager fi_error_mgr;

			// Step 1: allocate and initialize JPEG compression object

			// we set up the normal JPEG error routines, then override error_exit & output_message
			cinfo.err = moz_jpeg_std_error(&fi_error_mgr.pub);
			fi_error_mgr.pub.error_exit     = jpeg_error_exit;
			fi_error_mgr.pub.output_message = jpeg_output_message;
			
			// establish the setjmp return context for jpeg_error_exit to use
			if (setjmp(fi_error_mgr.setjmp_buffer)) {
				// If we get here, the JPEG code has signaled an error.
				// We need to clean up the JPEG object, close the input file, and return.
				moz_jpeg_destroy_compress(&cinfo);
				throw (const char*)NULL;
			}

			// Now we can initialize the JPEG compression object

			jpeg_create_compress(&cinfo);

			// Step 2: specify data destination (eg, a file)

			moz_jpeg_freeimage_dst(&cinfo, handle, io);

			// Step 3: set parameters for compression 

			cinfo.image_width = FreeImage_GetWidth(dib);
			cinfo.image_height = FreeImage_GetHeight(dib);

			switch(color_type) {
				case FIC_MINISBLACK :
				case FIC_MINISWHITE :
					cinfo.in_color_space = JCS_GRAYSCALE;
					cinfo.input_components = 1;
					break;
				case FIC_CMYK:
					cinfo.in_color_space = JCS_CMYK;
					cinfo.input_components = 4;
					break;
				default :
					cinfo.in_color_space = JCS_RGB;
					cinfo.input_components = 3;
					break;
			}

			moz_jpeg_set_defaults(&cinfo);

		    // progressive-JPEG support
			if((flags & JPEG_PROGRESSIVE) == JPEG_PROGRESSIVE) {
				moz_jpeg_simple_progression(&cinfo);
			}
			
			// compute optimal Huffman coding tables for the image
			//if((flags & JPEG_OPTIMIZE) == JPEG_OPTIMIZE) {
			cinfo.optimize_coding = TRUE;//强制使用JPEG优化大小功能
			//}

			// Set JFIF density parameters from the DIB data

			cinfo.X_density = (UINT16) (0.5 + 0.0254 * FreeImage_GetDotsPerMeterX(dib));
			cinfo.Y_density = (UINT16) (0.5 + 0.0254 * FreeImage_GetDotsPerMeterY(dib));
			cinfo.density_unit = 1;	// dots / inch

			// thumbnail support (JFIF 1.02 extension markers)
			if(FreeImage_GetThumbnail(dib) != NULL) {
				cinfo.write_JFIF_header = static_cast<boolean>(1); //<### force it, though when color is CMYK it will be incorrect
				cinfo.JFIF_minor_version = 2;
			}

			// baseline JPEG support
			if ((flags & JPEG_BASELINE) == JPEG_BASELINE) {
				cinfo.write_JFIF_header = static_cast<boolean>(0);	// No marker for non-JFIF colorspaces
				cinfo.write_Adobe_marker = static_cast<boolean>(0);	// write no Adobe marker by default				
			}

			// set subsampling options if required

			if(cinfo.in_color_space == JCS_RGB) {
				if((flags & JPEG_SUBSAMPLING_411) == JPEG_SUBSAMPLING_411) { 
					// 4:1:1 (4x1 1x1 1x1) - CrH 25% - CbH 25% - CrV 100% - CbV 100%
					// the horizontal color resolution is quartered
					cinfo.comp_info[0].h_samp_factor = 4;	// Y 
					cinfo.comp_info[0].v_samp_factor = 1; 
					cinfo.comp_info[1].h_samp_factor = 1;	// Cb 
					cinfo.comp_info[1].v_samp_factor = 1; 
					cinfo.comp_info[2].h_samp_factor = 1;	// Cr 
					cinfo.comp_info[2].v_samp_factor = 1; 
				} else if((flags & JPEG_SUBSAMPLING_420) == JPEG_SUBSAMPLING_420) {
					// 4:2:0 (2x2 1x1 1x1) - CrH 50% - CbH 50% - CrV 50% - CbV 50%
					// the chrominance resolution in both the horizontal and vertical directions is cut in half
					cinfo.comp_info[0].h_samp_factor = 2;	// Y
					cinfo.comp_info[0].v_samp_factor = 2; 
					cinfo.comp_info[1].h_samp_factor = 1;	// Cb
					cinfo.comp_info[1].v_samp_factor = 1; 
					cinfo.comp_info[2].h_samp_factor = 1;	// Cr
					cinfo.comp_info[2].v_samp_factor = 1; 
				} else if((flags & JPEG_SUBSAMPLING_422) == JPEG_SUBSAMPLING_422){ //2x1 (low) 
					// 4:2:2 (2x1 1x1 1x1) - CrH 50% - CbH 50% - CrV 100% - CbV 100%
					// half of the horizontal resolution in the chrominance is dropped (Cb & Cr), 
					// while the full resolution is retained in the vertical direction, with respect to the luminance
					cinfo.comp_info[0].h_samp_factor = 2;	// Y 
					cinfo.comp_info[0].v_samp_factor = 1; 
					cinfo.comp_info[1].h_samp_factor = 1;	// Cb 
					cinfo.comp_info[1].v_samp_factor = 1; 
					cinfo.comp_info[2].h_samp_factor = 1;	// Cr 
					cinfo.comp_info[2].v_samp_factor = 1; 
				} 
				else if((flags & JPEG_SUBSAMPLING_444) == JPEG_SUBSAMPLING_444){ //1x1 (no subsampling) 
					// 4:4:4 (1x1 1x1 1x1) - CrH 100% - CbH 100% - CrV 100% - CbV 100%
					// the resolution of chrominance information (Cb & Cr) is preserved 
					// at the same rate as the luminance (Y) information
					cinfo.comp_info[0].h_samp_factor = 1;	// Y 
					cinfo.comp_info[0].v_samp_factor = 1; 
					cinfo.comp_info[1].h_samp_factor = 1;	// Cb 
					cinfo.comp_info[1].v_samp_factor = 1; 
					cinfo.comp_info[2].h_samp_factor = 1;	// Cr 
					cinfo.comp_info[2].v_samp_factor = 1;  
				} 
			}

			// Step 4: set quality
			// the first 7 bits are reserved for low level quality settings
			// the other bits are high level (i.e. enum-ish)

			int quality;

			if ((flags & JPEG_QUALITYBAD) == JPEG_QUALITYBAD) {
				quality = 10;
			} else if ((flags & JPEG_QUALITYAVERAGE) == JPEG_QUALITYAVERAGE) {
				quality = 25;
			} else if ((flags & JPEG_QUALITYNORMAL) == JPEG_QUALITYNORMAL) {
				quality = 50;
			} else if ((flags & JPEG_QUALITYGOOD) == JPEG_QUALITYGOOD) {
				quality = 75;
			} else 	if ((flags & JPEG_QUALITYSUPERB) == JPEG_QUALITYSUPERB) {
				quality = 100;
			} else {
				if ((flags & 0x7F) == 0) {
					quality = 75;
				} else {
					quality = flags & 0x7F;
				}
			}

			moz_jpeg_set_quality(&cinfo, quality, TRUE); /* limit to baseline-JPEG values */

			// Step 5: Start compressor 

			moz_jpeg_start_compress(&cinfo, TRUE);

			// Step 6: Write special markers
			
			if ((flags & JPEG_BASELINE) !=  JPEG_BASELINE) {
				write_markers(&cinfo, dib);
			}

			// Step 7: while (scan lines remain to be written) 

			if(color_type == FIC_RGB) {
				// 24-bit RGB image : need to swap red and blue channels
				unsigned pitch = FreeImage_GetPitch(dib);
				BYTE *target = (BYTE*)malloc(pitch * sizeof(BYTE));
				if (target == NULL) {
					throw FI_MSG_ERROR_MEMORY;
				}

				while (cinfo.next_scanline < cinfo.image_height) {
					// get a copy of the scanline
					memcpy(target, FreeImage_GetScanLine(dib, FreeImage_GetHeight(dib) - cinfo.next_scanline - 1), pitch);
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
					// swap R and B channels
					BYTE *target_p = target;
					for(unsigned x = 0; x < cinfo.image_width; x++) {
						INPLACESWAP(target_p[0], target_p[2]);
						target_p += 3;
					}
#endif
					// write the scanline
					moz_jpeg_write_scanlines(&cinfo, &target, 1);
				}
				free(target);
			}
			else if(color_type == FIC_CMYK) {
				unsigned pitch = FreeImage_GetPitch(dib);
				BYTE *target = (BYTE*)malloc(pitch * sizeof(BYTE));
				if (target == NULL) {
					throw FI_MSG_ERROR_MEMORY;
				}
				
				while (cinfo.next_scanline < cinfo.image_height) {
					// get a copy of the scanline
					memcpy(target, FreeImage_GetScanLine(dib, FreeImage_GetHeight(dib) - cinfo.next_scanline - 1), pitch);
					
					BYTE *target_p = target;
					for(unsigned x = 0; x < cinfo.image_width; x++) {
						// CMYK pixels are inverted
						target_p[0] = ~target_p[0];	// C
						target_p[1] = ~target_p[1];	// M
						target_p[2] = ~target_p[2];	// Y
						target_p[3] = ~target_p[3];	// K

						target_p += 4;
					}
					
					// write the scanline
					moz_jpeg_write_scanlines(&cinfo, &target, 1);
				}
				free(target);
			}
			else if(color_type == FIC_MINISBLACK) {
				// 8-bit standard greyscale images
				while (cinfo.next_scanline < cinfo.image_height) {
					JSAMPROW b = FreeImage_GetScanLine(dib, FreeImage_GetHeight(dib) - cinfo.next_scanline - 1);

					moz_jpeg_write_scanlines(&cinfo, &b, 1);
				}
			}
			else if(color_type == FIC_PALETTE) {
				// 8-bit palettized images are converted to 24-bit images
				RGBQUAD *palette = FreeImage_GetPalette(dib);
				BYTE *target = (BYTE*)malloc(cinfo.image_width * 3);
				if (target == NULL) {
					throw FI_MSG_ERROR_MEMORY;
				}

				while (cinfo.next_scanline < cinfo.image_height) {
					BYTE *source = FreeImage_GetScanLine(dib, FreeImage_GetHeight(dib) - cinfo.next_scanline - 1);
					FreeImage_ConvertLine8To24(target, source, cinfo.image_width, palette);

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
					// swap R and B channels
					BYTE *target_p = target;
					for(unsigned x = 0; x < cinfo.image_width; x++) {
						INPLACESWAP(target_p[0], target_p[2]);
						target_p += 3;
					}
#endif
					moz_jpeg_write_scanlines(&cinfo, &target, 1);
				}

				free(target);
			}
			else if(color_type == FIC_MINISWHITE) {
				// reverse 8-bit greyscale image, so reverse grey value on the fly
				unsigned i;
				BYTE reverse[256];
				BYTE *target = (BYTE *)malloc(cinfo.image_width);
				if (target == NULL) {
					throw FI_MSG_ERROR_MEMORY;
				}

				for(i = 0; i < 256; i++) {
					reverse[i] = (BYTE)(255 - i);
				}

				while(cinfo.next_scanline < cinfo.image_height) {
					BYTE *source = FreeImage_GetScanLine(dib, FreeImage_GetHeight(dib) - cinfo.next_scanline - 1);
					for(i = 0; i < cinfo.image_width; i++) {
						target[i] = reverse[ source[i] ];
					}
					moz_jpeg_write_scanlines(&cinfo, &target, 1);
				}

				free(target);
			}

			// Step 8: Finish compression 

			moz_jpeg_finish_compress(&cinfo);

			// Step 9: release JPEG compression object 

			moz_jpeg_destroy_compress(&cinfo);

			return TRUE;

		} catch (const char *text) {
			if(text) {
				FreeImage_OutputMessageProc(s_format_id, text);
			}
			return FALSE;
		} 
	}

	return FALSE;
}


