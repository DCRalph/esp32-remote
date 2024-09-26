#include "ML.h"

static const char *TAG = "ML";

ML::ML()
{
}

void ML::init()
{
  ESP_LOGI(TAG, "init");

  tensor_arena = (uint8_t *)ps_malloc(kTensorArenaSize * sizeof(uint8_t));
  if (tensor_arena == NULL)
  {
    ESP_LOGI(TAG, "Error: could not allocate tensor arena");
    while (true)
      ;
  }

  model = tflite::GetModel(model_colour_channels4_tflite);

  if (model->version() != TFLITE_SCHEMA_VERSION)
  {
    ESP_LOGI(TAG, "Model provided is schema version %d not equal to supported version %d", model->version(), TFLITE_SCHEMA_VERSION);
    while (true)
      ; // stop program here
  }

  // This pulls in all the TensorFlow Lite operators.
  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  // if an error occurs, stop the program.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk)
  {
    ESP_LOGI(TAG, "AllocateTensors() failed");
    while (true)
      ; // stop program here
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);

  setupDone = true;
  ESP_LOGI(TAG, "init done");
}

void ML::run()
{
  if (!setupDone)
  {
    ESP_LOGI(TAG, "run not setup");
    return;
  }

  ESP_LOGI(TAG, "run");

  // Fill input buffer
  for (int i = 0; i < 4; i++)
  {
    input->data.f[i] = 0.0;
  }

  // Run inference
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk)
  {
    ESP_LOGI(TAG, "Invoke() failed");
    while (true)
      ;
  }

  // Read output buffer
  for (int i = 0; i < 4; i++)
  {
    ESP_LOGI(TAG, "output->data.f[%d] = %f", i, output->data.f[i]);
  }

  ESP_LOGI(TAG, "run done");
}

ML ml;