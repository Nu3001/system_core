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
    COM_IN,     //����ѡ��
    COM_OUT     //���ѡ��
};
 
class SerialPort
{
public:
    SerialPort(void);
    ~SerialPort(void);
     
    /*
     *�򿪴���
     *Param:
     * [IN]port    COM�˿�
     * [IN]opt     Windowsƽ̨   �Ƿ������ص�IO
     *             Linuxƽ̨    �Ƿ����÷�������ʽ
     * */
    bool   
    open(int port,bool opt = false);
     
    /*
     *�رմ���
     * */
    void
    close();
 
    /*
     *���ô�������
     *Param:
     * [IN]rate        ������
     * [IN]check_bit   У��λ
     * [IN]stop_bit    ֹͣλ(linux��1.5)
     * [IN]data_bit    ����λ
     * */
    bool   
    set_attribute(  size_t rate, 
                    COMPARAM check_bit    = CHECK_NONE,
                    COMPARAM stop_bit     = STOP_1,
                    COMPARAM data_bit     = DATA_8);
 
    /*
     *���ó�ʱ
     *Param:
     * [IN]write_ms    д��ʱ(��λ ����)
     * [IN]read_ms     ����ʱ(��λ ����)
     * [IN]flag        Windows��ָ������ӳ�
                        Linux��ָ���ǹ淶ģʽ����С�ַ���
     *Other
     * linux�ڷ�����ģʽ�����ó�ʱ��Ч
     * ��ʱ����Ϊ0ʱ��Ч������ͬ�ڷ�����ģʽ
     * read��ʱ����0
     * */
    bool
    set_timeout(size_t write_ms,size_t read_ms,int flag = 0);
 
    /*
     *��������
     *Param:
     * [IN]sendBuf     �������ݻ���
     * [IN]len         �������ݳ���
     */
    int
    send(const char* sendBuf,size_t len);
 
    /*
     *��������
     *Param:
     * [OUT]recvBuf    �������ݻ���
     * [IN] len       �������ݳ���
     */
    int
    recv(char* recvBuf,size_t len);
 
    /*
     *��ֹIO����
     *Param:
     * [IN]mode        ָ����Ҫ������IO
     */
    void   
    abort(COMIO mode);
 
    /*
     *��ջ���������
     *Param:
     * [IN]mode        ָ����Ҫ������IO
     */
    void   
    clear_buff(COMIO mode);
 
    /*
     *����ʹ�ú�ƽ̨��صĸ߼�IO����
     *��:
     *LINUX�µ�select��pollģ��
     *WIN32�µ�Overlappedģ��
     * */
    int
    get_serial_handle();
 
#ifdef _WIN32
    /*
     *��ȡ���ջ��������ݳ���
     */
    size_t
    peek();
#endif
 
#ifdef __linux_usb__
    /*
     *����LINUX�´���������ʽ
     *Param:
     * [IN]isBlock     �Ƿ�����
     */
    bool
    set_block(bool isBlock);
#endif

#ifdef __android__
int set_bonovo_bluetooth_power(int onOff);
int set_bonovo_update_fw();
#endif
 
private:
    int m_hSerial;//���ھ��/������
};
 
#endif //SERIALPORT_H_
