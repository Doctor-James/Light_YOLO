#include "serialPortReadWriteThread.h"
#include <iostream>
#include <utility>
namespace zjl{
    receiveData receive_;
    int status = 0;
    /**
     * @brief 构造函数
     * @param portName 串口名称
     */
    serialPortReadWriteThread::serialPortReadWriteThread()
    {

        init("/dev/ttyUSB0");
        start(100);

    }
    serialPortReadWriteThread::~serialPortReadWriteThread()
    {
        //delete read_data_;
    }
    /**
     * @brief 串口线程函数
     */
    void serialPortReadWriteThread::process()
    {
//std::cout<<"ss1"<<std::endl;
        readData();
        status++;
       // std::cout<<"ss2"<<std::endl;
	    if(status == 10)
        {
            writeData();
        }
        else if(status == 20)
        {
            writeData_2();
        }
        if(status > 20)
        {
            status =0;
        }
	//std::cout<<"ss"<<std::endl;
    }
    /**
     * @brief 读数据
     */
    void serialPortReadWriteThread::readData()
    {
	    short flag, counter, Gyro_Pitch, Gyro_Yaw;
        SP_Read(read_data,max_receive_len_read);
        //@TODO 读取数据
	    memcpy(temptemp_read+10,read_data,10);
        for(int start_bit=0;start_bit<10;start_bit++)
        {
            if(temptemp_read[start_bit]=='!')
            {
                for(int j=0;j<10;j++)
                {
                    temp_read[j]=temptemp_read[start_bit+j];
                }
                //@TODO CRC校验
                if(Verify_CRC8_Check_Sum(temp_read,10))//CRC校验
                {
                    stm32.flag = temp_read[1];
                    //std::cout<<(int)stm32.flag<<std::endl;

              //      stm32.yaw = (temp_read[2]<<24)|(temp_read[3]<<16)|(temp_read[4]<<8)|temp_read[5];
                stm32.flag_number = temp_read[2];
	stm32.flag_sun = temp_read[3];
		    stm32.yaw = (temp_read[4]<<8)|temp_read[5];

		    stm32.pitch = (temp_read[6]<<8)|temp_read[7];
                }
                break;
            }
	    }
	    memcpy(temptemp_read,temptemp_read+10,10);
        //互斥锁
        mutex_serial_receive_.lock();
        receive_ = stm32;
        //std::cout<<(int)receive_.flag<<std::endl;
        mutex_serial_receive_.unlock();
    }
    /**
     * @brief 获取收到的数据
     * @return 当前数据
     */
    receiveData serialPortReadWriteThread::getReceiveMsg()
    {
        mutex_serial_receive_.lock();
        receiveData temp = receive_;
        mutex_serial_receive_.unlock();
        return temp;
    }

    /**
 * @brief 写数据
 */
    void serialPortReadWriteThread::writeData()
    {
        unsigned char msg[max_receive_len_send];

        mutex_serial_send_.lock();
        auto temp = send_;
        mutex_serial_send_.unlock();

        //@TODO 添加CRC
        msg[0] = '!';
        msg[1] = temp.flag_1;
        msg[2] = '#';
//        std::cout<<"temp.flag_1"<<(int)temp.flag_1<<std::endl;
        unsigned char *tmp = (unsigned char*)(&temp.pitch);
       tmp = (unsigned char*)(&temp.send_color);
   //       std::cout<<"temp.flag_1"<<(int)temp.send_color<<std::endl;
     
       msg[3]=tmp[0];
        msg[4]=0;//tmp[0];

        tmp = (unsigned char*)(&temp.yaw);
        msg[5]=0;//tmp[1];
        msg[6]=0;//tmp[0];

//        temp.x = 20000;
        tmp = (unsigned char*)(&temp.x);
        msg[7]=tmp[1];
        msg[8]=tmp[0];

//        temp.y = 20000;
        tmp = (unsigned char*)(&temp.y);
        msg[9]=tmp[1];
        msg[10]=tmp[0];

        msg[11] = '#';
        Append_CRC8_Check_Sum(msg, 12);

        //@TODO 发送数据
        SP_Write(msg, max_receive_len_send);
    }
    /**
     * @brief 设置需要发送的数据
     * @param data 发送数据
     */
    void serialPortReadWriteThread::setSendMsg(sendData data)
    {
        mutex_serial_send_.lock();
        send_ = data;
        mutex_serial_send_.unlock();
    }


    /**
* @brief 写数据
*/
    void serialPortReadWriteThread::writeData_2()
    {
        unsigned char msg[max_receive_len_send];

        mutex_serial_send_2.lock();
        auto temp = send_2;
        mutex_serial_send_2.unlock();

        //@TODO 添加CRC
        msg[0] = '!';
        msg[1] = temp.flag_1;
        msg[2] = '$';
 //       std::cout<<"temp.flag_2"<<(int)temp.flag_1<<std::endl;
        unsigned char *tmp = (unsigned char*)(&temp.pitch);
        msg[3]=tmp[1];
        msg[4]=tmp[0];
	//std::cout<<temp.yaw<<std::endl;
        tmp = (unsigned char*)(&temp.yaw);
        msg[5]=tmp[1];
        msg[6]=tmp[0];
        tmp = (unsigned char*)(&temp.class_);
        msg[7]=tmp[0];
        msg[8]=0;//tmp[0];

        tmp = (unsigned char*)(&temp.y);
        msg[9]=0;//tmp[1];
        msg[10]=0;//tmp[0];

        msg[11] = '#';
        Append_CRC8_Check_Sum(msg, 12);

        //@TODO 发送数据
        SP_Write(msg, max_receive_len_send);
    }
    /**
     * @brief 设置需要发送的数据
     * @param data 发送数据
     */
    void serialPortReadWriteThread::setSendMsg_2(sendData data)
    {
        mutex_serial_send_.lock();
        send_2 = data;
        mutex_serial_send_.unlock();
    }

}

