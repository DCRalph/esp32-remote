#pragma once

#include "config.h"

#include <TensorFlowLite.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "Model/model_colour_channels4.h"

class ML
{

  const tflite::Model *model = nullptr;
  tflite::MicroInterpreter *interpreter = nullptr;
  TfLiteTensor *input = nullptr;
  TfLiteTensor *output = nullptr;

  int kTensorArenaSize = 2000;
  alignas(16) uint8_t tensor_arena[2000];

public:
  ML();
  void init();
};