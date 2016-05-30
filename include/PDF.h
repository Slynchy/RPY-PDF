#ifndef PDF_H
#define PDF_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include "hpdf.h"
#include "hpdf_types.h"

#include <string>
#include <vector>

void draw_divider  (HPDF_Page page, float y);
int WriteScriptToPDF(std::vector<std::vector<std::wstring>>& _input, HPDF_Doc& pdf, const char* font_name, std::vector<HPDF_Page>& pages, std::vector<std::vector<int>>&);

#endif