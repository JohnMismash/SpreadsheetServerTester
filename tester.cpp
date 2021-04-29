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
using namespace std::chrono;

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

void testUpdateSingleCell()
{
  std::cout << "Test Name: " << "testUpdateSingleCell" << std::endl;
  auto start = high_resolution_clock::now();
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

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;

}

void testUpdateMultipleCells()
{
  std::cout << "Test Name: " << "testUpdateMultipleCells" << std::endl;
  auto start = high_resolution_clock::now();
  bool pass = false;
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"hello\"}\n");

  if(currentMessage == "{messageType: \"cellUpdated\", cellName: \"A1\", contents: \"hello\"}\n")
  {
    pass = true;
  }
  else
  {
     pass = false;
  }

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"B1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"B1\",\"contents\":\"yeet\"}\n");
  if(currentMessage == "{messageType: \"cellUpdated\", cellName: \"B1\", contents: \"yeet\"}\n")
  {
    if(pass)
    {
      std::cout << "pass" << std::endl;
    }
  }
  else
  {
    std::cout << "fail" << std::endl;
  }

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;
}

void testCircularDependency()
{
  std::cout << "Test Name: " << "testCircularDependency" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=A2+1\"}\n");
  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A2\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A2\",\"contents\":\"=A1+1\"}\n");

  if(currentMessage.find("requestError"))
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;
}

void testLongCircularDependency()
{
  std::cout << "Test Name: " << "testLongCircularDependency" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=A2+1\"}\n");
  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A2\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A2\",\"contents\":\"=B5+1\"}\n");
  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"B5\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"B5\",\"contents\":\"=A1+1\"}\n");

  if(currentMessage.find("requestError"))
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;
}

void testRevert ()
{
  std::cout << "Test Name: " << "testRevert" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=1+1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"hello\"}\n");
  client->sendMessage("{\"requestType\":\"revertCell\",\"cellName\":\"A1\"}\n");

  if(currentMessage == "{messageType: \"cellUpdated\", cellName: \"A1\", contents: \"=1+1\"}\n")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;

}

void testMultipleReverts ()
{
  std::cout << "Test Name: " << "testMultipleReverts" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=1+1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"hello\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"yo\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"dog\"}\n");
  client->sendMessage("{\"requestType\":\"revertCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"revertCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"revertCell\",\"cellName\":\"A1\"}\n");

  if(currentMessage == "{messageType: \"cellUpdated\", cellName: \"A1\", contents: \"=1+1\"}\n")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;

}

void testUndo ()
{
  std::cout << "Test Name: " << "testUndo" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=1+1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"hello\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");

  if(currentMessage == "{messageType: \"cellUpdated\", cellName: \"A1\", contents: \"=1+1\"}\n")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;

}

void testUndo2()
{
  std::cout << "Test Name: " << "testUndo2" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=1+1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"hello\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"cheese\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");

  if(currentMessage == "{messageType: \"cellUpdated\", cellName: \"A1\", contents: \"=1+1\"}\n")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;

}


void testInvalidFormula ()
{
  std::cout << "Test Name: " << "testInvalidFormula" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=A2)+1\"}\n");

  if(currentMessage.find("requestError"))
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;

}

void testInvalidFormula2()
{
  std::cout << "Test Name: " << "testInvalidFormula2" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=(5+7))\"}\n");

  if(currentMessage.find("requestError"))
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;

}

void testInvalidFormula3()
{
  std::cout << "Test Name: " << "testInvalidFormula3" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=AA4\"}\n");

  if(currentMessage.find("requestError"))
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;

}

void testSendManyMessages()
{
  std::cout << "Test Name: " << "testSendManyMessages" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=1+1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"hello\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"cheese\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"yeet\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"star\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=5+5\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"=67/3\"}\n");

  if(currentMessage == "{messageType: \"cellUpdated\", cellName: \"A1\", contents: \"=67/3\"}\n")
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;
}

void testUndoAndRevert()
{
  std::cout << "Test Name: " << "testUndoAndRevert" << std::endl;
  auto start = high_resolution_clock::now();
  Client* client = new Client(io_context);
  std::string username = "username";
  handshake(client, username);

  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"Table\"}\n");
  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A2\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A2\",\"contents\":\"hello\"}\n");
  client->sendMessage("{\"requestType\":\"revertCell\",\"cellName\":\"A2\"}\n");
  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"editCell\",\"cellName\":\"A1\",\"contents\":\"cheese\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");
  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"revertCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");
  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"revertCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");
  client->sendNoRecieve("{\"requestType\":\"selectCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"revertCell\",\"cellName\":\"A1\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");
  client->sendMessage("{\"requestType\":\"undo\"}\n");


  if(currentMessage.find("requestError"))
  {
    std::cout << "pass" << std::endl;
  }
  else
  {
    std::cout << "fail" << std::endl;
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  std::cout << "Time: " << duration.count() << " milliseconds"<< std::endl;
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
      std::cout << "6" << std::endl;
      return 0;
    }

    else if(argc != 3)
    {

      std::cout << "Invalid number of arguments" << std::endl;
      return 1;
    }

    else
    {
      std::string addressAndPort= argv[2];
      std::vector<std::string> data = split(addressAndPort, ':');

      address = data[0];
      port = data[1];

      int test_number = atoi(argv[1]);

      switch(test_number) {
        case 1:
          testUpdateSingleCell();
          break;
        case 2:
          testUpdateMultipleCells();
          break;
        case 3:
          testCircularDependency();
          break;
        case 4:
          testRevert();
          break;
        case 5:
          testUndo();
          break;
        case 6:
          testInvalidFormula();
          break;
        case 7:
          testInvalidFormula2();
          break;
        case 8:
          testUndo2();
          break;
        case 9:
          testMultipleReverts();
          break;
        case 10:
          testUndoAndRevert();
          break;
        case 11:
          testSendManyMessages();
          break;
        case 12:
          testLongCircularDependency();
          break;
        case 13:
          testInvalidFormula3();
          break;


        default:
          break;
        }
    }
}
