#include "Printer.h"
#include "Config.h"

PrinterClass Printer(
  config::pins::printPwm,
  config::pins::printPin1,
  config::pins::printPin2
);
