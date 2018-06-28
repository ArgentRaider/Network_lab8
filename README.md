# Network_lab8

The 8th experiment for the course 'Computer Network'.

This project builds a **really simple** web server that accepts HTTP requests and returns the corresponding response, for the purpose of helping to get familiar with HTTP protocol.

### Structure

├─`assets` : The base directory for URL routing

│  ├─ `html`

│  ├─ `img`

│  └─ `txt`

├─ `common.h` : Some common settings.

└─ `Network_lab8` : The main project directory.

​    ├─ `WebServer.cpp` : The entrance of the program.

​    ├─ `ClientQueue.h` : Multi-thread manager.

​    ├─ `WebAgent.h/cpp` : Deal with the web request. All the job is done inside `work()`.

​    ├─ `WebRoute.h/cpp` : The web router. It translates the URL into real file system path.

​    ├─ `HttpRequest.h/cpp` : The HTTP request parser.

​    └─ `HttpResponse.h/cpp` : Fill the HTTP response content.

