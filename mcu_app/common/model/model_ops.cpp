/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"

tflite::MicroOpResolver &MODEL_GetOpsResolver(tflite::ErrorReporter* errorReporter)
{
    static tflite::MicroMutableOpResolver<15> s_microOpResolver(errorReporter);
    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddQuantize();
    s_microOpResolver.AddDequantize();
    s_microOpResolver.AddSub();
    s_microOpResolver.AddAdd();
    s_microOpResolver.AddShape();
    s_microOpResolver.AddBroadcastArgs();
    s_microOpResolver.AddBroadcastTo();
    s_microOpResolver.AddMul();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddRelu();

    return s_microOpResolver;
}
