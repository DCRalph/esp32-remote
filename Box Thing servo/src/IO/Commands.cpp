#include "Commands.h"

#include <RemoteProtocol.h>

#include "ServoController.h"

void registerCommands(void (*onCommand)())
{
  RemoteProtocol::Servo::command().onReceive([onCommand](const TransportAddress &, const RemoteProtocol::Servo::Command &cmd)
                                             {
                                               servoSetMan(cmd.manual == 1);
                                               servoSetX(cmd.x);
                                               servoSetY(cmd.y);
                                               onCommand(); });
}
