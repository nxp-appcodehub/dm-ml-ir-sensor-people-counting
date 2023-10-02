/*
 * Copyright 2020-2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "output_postproc.h"
#include "get_top_n.h"
#include "model_config.h"

status_t MODEL_ProcessOutput(const uint8_t* data, const tensor_dims_t* dims,
                             tensor_type_t type, int inferenceTime)
{
    const uint32_t num_classes = sizeof(labels) / sizeof(char*);
    result_t top_results[num_classes];
    const char* label = "No label detected";

    /* Find best label candidates. */
    MODEL_GetTopN(data, dims->data[dims->size - 1], type, num_classes, detect_threshold, top_results);

    float confidence = 0;
    if (top_results[0].index >= 0)
    {
        auto result = top_results[0];
        confidence = result.score;
        int index = result.index;
        if (confidence * 100 > detect_threshold)
        {
            label = labels[index];
        }
    }

    int score = (int)(confidence * 100);
    PRINTF("----------------------------------------" "\r\n");
    PRINTF("     Inference time: %d ms" "\r\n", inferenceTime / 1000);
    PRINTF("     Detected: %s (%d%%)" "\r\n", label, score);
    PRINTF("----------------------------------------" "\r\n");

    return kStatus_Success;
}
