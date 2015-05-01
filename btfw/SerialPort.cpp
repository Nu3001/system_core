
#include <stddef.h>
#include "ubcspcfg.h"
/*
#ifdef _WIN32
    #include <Windows.h>
    typedef HANDLE    serial_t;
#elif defined __linux_usb__
    typedef int   serial_t;
#elif defined __android__
    typedef int   serial_t;
#endif
*/

#include "SerialPort.h"
#include <stdio.h>
 
#ifdef _WIN32
    #define INVALID_SERRAL INVALID_HANDLE_VALUE
#elif defined __linux_usb__
    #include <unistd.h>
    #include <fcntl.h> //open close
    #include <termios.h>//struct termios
    #define INVALID_SERRAL -1
 
    size_t t_raw_rate[] =
    {
        50,         75,         110,        134,        150,
        200,        300,        600,        1200,       1800,
        2400,       4800,      9600,       19200,      38400,
        57600,      115200,     230400,     460800,     500000,
        576000,     921600,     1000000,    1152000,    1500000,
        2000000,    2500000,    3000000,    3500000,    4000000
    };
 
    size_t t_unix_rate[] =
    {
        B50,        B75,        B110,       B134,       B150,
        B200,       B300,       B600,       B1200,      B1800,
        B2400,      B4800,         B9600,      B19200,     B38400,
        B57600,     B115200,    B230400,    B460800,    B500000,
        B576000,    B921600,    B1000000,   B1152000,   B1500000,
        B2000000,   B2500000,   B3000000,   B3500000,   B4000000
    };
#elif defined __android__
#include <stdio.h>
#include <string.h>  
#include <stdlib.h>    
#include <unistd.h>    
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      
#include <termios.h>    
#include <errno.h>
#include <pthread.h>
#include <utils/Log.h>
#include <asm/ioctl.h>     
 	  #define INVALID_SERRAL -1
#endif
 
 #define BT_CTRL_DEV_MAJOR		331
#define IOCTL_HANDLE_BT_POWER_ON            _IO(BT_CTRL_DEV_MAJOR, 20)
#define IOCTL_HANDLE_BT_POWER_OFF          _IO(BT_CTRL_DEV_MAJOR, 21)
#define IOCTL_HANDLE_BT_CLEAR_BUF           _IO(BT_CTRL_DEV_MAJOR, 23)
#define IOCTL_HANDLE_BT_UPDATE_FW		   _IO(BT_CTRL_DEV_MAJOR, 24)
 
 
 
 
SerialPort::SerialPort(void)
{
    m_hSerial = INVALID_SERRAL;
}
 
SerialPort::~SerialPort(void)
{
    this->close();
}

#ifdef __android__
int SerialPort::set_bonovo_update_fw()
{
	printf("set_bonovo_update_fw\n");
	int ret = -1;
	
	if (m_hSerial < 0)
	{
		return ret;
	}
	ret = ioctl(m_hSerial, IOCTL_HANDLE_BT_UPDATE_FW);

	return ret;
}
#endif

#ifdef __android__
int SerialPort::set_bonovo_bluetooth_power(int onOff)
{
	int ret = -1;
	if (m_hSerial < 0)
	{
		return ret;
	}
	
	if (onOff)
	{
		ret = ioctl(m_hSerial, IOCTL_HANDLE_BT_POWER_ON);
	}
	else
	{
		ret = ioctl(m_hSerial, IOCTL_HANDLE_BT_POWER_OFF);
	}
	return ret;
}
#endif
 
