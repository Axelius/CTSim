/*
 * pgmImportExport.h
 *
 *  Created on: 31.07.2014
 *      Author: Alexander
 */

#ifndef PGMIMPORTEXPORT_H_
#define PGMIMPORTEXPORT_H_

#include "Logger.h"
#include "Constants.h"

int loadPGMToRaw(unsigned int ***raw, FILE *data);
int exportPGM(FILE* out, unsigned int** write, int x, int y);



#endif /* PGMIMPORTEXPORT_H_ */
