#include "lab_mosaic.h"
#include <iostream>

int main()
{
  try
  {
    runLabMosaic();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Caught exception:" << std::endl
              << e.what() << std::endl;
  }
  catch (...)
  {
    std::cerr << "Caught unknown exception"  << std::endl;
  }

  return EXIT_SUCCESS;
}
