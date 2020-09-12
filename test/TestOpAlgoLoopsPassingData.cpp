
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestProcessingIterations, IterateThroughMultipleSumAndCopies) {
    kp::Manager mgr;

    float TOTAL_ITER = 10;

    std::vector<float> testExpectedOutVec = {TOTAL_ITER, TOTAL_ITER, TOTAL_ITER};

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 })};
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 })};

    std::string shader(R"(
        #version 450

        layout (local_size_x = 1) in;

        layout(set = 0, binding = 0) buffer a { float pa[]; };
        layout(set = 0, binding = 1) buffer b { float pb[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            pb[index] = pa[index] + 1;
        }
    )");

    std::weak_ptr<kp::Sequence> sqWeakPtr = 
        mgr.getOrCreateManagedSequence("default");

    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) 
    {
        sq->begin();

        sq->record<kp::OpTensorCreate>({ tensorA, tensorB });

        sq->end();

        sq->eval();
    }

    std::weak_ptr<kp::Sequence> sqWeakPtr2 = 
        mgr.getOrCreateManagedSequence("run");

    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr2.lock()) 
    {
        sq->begin();

        sq->record<kp::OpAlgoBase<>>(
                { tensorA, tensorB }, 
                std::vector<char>(shader.begin(), shader.end()));

        sq->record<kp::OpTensorCopy>({tensorB, tensorA});
        sq->end();

        for (size_t i = 0; i < TOTAL_ITER; i++) {
            sq->eval();
        }
    }

    std::weak_ptr<kp::Sequence> sqWeakPtr3 = 
        mgr.getOrCreateManagedSequence("export");

    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr3.lock()) 
    {
        sq->begin();

        sq->record<kp::OpTensorSyncLocal>({tensorA, tensorB});

        sq->end();

        sq->eval();
    }

    EXPECT_EQ(tensorA->data(), testExpectedOutVec);
}

