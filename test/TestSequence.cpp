
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestSequence, SequenceDestructorViaManager)
{
    std::shared_ptr<kp::Sequence> sq = nullptr;

    {
        kp::Manager mgr;

        sq = mgr.sequence();

        EXPECT_TRUE(sq->isInit());
    }

    EXPECT_FALSE(sq->isInit());
}

TEST(TestSequence, SequenceDestructorOutsideManagerExplicit)
{
    std::shared_ptr<kp::Sequence> sq = nullptr;

    {
        kp::Manager mgr;

        sq = mgr.sequence();

        EXPECT_TRUE(sq->isInit());

        sq->destroy();

        EXPECT_FALSE(sq->isInit());
    }

    EXPECT_FALSE(sq->isInit());
}

TEST(TestSequence, SequenceDestructorOutsideManagerImplicit)
{
    kp::Manager mgr;

    std::weak_ptr<kp::Sequence> sqWeak;

    {
        std::shared_ptr<kp::Sequence> sq = mgr.sequence();

        sqWeak = sq;

        EXPECT_TRUE(sq->isInit());
    }

    EXPECT_FALSE(sqWeak.lock());
}

TEST(TestSequence, RerecordSequence)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Sequence> sq = mgr.sequence();

    std::shared_ptr<kp::Tensor> tensorA = mgr.tensor({1, 2, 3});
    std::shared_ptr<kp::Tensor> tensorB = mgr.tensor({2, 2, 2});
    std::shared_ptr<kp::Tensor> tensorOut = mgr.tensor({0, 0, 0});

    sq->eval<kp::OpTensorSyncDevice>({ tensorA, tensorB, tensorOut });

    std::vector<uint32_t> spirv = kp::Shader::compile_source(R"(
        #version 450

        layout (local_size_x = 1) in;

        // The input tensors bind index is relative to index in parameter passed
        layout(set = 0, binding = 0) buffer bina { float tina[]; };
        layout(set = 0, binding = 1) buffer binb { float tinb[]; };
        layout(set = 0, binding = 2) buffer bout { float tout[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            tout[index] = tina[index] * tinb[index];
        }
    )");

    std::shared_ptr<kp::Algorithm> algo =
        mgr.algorithm({tensorA, tensorB, tensorOut}, spirv);

    sq->record<kp::OpAlgoDispatch>(algo)
        ->record<kp::OpTensorSyncLocal>({tensorA, tensorB, tensorOut});

    sq->eval();

    EXPECT_EQ(tensorOut->data(), std::vector<float>({2, 4, 6}));

    algo->rebuild({tensorOut, tensorA, tensorB}, spirv);

    // Refresh and trigger a rerecord
    sq->rerecord();
    sq->eval();

    EXPECT_EQ(tensorB->data(), std::vector<float>({2, 8, 18}));
}
