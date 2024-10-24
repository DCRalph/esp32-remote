#pragma once

#include "config.h"

#include <unordered_map>
#include <vector>
#include <functional>

// Base class for decoders (non-templated, no canFrame member)
class CANFrameDecoderBase
{
protected:
  uint32_t decoderId; // ID of the decoder

public:
  uint64_t lastReceivedTime; // Last time a frame was received

  // Constructor accepting a decoder ID
  CANFrameDecoderBase();

  // Virtual destructor for proper cleanup
  virtual ~CANFrameDecoderBase() {}

  // Get the decoder ID
  uint32_t getId() const;

  // Virtual process method to be overridden by derived classes
  virtual void process(canFrame *frame) = 0;
};

// Template class for decoding CAN frames (inherits from base, no canFrame member)
template <typename T>
class CANFrameDecoder : public CANFrameDecoderBase
{
protected:
  T result; // Decoded result of type T

public:
  // Constructor accepting a decoder ID
  CANFrameDecoder();

  // Method to get the decoded value
  T get() const
  {
    return result;
  }
};

// Manager class to handle multiple decoders (using std::unordered_map for faster lookups)
class CANDecoderManager
{
private:
  // Hash map (unordered_map) to store decoders by their ID
  std::unordered_map<uint32_t, std::vector<CANFrameDecoderBase *>> decoders;

public:
  // Method to add a decoder for a specific frame ID
  void add(CANFrameDecoderBase *decoder);

  // Method to find and process the decoders for a specific frame
  bool process(canFrame *frame);
};

template class CANFrameDecoder<int>;
template class CANFrameDecoder<HeadLightsState>;
template class CANFrameDecoder<GearState>;
template class CANFrameDecoder<ShifterState>;

extern CANDecoderManager decoderManager;