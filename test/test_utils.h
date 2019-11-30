#pragma once

#include "json.h"
#include <vector>


void CheckResponses(const std::vector<Json::Node> &lhs, const std::vector<Json::Node> &rhs);
void CheckStreams(std::istream &lhs, std::istream &rhs);
std::vector<Json::Node> ProcessExample(const Json::Document &doc);