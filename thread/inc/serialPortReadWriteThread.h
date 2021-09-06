#ifndef __SERIAL_READ_PORT_THREAD_H
#define __SERIAL_READ_PORT_THREAD_H

#include "serialPort.h"
#include <mutex>
#include <thread>
#include "thread.h"
#include<opencv2/opencv.hpp>
namespace zjl
{
    // 接收数据格式
    class receiveData{
    public:
	unsigned char flag_number;
    	unsigned char flag;
	unsigned char flag_sun;
        short yaw;
        short pitch;
    };

    // 发送数据格式
    class sendData{
    public:
        unsigned char flag_1;   //位置
        unsigned char flag_2;   //垃圾
        short yaw;
        short pitch;
        short x;
        short y;
        unsigned char class_;
        unsigned char send_color;
    };
    // 串口设备的接收接口类
    class serialPortReadWriteThread:public serialPort,public thread
    {
    public:
        serialPortReadWriteThread();
        ~serialPortReadWriteThread();
        receiveData getReceiveMsg();
        void setSendMsg(sendData data);
        void setSendMsg_2(sendData data);
    private:
	    unsigned char temptemp_read[2*10];
	    unsigned char temp_read[10];
        int max_receive_len_read = 10;
        unsigned char read_data[10];
        int max_receive_len_send = 12;
//        receiveData receive_{};
        receiveData stm32;
        void process() override;
        void readData();
        void writeData();
        void writeData_2();
        sendData send_{};
        sendData send_2{};
    };
    typedef struct
    {
        cv::Mat mat;
        receiveData receiveData_;
    }Mat_ly;
}
#endif //__SERIAL_PORT_THREAD_H
