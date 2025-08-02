#pragma once
#include <windows.h>

struct pen {
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
#pragma once
