#include <httplib.h>
#include <input/input.hpp>
#include <qcircuit.hpp>
#include <sstream>
#include <string>

using namespace qc;
using namespace httplib;
using namespace std::string_literals;

int main(int argc, char **argv) {
  Server server;
  server.Get("/version", [](const Request &req, Response &res) {
    res.set_content("QCP Simulator - version 1.0", "text/plain");
  });
  server.Post("/simulate", [](const Request &req, Response &res) {
    if (!req.has_param("circuit")) {
      res.set_content("ERROR no circuit provided", "text/plain");
      return;
    }
    std::string strDesc = req.get_param_value("circuit");
    std::istringstream circuitStream{strDesc};
    try {
      auto [circuit, shots, noise] = parseCircuit(circuitStream, false);
      auto simResult = circuit->simulate(shots,false,noise);
      std::ostringstream results;
      results << "OK\n";
      results << "entries " << simResult.size() << "\n";
      for (const auto &value : simResult) {
        results << value << "\n";
      }
      res.set_content(results.str(), "text/plain");
    } catch (std::exception e) {
      res.set_content("ERROR "s + e.what(), "text/plain");
    }
  });
  server.listen("localhost", 12345);
  return 0;
}
