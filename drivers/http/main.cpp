#include <httplib.h>
#include <input/input.hpp>
#include <qcircuit.hpp>
#include <sstream>
#include <string>

using namespace qc;
using namespace httplib;
using namespace std::string_literals;

/*
 * Allow Cross-Origin Resource Sharing (CORS) 
 */
void allow_cors(Response &res, std::string method) {
  res.set_header("Access-Control-Allow-Origin", "*");
  res.set_header("Access-Control-Allow-Methods", method);
}

int main(int argc, char **argv) {
  Server server;
  server.Get("/version", [](const Request &req, Response &res) {
    allow_cors(res, "GET");
    res.set_content("QCP Simulator - version 1.0", "text/plain");
  });
  server.Post("/simulate", [](const Request &req, Response &res) {
    allow_cors(res, "POST");
    if (!req.has_param("circuit")) {
      res.set_content("ERROR no circuit provided", "text/plain");
      return;
    }
    std::string strDesc = req.get_param_value("circuit");
    std::istringstream circuitStream{strDesc};
    try {
      auto parsed = parseCircuit(circuitStream);
      auto simResult = parsed.circuit->simulate(parsed.shots, parsed.noGroup,
                                                parsed.noise, parsed.states);
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
  std::cout << "QCSim backend server running at localhost:12345" << std::endl;
  server.listen("localhost", 12345);
  return 0;
}
