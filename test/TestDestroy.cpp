
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestDestroy, TestDestroyTensorSingle)
{
    std::shared_ptr<kp::Tensor> tensorA = nullptr;

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            tensorA = mgr.tensor({ 0, 0, 0 });

            std::shared_ptr<kp::Algorithm> algo =
              mgr.algorithm({ tensorA }, spirv);

            mgr.sequence()
              ->record<kp::OpAlgoDispatch>(algo)
              ->eval()
              ->eval<kp::OpTensorSyncLocal>(algo->getTensors());

            tensorA->destroy();
            EXPECT_FALSE(tensorA->isInit());
        }
        EXPECT_FALSE(tensorA->isInit());
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 1, 1, 1 }));
}

TEST(TestDestroy, TestDestroyTensorVector)
{
    std::shared_ptr<kp::Tensor> tensorA = nullptr;
    std::shared_ptr<kp::Tensor> tensorB = nullptr;

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      layout(set = 0, binding = 1) buffer b { float pb[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
          pb[index] = pb[index] + 2;
      })");
    std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            tensorA = mgr.tensor({ 1, 1, 1 });
            tensorB = mgr.tensor({ 1, 1, 1 });

            std::shared_ptr<kp::Algorithm> algo =
              mgr.algorithm({ tensorA, tensorB }, spirv);

            mgr.sequence()
              ->record<kp::OpTensorSyncDevice>(algo->getTensors())
              ->record<kp::OpAlgoDispatch>(algo)
              ->record<kp::OpTensorSyncLocal>(algo->getTensors())
              ->eval();

            tensorA->destroy();
            tensorB->destroy();

            EXPECT_FALSE(tensorA->isInit());
            EXPECT_FALSE(tensorB->isInit());
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 2, 2, 2 }));
    EXPECT_EQ(tensorB->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestDestroy, TestDestroySequenceSingle)
{
    std::shared_ptr<kp::Tensor> tensorA = nullptr;

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            tensorA = mgr.tensor({ 0, 0, 0 });

            sq =
              mgr.sequence()
                ->record<kp::OpTensorSyncDevice>({ tensorA })
                ->record<kp::OpAlgoDispatch>(mgr.algorithm({ tensorA }, spirv))
                ->record<kp::OpTensorSyncLocal>({ tensorA })
                ->eval();

            sq->destroy();

            EXPECT_FALSE(sq->isInit());
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 1, 1, 1 }));
}
