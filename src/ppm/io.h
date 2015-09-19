#ifndef _PPM_IO_
#define _PPM_IO_

#include "ppm/ppm.h"
#include <iosfwd>

namespace io {

/// Lee la imagen del fichero dado.
PPM read_ppm (const char* filename);

/// Escribe el ppm en el fichero dado.
void write_ppm (const PPM& image, const char* filename);

} // namespace io end

#endif // _PPM_IO_
