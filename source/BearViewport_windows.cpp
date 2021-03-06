#include "BearEngine.hpp"
#ifdef WINDOWS
LRESULT CALLBACK GlobalOnEvent(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	}
	if (!BearEngine::GRender->GetGeneralViewport() || handle != BearEngine::GRender->GetGeneralViewport()->GetWindowHandle()) return DefWindowProc(handle, message, wParam, lParam);

	switch (message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		switch (wParam)
		{
		case VK_MENU:
			if (lParam & (1 << 24))
				wParam = VK_RMENU;
			else
				wParam = VK_LMENU;
			break;
		case VK_CONTROL:
			if (lParam & (1 << 24))
				wParam = VK_RCONTROL;
			else
				wParam = VK_LCONTROL;
			break;
		case VK_SHIFT:
			if (lParam & (1 << 24))
				wParam = VK_RSHIFT;
			else
				wParam = VK_LSHIFT;
			break;

		};


		auto item = BearEngine::GInput->m_keys_windows.find(DWORD(wParam));
		if (item == BearEngine::GInput->m_keys_windows.end())
			break;
		auto &stats = BearEngine::GInput->m_keys[item->second];
		if (stats != BearEngine::BearInput::KS_Press)
		{
			stats = BearEngine::BearInput::KS_Down;
		}
	}
	return 0;

	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		switch (wParam)
		{
		case VK_MENU:
			if (lParam & (1 << 24))
				wParam = VK_RMENU;
			else
				wParam = VK_LMENU;
			break;
		case VK_CONTROL:
			if (lParam & (1 << 24))
				wParam = VK_RCONTROL;
			else
				wParam = VK_LCONTROL;
			break;
		case VK_SHIFT:
			if (lParam & (1 << 24))
				wParam = VK_RSHIFT;
			else
				wParam = VK_LSHIFT;
			break;

		};


		auto item = BearEngine::GInput->m_keys_windows.find(DWORD(wParam));
		if (item == BearEngine::GInput->m_keys_windows.end())
			break;
		BearEngine::GInput->m_keys[item->second] = BearEngine::BearInput::KS_Up;
	}
	return 0;
	case WM_MOUSEWHEEL:
	{
		int16 delta = static_cast<int16>(HIWORD(wParam));
		if (delta > 0)
		{

			BearEngine::GInput->m_mouse_scroll = delta / 120.f;
			BearEngine::GInput->m_keys[BearEngine::KeyMouseScrollUp] = BearEngine::BearInput::KS_Up;
		}
		else if (delta < 0)
		{
			BearEngine::GInput->m_mouse_scroll = delta / 120.f;
			BearEngine::GInput->m_keys[BearEngine::KeyMouseScrollDown] = BearEngine::BearInput::KS_Up;
		}
	}
	break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		if (message == WM_LBUTTONDOWN)
			BearEngine::GInput->m_keys[BearEngine::KeyMouseLeft] = BearEngine::BearInput::KS_Down;
		else if (message == WM_MBUTTONDOWN)
			BearEngine::GInput->m_keys[BearEngine::KeyMouseMiddle] = BearEngine::BearInput::KS_Down;
		else  if (message == WM_RBUTTONDOWN)
			BearEngine::GInput->m_keys[BearEngine::KeyMouseRight] = BearEngine::BearInput::KS_Down;
	}
	return 0;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{
		if (message == WM_LBUTTONUP)
			BearEngine::GInput->m_keys[BearEngine::KeyMouseLeft] = BearEngine::BearInput::KS_Up;
		else if (message == WM_MBUTTONUP)
			BearEngine::GInput->m_keys[BearEngine::KeyMouseMiddle] = BearEngine::BearInput::KS_Up;
		else  if (message == WM_RBUTTONUP)
			BearEngine::GInput->m_keys[BearEngine::KeyMouseRight] = BearEngine::BearInput::KS_Up;
	}
	return 0;
	break;
	case WM_CHAR:
	{

		TCHAR ch = TCHAR(wParam);
		BearEngine::GInput->m_input_string.append(1,ch);
		return 0;
	}
	}
	return DefWindowProc(handle, message, wParam, lParam);
}
BearEngine::BearViewport::BearViewport(const BearName & type) :BearObject(type, 0)
{
	BEAR_OBJECT_VALUE(Width);
	BEAR_OBJECT_VALUE(Height);
	BEAR_OBJECT_VALUE(FullScreen);


	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(0);
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW ;
	wc.lpfnWndProc = GlobalOnEvent;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(69, 22, 28));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("BEAR");
	wc.cbSize = sizeof(WNDCLASSEX);


	RegisterClassEx(&wc);
	DWORD Style = WS_OVERLAPPED | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	{
		RECT rectangle = { 0, 0, static_cast<long>(Width), static_cast<long>(Height) };
		m_window = CreateWindow(TEXT("BEAR"), TEXT(""), Style, 0, 0, 1, 1, NULL, NULL, hInstance, NULL);

		AdjustWindowRect(&rectangle, GetWindowLong((HWND)m_window, GWL_STYLE), false);
		SetWindowPos((HWND)m_window, NULL, 0, 0, rectangle.right-rectangle.left, rectangle.bottom- rectangle.top, SWP_NOMOVE | SWP_NOZORDER);

		uint32 xpos = static_cast<int32>(((uint32)GetSystemMetrics(SM_CXSCREEN) / 2) - (Width / 2));
		uint32 ypos = static_cast<int32>(((uint32)GetSystemMetrics(SM_CYSCREEN) / 2) - (Height / 2));

		SetWindowPos((HWND)m_window, NULL, xpos, ypos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	ShowWindow((HWND)m_window, SW_SHOW);



	Create(m_window, GetSize().x, GetSize().y, FullScreen, false);


	BearGraphics::BearShaderConstantsInitializer sconst_othro;
	sconst_othro.Constants.push_back(BearGraphics::CF_MATRIX);
	m_sconst_othro = BearGraphics::BearShaderConstantsRef(sconst_othro, true);

	m_othro.BuildOrthoOffCenter((float)GetSizeFloat().x, (float)GetSizeFloat().y, FLT_MIN, 100.f);
	BearCore::bear_copy(m_sconst_othro.Lock(), *m_othro, sizeof(float) * 16);
	m_sconst_othro.Unlock();

}
BearEngine::BearViewport::~BearViewport()
{
	m_sconst_othro.Clear();
	DestroyWindow((HWND)m_window);
}

void BearEngine::BearViewport::Resize(bsize wigth, bsize height)
{
	
	Width = wigth;
	Height = height;
	
	if (!FullScreen)
	{
		uint32 xpos = static_cast<int32>(((uint32)GetSystemMetrics(SM_CXSCREEN) / 2) - (Width / 2));
		uint32 ypos = static_cast<int32>(((uint32)GetSystemMetrics(SM_CYSCREEN) / 2) - (Height / 2));

		RECT rectangle = { 0, 0, static_cast<long>(Width), static_cast<long>(Height) };
		AdjustWindowRect(&rectangle, GetWindowLong((HWND)m_window, GWL_STYLE), false);
		long w = rectangle.right - rectangle.left;
		long h = rectangle.bottom - rectangle.top;
		SetWindowPos((HWND)m_window, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
		SetWindowPos((HWND)m_window, NULL, xpos, ypos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	else
	{
		SetWindowPos((HWND)m_window, HWND_TOP, 0, 0, static_cast<int32>(Width), static_cast<int32>(Height), SWP_FRAMECHANGED);
	}
	
	m_othro.BuildOrthoOffCenter((float)GetSizeFloat().x, (float)GetSizeFloat().y, FLT_MIN, 100.f);
	BearCore::bear_copy(m_sconst_othro.Lock(), *m_othro, sizeof(float) * 16);
	m_sconst_othro.Unlock();
	BearGraphics::BearViewport::Resize((bsize)GetSizeFloat().x, (bsize)GetSizeFloat().y);

}

void BearEngine::BearViewport::SetFullScreen(bool fullscreen)
{

	BearGraphics::BearViewport::SetFullScreen(fullscreen);
	FullScreen = fullscreen;
	if (FullScreen)
	{
		SetWindowLong((HWND)m_window, GWL_STYLE, WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE);
		ShowWindow((HWND)m_window, SW_MAXIMIZE);

	}
	else
	{
		SetWindowLong((HWND)m_window, GWL_STYLE, WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU);
		ShowWindow((HWND)m_window, SW_SHOW);
	}
}


bool BearEngine::BearViewport::Update()
{

	if (GRender->GetGeneralViewport() == this)
	{
		GInput->m_input_string.clear_no_free();
		{
			
			auto &item = GInput->m_keys[KeyMouseScrollUp];
			if (item == BearInput::KS_Up)
				item = BearInput::KS_Down;
			else if (item == BearInput::KS_Down)
			{
				GInput->m_mouse_scroll = 0;
				item = BearInput::KS_None;
			}

		}
		{
			auto &item = GInput->m_keys[KeyMouseScrollDown];
			if (item == BearInput::KS_Up)
				item = BearInput::KS_Down;
			else if (item == BearInput::KS_Down)
			{
				GInput->m_mouse_scroll = 0;
				item = BearInput::KS_None;
			}
		}
		{
			auto&item = GInput->m_keys[KeyMouseLeft];
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				item = BearInput::KS_Press;
			}
			else
			{
				item = BearInput::KS_None;
			}
		}

		{
			auto&item = GInput->m_keys[KeyMouseRight];
			if (GetAsyncKeyState(VK_RBUTTON))
			{
				item = BearInput::KS_Press;
			}
			else
			{
				item = BearInput::KS_None;
			}
		}
		{
			auto&item = GInput->m_keys[KeyMouseMiddle];
			if (GetAsyncKeyState(VK_MBUTTON))
			{
				item = BearInput::KS_Press;
			}
			else
			{
				item = BearInput::KS_None;
			}
		}


	}
	{
		auto begin = GInput->m_keys_windows.begin();
		auto end = GInput->m_keys_windows.end();
		while (begin != end)
		{
			auto&key=GInput->m_keys[begin->second];
			if (key == BearInput::KS_Down)
			{
				key = BearInput::KS_Press;
			}
			begin++;
		}
	}
	MSG msg;
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	if (msg.message == WM_QUIT)
		return false;
	return true;
}

void BearEngine::BearViewport::Destroy()
{
	BEAR_OBJECT_DESTROY(BearViewport);
}

void BearEngine::BearViewport::Update(float time)
{
}

void BearEngine::BearViewport::Save(BearCore::BearOutputStream * stream)
{
}

void BearEngine::BearViewport::Load(const BearCore::BearInputStream * stream)
{
}

#endif