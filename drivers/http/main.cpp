#include <httplib.h>
#include <qcircuit.hpp>

using namespace qc;
using namespace httplib;

int main(int argc, char **argv) {
  Server server;
  server.Get("/version", [](const Request &req, Response &res) {
    res.set_content("QCP Simulator - version 1.0", "text/plain");
  });
  server.listen("localhost", 12345);
  return 0;
}
