// Implementation of the ClientSocket class

#include "ClientSocket.h"
#include "SocketException.h"


ClientSocket::ClientSocket ( std::string host, int port )
{
  if ( ! Socket::create() )
    {
      //throw SocketException ( "Could not create client socket." );
      printf("ClientSocket -----------create fail\n");
    }

  if ( ! Socket::connect ( host, port ) )
    {
      //throw SocketException ( "Could not bind to port." );
       printf("ClientSocket -----------connect fail\n");
    }

}


const ClientSocket& ClientSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      //throw SocketException ( "Could not write to socket." );
       printf("ClientSocket -----------send  fail\n");
    }

  return *this;

}


const ClientSocket& ClientSocket::operator >> ( std::string& s ) const
{
  if ( ! Socket::recv ( s ) )
    {
      //throw SocketException ( "Could not read from socket." );
      printf("ClientSocket -----------recv  fail\n");
    }

  return *this;
}
