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
std::string port = "";
std::string address = "";
boost::asio::io_context io_context;
std::string currentMessage;

class Client
{
  tcp::resolver::results_type endpoints;
  tcp::socket s;
  tcp::resolver resolver;

public:
    char message[max_length];

  Client(boost::asio::io_context& io_context) : endpoints(), s(io_context), resolver(io_context)
  {
    //tcp::resolver resolver(io_context);
    //tcp::socket s(io_context);
  }
  void dataRecieved(const boost::system::error_code& error)
  {
    currentMessage = message;
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

    boost::asio::async_write(s,
          boost::asio::buffer(messageToSend),
          boost::bind(&Client::RecievedMessage, this,
            boost::asio::placeholders::error));
    io_context.run();
}

//Sends a message and expects a response back
void sendMessage (std::string messageToSend)
{
  io_context.reset();
  std::string data;

  boost::asio::async_write(s,
        boost::asio::buffer(messageToSend),
        boost::bind(&Client::RecievedMessage, this,
          boost::asio::placeholders::error));
  io_context.run();
}

//Sends a message to the server, and doesn't expect a response back. Ex. cell selection
void sendNoRecieve(std::string messageToSend)
{
  io_context.reset();
  std::string data;

  boost::asio::async_write(s,
        boost::asio::buffer(messageToSend),
        boost::bind(&Client::dataRecieved, this,
          boost::asio::placeholders::error));
  io_context.run();
}
};



void handshake(Client* client, std::string username)
{
  client->setUpClient(username + "\n");
  client->sendMessage("spreadsheet1\n");
}

void test1()
{
  //TODO: Set up timer
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"hello\"}\n");

  if(currentMessage == "{messageType: \"cellUpdated\", cellName: \"A1\", contents: \"hello\"}\n")
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
  //TODO: Set up timer
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"hello\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"yeet\"}\n");

  if(currentMessage == "{messageType: \"cellUpdated\", cellName: \"A1\", contents: \"yeet\"}\n")
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
  //TODO: Set up timer
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"hello\"}\n");

  if(currentMessage == "{messageType: \"cellUpdated\", cellName: \"A1\", contents: \"hello\"}\n")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
}

std::vector<std::string> split(const std::string str, char delim)
{
  std::vector<std::string> result;
  std::istringstream ss{str};
  std::string token;
  while (std::getline(ss, token, delim)) {
    if (!token.empty()) {
      result.push_back(token);
    }
  }
  return result;
}

int main(int argc, char* argv[])
{





    // address = argv[1];
    // port = argv[2];

    if (argc == 1)
    {
      //std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      std::cout << "print number of tests" << std::endl;
      return 0;
    }

    else if(argc != 3)
    {
      //std::cerr << "Invalid arguments: Must provide two arguments or zero arguments\n";
      std::cout << argc << std::endl;
      return 1;
    }

    else
    {
      std::string addressAndPort= argv[2];
      std::vector<std::string> data = split(addressAndPort, ':');
      std::cout << "data size " << data.size() << std::endl;
      //strcpy(address, data[0].c_str());
      std::cout << data[0] << std::endl;
      std::cout << data[1] << std::endl;
      address = data[0];
      port = data[1];
      std::cout << "got address" << std::endl;
      //strcpy(port, data[1].c_str());
      //address = data[0];

      //port = data[1];
      int test_number = 2;

      switch(test_number) {
        case 1:
          test1();
          break; //optional
        case 2:
          test2();
          break; //optional
        case 3:
          //test3();
          break;
        case 4:
          //test4();
          break;

        default:
          break;
        }
        //test1();
    }
}
