#include "IndicatorFlickSequence.h"

IndicatorFlickSequence::IndicatorFlickSequence()
    : SequenceBase("IndicatorFlickSequence", 2000),
      currentIndex(0),
      firstFlashTime(0),
      debounceTime(50),
      lastDebounceTime(0),
      accOnState(false),
      leftIndicatorState(false),
      rightIndicatorState(false),
      lastLeftState(false),
      lastRightState(false)
{
}

bool IndicatorFlickSequence::update()
{
  unsigned long currentTime = millis();

  // Process this sequence only when ACC is on.
  if (accOnState)
  {

    if (leftIndicatorState && rightIndicatorState) // Both indicators are on. eg hazard lights
    {
      reset();
      return false;
    }

    // Check for rising edge for left indicator.
    bool leftEdge = leftIndicatorState && !lastLeftState &&
                    ((currentTime - lastDebounceTime) >= debounceTime);
    // Check for rising edge for right indicator.
    bool rightEdge = rightIndicatorState && !lastRightState &&
                     ((currentTime - lastDebounceTime) >= debounceTime);

    // Determine which side is expected for the current step.
    IndicatorSide expected = expectedSequence[currentIndex];

    // If the expected side's rising edge is detected, advance the sequence.
    if (expected == LEFT_SIDE && leftEdge)
    {
      if (firstFlashTime == 0)
      {
        firstFlashTime = currentTime;
      }
      start();
      currentIndex++;
      lastDebounceTime = currentTime;
    }
    else if (expected == RIGHT_SIDE && rightEdge)
    {
      if (firstFlashTime == 0)
      {
        firstFlashTime = currentTime;
      }
      start();
      currentIndex++;
      lastDebounceTime = currentTime;
    }
    // If an indicator flash (rising edge) is detected but it does not match the expected side,
    // then reset the sequence.
    else if ((leftEdge || rightEdge) && !((expected == LEFT_SIDE && leftEdge) ||
                                          (expected == RIGHT_SIDE && rightEdge)))
    {
      reset();
    }

    // Save states for the next call.
    lastLeftState = leftIndicatorState;
    lastRightState = rightIndicatorState;

    // If the sequence is complete, return true.
    if (currentIndex >= SEQUENCE_LENGTH)
    {
      return true;
    }
  }
  else
  {
    // When ACC is off, reset the sequence.
    reset();
  }
  return false;
}

void IndicatorFlickSequence::reset()
{
  currentIndex = 0;
  firstFlashTime = 0;
  // Reset debounce timer so that a new flash is counted correctly.
  lastDebounceTime = millis();
  lastLeftState = false;
  lastRightState = false;
}

void IndicatorFlickSequence::setInputs(bool accOnState,
                                       bool leftIndicatorState,
                                       bool rightIndicatorState)
{
  this->accOnState = accOnState;
  this->leftIndicatorState = leftIndicatorState;
  this->rightIndicatorState = rightIndicatorState;
}
