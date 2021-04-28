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

enum { max_length = 1024 };
char* port = 0;
char* address = 0;

boost::asio::io_context io_context;
tcp::resolver resolver(io_context);
tcp::resolver::results_type endpoints;

tcp::socket s(io_context);
std::string currentMessage;
char message[max_length];

void DataSent(const boost::system::error_code& error, size_t bytes_transferred)
  {
    std::cout << "data sent" << std::endl;
    if(error)
    {
      std::cerr << "error: " << error.message() << std::endl;
      s.close();
    }
  }

void Loop(const boost::system::error_code& error, size_t bytes_transferred)
{
  currentMessage = message;
  std::cout << "current message is " << currentMessage << std::endl;
  s.async_read_some(boost::asio::buffer(message, sizeof(message)),
          boost::bind(Loop,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

}
void setUpClient(std::string messageToSend)
{
    endpoints = resolver.resolve(tcp::v4(), address, port);
    boost::asio::connect(s, endpoints);
    strcpy(message, messageToSend.c_str());
    boost::asio::async_write(s,
          boost::asio::buffer(message),
          boost::bind(Loop,
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}
void sendMessage (std::string messageToSend)
{
  std::string data;
  try
  {
  //std::cout << "going to connect" << std::endl;

  //std::cout << "connecting" << std::endl;

  //using namespace std; // For strlen.
  //std::cout << "Enter message: ";

  //std::cin.getline(request, max_length);
  strcpy(message, messageToSend.c_str());
  //size_t request_length = strlen(request);
  boost::asio::async_write(s,
        boost::asio::buffer(message),
        boost::bind(DataSent,
          boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  //boost::asio:async_write(s, boost::asio::buffer(request, request_length));

  //std::cout << "made it past write" << std::endl;
  //char reply[max_length];
  //size_t reply_length = s.async_read_some(boost::asio::buffer(reply, max_length), recieveMessage);
  //std::cout << "Reply is: ";
  //std::cout.write(reply, reply_length);
  //std::cout << "\n";
  //data = reply;
}
catch (std::exception& e)
{
  std::cerr << "Exception: " << e.what() << "\n";
}

}

void handshake()
{
  setUpClient("username");
  std::cout << "finished setting up" << std::endl;
  //sendMessage("username");
  std::cout << "sending spreadsheets" << std::endl;
  sendMessage("spreadsheet1");

}

void test1()
{
  handshake();

  sendMessage("{requestType:\"editCell\", cellName: \"A1\",contents: \"hello\"}0");
  std::cout << "current message = " << currentMessage << std::endl;
  if(currentMessage == "{messageType:\"cellUpdated\", cellName: \"A1\",contents: \"hello\"}")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
}

void test2()
{
  handshake();

  sendMessage("{requestType:\"editCell\", cellName: \"A1\",contents: \"hello\"}0");
  std::cout << "current message = " << currentMessage << std::endl;
  if(currentMessage == "{messageType:\"cellUpdated\", cellName: \"A1\",contents: \"hello\"}")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
}

void test3()
{
  handshake();

  sendMessage("{requestType:\"editCell\", cellName: \"A1\",contents: \"hello\"}0");
  std::cout << "current message = " << currentMessage << std::endl;
  if(currentMessage == "{messageType:\"cellUpdated\", cellName: \"A1\",contents: \"hello\"}")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
}

void test4()
{
  handshake();

  sendMessage("{requestType:\"editCell\", cellName: \"A1\",contents: \"hello\"}0");
  std::cout << "current message = " << currentMessage << std::endl;
  if(currentMessage == "{messageType:\"cellUpdated\", cellName: \"A1\",contents: \"hello\"}")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
}

int main(int argc, char* argv[])
{

    if (argc == 0)
    {
      //std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      std::cout << "print number of tests" << std::endl;
      return 1;
    }

    else if(argc != 3)
    {
      std::cerr << "Invalid arguments\n";
      return 1;
    }

    else
    {
      int test_number = (int)(argv[1]);
      switch(test_number) {
        case 1:
          test1();
          break; //optional
        case 2:
          test2();
          break; //optional
        case 3:
          test3();
          break;
        case 4:
          test4();
          break;

        default:
          break;
        }
    }
}