bool
SerialPort::open(int port,bool opt)
{
    char com[16]={0};
 
#ifdef _WIN32
 
    #ifdef WINCE
        sprintf(com,"COM%d:",port);
    #else
        sprintf(com,"COM%d",port);
    #endif
     
    DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
    if(opt)
    {
        dwFlagsAndAttributes |= FILE_FLAG_OVERLAPPED;
    }
 
 		printf("open com =%s\n",com);
 		
    m_hSerial = CreateFile(com, GENERIC_READ | GENERIC_WRITE,
                            0,
                            NULL, 
                            OPEN_EXISTING,
                            dwFlagsAndAttributes,
                            NULL);
    
#elif defined __linux_usb__
 
    #ifdef __arm__
        sprintf(com,"/dev/ttySAC%d",port-1);
    #else
       // sprintf(com,"/dev/ttyS%d",port-1);
	sprintf(com,"/dev/ttyUSB%d",port-1);
    #endif
 
    
    int flags = O_RDWR | O_NOCTTY;
    if(opt)
    {
        flags |= O_NDELAY;
    }
    m_hSerial = ::open(com,flags);
 
    
    if(m_hSerial!=INVALID_SERRAL)
    {
	printf("COM Name:%s OK.\n",com);
	set_block(false);
    }
    else
    {
	printf("COM Name:%s fail.\n",com);
    }
#elif defined __android__

	#define BT_TTY_DEV     "/dev/bonovo_tty" 

	int flags = O_RDWR | O_NOCTTY;
	if(opt)
	{
	      flags |= O_NONBLOCK;
	}

	if ((m_hSerial = ::open(BT_TTY_DEV, flags)) < 0) {
		printf("open failed! error\n");
		return 0;
	}

	/*
	if ((m_hSerial = ::open(BT_TTY_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
		printf("open failed! error\n");
		return 0;
	}*/

	//set_bonovo_bluetooth_power(1);
	//usleep(1000);
    
#endif
 
    return (m_hSerial != INVALID_SERRAL)?true:false;
}
 
void
SerialPort::close()
{
    if(m_hSerial != INVALID_SERRAL)
    {
#ifdef _WIN32
        CloseHandle(m_hSerial);
#elif defined __linux_usb__
        ::close(m_hSerial);
#elif defined __android__
	//set_bonovo_bluetooth_power(0);
	::close(m_hSerial);
#endif
        m_hSerial = INVALID_SERRAL;
    }
}
 
