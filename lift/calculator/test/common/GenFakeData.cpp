/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include "folly/Random.h"

#include "../../InputData.h"
#include "GenFakeData.h"

namespace private_lift {
double GenFakeData::genAdjustedPurchaseRate(
    bool isTest,
    double purchaseRate,
    double incrementalityRate) {
  double adjstedPurchaseRate = purchaseRate;
  if (isTest) {
    adjstedPurchaseRate = purchaseRate + (incrementalityRate / 2.0);
    if (adjstedPurchaseRate > 1.0) {
      throw std::invalid_argument(
          ">1.0 incrementality_rate + purchase_rate is not yet supported");
    }
  } else {
    adjstedPurchaseRate = purchaseRate - (incrementalityRate / 2.0);
    if (adjstedPurchaseRate < 0.0) {
      throw std::invalid_argument(
          "Incrementality rate cannot be significantly higher than the purchase rate");
    }
  }
  return adjstedPurchaseRate;
}

GenFakeData::LiftInputColumns GenFakeData::genOneFakeLine(
    const std::string& id,
    double opportunityRate,
    double testRate,
    double purchaseRate,
    double incrementalityRate,
    int32_t epoch,
    int32_t numConversions) {
  LiftInputColumns oneLine;
  oneLine.id = id;
  oneLine.opportunity = folly::Random::secureRandDouble01() < opportunityRate;
  oneLine.test_flag =
      oneLine.opportunity && folly::Random::secureRandDouble01() < testRate;
  purchaseRate = genAdjustedPurchaseRate(
      oneLine.test_flag, purchaseRate, incrementalityRate);
  bool hasPurchase = folly::Random::secureRandDouble01() < purchaseRate;
  oneLine.opportunity_timestamp =
      oneLine.opportunity ? folly::Random::secureRand32(100) + epoch : 0;
  for (auto i = 0; i < numConversions; i++) {
    oneLine.event_timestamps.push_back(
        oneLine.opportunity ? folly::Random::secureRand32(100) + epoch : 0);
    oneLine.values.push_back(
        hasPurchase ? folly::Random::secureRand32(100) + 1 : 0);
  }
  std::sort(oneLine.event_timestamps.begin(), oneLine.event_timestamps.end());
  return oneLine;
}

void GenFakeData::genFakePublisherInputFile(
    std::string filename,
    int32_t numRows,
    double opportunityRate,
    double testRate,
    double purchaseRate,
    double incrementalityRate,
    int32_t epoch) {
  std::ofstream publisherFile{filename};

  // publisher header: id_,opportunity,test_flag,opportunity_timestamp
  publisherFile << "id_,opportunity,test_flag,opportunity_timestamp\n";

  for (auto i = 0; i < numRows; i++) {
    // generate one row of fake data
    LiftInputColumns oneLine = genOneFakeLine(
        std::to_string(i),
        opportunityRate,
        testRate,
        purchaseRate,
        incrementalityRate,
        epoch,
        1);

    // write one row to publisher fake data file
    std::string publisherRow = oneLine.id + "," +
        (oneLine.opportunity ? "1," : "0,") +
        (oneLine.test_flag ? "1," : "0,") +
        std::to_string(oneLine.opportunity_timestamp);
    publisherFile << publisherRow << '\n';
  }
}

void GenFakeData::genFakePartnerInputFile(
    std::string filename,
    int32_t numRows,
    double opportunityRate,
    double testRate,
    double purchaseRate,
    double incrementalityRate,
    int32_t epoch,
    int32_t numConversions) {
  std::ofstream partnerFile{filename};

  // partner header: id_,event_timestamps,values
  partnerFile << "id_,event_timestamps,values\n";

  for (auto i = 0; i < numRows; i++) {
    // generate one row of fake data
    LiftInputColumns oneLine = genOneFakeLine(
        std::to_string(i),
        opportunityRate,
        testRate,
        purchaseRate,
        incrementalityRate,
        epoch,
        numConversions);

    // write one row to publisher fake data file
    std::string eventTSString = "[";
    std::string valuesString = "[";
    for (auto j = 0; j < numConversions; j++) {
      eventTSString += std::to_string(oneLine.event_timestamps[j]);
      valuesString += std::to_string(oneLine.values[j]);
      if (j < numConversions - 1) {
        eventTSString += ",";
        valuesString += ",";
      } else {
        eventTSString += "]";
        valuesString += "]";
      }
    }
    partnerFile << oneLine.id << "," << eventTSString << "," << valuesString
                << "\n";
  }
}
} // namespace private_lift
