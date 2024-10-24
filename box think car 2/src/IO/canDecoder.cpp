#include "canDecoder.h"

// CANFrameDecoderBase constructor
CANFrameDecoderBase::CANFrameDecoderBase()
{
  // Initialize the last received time to 0
  lastReceivedTime = 0;
}

// Get the decoder ID
uint32_t CANFrameDecoderBase::getId() const
{
  return decoderId;
}

// CANFrameDecoder constructor
template <typename T>
CANFrameDecoder<T>::CANFrameDecoder()
{
  // Initialize the result to the default value of type T
  result = T();
}

// Add decoder to manager
void CANDecoderManager::add(CANFrameDecoderBase *decoder)
{
  // Add the decoder to the list for the corresponding frame ID
  decoders[decoder->getId()].push_back(decoder);
}

// Process the frame using the appropriate decoder(s)
bool CANDecoderManager::process(canFrame *frame)
{
  // check if the data is all 0
  bool allZero = true;
  for (int i = 0; i < frame->len; i++)
  {
    if (frame->data[i] != 0)
    {
      allZero = false;
      break;
    }
  }

  if (allZero)
  {
    return false;
  }

  auto it = decoders.find(frame->id); // Try to find the decoders by the frame ID
  if (it != decoders.end())
  {
    // Iterate over all decoders for this ID and call their process methods
    for (auto &decoder : it->second)
    {
      decoder->process(frame); // Call the process method of the found decoder
      decoder->lastReceivedTime = millis();
    }
    return true;
  }
  // No decoder found for this frame ID
  return false;
}

CANDecoderManager decoderManager; // Global decoder manager instance