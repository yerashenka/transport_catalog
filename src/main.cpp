#include "json.h"
#include "transport_data.h"
#include "transport_informer.h"
#include "transport_database.h"
#include "map_builder.h"

#include <iomanip>
#include <iostream>

using namespace std;
using namespace TransportDatabase;
using namespace TransportInformer;

int main() {
  const Json::Dict input = Json::Load(cin).GetRoot().AsMap();

  const Json::Array &database_input = input.at("base_requests").AsArray();
  const Json::Dict &routing_settings = input.at("routing_settings").AsMap();
  Database db(TransportData::ReadData(database_input), routing_settings);

  const Json::Dict &render_settings = input.at("render_settings").AsMap();
  Informer informer(make_shared<Visualisation::MapBuilder>(db, render_settings));
  const Json::Array &info_requests = input.at("stat_requests").AsArray();
  cout << setprecision(6);
  cout << informer.ProcessRequests(db, info_requests) << endl;

  return 0;
}
