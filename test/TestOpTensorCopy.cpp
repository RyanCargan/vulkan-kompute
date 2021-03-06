
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestOpTensorCopy, CopyDeviceToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::Tensor> tensorB = mgr.tensor(testVecB);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpTensorSyncDevice>({ tensorA, tensorB })
      ->eval<kp::OpTensorCopy>({ tensorA, tensorB })
      ->eval<kp::OpTensorSyncLocal>({ tensorA, tensorB });

    // Making sure the GPU holds the same data
    EXPECT_EQ(tensorA->data(), tensorB->data());
}

TEST(TestOpTensorCopy, CopyDeviceToDeviceTensorMulti)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 2, 3, 4 };
    std::vector<float> testVecB{ 0, 0, 0 };
    std::vector<float> testVecC{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::Tensor> tensorB = mgr.tensor(testVecB);
    std::shared_ptr<kp::Tensor> tensorC = mgr.tensor(testVecC);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());
    EXPECT_TRUE(tensorC->isInit());

    mgr.sequence()
      ->eval<kp::OpTensorSyncLocal>({ tensorA, tensorB, tensorC })
      ->eval<kp::OpTensorCopy>({ tensorA, tensorB, tensorC });

    EXPECT_EQ(tensorA->data(), tensorB->data());
    EXPECT_EQ(tensorA->data(), tensorC->data());

    // Making sure the GPU holds the same data
    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorB, tensorC });

    EXPECT_EQ(tensorA->data(), tensorB->data());
    EXPECT_EQ(tensorA->data(), tensorC->data());
}

TEST(TestOpTensorCopy, CopyDeviceToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 3, 4, 5 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::Tensor> tensorB =
      mgr.tensor(testVecB, kp::Tensor::TensorTypes::eHost);

    //  Only calling sync on device type tensor
    mgr.sequence()->eval<kp::OpTensorSyncDevice>({ tensorA });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()->eval<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), tensorB->data());

    // Making sure the GPU holds the same data
    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->data(), tensorB->data());
}

TEST(TestOpTensorCopy, CopyHostToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 4, 5, 6 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA =
      mgr.tensor(testVecA, kp::Tensor::TensorTypes::eHost);
    std::shared_ptr<kp::Tensor> tensorB = mgr.tensor(testVecB);

    //  Only calling sync on device type tensor
    mgr.sequence()->eval<kp::OpTensorSyncDevice>({ tensorA, tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()->eval<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), tensorB->data());

    // Making sure the GPU holds the same data
    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->data(), tensorB->data());
}

TEST(TestOpTensorCopy, CopyHostToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 5, 6, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA =
      mgr.tensor(testVecA, kp::Tensor::TensorTypes::eHost);
    std::shared_ptr<kp::Tensor> tensorB =
      mgr.tensor(testVecB, kp::Tensor::TensorTypes::eHost);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpTensorSyncDevice>({ tensorA })
      ->eval<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), tensorB->data());

    // Making sure the GPU holds the same data
    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->data(), tensorB->data());
}

TEST(TestOpTensorCopy, SingleTensorShouldFail)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 6, 7, 8 };

    std::shared_ptr<kp::Tensor> tensorA =
      mgr.tensor(testVecA, kp::Tensor::TensorTypes::eHost);

    EXPECT_TRUE(tensorA->isInit());

    EXPECT_THROW(mgr.sequence()->eval<kp::OpTensorCopy>({ tensorA }),
                 std::runtime_error);
}
