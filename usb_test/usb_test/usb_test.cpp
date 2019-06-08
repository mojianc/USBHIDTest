// hook_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Dbt.h>
#include <iostream>
#include "usb_hid.h"

using namespace std;

const _TCHAR CLASS_NAME[] = _T("usb_test msg window");
HWND m_hwnd;
HHOOK hook;
usb_hid usb;

/////////////////////////////////////////////////////////////////////
//usb 拔插相关代码
bool CreateMessageOnlyWindow()
{
	m_hwnd = CreateWindowEx(0, CLASS_NAME, _T(""), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,       // Parent window    
		NULL,       // Menu
		GetModuleHandle(NULL),  // Instance handle
		NULL        // Additional application data
		);

	return m_hwnd != NULL;
}


void UsbDeviceChange(WPARAM wParam, LPARAM lParam)
{
	if (DBT_DEVICEREMOVECOMPLETE == wParam)//拔
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		if (DBT_DEVTYP_DEVICEINTERFACE == pHdr->dbch_devicetype)
		{
			PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
			wstring dbccName = pDevInf->dbcc_name;
			wcout << L"usb remove" << dbccName.c_str() << endl;
			if (dbccName.length() > 0)
			{
				//这里判断pid vid是否对应的设备
				wstring::size_type id1 = dbccName.find(L"VID_040B");
				wstring::size_type id2 = dbccName.find(L"PID_2803");
				if (id1 != string::npos&&id2 != string::npos)
				{
					usb.stop();
				}
			}
			
			
		}
	}
	else if (DBT_DEVICEARRIVAL == wParam)//插
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		if (DBT_DEVTYP_DEVICEINTERFACE == pHdr->dbch_devicetype)
		{
			PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
			wstring dbccName = pDevInf->dbcc_name;
			wcout << L"usb arrival" << dbccName.c_str() << endl;
			if (dbccName.length() > 0)
			{
				//这里判断pid vid是否对应的设备
				wstring::size_type id1 = dbccName.find(L"VID_040B");
				wstring::size_type id2 = dbccName.find(L"PID_2803");
				if (id1 != string::npos&&id2 != string::npos)
				{
					usb.start();
				}
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		break;
	case WM_SIZE:
		break;
	case WM_DEVICECHANGE:
		UsbDeviceChange(wParam, lParam);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

ATOM MyRegisterClass()
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;
	return RegisterClass(&wc);
}

bool RegisterDevice()
{
	const GUID GUID_DEVINTERFACE_LIST[] = {
		{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
		{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
		{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } }, /* HID */
		{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } } };

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[0];
	HDEVNOTIFY hDevNotify = RegisterDeviceNotification(m_hwnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	if (!hDevNotify) {
		return false;
	}
	return true;
}
/////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	MyRegisterClass();
	CreateMessageOnlyWindow();
	RegisterDevice();

	usb.start();

	usb.usb_write_data_test();
	MSG msg;
	while (int ret = GetMessage(&msg, m_hwnd, 0, 0))
	{
		if (ret != -1) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}