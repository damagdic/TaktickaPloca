#pragma once
#include <windows.h>
#include <tchar.h>

struct gdi_pen {
	HPEN h{};
	gdi_pen(COLORREF color, int width = 1, int style = PS_SOLID) {
		h = CreatePen(style, width, color);
	}
	~gdi_pen() { if (h) DeleteObject(h); }
	operator HPEN() const { return h; }
	gdi_pen(const gdi_pen&) = delete;
	gdi_pen& operator=(const gdi_pen&) = delete;
};

struct gdi_brush {
	HBRUSH h{};
	explicit gdi_brush(COLORREF color) { h = CreateSolidBrush(color); }
	~gdi_brush() { if (h) DeleteObject(h); }
	operator HBRUSH() const { return h; }
	gdi_brush(const gdi_brush&) = delete;
	gdi_brush& operator=(const gdi_brush&) = delete;

	static HBRUSH hollow() { return (HBRUSH)GetStockObject(HOLLOW_BRUSH); } 
	static HBRUSH null_brush() { return (HBRUSH)GetStockObject(NULL_BRUSH); } 
};

struct gdi_font {
	HFONT h{};
	static HFONT make_pt(int pt = 14, LPCTSTR name = TEXT("Segoe UI"),
		int weight = FW_NORMAL, BOOL italic = FALSE) {
		HDC sdc = GetDC(nullptr);
		const int height = -MulDiv(pt, GetDeviceCaps(sdc, LOGPIXELSY), 72);
		ReleaseDC(nullptr, sdc);
		return CreateFont(height, 0, 0, 0, weight, italic, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, name);
	}
	gdi_font(int pt = 14, LPCTSTR name = TEXT("Segoe UI"),
		int weight = FW_NORMAL, BOOL italic = FALSE) {
		h = make_pt(pt, name, weight, italic);
	}
	~gdi_font() { if (h) DeleteObject(h); }
	operator HFONT() const { return h; }
	gdi_font(const gdi_font&) = delete;
	gdi_font& operator=(const gdi_font&) = delete;
};

template <class HandleT>
struct selector {
	HDC dc{};
	HGDIOBJ old{};
	selector(HDC dc, const HandleT& obj) : dc(dc), old(SelectObject(dc, obj)) {}
	~selector() { SelectObject(dc, old); }
	selector(const selector&) = delete;
	selector& operator=(const selector&) = delete;
};

struct mem_dc {
	HDC real{}, mem{};
	HBITMAP bmp{}, oldbmp{};
	RECT r{};

	explicit mem_dc(HDC real) : real(real) {
		GetClientRect(WindowFromDC(real), &r);
		mem = CreateCompatibleDC(real);
		bmp = CreateCompatibleBitmap(real, r.right, r.bottom);
		oldbmp = (HBITMAP)SelectObject(mem, bmp);
	}
	operator HDC() const { return mem; }
	~mem_dc() {
		BitBlt(real, 0, 0, r.right, r.bottom, mem, 0, 0, SRCCOPY);
		SelectObject(mem, oldbmp);
		DeleteObject(bmp);
		DeleteDC(mem);
	}

	mem_dc(const mem_dc&) = delete;
	mem_dc& operator=(const mem_dc&) = delete;
};
