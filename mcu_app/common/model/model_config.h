/*
 * Copyright 2023 NXP
 * All rights reserved.
 */

#ifndef MODEL_CONFIG_H_
#define MODEL_CONFIG_H_

/* Name of model buffer located in training/models/src */
#define MODEL model_tflite_quant

constexpr const char *model_name   = "Presence detect";
constexpr uint32_t max_tensor_dims = 4;
constexpr float detect_threshold   = 0.80;
constexpr int kTensorArenaSize     = 10 * 1024;
constexpr const char *labels[]     = {"no-presence", "one person", "two people"};

#endif /* MODEL_CONFIG_H_ */