bool
SerialPort::set_attribute(  size_t rate, 
                            COMPARAM check_bit,
                            COMPARAM stop_bit,
                            COMPARAM data_bit)
{
    if(m_hSerial == INVALID_SERRAL) return false;
 
#ifdef _WIN32
    DCB dcb = {0};
    GetCommState(m_hSerial, &dcb);						/* 读取串口的DCB */
    
    dcb.BaudRate = rate;
 
    dcb.fParity = TRUE;
    dcb.fBinary = FALSE;
 
    if(check_bit == CHECK_NONE)
    {
    		dcb.Parity = EVENPARITY;
        dcb.fParity = FALSE;
        //dcb.Parity = NOPARITY;
    }
    else if(check_bit == CHECK_ODD)
    {
        dcb.Parity = ODDPARITY;
    }
    else if(check_bit == CHECK_EVEN)
    {
        dcb.Parity = EVENPARITY;
    }
 
    if(stop_bit == STOP_1)
    {
        dcb.StopBits = ONESTOPBIT;
    }
    else if(stop_bit == STOP_1_5)
    {
        dcb.StopBits = ONE5STOPBITS;;
    }
    else if(stop_bit == STOP_2)
    {
        dcb.StopBits = TWOSTOPBITS;
    }
 
    if(data_bit == DATA_8)
    {
        dcb.ByteSize = 8;
    }
    else if(data_bit == DATA_7)
    {
        dcb.ByteSize = 7;
    }
     
    Sleep(3000);
    if(  ! SetCommState(m_hSerial,&dcb)  )
    {
        return false;
    }
    
    
    SetupComm(m_hSerial,50/*IN BUF*/,50/*OUT BUFF*/);
 
    PurgeComm(m_hSerial,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
 		
#elif defined __linux_usb__
    struct termios com_cfg = {0};
    size_t raw_rate = 0;
    size_t i=0;
 
 
    for(i=0; i< sizeof(t_raw_rate)/sizeof(size_t) ; i++)
    {
        if(t_raw_rate[i] == rate)
        {
            raw_rate = t_unix_rate[i];
            break;
        }
    }
 
    if(!raw_rate)
    {
        return false;
    }
    else
    {
       // cfsetispeed(&com_cfg,raw_rate);//IN  speend;
       // cfsetospeed(&com_cfg,raw_rate);//OUT speend;

        cfsetispeed(&com_cfg,B115200);//IN  speend;
        cfsetospeed(&com_cfg,B115200);//OUT speend;
    }
 

    switch(check_bit)
    {
    case CHECK_NONE:
        com_cfg.c_cflag &= ~PARENB;
        com_cfg.c_iflag &= ~INPCK;
        break;
    case CHECK_NONE_UPDATE:
	{
         com_cfg.c_cflag |= PARENB;
         com_cfg.c_iflag &= ~INPCK;		
	}
	break;
    case CHECK_ODD:
        com_cfg.c_cflag |= (PARODD|PARENB);
        com_cfg.c_iflag |= INPCK;
        break;
    case CHECK_EVEN:
        com_cfg.c_cflag |= PARENB;
        com_cfg.c_iflag &= ~PARODD;
        com_cfg.c_iflag |= INPCK;
        break;
    default:
        return false;
    }
 
    switch(stop_bit)
    {
    case STOP_1:
	{
         com_cfg.c_cflag &= ~CSTOPB;
	}
        break;
    case STOP_1_5:
        com_cfg.c_cflag &= ~CSTOPB;//Linux没有1.5停止位
        break;
    case STOP_2:
        com_cfg.c_cflag |= CSTOPB;
        break;
    default:
        return false;
    }
 
    switch(data_bit)
    {
    case DATA_8:
        com_cfg.c_cflag &= ~CSIZE;
        com_cfg.c_cflag |= CS8;
        break;
    case DATA_7:
        com_cfg.c_cflag &= ~CSIZE;
        com_cfg.c_cflag |= CS7;
        break;
    default:
        return false;
    }
 
    tcflush(m_hSerial,TCIOFLUSH);
 
    com_cfg.c_cc[VTIME] = 0;
    com_cfg.c_cc[VMIN] =  0;
 
    if( tcsetattr(m_hSerial,TCSANOW,&com_cfg) )
    {
        return false;
    }
#elif defined __android__
	
#endif
    return true;
}
 
bool 
SerialPort::set_timeout(size_t write_ms,size_t read_ms,int flag)
{
    if(m_hSerial == INVALID_SERRAL) return false;
 
#ifdef _WIN32
 
    COMMTIMEOUTS to;
    memset(&to,0,sizeof(to));
 
    /* 读间隔超时。 接收时，两字符间最大的时延。
     * 将ReadIntervalTimeout设置为0, 则不使用间隔超时, 只考虑总超时设置
     * */
    to.ReadIntervalTimeout = flag;
 
    to.WriteTotalTimeoutMultiplier = 1;//写每字节的超时
    to.WriteTotalTimeoutConstant = write_ms;//写串口数据的固定超时
    // TIMEOUT = WriteTotalTimeoutMultiplier  Bytecount  +   WriteTotalTimeoutConstant
 
    to.ReadTotalTimeoutMultiplier =  1; //读取每字节的超时
    to.ReadTotalTimeoutConstant = read_ms; //读串口数据的固定超时
    // TIMEOUT = ReadTotalTimeoutMultiplier * Bytecount   +   ReadTotalTimeoutConstant
    return (SetCommTimeouts(m_hSerial,&to) == TRUE)?true:false;
 
#elif defined __linux_usb__
    struct termios com_cfg = {0};
 
    tcgetattr(m_hSerial,&com_cfg);//get old setting
 
    com_cfg.c_cc[VTIME] = read_ms*0.01;//超时单位 百毫秒
    com_cfg.c_cc[VMIN] =  flag;//最少可读数据 超时有效必须是0
 
    return (!tcsetattr(m_hSerial,TCSANOW,&com_cfg) )?true:false;
#elif defined __android__
	return true;
#endif
}
 
#ifdef __linux_usb__
bool
SerialPort::set_block(bool isBlock)
{
    if(m_hSerial == INVALID_SERRAL) return false;
 
    if(isBlock)
    {
        return ( -1 == fcntl(m_hSerial,F_SETFL,0) )?false:true;
    }
    else
    {
        return ( -1 == fcntl(m_hSerial,F_SETFL,0) )?false:true;
    }
    return true;
}
#endif
int
SerialPort::send(const char* sendBuf,size_t len)
{
    if(m_hSerial == INVALID_SERRAL) return 0;

#ifdef _WIN32
    DWORD write_len = 0;
    WriteFile(m_hSerial,sendBuf,len,&write_len,NULL);
    return write_len;
#elif defined __linux_usb__
    int retLen=write(m_hSerial,sendBuf,len);
    return retLen;
#elif defined __android__
 	 int retLen=write(m_hSerial,sendBuf,len);
 	return retLen;
#endif
}
 
int
SerialPort::recv(char* recvBuf,size_t len)
{
    if(m_hSerial == INVALID_SERRAL)
    {
    	printf("open failed! error\n");
    	return 0;
    } 
	
#ifdef _WIN32
    DWORD read_len = 0;
    ReadFile(m_hSerial,recvBuf,len,&read_len,NULL);
    return read_len;
#elif defined __linux_usb__  
    int retLen=read(m_hSerial,recvBuf,len);
    return retLen;
#elif defined __android__ 
   
    int retLen=read(m_hSerial,recvBuf,len);

	if(retLen<0)
	{
		return 0;
	}
    return retLen;
#endif
}
 
void 
SerialPort::abort(COMIO mode)
{
    if(m_hSerial != INVALID_SERRAL)
    {
        if(mode == COM_IN)
        {
#ifdef _WIN32
            //中断所有读操作并立即返回，即使读操作还没有完成
            PurgeComm(m_hSerial,PURGE_RXABORT);
#elif defined __linux_usb__
            tcflush(m_hSerial,TCIOFF);//挂起输入
#elif defined __android__ 

#endif
        }
        else if(mode == COM_OUT)
        {
#ifdef _WIN32
            //中断所有写操作并立即返回，即使写操作还没有完成
            PurgeComm(m_hSerial,PURGE_TXABORT);
#elif defined __linux_usb__
            tcflush(m_hSerial,TCOOFF);//挂起输出
#elif defined __android__ 

#endif
        }
    }
}
 
void
SerialPort::clear_buff(COMIO mode)
{
    if(m_hSerial != INVALID_SERRAL)
    {
        if(mode == COM_IN)
        {
#ifdef _WIN32
            PurgeComm(m_hSerial,PURGE_RXCLEAR);
#elif defined __linux_usb__
            tcflush(m_hSerial,TCIFLUSH);
#elif defined __android__ 

#endif
        }
        else if(mode == COM_OUT)
        {
#ifdef _WIN32
            PurgeComm(m_hSerial,PURGE_TXCLEAR);
#elif defined __linux_usb__
            tcflush(m_hSerial,TCOFLUSH);
#elif defined __android__ 

#endif
        }
    }
}
 
int
SerialPort::get_serial_handle()
{
    return m_hSerial;
}
 
#ifdef _WIN32
size_t
SerialPort::peek()
{
    if(m_hSerial == INVALID_SERRAL)
    {
        return  0;
    }
 
    DWORD dwErrorFlags;
    COMSTAT ComStat;
 
    ClearCommError( m_hSerial, &dwErrorFlags, &ComStat );
 
    return ComStat.cbInQue;
}
#endif
