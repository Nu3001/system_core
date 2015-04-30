// Implementation of the ClientSocket class
#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include <pthread.h>
#include "system/core/btfw/btmsg.pb.h"

using namespace std;
using namespace btmsg;
#include "BTUpdateClientSocket.h"

 int packageid = -1;
 int packagetotal = 0;

void *BTUpdateThread(void *argv)
 {
 
 	ClientSocket client_socket ("127.0.0.1", 12345);
	msg mymsg=msg::default_instance();
	mymsg.set_name("btTest");
	mymsg.set_packageid(packageid);
	mymsg.set_packagetotal(packagetotal);


      while(packageid<=packagetotal)
      {
      	   if(packageid>0)
	   {
	      	    mymsg.set_packageid(packageid);
		    mymsg.set_packagetotal(packagetotal);

		    string data;
			
		    mymsg.SerializeToString(&data);

		    client_socket << data;	   
	   }
		   
	    usleep(1000 * 500);
      }

     return NULL;
	  
 }

void RunBTUpdateClientSocketTast ()
{
	pthread_t timerid;
	int err = 0;

	err = pthread_create(&timerid, NULL, BTUpdateThread, NULL); 
	if (err) {
		return;
	}
}

