
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

#include "kompute_test/shaders/shadertest_workgroup.hpp"

TEST(TestWorkgroup, TestSimpleWorkgroup)
{
    std::shared_ptr<kp::Tensor> tensorA = nullptr;
    std::shared_ptr<kp::Tensor> tensorB = nullptr;
    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            tensorA = mgr.tensor(std::vector<float>(16 * 8));
            tensorB = mgr.tensor(std::vector<float>(16 * 8));

            std::vector<std::shared_ptr<kp::Tensor>> params = { tensorA,
                                                                tensorB };

            std::vector<uint32_t> spirv(
              (uint32_t*)
                kp::shader_data::test_shaders_glsl_test_workgroup_comp_spv,
              (uint32_t*)(kp::shader_data::
                            test_shaders_glsl_test_workgroup_comp_spv +
                          kp::shader_data::
                            test_shaders_glsl_test_workgroup_comp_spv_len));

            kp::Workgroup workgroup = { 16, 8, 1 };

            std::shared_ptr<kp::Algorithm> algorithm =
              mgr.algorithm(params, spirv, workgroup);

            sq = mgr.sequence();
            sq->record<kp::OpTensorSyncDevice>(params);
            sq->record<kp::OpAlgoDispatch>(algorithm);
            sq->record<kp::OpTensorSyncLocal>(params);
            sq->eval();
        }
    }

    std::vector<float> expectedA = {
        0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
        2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,
        4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,
        6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,
        8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,
        10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11,
        12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13,
        14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15
    };

    std::vector<float> expectedB = {
        0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5,
        6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3,
        4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1,
        2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7,
        0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5,
        6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7
    };

    EXPECT_EQ(tensorA->data(), expectedA);
    EXPECT_EQ(tensorB->data(), expectedB);
}
