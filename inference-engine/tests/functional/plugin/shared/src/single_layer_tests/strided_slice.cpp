// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <ie_core.hpp>
#include <ngraph_functions/builders.hpp>

#include "functional_test_utils/blob_utils.hpp"
#include "functional_test_utils/precision_utils.hpp"
#include "common_test_utils/common_utils.hpp"

#include "single_layer_tests/strided_slice.hpp"

namespace LayerTestsDefinitions {

std::string StridedSliceLayerTest::getTestCaseName(const testing::TestParamInfo<StridedSliceParams> &obj) {
    StridedSliceSpecificParams params;
    InferenceEngine::Precision netPrc;
    std::string targetName;
    std::map<std::string, std::string> additionalConfig;
    std::tie(params, netPrc, targetName, additionalConfig) = obj.param;
    std::ostringstream result;
    result << "inShape=" << CommonTestUtils::vec2str(params.inputShape) << "_";
    result << "netPRC=" << netPrc.name() << "_";
    result << "begin=" << CommonTestUtils::vec2str(params.begin) << "_";
    result << "end=" << CommonTestUtils::vec2str(params.end) << "_";
    result << "stride=" << CommonTestUtils::vec2str(params.strides) << "_";
    result << "begin_m=" << CommonTestUtils::vec2str(params.beginMask) << "_";
    result << "end_m=" << CommonTestUtils::vec2str(params.endMask) << "_";
    result << "new_axis_m=" << (params.newAxisMask.empty() ? "def" : CommonTestUtils::vec2str(params.newAxisMask)) << "_";
    result << "shrink_m=" << (params.shrinkAxisMask.empty() ? "def" : CommonTestUtils::vec2str(params.shrinkAxisMask)) << "_";
    result << "ellipsis_m=" << (params.ellipsisAxisMask.empty() ? "def" : CommonTestUtils::vec2str(params.ellipsisAxisMask)) << "_";
    result << "targetDevice=" << targetName;
    return result.str();
}

void StridedSliceLayerTest::SetUp() {
    StridedSliceSpecificParams ssParams;
    InferenceEngine::Precision netPrecision;
    std::map<std::string, std::string> additionalConfig;
    std::tie(ssParams, netPrecision, targetDevice, additionalConfig) = this->GetParam();
    configuration.insert(additionalConfig.begin(), additionalConfig.end());

    auto ngPrc = FuncTestUtils::PrecisionUtils::convertIE2nGraphPrc(netPrecision);
    auto params = ngraph::builder::makeParams(ngPrc, {ssParams.inputShape});
    auto paramOuts = ngraph::helpers::convert2OutputVector(
            ngraph::helpers::castOps2Nodes<ngraph::op::Parameter>(params));
    auto ss = ngraph::builder::makeStridedSlice(paramOuts[0], ssParams.begin, ssParams.end, ssParams.strides, ngPrc, ssParams.beginMask,
                                                ssParams.endMask, ssParams.newAxisMask, ssParams.shrinkAxisMask, ssParams.ellipsisAxisMask);
    ngraph::ResultVector results{std::make_shared<ngraph::opset1::Result>(ss)};
    function = std::make_shared<ngraph::Function>(results, params, "StridedSlice");
}

TEST_P(StridedSliceLayerTest, CompareWithRefs) {
    Run();
}

}  // namespace LayerTestsDefinitions
