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

  int kTensorArenaSize = 2000000;
  uint8_t *tensor_arena;

  bool setupDone = false;

public:
  ML();
  void init();
  void run();
};

extern ML ml;