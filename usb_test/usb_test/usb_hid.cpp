#include "stdafx.h"
#include "usb_hid.h"
#include <process.h>
#include <mmsystem.h> 
#include <iostream>

usb_hid::usb_hid()
{
	hid_init();
}


usb_hid::~usb_hid()
{
	stop();
	hid_exit();
}


int usb_hid::start()
{
	dev_info_ = hid_enumerate(vendor_id, product_id);
	if (!dev_info_)//û�ж����豸��Ϣ
	{
		std::cout << "cannot enumerate dev!" << std::endl;
		return -1;
	}
	//����vid pid���豸
	dev_handle_ = hid_open(vendor_id, product_id, NULL);
	if (!dev_handle_) 
	{
		std::cout << "cannot open dev!" << std::endl;
		return -1;
	}

	is_run_ = true;
	thrad_handle_ = (HANDLE)_beginthreadex(NULL, 0, usb_read_proc, this, NULL, NULL);
	if (thrad_handle_ == NULL)
	{
		std::cout << "create thread error!" << std::endl;
		is_run_ = false;
		return -1;
	}
	//WaitForSingleObject(thrad_handle_, 5000);
	CloseHandle(thrad_handle_);
	
	return 0;
}

int usb_hid::stop()
{
	is_run_ = false;
	//WaitForSingleObject(thrad_handle_, 5000);
	//CloseHandle(thrad_handle_);
	if (dev_info_ != NULL)
	{
		hid_free_enumeration(dev_info_);
		dev_info_ = NULL;
	}
	if (dev_handle_ != NULL)
	{
		hid_close(dev_handle_);
		dev_handle_ = NULL;
	}
	return 0;
}

bool usb_hid::get_state()
{
	return is_run_;
}

unsigned int _stdcall usb_hid::usb_read_proc(LPVOID lp_param)
{
	usb_hid * params = (usb_hid *)lp_param;
	unsigned char buf[30];
	memset(buf, 0, sizeof(buf));
	int res = 0;
	while (params->is_run_)
	{
		if (!params || !params->dev_handle_)
		{
			break;
		}
		res = hid_read(params->dev_handle_, buf, sizeof(buf));

		if (res > 0)
		{
			for (int i = 0; i < res; i++)
				printf("%02hhx ", buf[i]);
			printf("\n");
		}
		if (res < 0)
		{
		}
		Sleep(500);
	}
	return 0;
}

int usb_hid::usb_write_data_test(){
	//׼��д�������(hex)��20 00 02 00 07 00 00 00
	unsigned char buffer[9];
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0x00;//��λ��� report id
	buffer[1] = 0x20;//���ݿ�ʼ
	buffer[2] = 0x00;
	buffer[3] = 0x02;
	buffer[4] = 0x00;
	buffer[5] = 0x07;
	int res = hid_write(dev_handle_, buffer, 9);
	printf("hid_write data res: %d\n",res);
}