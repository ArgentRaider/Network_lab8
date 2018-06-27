# Network_lab8

The 8th experiment for the course 'Computer Network'.

This project builds a **really simple** web server that accepts HTTP requests and returns the corresponding response, for the purpose of helping to get familiar with HTTP protocol.

### Structure

— `common.h` : Some common settings.

— `Network_lab8` : The main project directory.

|— `WebServer.cpp` : The entrance of the program.

|— `ClientQueue.h` : Multi-thread manager. It's a pure header simply because i am lazy wwwww

|— `WebAgent.h/cpp` : The class that actually communicate with the web client. All the job is done inside `work()`.

|— `WebRoute.h/cpp` : The web router. It translates the URL into real file system path.

|— `HttpRequest.h/cpp` : The HTTP request parser.

|— `HttpResponse.h/cpp` : Fill the HTTP response content.