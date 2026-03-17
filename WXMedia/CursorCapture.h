/*
鼠标捕获
*/
#ifndef _CursorCapture_H_
#define _CursorCapture_H_

#include "ScreenGrab.hpp"

//鼠标图像
class CursorCapture {
public:
	//ARGB图像叠加

public:
	HCURSOR                        current_cursor;
	long                           x_hotspot;
	long                           y_hotspot;

	bool                           visible;
	POINT                          cursor_pos;

	WXTcpVideoFrame m_dataFrame;//鼠标数据

	static inline uint8_t bit_to_alpha(uint8_t* data, long pixel, bool invert) {
		uint8_t pix_byte = data[pixel / 8];
		bool alpha = (pix_byte >> (7 - pixel % 8) & 1) != 0;
		if (invert) {
			return alpha ? 0xFF : 0;
		}
		else {
			return alpha ? 0 : 0xFF;
		}
	}

	static inline bool bitmap_has_alpha(uint8_t* data, long num_pixels) {
		for (long i = 0; i < num_pixels; i++) {
			if (data[i * 4 + 3] != 0) {
				return true;
			}
		}
		return false;
	}

	static inline void apply_mask(uint8_t* color, uint8_t* mask, long num_pixels) {
		for (long i = 0; i < num_pixels; i++)
			color[i * 4 + 3] = bit_to_alpha(mask, i, false);
	}

	static uint8_t* get_bitmap_data(HBITMAP hbmp, BITMAP* bmp) {
		if (GetObject(hbmp, sizeof(*bmp), bmp) != 0) {
			uint8_t* output;
			unsigned int size =
				(bmp->bmHeight * bmp->bmWidth * bmp->bmBitsPixel) / 8;

			output = (uint8_t*)malloc(size);
			::GetBitmapBits(hbmp, size, output);
			return output;
		}

		return NULL;
	}

	static inline uint8_t* copy_from_color(ICONINFO* ii, uint32_t* width, uint32_t* height) {
		BITMAP bmp_color;
		BITMAP bmp_mask;
		uint8_t* color;
		uint8_t* mask;

		color = get_bitmap_data(ii->hbmColor, &bmp_color);
		if (!color) {
			return NULL;
		}

		if (bmp_color.bmBitsPixel < 32) {
			free(color);
			return NULL;
		}

		mask = get_bitmap_data(ii->hbmMask, &bmp_mask);
		if (mask) {
			long pixels = bmp_color.bmHeight * bmp_color.bmWidth;

			if (!bitmap_has_alpha(color, pixels))
				apply_mask(color, mask, pixels);

			free(mask);
		}

		*width = bmp_color.bmWidth;
		*height = bmp_color.bmHeight;
		return color;
	}

	static inline uint8_t* copy_from_mask(ICONINFO* ii, uint32_t* width, uint32_t* height) {
		uint8_t* output;
		uint8_t* mask;
		long pixels;
		long bottom;
		BITMAP bmp;

		mask = get_bitmap_data(ii->hbmMask, &bmp);
		if (!mask) {
			return NULL;
		}

		bmp.bmHeight /= 2;

		pixels = bmp.bmHeight * bmp.bmWidth;
		output = (uint8_t*)malloc(pixels * 4);

		bottom = bmp.bmWidthBytes * bmp.bmHeight;

		for (long i = 0; i < pixels; i++) {
			uint8_t alpha = bit_to_alpha(mask, i, false);
			uint8_t color = bit_to_alpha(mask + bottom, i, true);

			if (!alpha) {
				output[i * 4 + 3] = color;
			}
			else {
				*(uint32_t*)&output[i * 4] = !!color ?
					0xFFFFFFFF : 0xFF000000;
			}
		}

		free(mask);

		*width = bmp.bmWidth;
		*height = bmp.bmHeight;
		return output;
	}

	static inline uint8_t* cursor_capture_icon_bitmap(ICONINFO* ii, uint32_t* width, uint32_t* height) {
		uint8_t* output;

		output = copy_from_color(ii, width, height);
		if (!output)
			output = copy_from_mask(ii, width, height);

		return output;
	}

	inline bool cursor_capture_icon(HICON icon) {
		uint8_t* bitmap;
		uint32_t height;
		uint32_t width;
		ICONINFO ii;

		if (!icon) {
			return false;
		}
		if (!GetIconInfo(icon, &ii)) {
			return false;
		}

		bitmap = cursor_capture_icon_bitmap(&ii, &width, &height);
		if (bitmap) {
			if (m_dataFrame.m_iWidth != width || m_dataFrame.m_iHeight != height) {
				m_dataFrame.Init(width, height);
			}
			memcpy(m_dataFrame.m_pBuf, bitmap, width * height * 4);

			free(bitmap);
			x_hotspot = ii.xHotspot;
			y_hotspot = ii.yHotspot;
		}
		DeleteObject(ii.hbmColor);
		DeleteObject(ii.hbmMask);
		return true;
	}
public:
	void Capture() {
		CURSORINFO ci = { 0 };
		ci.cbSize = sizeof(ci);

		if (!GetCursorInfo(&ci)) {
			visible = false;
			return;
		}

		memcpy(&cursor_pos, &ci.ptScreenPos, sizeof(cursor_pos));
		if (current_cursor == ci.hCursor) {//图标不变
			return;
		}

		HICON icon = CopyIcon(ci.hCursor);
		visible = cursor_capture_icon(icon);
		current_cursor = ci.hCursor;
		if ((ci.flags & CURSOR_SHOWING) == 0)
			visible = false;
		DestroyIcon(icon);
	}

	void Draw(WXTcpVideoFrame& MixFrame, const int PosX, const int PosY, const int Alpha) {
		if (visible)
			RgbaData::DrawMouse(MixFrame, PosX, PosY, m_dataFrame);
	}
};


#endif
