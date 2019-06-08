#pragma once

#include "HidApi/hidapi.h"

//设备的pid和vid，根据要读的设备设置
const unsigned short vendor_id = 0x2575;
const unsigned short product_id = 0x0001;

class usb_hid
{
public:
	usb_hid();
	~usb_hid();

	int start();
	int stop();

	bool get_state();
	int usb_write_data_test();

private:
	static unsigned int _stdcall usb_read_proc(LPVOID lp_param);
	//static unsigned int _stdcall usb_write_proc(LPVOID lp_param);
private:
	bool				is_run_;
	hid_device *		dev_handle_;
	hid_device_info*	dev_info_;
	HANDLE				thrad_handle_;
};

