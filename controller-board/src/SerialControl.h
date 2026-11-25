// USB serial control, after bioreactor-controller's agent commands: tap and hold
// the touch screen, stream the screen out, and jump between screens, so the UI can
// be driven from a computer (tools/agent.py). Runs on the loop task, between frames.
#pragma once

class ControllerPanel;

namespace SerialControl
{
  void begin(ControllerPanel &panel);
  /** Reads pending serial input and runs complete lines. */
  void loop();
}
