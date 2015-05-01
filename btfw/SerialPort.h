#ifndef SERIALPORT_H_
#define SERIALPORT_H_
 
#include <stddef.h>
//#define _WIN32

 
enum COMPARAM
{
    CHECK_NONE, //
    CHECK_ODD,  //
    CHECK_EVEN, //
    STOP_1,     //
    STOP_1_5,   //
    STOP_2,     //
    DATA_7,     //
    DATA_8,      //
    CHECK_NONE_UPDATE
};
 
enum COMIO
{
    COM_IN,     //输入选项
    COM_OUT     //输出选项
};
 
class SerialPort
{
public:
    SerialPort(void);
    ~SerialPort(void);
     
    /*
     *打开串口
     *Param:
     * [IN]port    COM端口
     * [IN]opt     Windows平台   是否启用重叠IO
     *             Linux平台    是否启用非阻塞方式
     * */
    bool   
    open(int port,bool opt = false);
     
    /*
     *关闭串口
     * */
    void
    close();
 
    /*
     *设置串口属性
     *Param:
     * [IN]rate        波特率
     * [IN]check_bit   校验位
     * [IN]stop_bit    停止位(linux无1.5)
     * [IN]data_bit    数据位
     * */
    bool   
    set_attribute(  size_t rate, 
                    COMPARAM check_bit    = CHECK_NONE,
                    COMPARAM stop_bit     = STOP_1,
                    COMPARAM data_bit     = DATA_8);
 
    /*
     *设置超时
     *Param:
     * [IN]write_ms    写超时(单位 毫秒)
     * [IN]read_ms     读超时(单位 毫秒)
     * [IN]flag        Windows下指定间隔延迟
                        Linux下指定非规范模式的最小字符数
     *Other
     * linux在非阻塞模式下设置超时无效
     * 超时设置为0时“效果”等同于非阻塞模式
     * read超时返回0
     * */
    bool
    set_timeout(size_t write_ms,size_t read_ms,int flag = 0);
 
    /*
     *发送数据
     *Param:
     * [IN]sendBuf     发送数据缓存
     * [IN]len         发送数据长度
     */
    int
    send(const char* sendBuf,size_t len);
 
    /*
     *接收数据
     *Param:
     * [OUT]recvBuf    接收数据缓存
     * [IN] len       接收数据长度
     */
    int
    recv(char* recvBuf,size_t len);
 
    /*
     *终止IO操作
     *Param:
     * [IN]mode        指定需要操作的IO
     */
    void   
    abort(COMIO mode);
 
    /*
     *清空缓冲区数据
     *Param:
     * [IN]mode        指定需要操作的IO
     */
    void   
    clear_buff(COMIO mode);
 
    /*
     *开放使用和平台相关的高级IO操作
     *如:
     *LINUX下的select和poll模型
     *WIN32下的Overlapped模型
     * */
    int
    get_serial_handle();
 
#ifdef _WIN32
    /*
     *获取接收缓冲区数据长度
     */
    size_t
    peek();
#endif
 
#ifdef __linux_usb__
    /*
     *设置LINUX下串口阻塞方式
     *Param:
     * [IN]isBlock     是否阻塞
     */
    bool
    set_block(bool isBlock);
#endif

#ifdef __android__
int set_bonovo_bluetooth_power(int onOff);
int set_bonovo_update_fw();
#endif
 
private:
    int m_hSerial;//串口句柄/描述符
};
 
#endif //SERIALPORT_H_
