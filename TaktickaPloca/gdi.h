#pragma once
#include <windows.h>

/*struct pen {
	HGDIOBJ old;
	HDC dc;
	HPEN h;
	HPEN handle;
	pen(HDC dc, COLORREF color, int width = 1, int style = PS_SOLID) : dc(dc) {
		h = CreatePen(style, width, color);
		old = SelectObject(dc, h);
	}
	~pen() {
		SelectObject(dc, old);
		DeleteObject(h);
	}
	operator HGDIOBJ() const { return handle; }
};

struct brush {
	HGDIOBJ old;
	HDC dc;
	HBRUSH h;
	brush(HDC dc, COLORREF color) : dc(dc) {
		h = CreateSolidBrush(color);
		old = SelectObject(dc, h);
	}
	~brush() {
		SelectObject(dc, old);
		DeleteObject(h);
	}
	HBRUSH get() const { return h; }
};

struct null_brush {
	HGDIOBJ old;
	HDC dc;
	null_brush(HDC dc) : dc(dc) {
		old = SelectObject(dc, GetStockObject(NULL_BRUSH));
	}
	~null_brush() {
		SelectObject(dc, old);
	}
};

struct font {
	HGDIOBJ old;
	HDC dc;
	HFONT h;
	font(HDC dc, HFONT h) : dc(dc), h(h) {
		old = SelectObject(dc, h);
	}
	~font() {
		SelectObject(dc, old);
	}
};

inline HFONT create_bold_font(int pointSize = 14, const wchar_t* name = L"Segoe UI") {
	HDC screenDC = GetDC(NULL);
	int height = -MulDiv(pointSize, GetDeviceCaps(screenDC, LOGPIXELSY), 72);
	ReleaseDC(NULL, screenDC);

	return CreateFont(
		height, 0, 0, 0,
		FW_BOLD, FALSE, FALSE, FALSE,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		name
	);
}

struct dc {
	HDC real, mem;
	HBITMAP bmp, old;
	RECT r;
	dc(HDC real) : real(real) {
		GetClientRect(WindowFromDC(real), &r);
		mem = CreateCompatibleDC(real);
		bmp = CreateCompatibleBitmap(real, r.right, r.bottom);
		old = (HBITMAP)SelectObject(mem, bmp);
	}
	operator HDC() {
		return mem;
	}
	~dc() {
		BitBlt(real, 0, 0, r.right, r.bottom, mem, 0, 0, SRCCOPY);
		SelectObject(mem, old);
		DeleteObject(bmp);
		DeleteDC(mem);
	}
};

struct selector {
	HGDIOBJ old;
	HDC dc;
	selector(HDC dc, HGDIOBJ obj) : dc(dc) {
		old = SelectObject(dc, obj);
	}
	~selector() {
		SelectObject(dc, old);
	}
};
*/

#pragma once
#include <windows.h>
#include <tchar.h>

// RAII PEN (vlasnik)
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

// RAII BRUSH (vlasnik)
struct gdi_brush {
	HBRUSH h{};
	explicit gdi_brush(COLORREF color) { h = CreateSolidBrush(color); }
	~gdi_brush() { if (h) DeleteObject(h); }
	operator HBRUSH() const { return h; }
	gdi_brush(const gdi_brush&) = delete;
	gdi_brush& operator=(const gdi_brush&) = delete;

	static HBRUSH hollow() { return (HBRUSH)GetStockObject(HOLLOW_BRUSH); } // ne brisati!
};

// RAII FONT (vlasnik)
struct gdi_font {
	HFONT h{};
	static HFONT make_pt(int pointSize = 14, LPCTSTR name = _T("Segoe UI"),
		int weight = FW_NORMAL, BOOL italic = FALSE) {
		HDC sdc = GetDC(nullptr);
		const int height = -MulDiv(pointSize, GetDeviceCaps(sdc, LOGPIXELSY), 72);
		ReleaseDC(nullptr, sdc);
		return CreateFont(
			height, 0, 0, 0, weight, italic, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, name
		);
	}
	gdi_font(int pt = 14, LPCTSTR name = _T("Segoe UI"),
		int weight = FW_NORMAL, BOOL italic = FALSE) {
		h = make_pt(pt, name, weight, italic);
	}
	~gdi_font() { if (h) DeleteObject(h); }
	operator HFONT() const { return h; }
	gdi_font(const gdi_font&) = delete;
	gdi_font& operator=(const gdi_font&) = delete;
};

// Jedinstveni SELECTOR (select/restore bilo kojeg GDI objekta)
struct selector {
	HDC dc{};
	HGDIOBJ old{};
	selector(HDC dc, HGDIOBJ obj) : dc(dc), old(SelectObject(dc, obj)) {}
	~selector() { SelectObject(dc, old); }
	selector(const selector&) = delete;
	selector& operator=(const selector&) = delete;
};

// Double-buffer DC (RAII)
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

#pragma once
