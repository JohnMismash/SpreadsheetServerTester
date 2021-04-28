/**
 * Spreadsheet header declarations.
 * Please read the corresponding .cpp file for function documentation.
 *
 * CS 3505 - Software Practice II
 * Spreadsheet Client/SpreadsheetServer
 *
 *  @authors: Sam Smith, Wil Thompson, Harrison Fackrell, John (Jack) Mismash
 *            William Le, Annie Ruiz
 *
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

 //class Client;
enum { max_length = 1024 };
char* port = 0;
char* address = 0;
boost::asio::io_context io_context;
std::thread* thread;


//boost::asio::io_context io_context;
//tcp::resolver::results_type endpoints;
std::string currentMessage;

class Client
{
  //boost::asio::io_context& io_context;
  tcp::resolver::results_type endpoints;

  tcp::socket s;
  tcp::resolver resolver;

public:

    char message[max_length];
  //renames shared_ptr to connection to pointer
  //typedef boost::shared_ptr<Client> pointer;

  //pointer Create(boost::asio::io_context& io_context)
//  {
  //  return pointer(new Client(io_context));
//  }

  Client(boost::asio::io_context& io_context) : endpoints(), s(io_context), resolver(io_context)
  {
    //tcp::resolver resolver(io_context);
    //tcp::socket s(io_context);
  }
void dataRecieved(const boost::system::error_code& error)
{
  currentMessage = message;
  std::cout << "message " << currentMessage << std::endl;
  memset(message, 0, sizeof(message));
  if(error)
  {
    std::cerr << "error: " << error.message() << std::endl;
    s.close();
  }

}


void RecievedMessage(const boost::system::error_code& error)
  {

    if(!error)
    {
      std::cout << "In recieved message: " << message << std::endl;
      memset(message, 0, sizeof(message));
      s.async_read_some(boost::asio::buffer(message, sizeof(message)),
              boost::bind(&Client::dataRecieved, this,
                boost::asio::placeholders::error));
    }
    else
    {
      std::cerr << "error: " << error.message() << std::endl;
      s.close();
    }
  }

void setUpClient(std::string messageToSend)
{
    endpoints = resolver.resolve(tcp::v4(), address, port);
    boost::asio::connect(s, endpoints);

    std::cout << "sending: " << messageToSend << std::endl;
    boost::asio::async_write(s,
          boost::asio::buffer(messageToSend),
          boost::bind(&Client::RecievedMessage, this,
            boost::asio::placeholders::error));
    io_context.run();
}
void sendMessage (std::string messageToSend)
{
  io_context.reset();
  std::string data;
  try
  {
  std::cout << "sending: " << messageToSend << std::endl;


  boost::asio::async_write(s,
        boost::asio::buffer(messageToSend),
        boost::bind(&Client::RecievedMessage, this,
          boost::asio::placeholders::error));
  io_context.run();
}
catch (std::exception& e)
{
  std::cerr << "Exception: " << e.what() << "\n";
}

}
};



void handshake(Client* client)
{
  client->setUpClient("username\n");
  client->sendMessage("spreadsheet1\n");
}

void test1()
{
  Client* client = new Client(io_context);

  handshake(client);

  //client->sendMessage("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"hello\"}\n");
  if(currentMessage == "{messageType:\"cellUpdated\",cellName:\"A1\",contents:\"hello\"}\n")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
}

// void test2()
// {
//   handshake();
//
//   sendMessage("{requestType:\"editCell\", cellName: \"A1\",contents: \"hello\"}0");
//   std::cout << "current message = " << currentMessage << std::endl;
//   if(currentMessage == "{messageType:\"cellUpdated\", cellName: \"A1\",contents: \"hello\"}")
//   {
//     std::cout << "pass" << std::endl;
//   }
//   else
//   {
//     std::cout << "fail" << std::endl;
//   }
// }
//
// void test3()
// {
//   handshake();
//
//   sendMessage("{requestType:\"editCell\", cellName: \"A1\",contents: \"hello\"}0");
//   std::cout << "current message = " << currentMessage << std::endl;
//   if(currentMessage == "{messageType:\"cellUpdated\", cellName: \"A1\",contents: \"hello\"}")
//   {
//     std::cout << "pass" << std::endl;
//   }
//   else
//   {
//     std::cout << "fail" << std::endl;
//   }
// }
//
// void test4()
// {
//   handshake();
//
//   sendMessage("{requestType:\"editCell\", cellName: \"A1\",contents: \"hello\"}0");
//   std::cout << "current message = " << currentMessage << std::endl;
//   if(currentMessage == "{messageType:\"cellUpdated\", cellName: \"A1\",contents: \"hello\"}")
//   {
//     std::cout << "pass" << std::endl;
//   }
//   else
//   {
//     std::cout << "fail" << std::endl;
//   }
// }

int main(int argc, char* argv[])
{
    address = argv[1];
    port = argv[2];

    //client = new Client(io_context);
    //std::thread thread(&Client::setUpClient, client, "username");

    if (argc == 0)
    {
      //std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      std::cout << "print number of tests" << std::endl;
      return 1;
    }

    else if(argc != 3)
    {
      std::cerr << "Invalid arguments: Must provide two arguments or zero arguments\n";
      return 1;
    }

    else
    {
      // int test_number = (int)(argv[1]);
      // switch(test_number) {
      //   case 1:
      //     test1();
      //     break; //optional
      //   case 2:
      //     test2();
      //     break; //optional
      //   case 3:
      //     test3();
      //     break;
      //   case 4:
      //     test4();
      //     break;
      //
      //   default:
      //     break;
        //}
        test1();
        //thread.join();
    }
}
